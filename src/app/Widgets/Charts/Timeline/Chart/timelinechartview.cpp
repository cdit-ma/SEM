#include "timelinechartview.h"
#include "timelinechart.h"
#include "entitychart.h"
#include "../Axis/axiswidget.h"
#include "../../Series/barseries.h"
#include "../../../../theme.h"

#include <QScrollBar>
#include <QHBoxLayout>
#include <QVBoxLayout>

#define MIN_ENTITY_HEIGHT 50
#define SCROLLBAR_WIDTH 20
#define AXIS_LINE_WIDTH 2
#define POINTS_WIDTH 14
#define ZOOM_FACTOR 1.025
#define SPACING 5
#define OPACITY 0.2

#define HOVERE_DISPLAY_ON true

/**
 * @brief TimelineChartView::TimelineChartView
 * @param parent
 */
TimelineChartView::TimelineChartView(QWidget* parent)
    : QWidget(parent)
{
    /*
     * CHART/AXES
     */
    _entityAxis = new EntityAxis(this);
    _entityAxis->setAxisLineVisible(false);

    _dateTimeAxis = new AxisWidget(Qt::Horizontal, Qt::AlignBottom, this, VALUE_TYPE::DATETIME);
    _dateTimeAxis->setZoomFactor(ZOOM_FACTOR);

    connect(_entityAxis, &EntityAxis::sizeChanged, this, &TimelineChartView::entityAxisSizeChanged);
    connect(_dateTimeAxis, &AxisWidget::displayedMinChanged, this, &TimelineChartView::displayedMinChanged);
    connect(_dateTimeAxis, &AxisWidget::displayedMaxChanged, this, &TimelineChartView::displayedMaxChanged);

    _timelineChart = new TimelineChart(this);
    _timelineChart->setAxisWidth(AXIS_LINE_WIDTH);
    _timelineChart->setPointsWidth(POINTS_WIDTH);
    _timelineChart->setAxisYVisible(true);

    if (HOVERE_DISPLAY_ON) {
        connect(_timelineChart, &TimelineChart::hoverLineUpdated, this, &TimelineChartView::updateChartHoverDisplay);
    }
    connect(_timelineChart, &TimelineChart::hoverLineUpdated, _dateTimeAxis, &AxisWidget::hoverLineUpdated);
    connect(_timelineChart, &TimelineChart::entityChartHovered, [=] (EntityChart* chart, bool hovered) {
        if (chart) {
            QString path = eventEntityCharts.key(chart, "");
            EntitySet* set = eventEntitySets.value(path, 0);
            if (!set)
                set = itemEntitySets.value(chart->getViewItemID(), 0);
            if (set)
                set->setHovered(hovered);
        }
    });

    // connect the chart's pan and zoom signals to the datetime axis
    connect(_timelineChart, &TimelineChart::panned, [=](double dx, double dy) {
        _dateTimeAxis->pan(dx, dy);
    });
    connect(_timelineChart, &TimelineChart::zoomed, [=](int delta) {
        double factor = delta < 0 ? ZOOM_FACTOR : 1 / ZOOM_FACTOR;
        _dateTimeAxis->zoom(factor);
    });
    connect(_timelineChart, &TimelineChart::changeDisplayedRange, [=](double min, double max) {
        _dateTimeAxis->setDisplayedRange(min, max);
    });


    /*
     * HOVER LAYOUT
     */
    _eventsButton = new QPushButton(this);
    _messagesButton = new QPushButton(this);
    _utilisationButton = new QPushButton(this);
    _barButton = new QPushButton(this);

    _hoverDisplayButtons[TIMELINE_SERIES_KIND::STATE] = _eventsButton;
    _hoverDisplayButtons[TIMELINE_SERIES_KIND::NOTIFICATION] = _messagesButton;
    _hoverDisplayButtons[TIMELINE_SERIES_KIND::LINE] = _utilisationButton;
    _hoverDisplayButtons[TIMELINE_SERIES_KIND::BAR] = _barButton;

    for (QPushButton* button : _hoverDisplayButtons.values()) {
        button->setStyleSheet("QPushButton{ text-align: left; }");
    }

    _hoverWidget = new QWidget(this);
    QVBoxLayout* hoverLayout = new QVBoxLayout(_hoverWidget);
    hoverLayout->setSpacing(SPACING * 2);
    hoverLayout->setMargin(SPACING);
    hoverLayout->addWidget(_eventsButton);
    hoverLayout->addWidget(_messagesButton);
    hoverLayout->addWidget(_utilisationButton);
    hoverLayout->addWidget(_barButton);

    _hoverDisplay = new HoverPopup(this);
    _hoverDisplay->setWidget(_hoverWidget);

    /*
     *  TOP (LEGEND) LAYOUT
     */
    legendToolbar = new QToolBar(this);
    legendToolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    stateLegendAction = legendToolbar->addAction("Events");
    notificationLegendAction = legendToolbar->addAction("Messages");
    lineLegendAction = legendToolbar->addAction("Utilisation");

    legendToolbar->widgetForAction(stateLegendAction)->setProperty("TIMELINE_SERIES_KIND", (int)TIMELINE_SERIES_KIND::STATE);
    legendToolbar->widgetForAction(notificationLegendAction)->setProperty("TIMELINE_SERIES_KIND", (int)TIMELINE_SERIES_KIND::NOTIFICATION);
    legendToolbar->widgetForAction(lineLegendAction)->setProperty("TIMELINE_SERIES_KIND", (int)TIMELINE_SERIES_KIND::LINE);

    connect(stateLegendAction, &QAction::toggled, this, &TimelineChartView::toggledStateLegend);
    connect(notificationLegendAction, &QAction::toggled, this, &TimelineChartView::toggledNotificationLegend);
    connect(lineLegendAction, &QAction::toggled, this, &TimelineChartView::toggledLineLegend);

    connect(stateLegendAction, &QAction::toggled, [=](bool checked){
        legendToolbar->widgetForAction(stateLegendAction)->setProperty("checked", checked);
        emit toggleSeriesLegend(TIMELINE_SERIES_KIND::STATE, checked);
        emit seriesLegendHovered(checked ? TIMELINE_SERIES_KIND::STATE : TIMELINE_SERIES_KIND::DATA);
    });
    connect(notificationLegendAction, &QAction::toggled,[=](bool checked){
        legendToolbar->widgetForAction(notificationLegendAction)->setProperty("checked", checked);
        emit toggleSeriesLegend(TIMELINE_SERIES_KIND::NOTIFICATION, checked);
        emit seriesLegendHovered(checked ? TIMELINE_SERIES_KIND::NOTIFICATION : TIMELINE_SERIES_KIND::DATA);
    });
    connect(lineLegendAction, &QAction::toggled, [=](bool checked){
        legendToolbar->widgetForAction(lineLegendAction)->setProperty("checked", checked);
        emit toggleSeriesLegend(TIMELINE_SERIES_KIND::LINE, checked);
        emit seriesLegendHovered(checked ? TIMELINE_SERIES_KIND::LINE : TIMELINE_SERIES_KIND::DATA);
    });

    for (QAction* action : legendToolbar->actions()) {
        // don't need the filter for the series that's painted on top by default
        /*if (action != lineLegendAction) {
            legendToolbar->widgetForAction(action)->installEventFilter(this);
        }*/
        legendToolbar->widgetForAction(action)->installEventFilter(this);
        action->setCheckable(true);
        action->setChecked(true);
        action->setToolTip("Show/Hide " + action->text());
    }

    /*
     * MID (SCROLL AREA) LAYOUT
     */
    QWidget* mainWidget = new QWidget(this);
    QHBoxLayout* scrollLayout = new QHBoxLayout(mainWidget);
    scrollLayout->setMargin(0);
    scrollLayout->setSpacing(0);
    scrollLayout->addWidget(_timelineChart, 1);
    scrollLayout->addWidget(_entityAxis);

    _scrollArea = new QScrollArea(this);
    _scrollArea->setWidget(mainWidget);
    _scrollArea->setWidgetResizable(true);
    _scrollArea->setLayoutDirection(Qt::RightToLeft);
    _scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _scrollArea->verticalScrollBar()->setFixedWidth(SCROLLBAR_WIDTH);

    _fillerWidget = new QWidget(this);

    /*
     * BOTTOM (TIME AXIS) LAYOUT
     */
    QHBoxLayout* bottomLayout = new QHBoxLayout();
    bottomLayout->setMargin(0);
    bottomLayout->setSpacing(0);
    bottomLayout->addWidget(_fillerWidget);
    bottomLayout->addWidget(_dateTimeAxis, 1);

    /*
     * MAIN LAYOUT
     */
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(SPACING, SPACING, SPACING, SPACING);
    mainLayout->addWidget(legendToolbar, 0, Qt::AlignCenter);
    mainLayout->addSpacerItem(new QSpacerItem(0, SPACING));
    mainLayout->addWidget(_scrollArea, 1);
    mainLayout->addLayout(bottomLayout);

    _scrollArea->verticalScrollBar()->setTracking(true);
    connect(_scrollArea->verticalScrollBar(), &QScrollBar::valueChanged, [=]() {
        verticalScrollValue = _scrollArea->verticalScrollBar()->value();
    });

    connect(Theme::theme(), &Theme::theme_Changed, this, &TimelineChartView::themeChanged);
    themeChanged();
}


