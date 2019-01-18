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

#define HOVER_DISPLAY_ON true
#define HOVER_DISPLAY_ITEM_COUNT 10

/**
 * @brief TimelineChartView::TimelineChartView
 * @param parent
 */
TimelineChartView::TimelineChartView(QWidget* parent)
    : QWidget(parent)
{
    /*
     * CHART/AXES - Note: The axis lines are on by default for both axes.
     * The timeline chart can draw its own axis lines but is off by default.
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

    if (HOVER_DISPLAY_ON) {
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
     *  TOP (LEGEND) LAYOUT
     */

    _topFillerWidget = new QWidget(this);
    _legendToolbar = new QToolBar(this);
    _legendToolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->setMargin(0);
    topLayout->setSpacing(0);
    topLayout->addWidget(_topFillerWidget);
    topLayout->addWidget(_legendToolbar, 1, Qt::AlignCenter);

    /*
     * HOVER LAYOUT
     */
    _hoverWidget = new QWidget(this);
    QVBoxLayout* hoverLayout = new QVBoxLayout(_hoverWidget);
    hoverLayout->setSpacing(SPACING * 2);
    hoverLayout->setMargin(SPACING);

    _hoverDisplay = new HoverPopup(this);
    _hoverDisplay->setWidget(_hoverWidget);

    /*
     * HOVER AND LEGEND TIMELINE_DATA_KIND WIDGETS
     */
    for (auto kind : GET_TIMELINE_DATA_KINDS()) {
        if (kind == TIMELINE_DATA_KIND::DATA || kind == TIMELINE_DATA_KIND::LINE)
            continue;
        // construct legend widgets
        QAction* action = _legendToolbar->addAction(GET_TIMELINE_DATA_KIND_STRING(kind));
        action->setToolTip("Show/Hide " + action->text() + " Series");
        action->setCheckable(true);
        action->setChecked(true);
        action->setVisible(false);
        _legendActions[kind] = action;
        QWidget* actionWidget = _legendToolbar->widgetForAction(action);
        actionWidget->installEventFilter(this);
        actionWidget->setProperty("TIMELINE_DATA_KIND", (uint)kind);
        connect(action, &QAction::toggled, [=](bool checked){
            actionWidget->setProperty("checked", checked);
            emit toggleSeriesLegend(kind, checked);
            emit seriesLegendHovered(checked ? kind : TIMELINE_DATA_KIND::DATA);
        });
        // construct hover display widgets
        QPushButton* button = new QPushButton(this);
        button->setStyleSheet("QPushButton{ text-align: left; }");
        hoverLayout->addWidget(button);
        _hoverDisplayButtons[kind] = button;
    }

    /*
     * MID (SCROLL AREA) LAYOUT
     */

    /*axisToolbar = new QToolBar(this);
    axisToolbar->setOrientation(Qt::Vertical);

    QWidget* stretchWidget = new QWidget(this);
    stretchWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    allEntitiesAction = axisToolbar->addAction("Available Entities");
    selectedEntityAction = axisToolbar->addAction("Selected Entity");
    axisToolbar->addWidget(stretchWidget);*/

    QWidget* mainWidget = new QWidget(this);
    QHBoxLayout* scrollLayout = new QHBoxLayout(mainWidget);
    scrollLayout->setMargin(0);
    scrollLayout->setSpacing(0);
    scrollLayout->addWidget(_timelineChart, 1);
    scrollLayout->addWidget(_entityAxis);
    //scrollLayout->addWidget(axisToolbar);

    _scrollArea = new QScrollArea(this);
    _scrollArea->setWidget(mainWidget);
    _scrollArea->setWidgetResizable(true);
    _scrollArea->setLayoutDirection(Qt::RightToLeft);
    _scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _scrollArea->verticalScrollBar()->setFixedWidth(SCROLLBAR_WIDTH);

    /*
     * BOTTOM (TIME AXIS) LAYOUT
     */
    _bottomFillerWidget = new QWidget(this);
    QHBoxLayout* bottomLayout = new QHBoxLayout();
    bottomLayout->setMargin(0);
    bottomLayout->setSpacing(0);
    bottomLayout->addWidget(_bottomFillerWidget);
    bottomLayout->addWidget(_dateTimeAxis, 1);

    /*
     * MAIN LAYOUT
     */
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(SPACING, SPACING, SPACING, SPACING);
    mainLayout->addLayout(topLayout);
    mainLayout->addSpacerItem(new QSpacerItem(0, SPACING));
    mainLayout->addWidget(_scrollArea, 1);
    mainLayout->addLayout(bottomLayout);

    _scrollArea->verticalScrollBar()->setTracking(true);
    connect(_scrollArea->verticalScrollBar(), &QScrollBar::valueChanged, [=]() {
        verticalScrollValue = _scrollArea->verticalScrollBar()->value();
    });

    auto minTimeAxisWidth = fontMetrics().width(QDateTime::fromMSecsSinceEpoch(0).toString(TIME_FORMAT));
    setMinimumWidth(_entityAxis->minimumWidth() + minTimeAxisWidth + SPACING * 2);

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
        TIMELINE_DATA_KIND kind = TIMELINE_DATA_KIND::DATA;
        if (event->type() == QEvent::HoverEnter) {
            kind = (TIMELINE_DATA_KIND) watched->property("TIMELINE_DATA_KIND").toUInt();
        }
        emit seriesLegendHovered(kind);
        return true;
    }
    return QWidget::eventFilter(watched, event);
}


