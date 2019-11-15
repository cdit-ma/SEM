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
    : QFrame(parent)
{
    view_ = new DataflowGraphicsView(this);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->addWidget(view_);

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
     * NOTE -  This function is only here to test getting events between a time interval from a series
     * The series will already be constructed when using the new experiment data classes
     */

    series_list_.clear();

    for (const auto& event : events) {
        if (event != nullptr) {
            PortLifecycleEventSeries* series = nullptr;
            auto series_id = event->getID();
            // Check if a series for the event already exists
            if (series_list_.contains(series_id)) {
                series = series_list_.value(series_id);
            } else {
                // Construct new series
                auto port_id = series_id.split("_").first();
                auto series_label = event->getName() + "_" + port_id;
                series = new PortLifecycleEventSeries(series_id);
                series->setProperty("series_label", series_label);
                series_list_[series_id] = series;
            }
            if (series) {
                series->addEvent(event);
            }
        }
    }
}


/**
 * @brief DataflowDialog::playbackDataflow
 */
void DataflowDialog::playbackDataflow()
{
    if (series_list_.isEmpty()) {
        qInfo("DataflowDialog::playbackDataflow - There are no events to playback.");
        return;
    }
    if (!timer_active_) {
        qDebug() << "\nPLAYBACK STARTED ------------------------------------------------------";
        playback_current_time_ = exp_run_start_time_;
        timer_id_ = startTimer(timer_interval_ms);
        timer_active_ = true;
    }
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
    series_list_.clear();
    port_items_.clear();
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
 * @brief DataflowDialog::clearPlaybackState
 */
void DataflowDialog::clearPlaybackState()
{
    playback_current_time_ = exp_run_start_time_;
    playback_elapsed_time_ = 0;

    timer_active_ = false;
    timer_id_ = 0;
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

    /**
     * NOTE: The difference between flashing per event instead of per series is that,
     * per event will flash the port for each of the event that was received; while as
     * per series will only flash the series once regardless of the number of events
     * received berween the given time interval
     */

    auto prev_time = playback_current_time_;
    playback_current_time_ += timer_interval_ms;

    playback_elapsed_time_ += timer_interval_ms;
    qDebug() << QDateTime::fromMSecsSinceEpoch(playback_current_time_).toString("hh:mm:ss.zzz") << " - " << playback_elapsed_time_ / 1000.000 << "S";

    for (const auto& s : series_list_) {
        const auto& events = s->getEventsBetween(prev_time, playback_current_time_);
        if (!events.isEmpty()) {
            qDebug() << "------------------------------------------------";
            qDebug() << "Time range: " << QDateTime::fromMSecsSinceEpoch(prev_time).toString("hh:mm:ss.zzz") << " to " << QDateTime::fromMSecsSinceEpoch(playback_current_time_).toString("hh:mm:ss.zzz");
            qDebug() << "events#: " << events.size();
            auto port = port_items_.value(s->getID(), nullptr);
            if (port) {
                qDebug() << "Port: " << port->getPortName();
                port->flashPort();
                for (const auto& e : events) {
                    qDebug() << "- event time: " << e->getDateTimeString("hh:mm:ss.zzz");
                }
            } else {
                qDebug() << "No port item to flash for event";
            }
            qDebug() << "------------------------------------------------";
        }
    }

    if (playback_current_time_ >= exp_run_end_time_) {
        killTimer(timer_id_);
        clearPlaybackState();
        qDebug() << "PLAYBACK FINISHED ----------------------------------------------------";
    }

    QFrame::timerEvent(event);
}


/**
 * @brief DataflowDialog::mouseDoubleClickEvent
 * @param event
 */
void DataflowDialog::mouseDoubleClickEvent(QMouseEvent *event)
{
    playbackDataflow();
    QFrame::mouseDoubleClickEvent(event);
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
        addItemToScene(edge_item);
        //qDebug() << "Edge scene rect: " << edge_item->sceneBoundingRect();
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