QList<QPointF> TimelineChartView::generateRandomNumbers(int count, double timeIncrementPx, int minIncrement, int maxIncrement)
{
    QList<QPointF> points;
    QDateTime currentDT = QDateTime::currentDateTime();
    QDateTime maxDT = currentDT.addSecs(count);
    while (currentDT.toMSecsSinceEpoch() < maxDT.toMSecsSinceEpoch()) {
        int y = rand() % 100;
        points.append(QPointF(currentDT.toMSecsSinceEpoch(), y));
        if (timeIncrementPx == -1) {
            int r = minIncrement + rand() % maxIncrement;
            currentDT = currentDT.addSecs(r);
        } else {
            currentDT = currentDT.addSecs(timeIncrementPx);
        }
    }
    return points;
}


/**
 * @brief TimelineChartView::eventFilter
 * @param watched
 * @param event
 * @return
 */
bool TimelineChartView::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::HoverEnter || event->type() == QEvent::HoverLeave) {
        if (!watched->property("checked").toBool())
            return false;
        TIMELINE_SERIES_KIND kind = TIMELINE_SERIES_KIND::DATA;
        if (event->type() == QEvent::HoverEnter) {
            kind = (TIMELINE_SERIES_KIND) watched->property("TIMELINE_SERIES_KIND").toInt();
        }
        emit seriesLegendHovered(kind);
        return true;
    }
    return QWidget::eventFilter(watched, event);
}


