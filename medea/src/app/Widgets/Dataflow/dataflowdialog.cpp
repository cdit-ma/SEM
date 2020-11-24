#include "dataflowdialog.h"
#include "dataflowgraphicsview.h"

#include "../DockWidgets/basedockwidget.h"
#include "../../theme.h"

#include "EntityItems/nodegraphicsitem.h"
#include "GraphicsItems/edgeitem.h"

#include "EntityItems/workerinstancegraphicsitem.h"

#include "../Pulse/Entity/defaultentity.h"
#include "../Pulse/Entity/portinstance.h"
#include "../Pulse/EntityContainer/componentinstance.h"
#include "../Pulse/EntityContainer/defaultentitycontainer.h"
#include "../Pulse/Edge/defaultedge.h"
#include "../Pulse/EdgeAnchor/naturalanchor.h"

#include <QGraphicsRectItem>
#include <QVBoxLayout>
#include <QDockWidget>
#include <QtConcurrent>

const int invalid_experiment_id = -1;

const int default_playback_interval_ms = 33;
const int frame_rate_ms = 33;

/**
 * @brief DataflowDialog::DataflowDialog
 * @param parent
 */
DataflowDialog::DataflowDialog(QWidget *parent)
    : QFrame(parent),
      view_(new DataflowGraphicsView(this)),
      playback_controls_(this),
      playback_interval_(default_playback_interval_ms)
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);
    mainLayout->addWidget(view_);
    mainLayout->addWidget(&playback_controls_);

    connect(&playback_controls_, &PlaybackControlsWidget::play, this, &DataflowDialog::playback);
    connect(&playback_controls_, &PlaybackControlsWidget::pause, this, &DataflowDialog::pausePlayback);
    connect(&playback_controls_, &PlaybackControlsWidget::jumpToStart, this, &DataflowDialog::jumpToStart);
    connect(&playback_controls_, &PlaybackControlsWidget::jumpToEnd, this, &DataflowDialog::jumpToEnd);
    connect(&playback_controls_, &PlaybackControlsWidget::jumpToPreviousActivity, this, &DataflowDialog::jumpToPreviousActivity);
    connect(&playback_controls_, &PlaybackControlsWidget::jumpToNextActivity, this, &DataflowDialog::jumpToNextActivity);
    connect(this, &DataflowDialog::playbackActivated, &playback_controls_, &PlaybackControlsWidget::setPlayPauseCheckedState);

    connect(Theme::theme(), &Theme::theme_Changed, this, &DataflowDialog::themeChanged);
    themeChanged();
}

/**
 * @brief DataflowDialog::themeChanged
 */
void DataflowDialog::themeChanged()
{
    Theme* theme = Theme::theme();
    view_->setStyleSheet("padding: 0px; border: 1px solid " + theme->getDisabledBackgroundColorHex() + ";");
    view_->setBackgroundBrush(theme->getBackgroundColor());
}

/**
 * @brief DataflowDialog::speedMultiplierChanged
 * @param multiplier
 */
void DataflowDialog::playbackSpeedChanged(double multiplier)
{
    playback_interval_ = default_playback_interval_ms * multiplier;
}

/**
 * @brief DataflowDialog::constructGraphicsItemsForExperimentRun
 * @param exp_run_data
 */
