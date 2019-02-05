#ifndef TIMELINECHARTVIEW_H
#define TIMELINECHARTVIEW_H

#include "../../../../Controllers/ViewController/viewitem.h"
#include "../../Series/lineseries.h"
#include "../../Series/stateseries.h"
#include "../../Series/notificationseries.h"
#include "../../Timeline/Chart/hoverpopup.h"
#include "../Axis/entityaxis.h"
#include "../Axis/entityset.h"
#include "entitychart.h"

#include "../../Data/Series/portlifecycleeventseries.h"
#include "../../Data/Series/workloadeventseries.h"
#include "../../Data/Series/cpuutilisationeventseries.h"
#include "../../Data/Series/memoryutilisationeventseries.h"

#include <QWidget>
#include <QToolBar>
#include <QPushButton>
#include <QScrollArea>
#include <QDateTime>
#include <QMap>

enum class VALUE_TYPE{DOUBLE, DATE_TIME};
enum class TIME_DISPLAY_FORMAT{VALUE, DATE_TIME, ELAPSED_TIME};

/**
 * @brief GET_DATE_TIME_FROM_VALUE
 * This returns value's corresponding date-time in milliseconds based on the time range
 * @param value
 * @param minTime
 * @param timeRange
 * @return
 */
/*
double GET_DATE_TIME_FROM_VALUE(double value, double minTime, double timeRange)
{
    return value * timeRange + minTime;
}
*/

class AxisWidget;
class TimelineChart;
class TimelineChartView : public QWidget
{
    Q_OBJECT

public:
    explicit TimelineChartView(QWidget* parent = 0);

    // this is just a tester function
    QList<QPointF> generateRandomNumbers(int count = 10, double timeIncrementPx = -1, int minIncrement = 5, int maxIncrement = 500);

    bool eventFilter(QObject *watched, QEvent* event);

    void clearTimelineChart();
    void updateTimelineChart();

    void setActiveEventKinds(QList<TIMELINE_DATA_KIND> kinds);
    const QList<TIMELINE_DATA_KIND>& getActiveEventKinds();

    void setTimelineRange(qint64 min, qint64 max);
    QPair<qint64, qint64> getTimelineRange();

    void toggleTimeDisplay(TIME_DISPLAY_FORMAT format);

signals:
    void toggleSeriesLegend(TIMELINE_DATA_KIND kind, bool checked);
    void seriesLegendHovered(TIMELINE_DATA_KIND kind);

public slots:
    void themeChanged();

    void entityAxisSizeChanged(QSizeF size);
    void entitySetClosed();

    void viewItemConstructed(ViewItem* item);
    void viewItemDestructed(int ID, ViewItem* item);

    void clearSeriesEvents();
    void receivedRequestedEvents(quint32 experimentRunID, QList<MEDEA::Event*> events);

    void updateChartHoverDisplay();
    
private:
    MEDEA::EventSeries* TimelineChartView::constructSeriesForEventKind(quint32 experimentRunID, TIMELINE_DATA_KIND kind, QString ID, QString label);
    EntityChart* TimelineChartView::constructChartForSeries(MEDEA::EventSeries* series, QString ID, QString label);

    void removeChart(QString chartID);
    void updateTimelineRangeFromExperimentRun(quint32 experimentRunID = -1);

    EntitySet* addEntitySet(ViewItem* item);
    void removeEntitySet(int ID);

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

    /*
    QToolBar* axisToolbar;
    QAction* allEntitiesAction;
    QAction* selectedEntityAction;
    */

    TIME_DISPLAY_FORMAT timeDisplayFormat_ = TIME_DISPLAY_FORMAT::DATE_TIME;

    qint64 lastRequestedFromTime;
    qint64 lastRequestedToTime;
    qint64 lastDataUpdatedTime;

    QList<TIMELINE_DATA_KIND> _activeEventKinds;
    QHash<TIMELINE_DATA_KIND, QAction*> _legendActions;
    QHash<TIMELINE_DATA_KIND, QPushButton*> _hoverDisplayButtons;

    QHash<quint32, int> experimentRunSeriesCount_;
    QHash<quint32, QPair<qint64, qint64>> experimentRunTimeRange_;
    QPair<quint32, qint64> longestExperimentRunDuration_;
    QPair<qint64, qint64> totalTimeRange_;

    QHash<int, EntitySet*> itemEntitySets;
    QHash<int, EntityChart*> itemEntityCharts;

    // MEDEA::Event related widgets/series
    QHash<QString, EntitySet*> eventEntitySets;
    QHash<QString, EntityChart*> eventEntityCharts;
    QHash<QString, MEDEA::EventSeries*> eventSeries;
};

#endif // TIMELINECHARTVIEW_H
