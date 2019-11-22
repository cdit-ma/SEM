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
 * @brief DataflowDialog::speedMultiplierChanged
 * @param multiplier
 */
void DataflowDialog::playbackSpeedChanged(double multiplier)
{
    playback_interval_ = default_playback_interval_ms * multiplier;
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
}


/**
 * @brief DataflowDialog::playback
 */
void DataflowDialog::playback()
{
    // If there are no graphics items, send a signal to the playback controls to update reset the play/pause button
    if (view_->scene()->items().isEmpty()) {
        emit playbackActivated(false);
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
    qDebug() << "JUMP to PREVIOUS activity"; // from " << QDateTime::fromMSecsSinceEpoch(playback_current_time_).toString("hh:mm:ss.zzz");

    qint64 prev_time = playback_current_time_;
    for (const auto& port : port_items_) {
        const auto& port_prev_time = port->getPreviousEventTime(playback_current_time_);
        prev_time = qMin(port_prev_time, prev_time);
    }
    if (prev_time < playback_current_time_) {
        playback_current_time_ = prev_time;
        playback_controls.setCurrentTime(prev_time);
    }
}


/**
 * @brief DataflowDialog::jumpToNextActivity
 */
void DataflowDialog::jumpToNextActivity()
{
    qDebug() << "JUMP to NEXT activity"; // from " << QDateTime::fromMSecsSinceEpoch(playback_current_time_).toString("hh:mm:ss.zzz");

    qint64 next_time = playback_current_time_;
    for (const auto& port : port_items_) {
        const auto& port_next_time = port->getNextEventTime(playback_current_time_);
        next_time = qMax(port_next_time, next_time);
    }
    if (next_time > playback_current_time_) {
        playback_current_time_ = next_time;
        playback_controls.setCurrentTime(next_time);
    }
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
            qDebug() << "PLAYBACK FINISHED -----------------------------------------------------";
        }

    } else {

        playback_controls.incrementCurrentTime(playback_interval_);
        playback_current_time_ += playback_interval_;

        //auto&& elapsed_time = playback_current_time_ - exp_run_start_time_;
        //qDebug() << QDateTime::fromMSecsSinceEpoch(playback_current_time_).toString("hh:mm:ss.zzz") << " - " << elapsed_time / 1000.000 << "S";

        /*qDebug() << "---";
        qDebug() << "FROM: " << QDateTime::fromMSecsSinceEpoch(from_time).toString("hh:mm:ss.zzz")
                 << " TO " << QDateTime::fromMSecsSinceEpoch(playback_current_time_).toString("hh:mm:ss.zzz");*/

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
            qWarning("DataflowDialog::displayExperimentState - Failed to construct edge; from_port/to_port is null.");
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
    if (item) {
        view_->scene()->addItem(item);
    } else {
        throw std::invalid_argument("DataflowDialog::addItemToScene - Trying to add a null item.");
    }
}