void DataflowDialog::constructGraphicsItemsForExperimentRun(const MEDEA::ExperimentRunData& exp_run_data)
{
    // Clear previous states and items
    clear();

    // This sets the experiment info displays on the title bar of the panel
    setExperimentInfo(exp_run_data.experiment_name(), exp_run_data.experiment_run_id());
    setPlaybackTimeRange(exp_run_data.start_time(), exp_run_data.end_time());
    playback_controls_.setControlsEnabled(true);

    // Connect the experiment run's signals
    connect(&exp_run_data, &MEDEA::ExperimentRunData::dataUpdated, this, &DataflowDialog::playbackEndTimeChanged);
    connect(&exp_run_data, &MEDEA::ExperimentRunData::experimentRunFinished, this, &DataflowDialog::turnOffLiveStatus);

    live_mode_ = exp_run_data.end_time() == 0;
    if (live_mode_) {
        // Set the last updated time as the initial playback end time if the experiment is live
        playbackEndTimeChanged(exp_run_data.last_updated_time());
        emit updateLiveStatus(live_mode_);
    }

    for (const auto& node_data : exp_run_data.getNodeData()) {

        auto node_item = new NodeGraphicsItem(*node_data);
        addItemToScene(node_item);

        for (const auto& container_inst_item : node_item->getContainerInstanceItems()) {
            for (const auto& comp_inst_item : container_inst_item->getComponentInstanceItems()) {
                for (const auto& port_inst_item : comp_inst_item->getPortInstanceItems()) {
                    const auto& port_id = port_inst_item->getGraphmlID();
                    port_items_.insert(port_id, port_inst_item);
                }
            }
        }
    }

    // Construct the edges
    constructEdgeItems(port_items_, exp_run_data.getPortConnectionData());
}

/**
 * @brief DataflowDialog::constructPulseViewItemsForExperimentRun
 * @param exp_run_data
 */
void DataflowDialog::constructPulseViewItemsForExperimentRun(const MEDEA::ExperimentRunData& exp_run_data)
{
    // Clear previous states and items
    clear();

    using namespace Pulse::View;

    QHash<QString, PortInstance*> port_instances;

    int node_count = 0;
    for (const auto& node : exp_run_data.getNodeData()) {
        auto node_item = new DefaultEntityContainer(node->getHostname(), "EntityIcons", "HardwareNode",
                                                    node->getIP(), "Icons", "ethernet");
        for (const auto& container : node->getContainerInstanceData()) {
            auto docker_type = "Generic OS Process";
            auto icon_name = "servers";
            if (container->getType() == AggServerResponse::Container::ContainerType::DOCKER) {
                docker_type = "Docker Process";
                icon_name = "docker";
            }
            auto container_item = new DefaultEntityContainer(container->getName(), "Icons", icon_name,
                                                             docker_type, "Icons", "terminal", node_item);
            for (const auto& comp_inst : container->getComponentInstanceData()) {
                auto comp_inst_item = new ComponentInstance(comp_inst->getName(), comp_inst->getType(), container_item);
                for (const auto& port_inst : comp_inst->getPortInstanceData()) {
                    auto port = new PortInstance(port_inst->getName(), port_inst->getKind());
                    port_instances.insert(port_inst->getGraphmlID(), port);
                    comp_inst_item->add(port);
                }
                for (const auto& worker_inst : comp_inst->getWorkerInstanceData()) {
                    auto worker = new DefaultEntity(worker_inst->getName(), "Icons", "spanner",
                                                    worker_inst->getType(), "Icons", "code");
                    auto icon_size = Defaults::primary_icon_size * 0.75;
                    worker->setPrimaryIconSize(icon_size.width(), icon_size.height());
                    comp_inst_item->add(worker);
                }
                container_item->add(comp_inst_item);
            }
            node_item->add(container_item);
        }
        auto stack_gap = Defaults::primary_icon_size.height() * node_count++;
        node_item->setPos(node_item->pos() + QPointF(stack_gap, stack_gap));
        addItemToScene(node_item);
    }


    const auto& port_connections = exp_run_data.getPortConnectionData();
    for (const auto& p_c : port_connections) {
        auto src = port_instances.value(p_c->getFromPortID(), nullptr);
        auto dst = port_instances.value(p_c->getToPortID(), nullptr);
        if (src && dst) {
            auto edge = new DefaultEdge(src->getOutputAnchor(), dst->getInputAnchor());
            addItemToScene(edge);
        }
    }
}

/**
 * @brief DataflowDialog::clear
 * This resets all previous experiment and playback states and deletes all existing graphics items
 */
void DataflowDialog::clear()
{
    setExperimentInfo("");

    turnOffLiveStatus();
    stopPlaybackTimer();
    setPlaybackTimeRange(0, 0);
    playback_controls_.setControlsEnabled(false);

    // Replace the scene with a new one instead of just clearing it to centralise on the items by default
    view_->scene()->deleteLater();
    view_->setScene(new QGraphicsScene);

    port_items_.clear();
}

