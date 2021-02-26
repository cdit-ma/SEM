#ifndef DATAFLOWDIALOG_H
#define DATAFLOWDIALOG_H

#include <QFrame>
#include <QGraphicsView>

#include "../Charts/Data/protomessagestructs.h"
#include "../Charts/Data/experimentrundata.h"
#include "EntityItems/componentinstancegraphicsitem.h"
#include "EntityItems/portinstancegraphicsitem.h"
#include "PlaybackWidget/playbackcontrolswidget.h"

#include "../Pulse/Entity/defaultentity.h"
#include "../Pulse/Entity/portinstance.h"
#include "../Pulse/EntityContainer/componentinstance.h"
#include "../Pulse/EntityContainer/defaultentitycontainer.h"
#include "../Pulse/Edge/defaultedge.h"

class DataflowDialog : public QFrame
{
    Q_OBJECT

public:
    explicit DataflowDialog(QWidget* parent = nullptr);

    void constructGraphicsItemsForExperimentRun(const MEDEA::ExperimentRunData& exp_run_data);
    void constructPulseViewItemsForExperimentRun(const MEDEA::ExperimentRunData& exp_run_data);

signals:
    void playbackActivated(bool active);
    void updateLiveStatus(bool on);

public slots:
    void playbackSpeedChanged(double multiplier);
    void clear();

private slots:
    // Playback Control Slots
    void playback();
    void pausePlayback();
    void jumpToPreviousActivity();
    void jumpToNextActivity();
    void jumpToStart();
    void jumpToEnd();

    void turnOffLiveStatus();

    void playbackEndTimeChanged(qint64 exp_run_last_updated_time);
    void themeChanged();

protected:
    void timerEvent(QTimerEvent* event) override;

private:
    void setExperimentInfo(const QString& exp_name, quint32 exp_run_id = 0);

    static void checkNotNull(QObject* data_obj, const QString& data_name);
    static Pulse::View::PortInstance& constructPortInstanceItem(PortInstanceData& port_inst);
    static Pulse::View::DefaultEntity& constructWorkerInstanceItem(WorkerInstanceData& worker_inst);

    void constructPortConnections(const QList<PortConnectionData*>& connections);
    Pulse::View::DefaultEntityContainer& constructNodeItem(NodeData& node);
    Pulse::View::DefaultEntityContainer& constructContainerInstanceItem(ContainerInstanceData& container, Pulse::View::DefaultEntityContainer* parent);
    Pulse::View::ComponentInstance& constructComponentInstanceItem(ComponentInstanceData& comp_inst, Pulse::View::DefaultEntityContainer* parent);

    void constructEdgeItems(const QHash<QString, PortInstanceGraphicsItem*>& port_instances, const QList<PortConnectionData*>& port_connections);
    void addItemToScene(QGraphicsItem* item);

    void startPlaybackTimer();
    void stopPlaybackTimer();

    void setPlaybackTimeRange(qint64 start_time, qint64 end_time);
    void resetPlayback();

    QGraphicsView* view_ = nullptr;
    PlaybackControlsWidget playback_controls_;

    QHash<QString, PortInstanceGraphicsItem*> port_items_;
    QHash<QString, Pulse::View::PortInstance*> port_instance_cache_;

    qint64 exp_run_start_time_ = 0;
    qint64 exp_run_end_time_ = 0;
    qint64 playback_current_time_ = 0;

    bool live_mode_ = false;
    bool timer_active_ = false;

    int timer_id_ = -1;
    double playback_interval_;
};

#endif // DATAFLOWDIALOG_H