/**
 * @brief TimelineChartView::clearTimelineChart
 */
void TimelineChartView::clearTimelineChart()
{
    // clear/delete the items in the entity axis
    auto axisItr = itemEntitySets.begin();
    while (axisItr != itemEntitySets.end()) {
        auto set = (*axisItr);
        _entityAxis->removeEntity(set);
        set->deleteLater();
        axisItr = itemEntitySets.erase(axisItr);
    }
    // clear/delete the entity charts in the timeline chart
    auto chartItr = itemChartWidgets.begin();
    while (chartItr != itemChartWidgets.end()) {
        auto chart = (*chartItr);
        _timelineChart->removeEntityChart(chart);
        chart->deleteLater();
        chartItr = itemChartWidgets.erase(chartItr);
    }
    _timelineChart->setInitialRange(true);
    _dateTimeAxis->setRange(_timelineChart->getRange().first, _timelineChart->getRange().second, true);
}


/**
 * @brief TimelineChartView::updateTimelineChart
 */
void TimelineChartView::updateTimelineChart()
{
    _timelineChart->update();
}


/**
 * @brief TimelineChartView::themeChanged
 */
void TimelineChartView::themeChanged()
{
    Theme* theme = Theme::theme();
    QColor bgColor = theme->getAltBackgroundColor();
    QColor handleColor = theme->getAltBackgroundColor();
    QColor highlightColor = theme->getHighlightColor();
    bgColor.setAlphaF(OPACITY);
    handleColor.setAlphaF(1 - OPACITY);
    highlightColor.setAlphaF(handleColor.alphaF());

    setStyleSheet("QWidget{ background: rgba(0,0,0,0); }"
                  "QScrollBar:vertical {"
                  "width:" + QString::number(SCROLLBAR_WIDTH) + ";"
                  "background:" + Theme::QColorToHex(bgColor) + ";"
                  "border-radius:" + theme->getCornerRadius() + ";"
                  "border: 2px solid " + theme->getDisabledBackgroundColorHex() + ";"
                  "margin: 0px;"
                  "padding: 0px;"
                  "}"
                  "QScrollBar::handle {"
                  "background:" + Theme::QColorToHex(handleColor) + ";"
                  "border-radius:" + theme->getSharpCornerRadius() + ";"
                  "border: 0px;"
                  "margin: 0px;"
                  "padding: 0px;"
                  "}"
                  "QScrollBar::handle:active{ background: " + theme->getHighlightColorHex() + ";}");
    //"QScrollBar::handle:active{ background: " + Theme::QColorToHex(highlightColor) + ";}");

    _timelineChart->setStyleSheet("background: " + Theme::QColorToHex(bgColor) + ";");

    legendToolbar->setFixedHeight(theme->getLargeIconSize().height());
    legendToolbar->setStyleSheet(theme->getToolTipStyleSheet() +
                                 theme->getToolBarStyleSheet() +
                                 "QToolButton{ border: 0px; color:" + theme->getTextColorHex(ColorRole::DISABLED) + ";}"
                                 "QToolButton::checked:!hover{ color:" + theme->getTextColorHex() + ";}"
                                 "QToolButton:!hover{ background: rgba(0,0,0,0); }");

    stateLegendAction->setIcon(theme->getIcon("ToggleIcons", "stateLegendToggle"));
    notificationLegendAction->setIcon(theme->getIcon("ToggleIcons", "notificationLegendToggle"));
    lineLegendAction->setIcon(theme->getIcon("ToggleIcons", "lineLegendToggle"));

    _eventsButton->setIcon(theme->getIcon("ToggleIcons", "stateLegendToggle"));
    _messagesButton->setIcon(theme->getIcon("ToggleIcons", "notificationLegendToggle"));
    _utilisationButton->setIcon(theme->getIcon("ToggleIcons", "lineLegendToggle"));
    _barButton->setIcon(theme->getIcon("ToggleIcons", "barLegendToggle"));
}


