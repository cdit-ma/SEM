#ifndef CHARTINPUTPOPUP_H
#define CHARTINPUTPOPUP_H

#include "hoverpopup.h"
#include "../Data/Events/protoMessageStructs.h"

#include <QGroupBox>
#include <QLineEdit>
#include <QToolBar>
#include <QAction>

class ChartInputPopup : public HoverPopup
{
    Q_OBJECT

public:
    explicit ChartInputPopup(QWidget* parent = 0);

signals:
    void requestExperimentRuns(QString experimentName = "");
    void requestExperimentState(quint32 experimentRunID);
    void requestEvents(quint32 experimentRunID, QString componentName = "");

public slots:
    void themeChanged();

    void setPopupVisible(bool visible);
    void populateExperimentRuns(QList<ExperimentRun> runs);

    void accept();
    void reject();

private:
    QLineEdit* experimentNameLineEdit_;
    QGroupBox* experimentNameGroupBox_;
    QGroupBox* experimentRunsGroupBox_;
    QToolBar* toolbar_;
    QAction* okAction_;
    QAction* cancelAction_;

    QPointF originalCenterPos_;
};

#endif // CHARTINPUTPOPUP_H
