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

/**
 * @brief DataflowDialog::DataflowDialog
 * @param parent
 */
DataflowDialog::DataflowDialog(QWidget *parent) : QFrame(parent)
{
    view_ = new DataflowGraphicsView(this);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->addWidget(view_);

    connect(Theme::theme(), &Theme::theme_Changed, this, &DataflowDialog::themeChanged);
    themeChanged();
}


/**
 * @brief DataflowDialog::storePortLifecycleEvents
 * @param events
 */
void DataflowDialog::storePortLifecycleEvents(const QVector<PortLifecycleEvent*>& events)
{
    if (events.isEmpty()) {
        return;
    }

    // Clear the hash used for playback
    port_ids_at_elapsed_time_.clear();
    port_lifecycle_events_ = events;

    // There should be a valid selected experiment run at this point
    if (experiment_run_.experiment_run_id == invalid_experiment_id) {
        return;
    }

    auto start_time = experiment_run_.start_time;
    auto end_time = experiment_run_.last_updated_time;
    playback_duration_ms_ = static_cast<qint64>(end_time - start_time);

    qDebug() << "\nExperiment Run Duration:\t" << playback_duration_ms_ / 1000.0 << "S";
    qDebug() << "Experiment Start Time:\t" << QDateTime::fromMSecsSinceEpoch(start_time).toString("hh:mm:ss.zzz");
    qDebug() << "Experiment End Time:\t" << QDateTime::fromMSecsSinceEpoch(end_time).toString("hh:mm:ss.zzz");

    // Initialise the hash keys with the elapsed times (ms)
    for (qint64 i = 0; i <= playback_duration_ms_; i++) {
        port_ids_at_elapsed_time_.insert(i, QString());
    }

    for (int i = 0; i < port_lifecycle_events_.length(); i++) {
        const auto& event = port_lifecycle_events_[i];
        auto elapsed_time = event->getTimeMS() - start_time;
        if (port_ids_at_elapsed_time_.contains(elapsed_time)) {
            port_ids_at_elapsed_time_.insertMulti(elapsed_time, event->getID());
        } else {
            qWarning("DataflowDialog::storePortLifecycleEvents - Event time lies outside of the calculated experiment duration.");
            qDebug() << "Event time: " + QDateTime::fromMSecsSinceEpoch(event->getTimeMS()).toString("hh:mm:ss.zzz");
        }
    }
}


/**
 * @brief DataflowDialog::playbackDataflow
 */