/**
 * @brief TimelineChartView::entityAxisSizeChanged
 * @param size
 */
void TimelineChartView::entityAxisSizeChanged(QSizeF size)
{
    qreal chartHeight = height() - _dateTimeAxis->height() - legendToolbar->height() - SPACING * 3;
    if (size.height() > chartHeight) {
        size.setWidth(size.width() + SCROLLBAR_WIDTH);
    }
    _fillerWidget->setFixedWidth(size.width());
}


/**
 * @brief TimelineChartView::viewItemConstructed
 * @param item
 */
void TimelineChartView::viewItemConstructed(ViewItem* item)
{
    addEntitySet(item);
}


/**
 * @brief TimelineChartView::viewItemDestructed
 * @param ID
 * @param item
 */
void TimelineChartView::viewItemDestructed(int ID, ViewItem* item)
{
    removeEntitySet(ID);
}


/**
 * @brief TimelineChartView::displayedMinChanged
 * @param min
 */
void TimelineChartView::displayedMinChanged(double min)
{
    //qDebug() << "TimelineChartView::displayedMinChanged: " << min;
    _timelineChart->setMin(min);
}


/**
 * @brief TimelineChartView::displayedMaxChanged
 * @param max
 */
void TimelineChartView::displayedMaxChanged(double max)
{
    _timelineChart->setMax(max);
}


