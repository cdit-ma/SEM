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

#include <QWidget>
#include <QToolBar>
#include <QPushButton>
#include <QScrollArea>
#include <QDateTime>


enum class VALUE_TYPE{DOUBLE, DATETIME};
QString getDateTimeString(qint64 mSecsSinceEpoch);

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
    void toggledStateLegend(bool checked);
    void toggledNotificationLegend(bool checked);
    void toggledLineLegend(bool checked);
    void toggleSeriesKind(TIMELINE_SERIES_KIND kind, bool checked);
    void seriesHovered(TIMELINE_SERIES_KIND kind);

public slots:
    void themeChanged();

    void entityAxisSizeChanged(QSizeF size);

    void viewItemConstructed(ViewItem* item);
    void viewItemDestructed(int ID, ViewItem* item);

    void displayedMinChanged(double min);
    void displayedMaxChanged(double max);

    void entityChartPointsHovered(QHash<TIMELINE_SERIES_KIND, QList<QPointF>> points);

    void clearSeriesEvents();
    void receivedRequestedEvent(MEDEA::Event* event);

private:
    void constructChartForEvent(QString workerInstID, quint32 workloadID, QString label);

    EntitySet* addEntitySet(ViewItem* item);
    void removeEntitySet(int ID);

    bool scrollbarVisible = false;
    bool showHoverLine = false;
    double verticalScrollValue = 0.0;

    TimelineChart* _timelineChart;
    EntityAxis* _entityAxis;
    AxisWidget* _dateTimeAxis;
    HoverPopup* _hoverDisplay;

    QScrollArea* _scrollArea;
    QWidget* _fillerWidget;
    QWidget* _hoverWidget;

    QToolBar* legendToolbar;
    QAction* stateLegendAction;
    QAction* notificationLegendAction;
    QAction* lineLegendAction;

    QColor hoverItemBackgroundColor;
    QString hoveredEntityLabel;

    QHash<int, EntitySet*> itemEntitySets;
    QHash<int, EntityChart*> itemChartWidgets;

    // MEDEA::Event related widgets/series
    QHash<QString, EntitySet*> eventEntitySets;
    QHash<QString, EntityChart*> eventEntityCharts;
    QHash<QString, MEDEA::EventSeries*> eventSeries;

    QHash<TIMELINE_SERIES_KIND, QPushButton*> _hoverDisplayButtons;
    QPushButton* _eventsButton;
    QPushButton* _messagesButton;
    QPushButton* _utilisationButton;
    QPushButton* _barButton;

};

#endif // TIMELINECHARTVIEW_H
