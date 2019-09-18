#ifndef DATAFLOWDIALOG_H
#define DATAFLOWDIALOG_H

#include <QFrame>
#include <QGraphicsView>

#include "../../Widgets/Charts/Data/Events/portlifecycleevent.h"
#include "../Charts/Data/Events/protomessagestructs.h"
#include "EntityItems/portinstancegraphicsitem.h"

class DataflowDialog : public QFrame
{
    Q_OBJECT

public:
    explicit DataflowDialog(QWidget* parent = nullptr);

    void storePortLifecycleEvents(const QVector<PortLifecycleEvent *>& events);

    void playbackDataflow();

signals:
    void showDataflowDockWidget();

public slots:
    void themeChanged();

    void constructGraphicsItems(const AggServerResponse::ExperimentRun& exp_run, const AggServerResponse::ExperimentState& exp_state);
    void clearScene();

protected:
    void timerEvent(QTimerEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);

private:
    void constructEdgeItems(const QHash<QString, PortInstanceGraphicsItem*>& port_instances, const QVector<AggServerResponse::PortConnection>& port_connections);
    void addItemToScene(QGraphicsItem* item);

    void setExperimentInfo(const QString& exp_name, qint32 exp_run_id);

    void clearPlaybackState();

    QGraphicsView* view_ = nullptr;

    AggServerResponse::ExperimentRun experiment_run_;

    QVector<PortLifecycleEvent*> port_lifecycle_events_;
    QHash<QString, PortInstanceGraphicsItem*> port_items_;
    QMultiHash<qint64, QString> port_ids_at_elapsed_time_;

    qint64 playback_duration_ms_ = 0;
    qint64 playback_elapsed_time_ = 0;
    bool timer_active_ = false;
    int timer_id_ = 0;
};

#endif // DATAFLOWDIALOG_H
