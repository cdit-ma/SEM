#ifndef DATAFLOWDIALOG_H
#define DATAFLOWDIALOG_H

#include <QFrame>
#include <QGraphicsView>
#include <QDoubleSpinBox>
#include <set>

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

    void constructGraphicsItemsForExperimentRun(const QString& exp_name, const MEDEA::ExperimentRunData& exp_run_data);

    void addPortLifecycleEventsToSeries(const QVector<PortLifecycleEvent*>& events);
    void addPortEventsToSeries(const QVector<PortEvent*>& events);

signals:
    void showPulseDockWidget();
    void playbackActivated(bool active);
    void updateLiveStatus(bool on);

public slots:
    void themeChanged();
    void clearScene();

    void playbackSpeedChanged(double multiplier);

private slots:
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
    void playbackEvents(qint64 from_time, qint64 to_time);

    void setExperimentInfo(const QString& exp_name, quint32 exp_run_id = 0);

    void calculateActiveTimes();

    void constructEdgeItems(const QHash<QString, PortInstanceGraphicsItem*>& port_instances, const QList<PortConnectionData*>& port_connections);
    void addItemToScene(QGraphicsItem* item);

    void startPlaybackTimer();
    void stopPlaybackTimer();

    void setPlaybackTimeRange(qint64 start_time, qint64 end_time);
    void resetPlayback();

    QGraphicsView* view_ = nullptr;
    PlaybackControlsWidget playback_controls;
    QDoubleSpinBox speed_multiplier_spinbox_;

    QHash<QString, PortInstanceGraphicsItem*> port_items_;
    QHash<QString, MEDEA::EdgeItem*> edge_items_;
    QHash<QString, QString> source_id_for_destination_id_;

    QHash<QString, MEDEA::EventSeries*> portlifecycle_series_list_;
    QHash<QString, MEDEA::EventSeries*> portevent_series_list_;
    QList<MEDEA::EventSeries*> series_list_;

    QHash<qint64, MEDEA::EventSeries*> active_series_;
    std::set<qint64> active_times_;

    qint64 exp_run_start_time_ = 0;
    qint64 exp_run_end_time_ = 0;

    qint64 playback_duration_ms_ = 0;
    qint64 playback_current_time_ = 0;

    bool live_mode_ = false;
    bool timer_active_ = false;

    int timer_id_ = -1;
    double playback_interval_;
};

#endif // DATAFLOWDIALOG_H
