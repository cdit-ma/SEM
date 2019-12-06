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

const int default_playback_interval_ms = 33;
const int frame_rate_ms = 33;
const int flash_duration_ms = 200;

/**
 * @brief DataflowDialog::DataflowDialog
 * @param parent
 */
DataflowDialog::DataflowDialog(QWidget *parent)
    : QFrame(parent),
      view_(new DataflowGraphicsView(this)),
      playback_controls(this),
      playback_interval_(default_playback_interval_ms)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);
    mainLayout->addWidget(view_);
    mainLayout->addWidget(&playback_controls);

    connect(&playback_controls, &PlaybackControlsWidget::play, this, &DataflowDialog::playback);
    connect(&playback_controls, &PlaybackControlsWidget::pause, this, &DataflowDialog::pausePlayback);
    connect(&playback_controls, &PlaybackControlsWidget::jumpToStart, this, &DataflowDialog::jumpToStart);
    connect(&playback_controls, &PlaybackControlsWidget::jumpToEnd, this, &DataflowDialog::jumpToEnd);
    connect(&playback_controls, &PlaybackControlsWidget::jumpToPreviousActivity, this, &DataflowDialog::jumpToPreviousActivity);
    connect(&playback_controls, &PlaybackControlsWidget::jumpToNextActivity, this, &DataflowDialog::jumpToNextActivity);
    connect(this, &DataflowDialog::playbackActivated, &playback_controls, &PlaybackControlsWidget::setPlayPauseCheckedState);

    connect(Theme::theme(), &Theme::theme_Changed, this, &DataflowDialog::themeChanged);
    themeChanged();
}


/**
 * @brief DataflowDialog::addPortLifecycleEventsToSeries
 * NOTE - This is only used to prototyte the flashing of ports for port lifecycle events
 * @param events
 */
