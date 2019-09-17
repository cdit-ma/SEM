#ifndef DATAFLOWDIALOG_H
#define DATAFLOWDIALOG_H

#include <QFrame>
#include <QGraphicsView>

#include "../Charts/Data/Events/protomessagestructs.h"
#include "EntityItems/portinstancegraphicsitem.h"

class DataflowDialog : public QFrame
{
    Q_OBJECT

public:
    explicit DataflowDialog(QWidget* parent = nullptr);

    void setExperimentInfo(const QString& exp_name, quint32 exp_run_id);

signals:
    void showDataflowDockWidget();

public slots:
    void themeChanged();

    void displayExperimentState(const AggServerResponse::ExperimentState& exp_state);
    void constructEdgeItems(const QHash<QString, PortInstanceGraphicsItem*>& port_instances, const QVector<AggServerResponse::PortConnection>& port_connections);
    void clear();

private:
    void addItemToScene(QGraphicsItem* item);

    QGraphicsView* view_ = nullptr;

};

#endif // DATAFLOWDIALOG_H
