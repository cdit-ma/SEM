#include "dataflowdialog.h"
#include "dataflowgraphicsview.h"
#include "../DockWidgets/basedockwidget.h"
#include "../../theme.h"

#include "EntityItems/componentinstancegraphicsitem.h"
#include "GraphicsItems/edgeitem.h"

#include <QGraphicsRectItem>
#include <QVBoxLayout>
#include <QDockWidget>
#include <QtConcurrent>

const int invalid_experiment_id = -1;
const int timer_interval_ms = 330;

/**
 * @brief DataflowDialog::DataflowDialog
 * @param parent
 */
DataflowDialog::DataflowDialog(QWidget *parent)
    : QFrame(parent),
      playback_controls(this)
{
    view_ = new DataflowGraphicsView(this);

    connect(&playback_controls, &PlaybackControlsWidget::play, this, &DataflowDialog::playback);
    connect(&playback_controls, &PlaybackControlsWidget::pause, this, &DataflowDialog::pausePlayback);
    connect(&playback_controls, &PlaybackControlsWidget::jumpToStart, this, &DataflowDialog::jumpToStart);
    connect(&playback_controls, &PlaybackControlsWidget::jumpToEnd, this, &DataflowDialog::jumpToEnd);
    connect(&playback_controls, &PlaybackControlsWidget::jumpToPreviousActivity, this, &DataflowDialog::jumpToPreviousActivity);
    connect(&playback_controls, &PlaybackControlsWidget::jumpToNextActivity, this, &DataflowDialog::jumpToNextActivity);
    connect(this, &DataflowDialog::playbackActivated, &playback_controls, &PlaybackControlsWidget::setPlayPauseCheckedState);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);
    mainLayout->addWidget(view_);
    mainLayout->addWidget(&playback_controls);

    connect(Theme::theme(), &Theme::theme_Changed, this, &DataflowDialog::themeChanged);
    themeChanged();
}


/**
 * @brief DataflowDialog::addPortLifecycleEventsToSeries
 * @param events
 */
void DataflowDialog::addPortLifecycleEventsToSeries(const QVector<PortLifecycleEvent*>& events)
{
    /**
     * NOTE: This function is only here to test getting events between a time interval from a series
     * The series will already be constructed when using the new experiment data classes
     */

    portlifecycle_series_list_.clear();

    for (const auto& event : events) {
        if (event != nullptr) {
            PortLifecycleEventSeries* series = nullptr;
            auto&& series_id = event->getID();
            // Check if a series for the event already exists
            if (portlifecycle_series_list_.contains(series_id)) {
                series = portlifecycle_series_list_.value(series_id);
            } else {
                // Construct new series
                auto port_id = series_id.split("_").first();
                series = new PortLifecycleEventSeries(series_id);
                portlifecycle_series_list_.insert(series_id, series);
            }
            if (series) {
                series->addEvent(event);
            }
        }
    }

    calculateActiveTimes();
}


/**
 * @brief DataflowDialog::addPortEventsToSeries
 * NOTE - This is only used to prototyte the flashing of ports for port events
 * @param events
 */
void DataflowDialog::addPortEventsToSeries(const QVector<PortEvent*>& events)
{
    portevent_series_list_.clear();

    for (const auto& event : events) {
        if (event != nullptr) {
            PortEventSeries* series = nullptr;
            auto&& series_id = event->getID();
            // Check if a series for the event already exists
            if (portevent_series_list_.contains(series_id)) {
                series = portevent_series_list_.value(series_id);
            } else {
                // Construct new series
                auto port_id = series_id.split("_").first();
                series = new PortEventSeries(series_id);
                portevent_series_list_.insert(series_id, series);
            }
            if (series) {
                series->addEvent(event);
            }
        }
    }

    calculateActiveTimes();
}


/**
 * @brief DataflowDialog::themeChanged
 */
void DataflowDialog::themeChanged()
{
    Theme* theme = Theme::theme();
    setStyleSheet(theme->getScrollBarStyleSheet());
    view_->setStyleSheet("padding: 0px; border: 1px solid " + theme->getDisabledBackgroundColorHex() + ";");
    view_->setBackgroundBrush(theme->getBackgroundColor());

    for (const auto& edge_item : edge_items_) {
        edge_item->themeChanged(theme);
    }
}


/**
 * @brief DataflowDialog::constructGraphicsItemsForExperimentRun
 * @param exp_name
 * @param exp_run_data
 */