/**
 * @brief DataflowDialog::playback
 */
void DataflowDialog::playback()
{
    // If there are no graphics items, stop the current timer and send a signal to the playback controls to update the play/pause button
    if (view_->scene()->items().isEmpty()) {
        resetPlayback();
        emit playbackActivated(false);
        return;
    }
    // If the timer is already running, do nothing
    if (timer_active_) {
        return;
    }
    if (playback_current_time_ >= exp_run_end_time_) {
        if (!live_mode_) {
            resetPlayback();
        }
    }
    startPlaybackTimer();
}

/**
 * @brief DataflowDialog::pausePlayback
 */
void DataflowDialog::pausePlayback()
{
    stopPlaybackTimer();
}

/**
 * @brief DataflowDialog::jumpToPreviousActivity
 */
void DataflowDialog::jumpToPreviousActivity()
{
    qint64 prev_time = playback_current_time_;
    for (const auto& port : port_items_) {
        const auto& port_prev_time = port->getPreviousEventTime(playback_current_time_);
        prev_time = qMin(port_prev_time, prev_time);
    }
    if (prev_time < playback_current_time_) {
        playback_current_time_ = prev_time;
        playback_controls_.setCurrentTime(prev_time);
    }
}

/**
 * @brief DataflowDialog::jumpToNextActivity
 */
void DataflowDialog::jumpToNextActivity()
{
    qint64 next_time = playback_current_time_;
    for (const auto& port : port_items_) {
        const auto& port_next_time = port->getNextEventTime(playback_current_time_);
        next_time = qMax(port_next_time, next_time);
    }
    if (next_time > playback_current_time_) {
        playback_current_time_ = next_time;
        playback_controls_.setCurrentTime(next_time);
    }
}

/**
 * @brief DataflowDialog::jumpToStart
 */
void DataflowDialog::jumpToStart()
{
    playback_current_time_ = exp_run_start_time_;
    playback_controls_.setCurrentTime(playback_current_time_);
}

/**
 * @brief DataflowDialog::jumpToEnd
 */
void DataflowDialog::jumpToEnd()
{
    playback_current_time_ = exp_run_end_time_;
    playback_controls_.setCurrentTime(playback_current_time_);
}

/**
 * @brief DataflowDialog::turnOffLiveStatus
 * This is triggered when the live experiment being visualised has finished
 * It sends a signal to remove the live indicator from Pulse's titlebar
 */
void DataflowDialog::turnOffLiveStatus()
{
    live_mode_ = false;
    emit updateLiveStatus(false);
}

/**
 * @brief DataflowDialog::playbackEndTimeChanged
 * @param exp_run_last_updated_time
 */
void DataflowDialog::playbackEndTimeChanged(qint64 exp_run_last_updated_time)
{
    exp_run_end_time_ = exp_run_last_updated_time;
    playback_controls_.updateEndTime(exp_run_last_updated_time);
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
    playback_controls_.setTimeRange(start_time, end_time);
}

/**
 * @brief DataflowDialog::resetPlayback
 */
void DataflowDialog::resetPlayback()
{
    stopPlaybackTimer();
    playback_controls_.resetTimeProgress();
    playback_current_time_ = exp_run_start_time_;
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
        }

    } else {

        playback_controls_.incrementCurrentTime(playback_interval_);
        playback_current_time_ += playback_interval_;

        for (const auto& port_item : port_items_) {
            port_item->playEvents(from_time, playback_current_time_);
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
            qWarning("DataflowDialog::constructEdgeItems - Failed to construct edge; from_port/to_port is null.");
            continue;
        }
        auto edge_item = new MEDEA::EdgeItem(from_port, to_port);
        addItemToScene(edge_item);
    }
}

/**
 * @brief DataflowDialog::addItemToScene
 * @param item
 * @throws std::invalid_argument
 */
void DataflowDialog::addItemToScene(QGraphicsItem *item)
{
    if (item == nullptr) {
        throw std::invalid_argument("DataflowDialog::addItemToScene - Trying to add a null item.");
    }
    view_->scene()->addItem(item);
}