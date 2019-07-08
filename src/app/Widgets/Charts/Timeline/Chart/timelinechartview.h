#ifndef TIMELINECHARTVIEW_H
#define TIMELINECHARTVIEW_H

#include "../../../../Controllers/ViewController/viewitem.h"
#include "../../Timeline/Chart/hoverpopup.h"
#include "../Axis/chartlabellist.h"
#include "../Axis/chartlabel.h"
#include "chart.h"

#include "../../Data/Events/portlifecycleevent.h"
#include "../../Data/Events/workloadevent.h"
#include "../../Data/Events/cpuutilisationevent.h"
#include "../../Data/Events/memoryutilisationevent.h"
#include "../../Data/Events/markerevent.h"

#include <QWidget>
#include <QToolBar>
#include <QPushButton>
#include <QScrollArea>
#include <QMultiHash>

enum class VALUE_TYPE{DOUBLE, DATE_TIME};
enum class TIME_DISPLAY_FORMAT{VALUE, DATE_TIME, ELAPSED_TIME};

class AxisWidget;
class ChartList;

class TimelineChartView : public QWidget
{
    Q_OBJECT

public:
    explicit TimelineChartView(QWidget* parent = 0);

    bool eventFilter(QObject *watched, QEvent* event);

    void addPortLifecycleEvents(const AggServerResponse::ExperimentRun& experimentRun, const QVector<PortLifecycleEvent *>& events);
    void addWorkloadEvents(const AggServerResponse::ExperimentRun& experimentRun, const QVector<WorkloadEvent *>& events);
    void addCPUUtilisationEvents(const AggServerResponse::ExperimentRun& experimentRun, const QVector<CPUUtilisationEvent *>& events);
    void addMemoryUtilisationEvents(const AggServerResponse::ExperimentRun& experimentRun, const QVector<MemoryUtilisationEvent *>& events);
    void addMarkerEvents(const AggServerResponse::ExperimentRun& experimentRun, const QVector<MarkerEvent *>& events);

    void updateExperimentRunLastUpdatedTime(const quint32 experimentRunID, const qint64 time);

    void setTimeDisplayFormat(const TIME_DISPLAY_FORMAT format);

    void clearChartList();
    void updateChartList();

signals:
    void seriesLegendHovered(MEDEA::ChartDataKind kind);

public slots:
    void themeChanged();

    void toggledSeriesLegend(bool checked);

    void chartLabelListSizeChanged(QSizeF size);
    void chartHovered(MEDEA::Chart* chart, bool hovered);
    void chartClosed();

    void updateHoverDisplay();

private slots:
    void minSliderMoved(const double ratio);
    void maxSliderMoved(const double ratio);

    void timelineZoomed(const int delta);
    void timelinePanned(const double dx, const double dy);
    void timelineRubberbandUsed(double left, double right);
    
private:
    void addedChartEvents(const MEDEA::ChartDataKind kind, const AggServerResponse::ExperimentRun& experimentRun);

    MEDEA::EventSeries* constructSeriesForEventKind(const AggServerResponse::ExperimentRun &experimentRun, const MEDEA::ChartDataKind kind, const QString& ID, const QString& label);
    MEDEA::Chart* constructChartForSeries(MEDEA::EventSeries *series, const QString& ID, const QString& label);
    void removeChart(const QString& ID, bool clearing = false);

    void updateRangeForExperimentRun(const quint32 experimentRunID, const qint64 startTime, const qint64 lastUpdatedTime);
    void removedDataFromExperimentRun(const quint32 experimentRunID);
    void updateTimelineRange(bool updateDisplayRange = true);

    const QString& getDateTimeDisplayFormat(const MEDEA::ChartDataKind& kind) const;

    void setupLayout();

    bool rangeSet = false;
    bool scrollbarVisible = false;
    bool showHoverLine = false;
    double verticalScrollValue = 0.0;

    QLabel* emptyLabel_ = 0;
    QWidget* mainWidget_ = 0;

    MEDEA::ChartList* chartList_;
    MEDEA::ChartLabelList* chartLabelList_;
    AxisWidget* timelineAxis_;
    HoverPopup* hoverDisplay_;

    QToolBar* legendToolbar_;
    QScrollArea* scrollArea_;

    QWidget* topFillerWidget_;
    QWidget* bottomFillerWidget_;
    QWidget* hoverWidget_;

    TIME_DISPLAY_FORMAT timeDisplayFormat_ = TIME_DISPLAY_FORMAT::DATE_TIME;

    QHash<MEDEA::ChartDataKind, QAction*> legendActions_;
    QHash<MEDEA::ChartDataKind, QPushButton*> hoverDisplayButtons_;

    QHash<quint32, int> experimentRunSeriesCount_;
    QHash<quint32, QPair<qint64, qint64>> experimentRunTimeRange_;
    QPair<quint32, qint64> longestExperimentRunDuration_;
    QPair<qint64, qint64> totalTimeRange_;

    // MEDEA::Event related widgets/series
    QHash<QString, MEDEA::ChartLabel*> chartLabels_;
    QHash<QString, MEDEA::Chart*> charts_;
    QMultiHash<QString, MEDEA::EventSeries*> seriesList_;
};

#endif // TIMELINECHARTVIEW_H
