#ifndef TIMELINECHARTVIEW_H
#define TIMELINECHARTVIEW_H

#include "../../../../Controllers/ViewController/viewitem.h"
#include "../../Series/lineseries.h"
#include "../../Series/stateseries.h"
#include "../../Series/notificationseries.h"
#include "../../Timeline/hoverpopup.h"
#include "../Axis/entityaxis.h"
#include "../Axis/entityset.h"
#include "entitychart.h"

#include "../../Data/Series/eventseries.h"

#include <QWidget>
#include <QToolBar>
#include <QPushButton>
#include <QScrollArea>
#include <QDateTime>


enum class VALUE_TYPE{DOUBLE, DATETIME};

class AxisWidget;
class TimelineChart;
class TimelineChartView : public QWidget
{
    Q_OBJECT

public:
    explicit TimelineChartView(QWidget* parent = 0);

    // this is just a tester function
    QList<QPointF> generateRandomNumbers(int count = 10, double timeIncrementPx = -1, int minIncrement = 5, int maxIncrement = 500);

    bool eventFilter(QObject *watched, QEvent *event);

    void clearTimelineChart();
    void updateTimelineChart();

signals:
    void toggleSeriesLegend(TIMELINE_SERIES_KIND kind, bool checked);
    void seriesLegendHovered(TIMELINE_SERIES_KIND kind);

public slots:
    void themeChanged();

    void entityAxisSizeChanged(QSizeF size);

    void viewItemConstructed(ViewItem* item);
    void viewItemDestructed(int ID, ViewItem* item);

    void displayedMinChanged(double min);
    void displayedMaxChanged(double max);

    void clearSeriesEvents();
    void receivedRequestedEvent(MEDEA::Event* event);
    
    void updateChartHoverDisplay();
    
private:
    void constructChartForEvent(QString path, QString label);

    EntitySet* addEntitySet(ViewItem* item);
    void removeEntitySet(int ID);

    bool scrollbarVisible = false;
    bool showHoverLine = false;
    double verticalScrollValue = 0.0;

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

    QHash<TIMELINE_SERIES_KIND, QAction*> _legendActions;
    QHash<TIMELINE_SERIES_KIND, QPushButton*> _hoverDisplayButtons;

    QHash<int, EntitySet*> itemEntitySets;
    QHash<int, EntityChart*> itemEntityCharts;

    qint64 lastRequestedFromTime;
    qint64 lastRequestedToTime;
    qint64 lastDataUpdatedTime;

    // MEDEA::Event related widgets/series
    QHash<QString, EntitySet*> eventEntitySets;
    QHash<QString, EntityChart*> eventEntityCharts;
    QHash<QString, MEDEA::EventSeries*> eventSeries;
};

#endif // TIMELINECHARTVIEW_H