void DataflowDialog::constructGraphicsItemsForExperimentRun(const QString& exp_name, const MEDEA::ExperimentRunData& exp_run_data)
{
    if (exp_name.isEmpty()) {
        throw std::invalid_argument("DataflowDialog::constructGraphicsItemsForExperimentRun - Experiment name cannot be empty.");
    }

    // Clear previous items
    clearScene();

    exp_run_start_time_ = exp_run_data.start_time();
    exp_run_end_time_ = exp_run_data.end_time();
    playback_duration_ms_ = exp_run_end_time_ - exp_run_start_time_;
    playback_controls.setTimeRange(exp_run_start_time_, exp_run_end_time_);

    // This sets the experiment info displays on the title bar of the panel
    setExperimentInfo(exp_name, exp_run_data.experiment_run_id());

    for (const auto& node_data : exp_run_data.getNodeData()) {
        for (const auto& container_data : node_data->getContainerInstanceData()) {
            for (const auto& comp_inst_data : container_data->getComponentInstanceData()) {
                if (comp_inst_data) {

                    auto comp_inst_item = new ComponentInstanceGraphicsItem(*comp_inst_data);
                    addItemToScene(comp_inst_item);

                    for (const auto& port_data : comp_inst_data->getPortInstanceData()) {
                        if (port_data) {
                            auto port_inst_item = comp_inst_item->addPortInstanceItem(*port_data);
                            port_items_.insert(port_data->getGraphmlID(), port_inst_item);
                        } else {
                            throw std::invalid_argument("DataflowDialog::constructGraphicsItemsForExperimentRun - PortInstanceData is null.");
                        }
                    }

                } else {
                    throw std::invalid_argument("DataflowDialog::constructGraphicsItemsForExperimentRun - ComponentInstanceData is null.");
                }
            }
        }
    }

    // Construct the edges
    constructEdgeItems(port_items_, exp_run_data.getPortConnectionData());
}


/**
 * @brief DataflowDialog::clearScene
 * This clears all the graphics items in the scene
 */
void DataflowDialog::clearScene()
{
    setExperimentInfo("");

    view_->scene()->clear();
    portlifecycle_series_list_.clear();
    portevent_series_list_.clear();
    port_items_.clear();
    active_series_.clear();
    active_times_.clear();

    resetPlayback();
}


/**
 * @brief DataflowDialog::playback
 */
void DataflowDialog::playback()
{
    if (!timer_active_) {
        if (playback_elapsed_time_ == 0) {
            qDebug() << "\nPLAYBACK STARTED ------------------------------------------------------";
            resetPlayback();
        } else {
            qDebug() << "CONTINUE playback";
        }
        startPlaybackTimer();
    }
}


/**
 * @brief DataflowDialog::pausePlayback
 */
void DataflowDialog::pausePlayback()
{
    qDebug() << "PAUSE playback";
    stopPlaybackTimer();
}


/**
 * @brief DataflowDialog::jumpToPreviousActivity
 */
void DataflowDialog::jumpToPreviousActivity()
{
    qDebug() << "JUMP to PREVIOUS activity";
    if (!active_times_.isEmpty()) {
        if ((last_active_time_index_ > 0) && (last_active_time_index_ < active_times_.length())) {
            auto&& last_active_time = active_times_.at(last_active_time_index_);
            if (playback_current_time_ == last_active_time) {
                last_active_time_index_--;
            }
            playback_current_time_ = active_times_.at(last_active_time_index_);
            playback_elapsed_time_ = playback_current_time_ - exp_run_start_time_;
            playback_controls.setCurrentTime(playback_current_time_);
        }
    }
}


/**
 * @brief DataflowDialog::jumpToNextActivity
 */
void DataflowDialog::jumpToNextActivity()
{
    qDebug() << "JUMP to NEXT activity";
    if (!active_times_.isEmpty()) {
        if (last_active_time_index_ < active_times_.length() - 1) {
            playback_current_time_ = active_times_.at(++last_active_time_index_);
            playback_elapsed_time_ = playback_current_time_ - exp_run_start_time_;
            playback_controls.setCurrentTime(playback_current_time_);
        }
    }
}


/**
 * @brief DataflowDialog::jumpToStart
 */