void DataflowDialog::addPortLifecycleEventsToSeries(const QVector<PortLifecycleEvent*>& events)
{
    /**
     * NOTE: This function is only here to test getting events between a time interval from a series
     * The series will already be constructed when using the new experiment data classes
     */

    for (const auto& event : events) {
        if (event != nullptr) {
            MEDEA::EventSeries* series = nullptr;
            auto&& series_id = event->getID();
            // Check if a series for the event already exists
            if (portlifecycle_series_list_.contains(series_id)) {
                series = portlifecycle_series_list_.value(series_id);
            } else {
                // Construct new series
                auto port_id = series_id.split("_").first();
                series = new PortLifecycleEventSeries(series_id);
                portlifecycle_series_list_.insert(series_id, series);
                if (!series_list_.contains(series)) {
                    series_list_.append(series);
                }
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

    for (const auto& event : events) {
        if (event != nullptr) {
            MEDEA::EventSeries* series = nullptr;
            auto&& series_id = event->getID();
            // Check if a series for the event already exists
            if (portevent_series_list_.contains(series_id)) {
                series = portevent_series_list_.value(series_id);
            } else {
                // Construct new series
                auto port_id = series_id.split("_").first();
                series = new PortEventSeries(series_id);
                portevent_series_list_.insert(series_id, series);
                if (!series_list_.contains(series)) {
                    series_list_.append(series);
                }
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
 * @brief DataflowDialog::speedMultiplierChanged
 * @param multiplier
 */
void DataflowDialog::playbackSpeedChanged(double multiplier)
{
    playback_interval_ = default_playback_interval_ms * multiplier;

    // TODO: Once the graphics items deal with rendering their own events at a given timeframe,
    // we won't need the active time hash and we will no longer need to reset the playback here
    calculateActiveTimes();
    resetPlayback();
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

    setPlaybackTimeRange(exp_run_data.start_time(), exp_run_data.end_time());
    playback_controls.setControlsEnabled(true);

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

    stopPlaybackTimer();
    setPlaybackTimeRange(0, 0);
    playback_controls.setControlsEnabled(false);

    view_->scene()->clear();
    port_items_.clear();

    portlifecycle_series_list_.clear();
    portevent_series_list_.clear();
    series_list_.clear();

    active_series_.clear();
    active_times_.clear();
}


/**
 * @brief DataflowDialog::playback
 */
void DataflowDialog::playback()
{
    // If there are no graphics items, stop the current timer and send a signal to the playback controls to update the play/pause button
    if (view_->scene()->items().isEmpty()) {
        resetPlayback();
        return;
    }

    // If the timer is already running, do nothing
    if (timer_active_) {
        return;
    }

    if (playback_current_time_ >= exp_run_end_time_) {
        if (!live_mode_) {
            qDebug() << "\nPLAYBACK STARTED ------------------------------------------------------";
            resetPlayback();
        }
    } else {
        if (playback_current_time_ == exp_run_start_time_) {
            qDebug() << "\nPLAYBACK STARTED ------------------------------------------------------";
        } else {
            qDebug() << "CONTINUE playback";
        }
    }

    startPlaybackTimer();
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

    if (active_times_.size() == 0) {
        return;
    }

    // Get the iterator to the first active time that is greater than or equal to the current playback time
    auto prev_active_time_itr = active_times_.lower_bound(playback_current_time_);
    if (prev_active_time_itr != active_times_.begin()) {
        prev_active_time_itr--;
    }

    auto&& last_active_time = *prev_active_time_itr;
    playback_current_time_ = last_active_time;
    playback_controls.setCurrentTime(last_active_time);
}


/**
 * @brief DataflowDialog::jumpToNextActivity
 */
void DataflowDialog::jumpToNextActivity()
{
    qDebug() << "JUMP to NEXT activity";

    if (active_times_.size() == 0) {
        return;
    }

    // Get the iterator to the first active time that is greater than the current playback time
    auto next_active_time_itr = active_times_.upper_bound(playback_current_time_);

    // If there is no active time after the current playback time, do nothing
    if (next_active_time_itr == active_times_.end()) {
        return;
    }

    auto&& last_active_time = *next_active_time_itr;
    playback_current_time_ = last_active_time;
    playback_controls.setCurrentTime(last_active_time);
}


/**
 * @brief DataflowDialog::jumpToStart
 */
void DataflowDialog::jumpToStart()
{
    qDebug() << "JUMP to START";
    playback_current_time_ = exp_run_start_time_;
    playback_controls.setCurrentTime(playback_current_time_);
}


/**
 * @brief DataflowDialog::jumpToEnd
 */
void DataflowDialog::jumpToEnd()
{
    qDebug() << "JUMP to END";
    playback_current_time_ = exp_run_end_time_;
    playback_controls.setCurrentTime(playback_current_time_);
}


/**
 * @brief DataflowDialog::setExperimentInfo
 * @param exp_name
 * @param exp_run_id
 * @throws std::runtime_error
 */
void DataflowDialog::setExperimentInfo(const QString& exp_name, quint32 exp_run_id)
{
    if (parentWidget() == nullptr) {
        throw std::runtime_error("DataflowDialog::setExperimentInfo - The parent widget is not set.");
    }

    QString title = "Pulse";
    if (!exp_name.isEmpty()) {
        title += " [" + exp_name + " #" + QString::number(exp_run_id) + "]";
    }
    auto parent_dockwidget = qobject_cast<BaseDockWidget*>(parentWidget());
    parent_dockwidget->setTitle(title);
}


/**
 * @brief DataflowDialog::calculateActiveTimes
 */
void DataflowDialog::calculateActiveTimes()
{
    active_times_.clear();
    active_series_.clear();

    auto current_time = exp_run_start_time_;
    while (current_time < exp_run_end_time_) {
        auto to_time = current_time + playback_interval_;
        for (const auto& series : series_list_) {
            const auto& events = series->getEventsBetween(current_time, to_time);
            if (!events.isEmpty()) {
                active_times_.insert(current_time);
                active_series_.insertMulti(current_time, series);
            }
        }
        current_time = to_time;
    }
}


/**
 * @brief DataflowDialog::startPlaybackTimer
 */
void DataflowDialog::startPlaybackTimer()
{
    if (timer_id_ != -1) {
        killTimer(timer_id_);
    }
    timer_id_ = startTimer(frame_rate_ms);
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
 * @brief DataflowDialog::setPlaybackTimeRange
 * @param start_time
 * @param end_time
 */
void DataflowDialog::setPlaybackTimeRange(qint64 start_time, qint64 end_time)
{
    exp_run_start_time_ = start_time;
    exp_run_end_time_ = end_time;
    playback_current_time_ = start_time;
    playback_controls.setTimeRange(start_time, end_time);
}


/**
 * @brief DataflowDialog::resetPlayback
 */
void DataflowDialog::resetPlayback()
{
    stopPlaybackTimer();
    playback_controls.resetTimeProgress();
    playback_current_time_ = exp_run_start_time_;
    emit playbackActivated(false);
}


/**
 * @brief DataflowDialog::playbackEvents
 * @param from_time
 * @param to_time
 */
void DataflowDialog::playbackEvents(qint64 from_time, qint64 to_time)
{
    QHash<QString, qreal> active_edges_event_count;

    // The temporary min_event_count value should be set as close to 'infinity' as possible, so that any future
    // updates are guaranteed to be smaller than the default. Same applies to max_event_count, but in reverse
    int min_event_count = INT_MAX;
    int max_event_count = 0;

    for (const auto& series : active_series_.values(from_time)) {
        auto port = port_items_.value(series->getID(), nullptr);
        if (port) {
            if (series->getKind() == MEDEA::ChartDataKind::PORT_EVENT) {
                const auto& port_id = port->getGraphmlID();
                auto&& port_kind = port->getPortKind();
                if (port_kind == AggServerResponse::Port::PUBLISHER || port_kind == AggServerResponse::Port::REQUESTER || port_kind == AggServerResponse::Port::REPLIER) {
                    auto&& events = series->getEventsBetween(from_time, to_time);
                    int event_count = events.size();
                    min_event_count = qMin(event_count, min_event_count);
                    max_event_count = qMax(event_count, max_event_count);

                    // If the port is a Replier, insert the attached port connection's source graphml id
                    if (port_kind == AggServerResponse::Port::REPLIER) {
                        const auto& from_port_id = source_id_for_destination_id_.value(port_id, "");
                        active_edges_event_count.insert(from_port_id, event_count);
                    } else {
                        active_edges_event_count.insert(port_id, event_count);
                    }
                }
            }
            port->flashPort(flash_duration_ms);
            qDebug() << "Port: " << port->getGraphmlID() << ", " << port->getPortName() << " "
                     << series->getEventsBetween(from_time, playback_current_time_).count() << " events";
        } else {
            qDebug() << "No port item to flash for event";
        }
    }

    // Flash the edges from ports that either sent out, requested or received messages
    if (!active_edges_event_count.isEmpty()) {
        auto&& event_range = (max_event_count > min_event_count) ? max_event_count - min_event_count : 1.0;
        for (const auto& from_port_id : active_edges_event_count.keys()) {
            const auto& from_edge = edge_items_.value(from_port_id, nullptr);
            if (from_edge) {
                auto&& event_count_ratio = (active_edges_event_count.value(from_port_id) - min_event_count) / event_range;
                from_edge->flashEdge(flash_duration_ms, 1 + event_count_ratio);
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
            stopPlaybackTimer(); //  this stops the timer without resetting the playback time
            qDebug() << "PLAYBACK FINISHED ----------------------------------------------------";
        }

    } else {

        playback_controls.incrementCurrentTime(playback_interval_);
        playback_current_time_ += playback_interval_;

        //auto&& elapsed_time = playback_current_time_ - exp_run_start_time_;
        //qDebug() << QDateTime::fromMSecsSinceEpoch(playback_current_time_).toString("hh:mm:ss.zzz") << " - " << elapsed_time / 1000.000 << "S";

        auto&& active_time_itr = active_times_.find(from_time);
        if (active_time_itr != active_times_.end()) {
            qDebug() << "Time range: " << QDateTime::fromMSecsSinceEpoch(from_time).toString("hh:mm:ss.zzz")
                     << " to " << QDateTime::fromMSecsSinceEpoch(playback_current_time_).toString("hh:mm:ss.zzz");
            playbackEvents(from_time, playback_current_time_);
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
