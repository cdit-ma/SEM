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

    void clearTimelineChart();
    void updateTimelineChart();

    void setActiveEventKinds(QList<TIMELINE_DATA_KIND> kinds);
    const QList<TIMELINE_DATA_KIND>& getActiveEventKinds();

    void setTimeDisplayFormat(TIME_DISPLAY_FORMAT format);

signals:
    void toggleSeriesLegend(TIMELINE_DATA_KIND kind, bool checked);
    void seriesLegendHovered(TIMELINE_DATA_KIND kind);

public slots:
    void themeChanged();

    void entityAxisSizeChanged(QSizeF size);
    void entitySetClosed();

    void updateChartHoverDisplay();

    void receivedRequestedEvents(quint32 experimentRunID, QList<MEDEA::Event*> events);

private slots:
    void minSliderMoved(double ratio);
    void maxSliderMoved(double ratio);

    void timelineZoomed(int delta);
    void timelinePanned(double dx, double dy);
    void timelineRubberbandUsed(double left, double right);
    
private:
    MEDEA::EventSeries* TimelineChartView::constructSeriesForEventKind(quint32 experimentRunID, TIMELINE_DATA_KIND kind, QString ID, QString label);
    EntityChart* TimelineChartView::constructChartForSeries(MEDEA::EventSeries* series, QString ID, QString label);
    void removeChart(QString chartID);

    void addedDataFromExperimentRun(quint32 experimentRunID);
    void removedDataFromExperimentRun(quint32 experimentRunID);
    void updateTimelineRange(bool updateDisplayRange = true);

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

    QList<TIMELINE_DATA_KIND> _activeEventKinds;
    QHash<TIMELINE_DATA_KIND, QAction*> _legendActions;
    QHash<TIMELINE_DATA_KIND, QPushButton*> _hoverDisplayButtons;

    QHash<quint32, int> experimentRunSeriesCount_;
    QHash<quint32, QPair<qint64, qint64>> experimentRunTimeRange_;
    QPair<quint32, qint64> longestExperimentRunDuration_;
    QPair<qint64, qint64> totalTimeRange_;

    // MEDEA::Event related widgets/series
    QHash<QString, EntitySet*> eventEntitySets;
    QHash<QString, EntityChart*> eventEntityCharts;
    QHash<QString, MEDEA::EventSeries*> eventSeries;
};

#endif // TIMELINECHARTVIEW_H