/**
 * @brief TimelineChartView::clearTimelineChart
 * Clear/delete all the axis items and entity charts and reset the timeline range.
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
    auto chartItr = itemEntityCharts.begin();
    while (chartItr != itemEntityCharts.end()) {
        auto chart = (*chartItr);
        _timelineChart->removeEntityChart(chart);
        chart->deleteLater();
        chartItr = itemEntityCharts.erase(chartItr);
    }

    /*
     * NOTE:: Only clear the widgets when:
     * New project is triggered or the project is closed
     * The user unchecks everything in the entity axis
     */
    // TODO - These hashes will be combined with the ones above eventually
    // clear/delete the items in the entity axis
    auto axisItr_e = eventEntitySets.begin();
    while (axisItr_e != eventEntitySets.end()) {
        auto set = (*axisItr_e);
        _entityAxis->removeEntity(set);
        set->deleteLater();
        axisItr_e = eventEntitySets.erase(axisItr_e);
    }
    // clear/delete the entity charts in the timeline chart
    auto chartItr_e = eventEntityCharts.begin();
    while (chartItr_e != eventEntityCharts.end()) {
        auto chart = (*chartItr_e);
        _timelineChart->removeEntityChart(chart);
        chart->deleteLater();
        chartItr_e = eventEntityCharts.erase(chartItr_e);
    }

    auto seriesItr_e = eventSeries.begin();
    while (seriesItr_e != eventSeries.end()) {
        (*seriesItr_e)->deleteLater();
        seriesItr_e = eventSeries.erase(seriesItr_e);
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
 * @brief TimelineChartView::setActiveEventKinds
 * @param kinds
 */
void TimelineChartView::setActiveEventKinds(QList<TIMELINE_DATA_KIND> kinds)
{
    _activeEventKinds = kinds;

    for (auto kind : kinds) {
        switch (kind) {
        case TIMELINE_DATA_KIND::PORT_LIFECYCLE:
            _legendActions.value(TIMELINE_DATA_KIND::PORT_LIFECYCLE)->setVisible(true);
            break;
        case TIMELINE_DATA_KIND::WORKLOAD:
            _legendActions.value(TIMELINE_DATA_KIND::WORKLOAD)->setVisible(true);
            break;
        case TIMELINE_DATA_KIND::CPU_UTILISATION:
            _legendActions.value(TIMELINE_DATA_KIND::CPU_UTILISATION)->setVisible(true);
            break;
        case TIMELINE_DATA_KIND::MEMORY_UTILISATION:
            _legendActions.value(TIMELINE_DATA_KIND::MEMORY_UTILISATION)->setVisible(true);
            break;
        default: {
            // NOTE: this case is temporary - only added it for the model entities chart
            _legendActions.value(TIMELINE_DATA_KIND::STATE)->setVisible(true);
            _legendActions.value(TIMELINE_DATA_KIND::NOTIFICATION)->setVisible(true);
            _legendActions.value(TIMELINE_DATA_KIND::BAR)->setVisible(true);
            break;
        }
        }
    }
}


/**
 * @brief TimelineChartView::getActiveEventKinds
 * @return
 */
const QList<TIMELINE_DATA_KIND> &TimelineChartView::getActiveEventKinds()
{
    return _activeEventKinds;
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

    _legendToolbar->setFixedHeight(theme->getLargeIconSize().height());
    _legendToolbar->setStyleSheet(theme->getToolTipStyleSheet() +
                                 theme->getToolBarStyleSheet() +
                                 "QToolButton{ border: 0px; color:" + theme->getTextColorHex(ColorRole::DISABLED) + ";}"
                                 "QToolButton::checked:!hover{ color:" + theme->getTextColorHex() + ";}"
                                 "QToolButton:!hover{ background: rgba(0,0,0,0); }");

    for (auto kind : GET_TIMELINE_DATA_KINDS()) {
        QIcon buttonIcon;
        switch (kind) {
        case TIMELINE_DATA_KIND::STATE:
            buttonIcon = theme->getIcon("ToggleIcons", "stateHover");
            break;
        case TIMELINE_DATA_KIND::NOTIFICATION:
            buttonIcon = theme->getIcon("ToggleIcons", "notificationHover");
            break;
        case TIMELINE_DATA_KIND::BAR:
            buttonIcon = theme->getIcon("ToggleIcons", "barHover");
            break;
        case TIMELINE_DATA_KIND::PORT_LIFECYCLE:
            buttonIcon = theme->getIcon("ToggleIcons", "portLifecycleHover");
            break;

        case TIMELINE_DATA_KIND::WORKLOAD:
            buttonIcon = theme->getIcon("ToggleIcons", "workloadHover");
            break;
        case TIMELINE_DATA_KIND::CPU_UTILISATION:
            buttonIcon = theme->getIcon("ToggleIcons", "utilisationHover");
            break;
        case TIMELINE_DATA_KIND::MEMORY_UTILISATION:
            buttonIcon = theme->getIcon("ToggleIcons", "memoryHover");
            break;
        default:
            continue;
        }
        auto button = _hoverDisplayButtons.value(kind, 0);
        if (button)
            button->setIcon(buttonIcon);
        auto action = _legendActions.value(kind, 0);
        if (action)
            action->setIcon(theme->getIcon("ToggleIcons", GET_TIMELINE_DATA_KIND_STRING(kind)));
    }
}


/**
 * @brief TimelineChartView::entityAxisSizeChanged
 * @param size
 */
void TimelineChartView::entityAxisSizeChanged(QSizeF size)
{
    qreal chartHeight = height() - _dateTimeAxis->height() - _legendToolbar->height() - SPACING * 3;
    if (size.height() > chartHeight) {
        size.setWidth(size.width() + SCROLLBAR_WIDTH);
    }
    _topFillerWidget->setFixedWidth(size.width());
    _bottomFillerWidget->setFixedWidth(size.width());
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

    QHash<TIMELINE_DATA_KIND, QString> hoveredData;

    for (auto entityChart : _timelineChart->getEntityCharts()) {
        if (!entityChart || !entityChart->isHovered())
            continue;
        const auto& series = entityChart->getSeries();
        auto hoveredKinds = entityChart->getHovereSeriesKinds();
        for (auto s : series) {
            if (!s)
                continue;
            auto kind = s->getKind();
            auto action = _legendActions.value(kind, 0);
            if (action && action->isChecked()) {
                if (!hoveredKinds.contains(kind))
                    continue;
                auto hoveredInfo = s->getHoveredDataString(entityChart->getHoveredTimeRange(kind), HOVER_DISPLAY_ITEM_COUNT, DATETIME_FORMAT);
                if (!hoveredInfo.isEmpty())
                    hoveredData[kind] += hoveredInfo + "\n";
            }
        }
    }

    if (hoveredData.isEmpty())
        return;

    //int childrenHeight = SPACING;
    for (auto kind : _hoverDisplayButtons.keys()) {
        auto button = _hoverDisplayButtons.value(kind, 0);
        if (button) {
            bool hasData = hoveredData.contains(kind);
            button->setVisible(hasData);
            if (hasData) {
                auto data = hoveredData.value(kind);
                button->setText(data.trimmed());
                //childrenHeight += button->height() + SPACING;
            }
        }
    }

    // adjust the hover display's position to make sure that it is fully visible
    auto globalPos = mapToGlobal(pos());
    auto hoverPos = mapTo(this, cursor().pos()) - QPoint(0, _hoverDisplay->height() / 2.0);
    if (hoverPos.x() >= (globalPos.x() + width() / 2.0)) {
        hoverPos.setX(hoverPos.x() - _hoverDisplay->width() - 25);
    } else {
        hoverPos.setX(hoverPos.x() + 25);
    }

    auto bottom = globalPos.y() + height() - _dateTimeAxis->height();
    if ((hoverPos.y() + _hoverDisplay->height()) > bottom) {
        hoverPos.setY(bottom - _hoverDisplay->height());
    } else if (hoverPos.y() < globalPos.y()){
        hoverPos.setY(globalPos.y());
    }

    //_hoverDisplay->resize(_hoverDisplay->width(), childrenHeight);
    _hoverDisplay->move(hoverPos);
    _hoverDisplay->show();
}


/**
 * @brief TimelineChartView::clearSeriesEvents
 */
void TimelineChartView::clearSeriesEvents()
{
    clearTimelineChart();
}


/**
 * @brief TimelineChartView::receivedRequestedEvent
 * @param event
 */
void TimelineChartView::receivedRequestedEvent(MEDEA::Event* event)
{
    if (!event)
        return;

    auto series = constructSeriesForEventKind(event->getKind(), event->getID(), event->getName());

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
 * @brief TimelineChartView::receivedRequestedEvents
 * @param events
 */
void TimelineChartView::receivedRequestedEvents(QList<MEDEA::Event*> events)
{
    // NOTE - At the moment, there is one series per kind per entity chart
    if (events.isEmpty())
        return;

    QSet<MEDEA::EventSeries*> updatedSeries;
    qint64 minTime = INT64_MAX, maxTime = INT64_MIN;

    // before storing newly requested data, clear corresponding series (if there is one)
    for (auto event : events) {
        auto series = constructSeriesForEventKind(event->getKind(), event->getID(), event->getName());
        if (series) {
            if (!updatedSeries.contains(series)) {
                series->clear();
                updatedSeries.insert(series);
            }
            series->addEvent(event);
            minTime = qMin(series->getMinTimeMS(), minTime);
            maxTime = qMax(series->getMaxTimeMS(), maxTime);
        }
    }

    auto timelineRange = _timelineChart->getRange();
    if (!_timelineChart->isRangeSet()) {
        _timelineChart->setInitialRange(false, minTime, maxTime);
    } else {
        if (minTime <= timelineRange.first)
            _timelineChart->setMin(minTime);
        if (maxTime >= timelineRange.second)
            _timelineChart->setMax(maxTime);
    }

    // if the timeline chart's range was changed, update the date/time axis' range
    if (timelineRange != _timelineChart->getRange()) {
        _dateTimeAxis->setRange(_timelineChart->getRange().first, _timelineChart->getRange().second, true);
    }
}


/**
 * @brief TimelineChartView::constructSeriesForEventKind
 * @param kind
 * @param ID
 * @param label
 * @return
 */
MEDEA::EventSeries* TimelineChartView::constructSeriesForEventKind(TIMELINE_DATA_KIND kind, QString ID, QString label)
{
    if (eventSeries.contains(ID)) {
        for (auto s : eventSeries.values(ID)) {
            if (kind == s->getKind())
                return s;
        }
    }

    MEDEA::EventSeries* series = 0;

    switch (kind) {
    case TIMELINE_DATA_KIND::PORT_LIFECYCLE: {
        series = new PortLifecycleEventSeries(ID, this);
        label += "_" + ID;
        break;
    }
    /*case TIMELINE_DATA_KIND::WORKLOAD:
        series = new WorkloadEventSeries(ID, this);
        break;
    case TIMELINE_DATA_KIND::CPU_UTILISATION:
        series = new CPUUtilisationEventSeries(ID, this);
        break;
    case TIMELINE_DATA_KIND::MEMORY_UTILISATION:
        series = new MemoryUtilisationEventSeries(ID, this);
        break;*/
    default:
        series = new MEDEA::EventSeries(this);
        break;
    }

    if (series) {
        // construct a chart for the new series
        constructChartForSeries(series, ID, label + GET_TIMELINE_DATA_KIND_STRING_SUFFIX(kind));
        eventSeries.insertMulti(ID, series);
    }

    return series;
}


/**
 * @brief TimelineChartView::constructChartForSeries
 * @param series
 * @param ID
 * @param label
 * @return
 */
EntityChart* TimelineChartView::constructChartForSeries(MEDEA::EventSeries* series, QString ID, QString label)
{
    if (!series)
        return 0;

    // un-comment this and comment out the line below if we want to paint multiple series with the same event series ID on the same chart
    /*if (eventEntityCharts.contains(ID)) {
        eventEntityCharts.value(ID)->addEventSeries(series);
        return eventEntityCharts.value(ID);
    }*/

    // can't use event series ID as the chart ID because multiple event series can share the same ID
    ID = series->getID();

    EntityChart* chart = new EntityChart(0, this);
    chart->addEventSeries(series);
    _timelineChart->addEntityChart(chart);
    eventEntityCharts[ID] = chart;

    EntitySet* set = new EntitySet(label, this);
    set->setMinimumHeight(MIN_ENTITY_HEIGHT);
    set->themeChanged(Theme::theme());
    _entityAxis->appendEntity(set);
    eventEntitySets[ID] = set;

    connect(set, &EntitySet::visibilityChanged, chart, &EntityChart::setVisible);
    connect(set, &EntitySet::hovered, [=] (bool hovered) {
        _timelineChart->setEntityChartHovered(chart, hovered);
    });

    // set the initial visibility states of each individual series in the chart
    for (auto& action : _legendActions.values()) {
        auto kind = _legendActions.key(action, TIMELINE_DATA_KIND::DATA);
        chart->setSeriesKindVisible(kind, action->isChecked());
    }

    connect(this, &TimelineChartView::seriesLegendHovered, chart, &EntityChart::seriesKindHovered);
    connect(this, &TimelineChartView::toggleSeriesLegend, chart, &EntityChart::setSeriesKindVisible);

    chart->addEventSeries(series);
    return chart;
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
    //set->setID(itemID);
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
    for (auto& action : _legendToolbar->actions()) {
        auto kind = _legendActions.key(action, TIMELINE_DATA_KIND::DATA);
        seriesChart->setSeriesKindVisible(kind, action->isChecked());
    }

    itemEntityCharts[itemID] = seriesChart;
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
        EntityChart* entityChart = itemEntityCharts.take(ID);
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
inline uint qHash(TIMELINE_DATA_KIND key, uint seed)
{
    return ::qHash(static_cast<uint>(key), seed);
}
