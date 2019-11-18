#ifndef DATAFLOWDIALOG_H
#define DATAFLOWDIALOG_H

#include <QFrame>
#include <QGraphicsView>

#include "../../Widgets/Charts/Data/Events/portlifecycleevent.h"
#include "../../Widgets/Charts/Data/Series/portlifecycleeventseries.h"
#include "../../Widgets/Charts/Data/Events/portevent.h"
#include "../../Widgets/Charts/Data/Series/porteventseries.h"
#include "../Charts/Data/protomessagestructs.h"
#include "../Charts/Data/experimentrundata.h"
#include "EntityItems/portinstancegraphicsitem.h"
#include "GraphicsItems/edgeitem.h"

#include "PlaybackWidget/playbackcontrolswidget.h"

class DataflowDialog : public QFrame
{
    Q_OBJECT

public:
    explicit DataflowDialog(QWidget* parent = nullptr);

    void addPortLifecycleEventsToSeries(const QVector<PortLifecycleEvent*>& events);
    void addPortEventsToSeries(const QVector<PortEvent*>& events);

signals:
    void showDataflowDockWidget();
    void playbackActivated(bool active);

public slots:
    void themeChanged();

    void constructGraphicsItemsForExperimentRun(const QString& exp_name, const MEDEA::ExperimentRunData& exp_run_data);
    void clearScene();

    // Playback Control Slots
    void playback();
    void pausePlayback();
    void jumpToPreviousActivity();
    void jumpToNextActivity();
    void jumpToStart();
    void jumpToEnd();

protected:
    void timerEvent(QTimerEvent* event);

private:
    void constructEdgeItems(const QHash<QString, PortInstanceGraphicsItem*>& port_instances, const QList<PortConnectionData*>& port_connections);

    void addItemToScene(QGraphicsItem* item);

    void setExperimentInfo(const QString& exp_name, quint32 exp_run_id = 0);

    void calculateActiveTimes();

    void startPlaybackTimer();
    void stopPlaybackTimer();

    void playbackEvents(qint64 from_time, qint64 to_time);
    void resetPlayback();

    QGraphicsView* view_ = nullptr;
    PlaybackControlsWidget playback_controls;

    qint64 exp_run_start_time_;
    qint64 exp_run_end_time_;

    QHash<QString, PortInstanceGraphicsItem*> port_items_;
    QHash<QString, MEDEA::EdgeItem*> edge_items_;
    QHash<QString, QString> source_id_for_destination_id_;

    QHash<QString, PortLifecycleEventSeries*> portlifecycle_series_list_;
    QHash<QString, PortEventSeries*> portevent_series_list_;

    QHash<qint64, MEDEA::EventSeries*> active_series_;
    QList<qint64> active_times_;
    int last_active_time_index_ = -1;

    qint64 playback_duration_ms_ = 0;
    qint64 playback_current_time_ = 0;
    qint64 playback_elapsed_time_ = 0;

    bool live_mode_ = false;
    bool timer_active_ = false;
    int timer_id_ = -1;
};

#endif // DATAFLOWDIALOG_H