void DataflowDialog::jumpToStart()
{
    qDebug() << "JUMP to START";
    last_active_time_index_ = -1;
    playback_elapsed_time_ = 0;
    playback_current_time_ = exp_run_start_time_;
    playback_controls.setCurrentTime(playback_current_time_);
}


/**
 * @brief DataflowDialog::jumpToEnd
 */
void DataflowDialog::jumpToEnd()
{
    qDebug() << "JUMP to END";
    last_active_time_index_ = active_times_.length() - 1;
    playback_elapsed_time_ = playback_duration_ms_;
    playback_current_time_ = exp_run_end_time_;
    playback_controls.setCurrentTime(playback_current_time_);
}


/**
 * @brief DataflowDialog::setExperimentInfo
 * @param exp_name
 * @param exp_run_id
 */
void DataflowDialog::setExperimentInfo(const QString& exp_name, quint32 exp_run_id)
{
    if (parentWidget()) {
        QString title = "Pulse";
        if (!exp_name.isEmpty()) {
            title += " [" + exp_name + " #" + QString::number(exp_run_id) + "]";
        }
        auto parent_dockwidget = qobject_cast<BaseDockWidget*>(parentWidget());
        parent_dockwidget->setTitle(title);
    }
}


/**
 * @brief DataflowDialog::calculateActiveTimes
 */
void DataflowDialog::calculateActiveTimes()
{
    active_times_.clear();
    active_series_.clear();

    // This is where the series that we want to see in the playback gets added
    // Right now, it's adding both port lifecycle and port event series

    auto current_time = exp_run_start_time_;
    while (current_time < exp_run_end_time_) {
        auto to_time = current_time + timer_interval_ms;
        for (const auto& series : portlifecycle_series_list_) {
            const auto& events = series->getEventsBetween(current_time, to_time);
            if (!events.isEmpty()) {
                if (!active_times_.contains(current_time)) {
                    active_times_.append(current_time);
                }
                active_series_.insertMulti(current_time, series);
            }
        }
        for (const auto& series : portevent_series_list_) {
            const auto& events = series->getEventsBetween(current_time, to_time);
            if (!events.isEmpty()) {
                if (!active_times_.contains(current_time)) {
                    active_times_.append(current_time);
                }
                active_series_.insertMulti(current_time, series);
            }
        }
        current_time = to_time;
    }

    // TODO: Find a better way to do this
    if (!active_times_.isEmpty()) {
        qSort(active_times_.begin(), active_times_.end());
    }
}


/**
 * @brief DataflowDialog::resetPlayback
 */
void DataflowDialog::resetPlayback()
{
    stopPlaybackTimer();
    playback_controls.resetTimeProgress();

    playback_current_time_ = exp_run_start_time_;
    playback_elapsed_time_ = 0;
    last_active_time_index_ = -1;
}


/**
 * @brief DataflowDialog::startPlaybackTimer
 */
void DataflowDialog::startPlaybackTimer()
{
    if (timer_id_ != -1) {
        killTimer(timer_id_);
    }
    timer_id_ = startTimer(timer_interval_ms);
    timer_active_ = true;
}


/**
 * @brief DataflowDialog::stopPlaybackTimer
 */
void DataflowDialog::stopPlaybackTimer()
{
    if (timer_id_ != -1) {
        killTimer(timer_id_);
    }
    timer_id_ = -1;
    timer_active_ = false;
}


/**
 * @brief DataflowDialog::playbackEvents
 * @param from_time
 * @param to_time
 */