/**
 * @brief TimelineChartView::udpateChartHoverDisplay
 */
void TimelineChartView::updateChartHoverDisplay()
{
    _hoverDisplay->hide();

    if (_timelineChart->isPanning())
        return;

    QHash<TIMELINE_SERIES_KIND, QString> hoveredData;

    for (auto entityChart : _timelineChart->getEntityCharts()) {
        if (!entityChart || !entityChart->isHovered())
            continue;
        const auto& series = entityChart->getSeries();
        auto hoveredKinds = entityChart->getHovereSeriesKinds();
        for (auto s : series) {
            if (s) {
                auto kind = s->getKind();
                if (hoveredKinds.contains(kind)) {
                    auto range = entityChart->getHoveredTimeRange(kind);
                    //qDebug() << ""
                    auto hoveredInfo = s->getHoveredDataString(range.first, range.second);
                    if (!hoveredInfo.isEmpty()) {
                        hoveredData[s->getKind()] += hoveredInfo + "\n";
                    }
                }
            } else {
                qWarning("TimelineChartView::updateChartHoverDisplay - Got NULL series somehow");
            }
        }
    }

    if (hoveredData.isEmpty())
        return;

    for (auto kind : _hoverDisplayButtons.keys()) {
        auto button = _hoverDisplayButtons.value(kind, 0);
        if (button) {
            bool hasData = hoveredData.contains(kind);
            button->setVisible(hasData);
            if (hasData) {
                auto data = hoveredData.value(kind);
                button->setText(data.trimmed());
            }
        }
    }

    _hoverDisplay->setVisible(!hoveredData.isEmpty());
    if (_hoverDisplay->isVisible()) {
        _hoverDisplay->adjustSize();
        _hoverDisplay->move(mapTo(this, cursor().pos()  + QPoint(30, 0)));
    }
}


/**
 * @brief TimelineChartView::clearSeriesEvents
 */
void TimelineChartView::clearSeriesEvents()
{
    for (auto series : eventSeries.values()) {
        series->clear();
    }
    _timelineChart->setInitialRange(true);
}


/**
 * @brief TimelineChartView::receivedRequestedEvent
 * @param event
 */
void TimelineChartView::receivedRequestedEvent(MEDEA::Event* event)
{
    if (!event)
        return;

    MEDEA::EventSeries* series = 0;

    auto ID = event->getID();
    if (eventSeries.contains(ID)) {
        series = eventSeries.value(ID);
    } else {
        constructChartForEvent(ID, event->getName());

        auto chart = eventEntityCharts[ID];
        auto set = eventEntitySets[ID];
        series = eventSeries[ID];

        _entityAxis->appendEntity(set);
        _timelineChart->addEntityChart(chart);
    }

    if (series) {

        series->addEvent(event);

        // TODO: Don't need this once we have the start/end time of the experiment
        // update this timeline chart's range
        auto timelineRange = _timelineChart->getRange();
        auto seriesRange = series->getTimeRangeMS();
        if (!_timelineChart->isRangeSet()) {
            _timelineChart->setInitialRange(false, seriesRange.first, seriesRange.second);
        } else {
            if (seriesRange.first <= timelineRange.first) {
                _timelineChart->setMin(seriesRange.first);
            }
            if (seriesRange.second >= timelineRange.second) {
                _timelineChart->setMax(seriesRange.second);
            }
        }
        // if the timeline chart's range was changed, update the date/time axis' range
        if (timelineRange != _timelineChart->getRange()) {
            _dateTimeAxis->setRange(_timelineChart->getRange().first, _timelineChart->getRange().second, true);
        }
    }
}