void DataflowDialog::playbackDataflow()
{
    if (port_lifecycle_events_.isEmpty()) {
        qInfo("DataflowDialog::playbackDataflow - There are no events to playback.");
        return;
    }
    if (!timer_active_) {
        qDebug() << "\nPLAYBACK STARTED ------------------------------------------------------";
        auto tick_interval_ms = 1;
        timer_id_ = startTimer(tick_interval_ms);
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

    exp_run_start_time_ = exp_run_data.start_time();
    exp_run_end_time_ = exp_run_data.end_time();
    setExperimentInfo(exp_name, exp_run_data.experiment_run_id());

    // Clear previous items
    clearScene();

    for (const auto& node_data : exp_run_data.getNodeData()) {
        for (const auto& container_data : node_data.getContainerInstanceData()) {
            for (const auto& comp_inst_data : container_data.getComponentInstanceData()) {

                auto c_instItem = new ComponentInstanceGraphicsItem(comp_inst_data);
                addItemToScene(c_instItem);

                for (const auto& port_data : comp_inst_data.getPortInstanceData()) {
                    auto p_instItem = new PortInstanceGraphicsItem(port_data);
                    port_items_.insert(port_data.getGraphmlID(), p_instItem);
                    c_instItem->addPortInstanceItem(p_instItem);
                    connect(c_instItem, &ComponentInstanceGraphicsItem::itemMoved, p_instItem, &PortInstanceGraphicsItem::itemMoved);
                }
            }
        }
    }

    // Construct the edges
    constructEdgeItems(port_items_, exp_run_data.getPortConnectionData());
}


/**
 * @brief DataflowDialog::constructGraphicsItems
 * @param exp_run
 * @param exp_state
 */
void DataflowDialog::constructGraphicsItems(const AggServerResponse::ExperimentRun& exp_run, const AggServerResponse::ExperimentState& exp_state)
{
    auto exp_id = exp_run.experiment_run_id;
    if (exp_id == invalid_experiment_id) {
        return;
    }

    // This shouldn't be needed, but just in case
    /*if (exp_id != exp_state.experiment_run_id) {
        return;
    }*/

    experiment_run_ = exp_run;
    experiment_run_.last_updated_time = exp_state.last_updated_time;
    setExperimentInfo(exp_run.experiment_name, exp_id);

    // Clear previous items
    clearScene();

    qDebug() << "-----------------------------------------------------------------------";
    //qDebug() << "nodes#: " << expState.nodes.size();

    for (const auto& n : exp_state.nodes) {
        //qDebug() << "containers#: " << n.containers.size();
        for (const auto& c : n.containers) {
            //qDebug() << "comp inst#: " << c.component_instances.size();
            for (const auto& inst : c.component_instances) {

                qDebug() << "Component Instance: " << inst.name << " - " << inst.path;
                auto c_instItem = new ComponentInstanceGraphicsItem(inst);
                addItemToScene(c_instItem);

                //qDebug() << "Comp Inst scene rect: " << c_instItem->sceneBoundingRect();

                for (const auto& port : inst.ports) {
                    qDebug() << "Port: " << port.name << " - " << port.path;
                    auto p_instItem = new PortInstanceGraphicsItem(port);
                    port_items_[port.graphml_id] = p_instItem;
                    c_instItem->addPortInstanceItem(p_instItem);
                    connect(c_instItem, &ComponentInstanceGraphicsItem::itemMoved, p_instItem, &PortInstanceGraphicsItem::itemMoved);

                    /*
                    // TODO - Figure out why the sceneBoundingRect is empty
                    qDebug() << "Port scene rect: " << p_instItem->sceneBoundingRect();
                    qDebug() << "Port mapped rect: " << view_->mapToScene(p_instItem->boundingRect().toRect());
                    qDebug() << "Port geometry: " << p_instItem->geometry();
                    */
                }
            }
        }
    }

    // Construct the edges
    constructEdgeItems(port_items_, exp_state.port_connections);
    qDebug() << "-----------------------------------------------------------------------";
}


/**
 * @brief DataflowDialog::constructEdgeItems
 * @param port_instances
 * @param port_connections
 */
void DataflowDialog::constructEdgeItems(const QHash<QString, PortInstanceGraphicsItem*>& port_instances, const QVector<AggServerResponse::PortConnection>& port_connections)
{
    for (const auto& p_c : port_connections) {
        auto from_port = port_instances.value(p_c.from_port_graphml, nullptr);
        auto to_port = port_instances.value(p_c.to_port_graphml, nullptr);
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
 * @brief DataflowDialog::clearScene
 * This clears all the graphics items in the scene
 */
void DataflowDialog::clearScene()
{
    view_->scene()->clear();
    port_items_.clear();
}


/**
 * @brief DataflowDialog::setExperimentInfo
 * @param exp_name
 * @param exp_run_id
 */
void DataflowDialog::setExperimentInfo(const QString &exp_name, quint32 exp_run_id)
{
    if (parentWidget()) {
        auto parent_dockwidget = qobject_cast<BaseDockWidget*>(parentWidget());
        parent_dockwidget->setTitle("Pulse [" + exp_name + " #" + QString::number(exp_run_id) + "]");
    }
}


/**
 * @brief DataflowDialog::clearPlaybackState
 */
void DataflowDialog::clearPlaybackState()
{
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

    if (playback_elapsed_time_ > playback_duration_ms_) {
        killTimer(timer_id_);
        clearPlaybackState();
        qDebug() << "PLAYBACK FINISHED ----------------------------------------------------";
        return;
    }

    if (playback_elapsed_time_ % 1000 == 0) {
        qDebug() << "-" << playback_elapsed_time_ / 1000 << "S";
    }

    if (port_ids_at_elapsed_time_.contains(playback_elapsed_time_)) {
        for (const auto& id : port_ids_at_elapsed_time_.values(playback_elapsed_time_)) {
            auto port = port_items_.value(id, nullptr);
            if (port) {
                qDebug() << "------" << playback_elapsed_time_ / 1000.0 << "S - " << port->getPortName();
                port->flashPort();
            }
        }
    }

    playback_elapsed_time_++;
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
void DataflowDialog::constructEdgeItems(const QHash<QString, PortInstanceGraphicsItem *> &port_instances, const QList<PortConnectionData> &port_connections)
{
    for (const auto& p_c : port_connections) {
        auto from_port = port_instances.value(p_c.getFromPortID(), nullptr);
        auto to_port = port_instances.value(p_c.getToPortID(), nullptr);
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