void DataflowDialog::playbackEvents(qint64 from_time, qint64 to_time)
{
    QHash<QString, qreal> active_edges_traffic;

    // The temporary min_event_count value should be set as close to 'infinity' as possible, so that any future
    // updates are guaranteed to be smaller than the default. Same applies to max_event_count, but in reverse
    qreal min_event_count = DBL_MAX;
    qreal max_event_count = 0;

    for (const auto& series : active_series_.values(from_time)) {
        auto port = port_items_.value(series->getID(), nullptr);
        if (port) {
            if (series->getKind() == MEDEA::ChartDataKind::PORT_EVENT) {
                const auto& port_id = port->getGraphmlID();
                auto&& port_kind = port->getPortKind();
                if (port_kind == AggServerResponse::Port::PUBLISHER || port_kind == AggServerResponse::Port::REQUESTER || port_kind == AggServerResponse::Port::REPLIER) {
                    auto&& events = series->getEventsBetween(from_time, to_time);
                    qreal event_count = events.size();
                    min_event_count = qMin(event_count, min_event_count);
                    max_event_count = qMax(event_count, max_event_count);

                    // If the port is a Replier, insert the attached port connection's source graphml id
                    if (port_kind == AggServerResponse::Port::REPLIER) {
                        const auto& from_port_id = source_id_for_destination_id_.value(port_id, "");
                        active_edges_traffic.insert(from_port_id, event_count);
                    } else {
                        active_edges_traffic.insert(port_id, event_count);
                    }
                }
            }
            port->flashPort(timer_interval_ms);
            qDebug() << "Port: " << port->getPortName() << " "
                     << series->getEventsBetween(from_time, playback_current_time_).count() << " events";
        } else {
            qDebug() << "No port item to flash for event";
        }
    }

    auto&& event_range = (max_event_count > min_event_count) ? max_event_count - min_event_count : 0.0;
    if (event_range > 0) {
        // Flash the edges from ports that either sent out or requested messages
        for (const auto& from_port_id : active_edges_traffic.keys()) {
            const auto& from_edge = edge_items_.value(from_port_id, nullptr);
            if (from_edge) {
                auto&& traffic = (active_edges_traffic.value(from_port_id) - min_event_count) / event_range;
                from_edge->flashEdge(timer_interval_ms, 1 + (traffic / 2.0));
            }
        }
    }
}


/**
 * @brief DataflowDialog::timerEvent
 * @param event
 */
void DataflowDialog::timerEvent(QTimerEvent* event)
{
    // We only care about the playback timer
    if (event->timerId() != timer_id_) {
        return;
    }

    auto from_time = playback_current_time_;
    if (from_time >= exp_run_end_time_) {
        if (!live_mode_) {
            // Send a signal to the playback controls to update the play/pause button
            emit playbackActivated(false);
            stopPlaybackTimer();
            qDebug() << "PLAYBACK FINISHED ----------------------------------------------------";
        }
    } else {

        playback_controls.incrementCurrentTime(timer_interval_ms);
        playback_current_time_ += timer_interval_ms;
        playback_elapsed_time_ += timer_interval_ms;

        if (active_series_.contains(from_time)) {
            qDebug() << "----------------------------------------------------";
            qDebug() << "Time range: " << QDateTime::fromMSecsSinceEpoch(from_time).toString("hh:mm:ss.zzz")
                     << " to " << QDateTime::fromMSecsSinceEpoch(playback_current_time_).toString("hh:mm:ss.zzz");
            playbackEvents(from_time, playback_current_time_);
            last_active_time_index_ = active_times_.indexOf(from_time);
            qDebug() << "----------------------------------------------------";
        }
    }

    QFrame::timerEvent(event);
}


/**
 * @brief DataflowDialog::constructEdgeItems
 * @param port_instances
 * @param port_connections
 */
void DataflowDialog::constructEdgeItems(const QHash<QString, PortInstanceGraphicsItem*>& port_instances, const QList<PortConnectionData*>& port_connections)
{
    for (const auto& p_c : port_connections) {
        auto from_port = port_instances.value(p_c->getFromPortID(), nullptr);
        auto to_port = port_instances.value(p_c->getToPortID(), nullptr);
        if (!(from_port && to_port)) {
            qWarning("DataflowDialog::displayExperimentState - Failed to construct edge; from_port/to_port is null.");
            continue;
        }

        auto edge_item = new MEDEA::EdgeItem(from_port, to_port);
        connect(from_port, &PortInstanceGraphicsItem::itemMoved, [edge_item]{ edge_item->updateSourcePos(); });
        connect(to_port, &PortInstanceGraphicsItem::itemMoved, [edge_item]{ edge_item->updateDestinationPos(); });
        edge_item->themeChanged(Theme::theme());
        addItemToScene(edge_item);

        auto&& from_port_id = from_port->getGraphmlID();
        edge_items_.insert(from_port_id, edge_item);
        source_id_for_destination_id_.insert(from_port_id, to_port->getGraphmlID());
    }
}


/**
 * @brief DataflowDialog::addItemToScene
 * @param item
 * @throws std::invalid_argument
 */
void DataflowDialog::addItemToScene(QGraphicsItem *item)
{
    if (item) {
        view_->scene()->addItem(item);
    } else {
        throw std::invalid_argument("DataflowDialog::addItemToScene - Trying to add a null item.");
    }
}
