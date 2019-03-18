#ifndef TIMELINECHARTVIEW_H
#define TIMELINECHARTVIEW_H

#include "../../../../Controllers/ViewController/viewitem.h"
#include "../../Timeline/Chart/hoverpopup.h"
#include "../Axis/entityaxis.h"
#include "../Axis/entityset.h"
#include "entitychart.h"

#include "../../Data/Events/portlifecycleevent.h"
#include "../../Data/Events/workloadevent.h"
#include "../../Data/Events/cpuutilisationevent.h"
#include "../../Data/Events/memoryutilisationevent.h"
#include "../../Data/Events/markerevent.h"

#include <QWidget>
#include <QToolBar>
#include <QPushButton>
#include <QScrollArea>
#include <QDateTime>
#include <QMultiHash>

enum class VALUE_TYPE{DOUBLE, DATE_TIME};
enum class TIME_DISPLAY_FORMAT{VALUE, DATE_TIME, ELAPSED_TIME};

class AxisWidget;
class TimelineChart;
class TimelineChartView : public QWidget
{
    Q_OBJECT

public:
    explicit TimelineChartView(QWidget* parent = 0);

    bool eventFilter(QObject *watched, QEvent* event);

    void addPortLifecycleEvents(const ExperimentRun& experimentRun, const QVector<PortLifecycleEvent *>& events);
    void addWorkloadEvents(const ExperimentRun& experimentRun, const QVector<WorkloadEvent *>& events);
    void addCPUUtilisationEvents(const ExperimentRun& experimentRun, const QVector<CPUUtilisationEvent *>& events);
    void addMemoryUtilisationEvents(const ExperimentRun& experimentRun, const QVector<MemoryUtilisationEvent *>& events);
    void addMarkerEvents(const ExperimentRun& experimentRun, const QVector<MarkerEvent *>& events);

    void updateExperimentRunLastUpdatedTime(const quint32 experimentRunID, const qint64 time);

    void setTimeDisplayFormat(const TIME_DISPLAY_FORMAT format);

    void clearTimelineChart();
    void updateTimelineChart();

signals:
    void seriesLegendHovered(TIMELINE_DATA_KIND kind);

public slots:
    void themeChanged();

    void toggledSeriesLegend(bool checked);

    void entityAxisSizeChanged(QSizeF size);
    void entityChartHovered(EntityChart* chart, bool hovered);
    void entitySetClosed();

    void updateHoverDisplay();

private slots:
    void minSliderMoved(const double ratio);
    void maxSliderMoved(const double ratio);

    void timelineZoomed(const int delta);
    void timelinePanned(const double dx, const double dy);
    void timelineRubberbandUsed(double left, double right);
    
private:
    void addedChartEvents(const TIMELINE_DATA_KIND kind, const ExperimentRun& experimentRun);

    MEDEA::EventSeries* constructSeriesForEventKind(const ExperimentRun &experimentRun, const TIMELINE_DATA_KIND kind, const QString& ID, const QString& label);
    EntityChart* constructChartForSeries(MEDEA::EventSeries *series, const QString& ID, const QString& label);
    void removeChart(const QString& ID, bool clearing = false);

    void updateRangeForExperimentRun(const quint32 experimentRunID, const qint64 startTime, const qint64 lastUpdatedTime);
    void removedDataFromExperimentRun(const quint32 experimentRunID);
    void updateTimelineRange(bool updateDisplayRange = true);

    void setupLayout();

    bool rangeSet = false;
    bool scrollbarVisible = false;
    bool showHoverLine = false;
    double verticalScrollValue = 0.0;

    QLabel* emptyLabel_ = 0;
    QWidget* mainWidget_ = 0;

    TimelineChart* _timelineChart;
    EntityAxis* _entityAxis;
    AxisWidget* _dateTimeAxis;
    HoverPopup* _hoverDisplay;

    QToolBar* _legendToolbar;
    QScrollArea* _scrollArea;

    QWidget* _topFillerWidget;
    QWidget* _bottomFillerWidget;
    QWidget* _hoverWidget;

    TIME_DISPLAY_FORMAT timeDisplayFormat_ = TIME_DISPLAY_FORMAT::DATE_TIME;

    QHash<TIMELINE_DATA_KIND, QAction*> _legendActions;
    QHash<TIMELINE_DATA_KIND, QPushButton*> _hoverDisplayButtons;

    QHash<quint32, int> experimentRunSeriesCount_;
    QHash<quint32, QPair<qint64, qint64>> experimentRunTimeRange_;
    QPair<quint32, qint64> longestExperimentRunDuration_;
    QPair<qint64, qint64> totalTimeRange_;

    // MEDEA::Event related widgets/series
    QHash<QString, EntitySet*> eventEntitySets;
    QHash<QString, EntityChart*> eventEntityCharts;
    QMultiHash<QString, MEDEA::EventSeries*> eventSeries;
};

#endif // TIMELINECHARTVIEW_H