/**
 * @brief TimelineChartView::constructChartForEvent
 * @param ID
 * @param label
 */
void TimelineChartView::constructChartForEvent(QString ID, QString label)
{
    MEDEA::EventSeries* series = new MEDEA::EventSeries(this);
    eventSeries[ID] = series;

    EntityChart* chart = new EntityChart(0, this);
    eventEntityCharts[ID] = chart;
    chart->addEventSeries(series);

    EntitySet* set = new EntitySet(label, this);
    eventEntitySets[ID] = set;
    set->setMinimumHeight(MIN_ENTITY_HEIGHT);
    set->themeChanged(Theme::theme());
    connect(set, &EntitySet::visibilityChanged, chart, &EntityChart::setVisible);
    connect(set, &EntitySet::hovered, [=] (bool hovered) {
        _timelineChart->setEntityChartHovered(chart, hovered);
    });
}


/**
 * @brief TimelineChartView::addEntitySet
 * @param item
 * @return
 */
EntitySet* TimelineChartView::addEntitySet(ViewItem* item)
{
    // we only care about node items
    if (!item || !item->isNode())
        return 0;

    QString itemLabel = item->getData("label").toString();
    int itemID = item->getID();

    // check if we already have an entity set for the view item
    if (itemEntitySets.contains(itemID))
        return itemEntitySets.value(itemID);

    EntitySet* set = new EntitySet(itemLabel, this);
    set->setMinimumHeight(MIN_ENTITY_HEIGHT);
    set->setID(itemID);
    set->themeChanged(Theme::theme());
    itemEntitySets[itemID] = set;

    int barCount = 120000;

    QList<QPointF> samplePoints;
    EntityChart* seriesChart = new EntityChart(item, this);

    if (itemLabel == "Model") {

        MEDEA::BarSeries* barSeries = new MEDEA::BarSeries(item);
        samplePoints = generateRandomNumbers(barCount, 5);

        for (QPointF p : samplePoints) {
            int count = 5;
            int val = 0;
            QVector<double> data;
            for (int i = 0; i < count; i++) {
                double y = (double)(rand() % 100);
                data.insert(0, val + y);
                val += y;
            }
            barSeries->addData(p.x(), data);
        }
        seriesChart->addSeries(barSeries);

    } else {

        int random = 1 + rand() % 30;
        bool filled = false;
        samplePoints = generateRandomNumbers(barCount);

        if (random % 2 == 0) {
            MEDEA::StateSeries* stateSeries = new MEDEA::StateSeries(item);
            for (int i = 1; i < samplePoints.count(); i += 2) {
                stateSeries->addLine(samplePoints.at(i - 1), samplePoints.at(i));
            }
            seriesChart->addSeries(stateSeries);
            filled = true;
        }
         if (random % 5 == 0 || random % 3 == 0) {
            MEDEA::NotificationSeries* notificationSeries = new MEDEA::NotificationSeries(item);
            for (QPointF point : samplePoints) {
                int randomType = rand() % 4;
                notificationSeries->addTime(point.x(), (MEDEA::NotificationSeries::NOTIFICATION_TYPE) randomType);
            }
            seriesChart->addSeries(notificationSeries);
            filled = true;
        }
         if (random % 3 == 0) {
             MEDEA::BarSeries* barSeries = new MEDEA::BarSeries(item);
             for (QPointF p : samplePoints) {
                 int count = 1 + rand() % 5;
                 int val = 0;
                 QVector<double> data;
                 for (int i = 0; i < count; i++) {
                     double y = (double)(rand() % 200);
                     data.insert(0, val + y);
                     val += y;
                 }
                 barSeries->addData(p.x(), data);
             }
             seriesChart->addSeries(barSeries);
             filled = true;
         }

        if (!filled) {
            MEDEA::BarSeries* barSeries = new MEDEA::BarSeries(item);
            for (QPointF p : samplePoints) {
                int count = 1 + rand() % 5;
                int val = 0;
                QVector<double> data;
                for (int i = 0; i < count; i++) {
                    double y = (double)(rand() % 200);
                    data.insert(0, val + y);
                    val += y;
                }
                barSeries->addData(p.x(), data);
            }
            seriesChart->addSeries(barSeries);
        }
    }

    EntitySet* parentSet = addEntitySet(item->getParentItem());
    bool showSeries = true;

    if (parentSet) {
        // insert the new set at the end of the parent's tree and connect it to the parent
        showSeries = parentSet->isExpanded();
        parentSet->addChildEntitySet(set);
        int index = _entityAxis->insertEntity(parentSet, set);
        _timelineChart->insertEntityChart(index, seriesChart);
    } else {
        // we only need to set the depth for the top level entity sets
        set->setDepth(0);
        _entityAxis->appendEntity(set);
        _timelineChart->addEntityChart(seriesChart);
    }

    for (auto& action : legendToolbar->actions()) {
        TIMELINE_SERIES_KIND kind = (TIMELINE_SERIES_KIND)action->property("TIMELINE_SERIES_KIND").toInt();
        seriesChart->setSeriesKindVisible(kind, action->isChecked());
    }

    // update this timeline chart's range
    auto timelineRange = _timelineChart->getRange();
    auto chartRange = seriesChart->getRangeX();
    if (!_timelineChart->isRangeSet()) {
        _timelineChart->setInitialRange(false, chartRange.first, chartRange.second);
    } else {
        if (chartRange.first <= timelineRange.first) {
            _timelineChart->setMin(chartRange.first);
        }
        if (chartRange.second >= timelineRange.second) {
            _timelineChart->setMax(chartRange.second);
        }
    }
    // if the timeline chart's range was changed, update the date/time axis' range
    if (timelineRange != _timelineChart->getRange()) {
        _dateTimeAxis->setRange(_timelineChart->getRange().first, _timelineChart->getRange().second, true);
    }

    connect(this, &TimelineChartView::seriesLegendHovered, seriesChart, &EntityChart::seriesKindHovered);
    connect(this, &TimelineChartView::toggleSeriesLegend, seriesChart, &EntityChart::setSeriesKindVisible);
    connect(set, &EntitySet::visibilityChanged, seriesChart, &EntityChart::setVisible);
    connect(set, &EntitySet::hovered, [=] (bool hovered) {
        _timelineChart->setEntityChartHovered(seriesChart, hovered);
    });

    // set the initial visibility states of the chart and each individual series in the chart
    seriesChart->setVisible(showSeries);
    seriesChart->setSeriesKindVisible(TIMELINE_SERIES_KIND::STATE, stateLegendAction->isChecked());
    seriesChart->setSeriesKindVisible(TIMELINE_SERIES_KIND::NOTIFICATION, notificationLegendAction->isChecked());
    seriesChart->setSeriesKindVisible(TIMELINE_SERIES_KIND::LINE, lineLegendAction->isChecked());

    itemChartWidgets[itemID] = seriesChart;
    return set;
}


/**
 * @brief TimelineView::removeEntitySet
 * @param ID
 */
void TimelineChartView::removeEntitySet(int ID)
{
    if (itemEntitySets.contains(ID)) {
        EntitySet* set = itemEntitySets.take(ID);
        _entityAxis->removeEntity(set);
        set->deleteLater();
        // remove chart from the hash and layout
        EntityChart* entityChart = itemChartWidgets.take(ID);
        _timelineChart->removeEntityChart(entityChart);
        entityChart->deleteLater();
    }
}


/**
 * @brief qHash
 * @param key
 * @param seed
 * @return
 */
inline uint qHash(TIMELINE_SERIES_KIND key, uint seed)
{
    return ::qHash(static_cast<uint>(key), seed);
}
