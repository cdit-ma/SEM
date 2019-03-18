#include "timelinechartview.h"
#include "timelinechart.h"
#include "entitychart.h"
#include "../Axis/axiswidget.h"
#include "../../../../theme.h"

#include "../../Data/Series/portlifecycleeventseries.h"
#include "../../Data/Series/workloadeventseries.h"
#include "../../Data/Series/cpuutilisationeventseries.h"
#include "../../Data/Series/memoryutilisationeventseries.h"
#include "../../Data/Series/markereventseries.h"

#include <QScrollBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>

#define MIN_ENTITY_HEIGHT 50
#define SCROLLBAR_WIDTH 20
#define AXIS_LINE_WIDTH 2
#define ZOOM_FACTOR 1.025
#define SPACING 5
#define OPACITY 0.2

#define HOVER_DISPLAY_ON true
#define HOVER_DISPLAY_ITEM_COUNT 10

#define EXPERIMENT_RUN_ID "experimentRunID"
#define EXPERIMENT_RUN_START_TIME "experimentRunStartTime"

/**
 * @brief TimelineChartView::TimelineChartView
 * @param parent
 */
TimelineChartView::TimelineChartView(QWidget* parent)
    : QWidget(parent)
{
    setupLayout();

    connect(Theme::theme(), &Theme::theme_Changed, this, &TimelineChartView::themeChanged);
    themeChanged();

    // initialise stored ranges
    longestExperimentRunDuration_ = {0, INT64_MIN};
    totalTimeRange_ = {INT64_MAX, INT64_MIN};
}


/**
 * @brief TimelineChartView::eventFilter
 * This catches the legend toolbar actions' hover enter/leave events.
 * It sends a signal indicating which series kind is being hovered over.
 * @param watched
 * @param event
 * @return
 */
bool TimelineChartView::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::HoverEnter || event->type() == QEvent::HoverLeave) {
        if (!watched->property("checked").toBool())
            return false;
        auto kind = TIMELINE_DATA_KIND::DATA;
        if (event->type() == QEvent::HoverEnter) {
            kind = (TIMELINE_DATA_KIND) watched->property("TIMELINE_DATA_KIND").toUInt();
        }
        emit seriesLegendHovered(kind);
        return true;
    }
    return QWidget::eventFilter(watched, event);
}


/**
 * @brief TimelineChartView::addPortLifecycleEvents
 * @param experimentRun
 * @param events
 */
void TimelineChartView::addPortLifecycleEvents(const ExperimentRun &experimentRun, const QVector<PortLifecycleEvent*>& events)
{
    if (events.isEmpty())
        return;

    QList<MEDEA::EventSeries*> clearedSeries;
    for (auto event : events) {
        auto series = constructSeriesForEventKind(experimentRun, TIMELINE_DATA_KIND::PORT_LIFECYCLE, event->getID(), event->getName());
        if (series) {
            if (!clearedSeries.contains(series)) {
                series->clear();
                clearedSeries.append(series);
                //qDebug() << "New series: " << event->getName();
            }
            series->addEvent(event);
        }
    }

    addedChartEvents(TIMELINE_DATA_KIND::PORT_LIFECYCLE, experimentRun);
}


/**
 * @brief TimelineChartView::addWorkloadEvents
 * @param experimentRun
 * @param events
 */
void TimelineChartView::addWorkloadEvents(const ExperimentRun &experimentRun, const QVector<WorkloadEvent *> &events)
{
    if (events.isEmpty())
        return;

    QList<MEDEA::EventSeries*> clearedSeries;
    for (auto event : events) {
        auto series = constructSeriesForEventKind(experimentRun, TIMELINE_DATA_KIND::WORKLOAD, event->getID(), event->getName());
        if (series) {
            if (!clearedSeries.contains(series)) {
                series->clear();
                clearedSeries.append(series);
            }
            series->addEvent(event);
        }
    }

    addedChartEvents(TIMELINE_DATA_KIND::WORKLOAD, experimentRun);
}


/**
 * @brief TimelineChartView::addCPUUtilisationEvents
 * @param experimentRun
 * @param events
 */
void TimelineChartView::addCPUUtilisationEvents(const ExperimentRun &experimentRun, const QVector<CPUUtilisationEvent *> &events)
{
    if (events.isEmpty())
        return;

    QList<MEDEA::EventSeries*> clearedSeries;
    for (auto event : events) {
        auto series = constructSeriesForEventKind(experimentRun, TIMELINE_DATA_KIND::CPU_UTILISATION, event->getID(), event->getName());
        if (series) {
            if (!clearedSeries.contains(series)) {
                series->clear();
                clearedSeries.append(series);
            }
            series->addEvent(event);
        }
    }

    addedChartEvents(TIMELINE_DATA_KIND::CPU_UTILISATION, experimentRun);
}


/**
 * @brief TimelineChartView::addMemoryUtilisationEvents
 * @param experimentRun
 * @param events
 */
void TimelineChartView::addMemoryUtilisationEvents(const ExperimentRun &experimentRun, const QVector<MemoryUtilisationEvent *> &events)
{
    if (events.isEmpty())
        return;

    QList<MEDEA::EventSeries*> clearedSeries;
    for (auto event : events) {
        auto series = constructSeriesForEventKind(experimentRun, TIMELINE_DATA_KIND::MEMORY_UTILISATION, event->getID(), event->getName());
        if (series) {
            if (!clearedSeries.contains(series)) {
                series->clear();
                clearedSeries.append(series);
            }
            series->addEvent(event);
        }
    }

    addedChartEvents(TIMELINE_DATA_KIND::MEMORY_UTILISATION, experimentRun);
}


/**
 * @brief TimelineChartView::addMarkerEvents
 * @param experimentRun
 * @param events
 */
void TimelineChartView::addMarkerEvents(const ExperimentRun &experimentRun, const QVector<MarkerEvent *> &events)
{
    if (events.isEmpty())
        return;

    QList<MEDEA::EventSeries*> clearedSeries;
    for (auto event : events) {
        auto series = constructSeriesForEventKind(experimentRun, TIMELINE_DATA_KIND::MARKER, event->getID(), event->getName());
        if (series) {
            if (!clearedSeries.contains(series)) {
                series->clear();
                clearedSeries.append(series);
            }
            series->addEvent(event);
        }
    }

    addedChartEvents(TIMELINE_DATA_KIND::MARKER, experimentRun);
}


/**
 * @brief TimelineChartView::updateExperimentRunLastUpdatedTime
 * @param experimentRunID
 * @param time
 */
void TimelineChartView::updateExperimentRunLastUpdatedTime(const quint32 experimentRunID, const qint64 time)
{
    if (experimentRunTimeRange_.contains(experimentRunID)) {
        updateRangeForExperimentRun(experimentRunID, experimentRunTimeRange_[experimentRunID].first, time);
    }
}


/**
 * @brief TimelineChartView::clearTimelineChart
 * Clear/delete all the axis items and entity charts and reset the timeline range.
 */
void TimelineChartView::clearTimelineChart()
{
    auto chartItr = eventEntityCharts.begin();
    while (chartItr != eventEntityCharts.end()) {
        auto chartID = eventEntityCharts.key(*chartItr, "");
        removeChart(chartID, true);
        chartItr = eventEntityCharts.erase(chartItr);
    }

    // show empty label
    mainWidget_->hide();
    emptyLabel_->show();

    // clear stored ranges
    totalTimeRange_ = {INT64_MAX, INT64_MIN};
    longestExperimentRunDuration_ = {0, INT64_MIN};
    experimentRunTimeRange_.clear();
    experimentRunSeriesCount_.clear();
    rangeSet = false;
}


/**
 * @brief TimelineChartView::updateTimelineChart
 */
void TimelineChartView::updateTimelineChart()
{
    _timelineChart->update();
}


/**
 * @brief TimelineChartView::setTimeDisplayFormat
 * @param format
 */
void TimelineChartView::setTimeDisplayFormat(const TIME_DISPLAY_FORMAT format)
{
    _dateTimeAxis->setDisplayFormat(format);
    timeDisplayFormat_ = format;
    updateTimelineRange();
    update();
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

    setStyleSheet("QScrollBar:vertical {"
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

    _timelineChart->setStyleSheet("background: " + Theme::QColorToHex(bgColor) + ";");

    _legendToolbar->setFixedHeight(theme->getLargeIconSize().height());
    _legendToolbar->setStyleSheet(theme->getToolTipStyleSheet() +
                                 theme->getToolBarStyleSheet() +
                                 "QToolButton{ border: 0px; color:" + theme->getTextColorHex(ColorRole::DISABLED) + ";}"
                                 "QToolButton::checked:!hover{ color:" + theme->getTextColorHex() + ";}"
                                 "QToolButton:!hover{ background: rgba(0,0,0,0); }");

    for (auto action : _legendToolbar->actions()) {
        auto widget = _legendToolbar->widgetForAction(action);
        widget->setMinimumSize(theme->getLargeIconSize());
    }

    for (auto kind : GET_TIMELINE_DATA_KINDS()) {
        QIcon buttonIcon;
        switch (kind) {
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

    emptyLabel_->setFont(QFont(theme->getFont().family(), 12));
    emptyLabel_->setStyleSheet("background: rgba(0,0,0,0);"
                               "color:" + theme->getTextColorHex(ColorRole::DISABLED) + ";"
                               "border: 1px solid " + theme->getDisabledBackgroundColorHex() + ";");

    mainWidget_->setStyleSheet("QWidget #MAIN_WIDGET{ border: 1px solid " + theme->getDisabledBackgroundColorHex() + ";}");
}


/**
 * @brief TimelineChartView::toggledSeriesLegend
 * This is called when the legend toolbar has been triggered
 * It shows/hides the charts and axis items that match the toggled data kind
 * @param checked
 */
void TimelineChartView::toggledSeriesLegend(bool checked)
{
    if (!sender())
        return;

    auto kind = (TIMELINE_DATA_KIND) sender()->property("TIMELINE_DATA_KIND").toUInt();
    for (auto series : eventSeries) {
        if (series->getKind() != kind)
            continue;
        auto ID = series->getEventSeriesID();
        auto chart = eventEntityCharts.value(ID, 0);
        if (chart)
            chart->setVisible(checked);
        auto set = eventEntitySets.value(ID, 0);
        if (set)
            set->setVisible(checked);
    }

    sender()->setProperty("checked", checked);
    emit seriesLegendHovered(checked ? kind : TIMELINE_DATA_KIND::DATA);
}


/**
 * @brief TimelineChartView::entityAxisSizeChanged
 * This is called when the entity axis is resized
 * It resizes the invisible filler widgets above and below it to match its width
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

    auto minTimeAxisWidth = fontMetrics().width(QDateTime::fromMSecsSinceEpoch(0).toString(TIME_FORMAT));
    setMinimumWidth(size.width() + minTimeAxisWidth + SPACING * 2);
}


/**
 * @brief TimelineChartView::entityChartHovered
 * All this slot does is set the corresponding axis item's hovered state to hovered
 * @param chart
 * @param hovered
 */
void TimelineChartView::entityChartHovered(EntityChart* chart, bool hovered)
{
    if (!chart)
        return;

    QString path = eventEntityCharts.key(chart, "");
    EntitySet* set = eventEntitySets.value(path, 0);
    if (set)
        set->setHovered(hovered);
}


/**
 * @brief TimelineChartView::entitySetClosed
 * This is called when a user has closed a chart (entity set/chart)
 */
void TimelineChartView::entitySetClosed()
{
    auto set = qobject_cast<EntitySet*>(sender());
    if (set) {
        removeChart(eventEntitySets.key(set, ""));
    }
}


/**
 * @brief TimelineChartView::updateHoverDisplay
 * This is called whenever the hover line is moved
 * It checks to see which chart is hovered and gets the hovered data from that chart
 */
void TimelineChartView::updateHoverDisplay()
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
            if (!action || !action->isChecked())
                continue;
            if (!hoveredKinds.contains(kind))
                continue;
            auto hoveredRange = entityChart->getHoveredTimeRange(kind);
            auto hoveredInfo = s->getHoveredDataString(hoveredRange, HOVER_DISPLAY_ITEM_COUNT, DATE_TIME_FORMAT);
            if (!hoveredInfo.isEmpty())
                hoveredData[kind] += hoveredInfo + "\n";
        }
    }

    if (hoveredData.isEmpty())
        return;

    for (auto kind : _hoverDisplayButtons.keys()) {
        auto button = _hoverDisplayButtons.value(kind, 0);
        if (!button)
            continue;
        auto hasData = hoveredData.contains(kind);
        button->setVisible(hasData);
        if (hasData) {
            auto data = hoveredData.value(kind);
            button->setText(data.trimmed());
        }
    }

    // adjust the hover's size before calculating its position
    _hoverDisplay->adjustChildrenSize();

    auto globalPos = mapToGlobal(pos());
    auto hoverPos = mapTo(this, cursor().pos()) - QPoint(0, _hoverDisplay->height() / 2.0);
    auto bottom = globalPos.y() + height() - _dateTimeAxis->height();

    // adjust the hover display's position to make sure that it is fully visible
    if (hoverPos.x() >= (globalPos.x() + width() / 2.0)) {
        hoverPos.setX(hoverPos.x() - _hoverDisplay->width() - 25);
    } else {
        hoverPos.setX(hoverPos.x() + 25);
    }
    if ((hoverPos.y() + _hoverDisplay->height()) > bottom) {
        hoverPos.setY(bottom - _hoverDisplay->height());
    } else if (hoverPos.y() < globalPos.y()){
        hoverPos.setY(globalPos.y());
    }

    _hoverDisplay->move(hoverPos);
    _hoverDisplay->show();
}


/**
 * @brief TimelineChartView::minSliderMoved
 * @param ratio
 */
void TimelineChartView::minSliderMoved(const double ratio)
{
    for (auto chart : eventEntityCharts) {
        chart->setDisplayMinRatio(ratio);
        chart->updateBinnedData();
    }
}


/**
 * @brief TimelineChartView::maxSliderMoved
 * @param ratio
 */
void TimelineChartView::maxSliderMoved(const double ratio)
{
    for (auto chart : eventEntityCharts) {
        chart->setDisplayMaxRatio(ratio);
        chart->updateBinnedData();
    }
}


/**
 * @brief TimelineChartView::timelineZoomed
 * @param delta
 */
void TimelineChartView::timelineZoomed(const int delta)
{
    double factor = delta < 0 ? ZOOM_FACTOR : 1 / ZOOM_FACTOR;
    _dateTimeAxis->zoom(factor);
}


/**
 * @brief TimelineChartView::timelinePanned
 * @param dx
 * @param dy
 */
void TimelineChartView::timelinePanned(const double dx, const double dy)
{
    auto displayRange = _dateTimeAxis->getDisplayedRange();
    auto actualRange = _dateTimeAxis->getRange();
    auto ratio = (displayRange.second - displayRange.first) / (actualRange.second - actualRange.first);
    _dateTimeAxis->pan(dx * ratio, dy * ratio);
}


/**
 * @brief TimelineChartView::timelineRubberbandUsed
 * @param left
 * @param right
 */
void TimelineChartView::timelineRubberbandUsed(double left, double right)
{
    // make sure that min < max
    if (left > right) {
        auto temp = right;
        right = left;
        left = temp;
    }

    // keep min/max within the bounds
    auto timelineWidth = (double)_timelineChart->width();
    left = qMax(left, 0.0);
    right = qMin(right, timelineWidth);

    // set the new display min/max
    auto minRatio = left / timelineWidth;
    auto maxRatio = right / timelineWidth;
    auto displayRange = _dateTimeAxis->getDisplayedRange();
    auto displayDist = displayRange.second - displayRange.first;
    auto min = displayDist * minRatio + displayRange.first;
    auto max = displayDist * maxRatio + displayRange.first;
    _dateTimeAxis->setDisplayRange(min, max);

    // update the entity charts' display range
    auto actualRange = _dateTimeAxis->getRange();
    auto actualDist = actualRange.second - actualRange.first;
    minRatio = actualDist > 0 ? (min - actualRange.first) / actualDist : 0.0;
    maxRatio = actualDist > 0 ? (max - actualRange.first) / actualDist : 0.0;
    for (auto chart : eventEntityCharts) {
        chart->setDisplayRangeRatio(minRatio, maxRatio);
        chart->updateBinnedData();
    }
}


/**
 * @brief TimelineChartView::addedChartEvents
 * @param kind
 * @param experimentRun
 */
void TimelineChartView::addedChartEvents(const TIMELINE_DATA_KIND kind, const ExperimentRun &experimentRun)
{
    auto experimentRunID = experimentRun.experiment_run_id;
    auto experimentRunStartTime = experimentRun.start_time;
    auto experimentInfo = "Experiment name:\t" + experimentRun.experiment_name +
                          "\nJob number#:\t" + QString::number(experimentRun.job_num) +
                          "\nStarted at:\t" + QDateTime::fromMSecsSinceEpoch(experimentRun.start_time).toString(DATE_TIME_FORMAT);

    updateRangeForExperimentRun(experimentRunID, experimentRunStartTime, experimentRun.last_updated_time);

    // set the experiment info as the chart's tooltip
    for (auto ID : eventEntityCharts.keys()) {
        auto chart = eventEntityCharts.value(ID);
        if (!chart || (chart->getExperimentRunID() != experimentRunID))
            continue;
        if (eventEntitySets.contains(ID)) {
            eventEntitySets.value(ID)->setToolTip(experimentInfo);
        }
    }
}


/**
 * @brief TimelineChartView::constructSeriesForEventKind
 * @param experimentRunID
 * @param kind
 * @param ID
 * @param label
 * @return
 */
MEDEA::EventSeries* TimelineChartView::constructSeriesForEventKind(const ExperimentRun& experimentRun, const TIMELINE_DATA_KIND kind, const QString &ID, const QString &label)
{
    auto experimentRunID = experimentRun.experiment_run_id;
    auto seriesID = ID + QString::number(experimentRunID);

    if (eventSeries.contains(seriesID)) {
        for (auto s : eventSeries.values(seriesID)) {
            if (s && s->getKind() == kind)
                return s;
        }
    }

    MEDEA::EventSeries* series = 0;
    auto seriesLabel = label;

    //qDebug() << "seriesID: " << seriesID;
    //qDebug() << "label: " << label;

    switch (kind) {
    case TIMELINE_DATA_KIND::PORT_LIFECYCLE: {
        auto strList = seriesID.split("_");
        seriesLabel += "_" + strList.first();
        series = new PortLifecycleEventSeries(seriesID, this);
        break;
    }
    case TIMELINE_DATA_KIND::WORKLOAD: {
        auto strList = seriesID.split("_");
        seriesLabel += "_" + strList.first();
        series = new WorkloadEventSeries(seriesID, this);
        break;
    }
    case TIMELINE_DATA_KIND::CPU_UTILISATION:
        series = new CPUUtilisationEventSeries(seriesID, this);
        break;
    case TIMELINE_DATA_KIND::MEMORY_UTILISATION:
        series = new MemoryUtilisationEventSeries(seriesID, this);
        break;
    case TIMELINE_DATA_KIND::MARKER:
        series = new MarkerEventSeries(seriesID, this);
    default:
        return 0;
    }

    if (series) {
        // this needs to be set before the chart is constructed
        series->setProperty(EXPERIMENT_RUN_ID, experimentRunID);
        series->setProperty(EXPERIMENT_RUN_START_TIME, experimentRun.start_time);
        constructChartForSeries(series, seriesID, seriesLabel + GET_TIMELINE_DATA_KIND_STRING_SUFFIX(kind));
        eventSeries.insert(seriesID, series);
        experimentRunSeriesCount_[experimentRunID]++;
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
EntityChart* TimelineChartView::constructChartForSeries(MEDEA::EventSeries *series, const QString &ID, const QString &label)
{
    //qDebug() << "construct chart with label: " << label;
    //qDebug() << "-----------------";

    if (!series)
        return 0;

    // NOTE - At the moment, a new entity chart is constructed per series
    // un-comment this and comment out the line below if we want to paint multiple series with that share an ID on the same chart
    /*if (eventEntityCharts.contains(ID)) {
        eventEntityCharts.value(ID)->addEventSeries(series);
        return eventEntityCharts.value(ID);
    }*/

    // can't use event series ID as the chart ID because multiple event series can share the same ID
    auto seriesID = series->getEventSeriesID();
    auto experimentRunID = series->property(EXPERIMENT_RUN_ID).toUInt();
    auto experimentRunStartTime = (qint64) series->property(EXPERIMENT_RUN_START_TIME).toLongLong();
    auto seriesLabel = "[" + QString::number(experimentRunID) + "] " + label;

    EntityChart* chart = new EntityChart(experimentRunID, experimentRunStartTime, this);
    chart->addSeries(series);
    _timelineChart->addEntityChart(chart);
    eventEntityCharts[seriesID] = chart;

    EntitySet* set = new EntitySet(seriesLabel, this);
    set->setMinimumHeight(MIN_ENTITY_HEIGHT);
    set->themeChanged(Theme::theme());
    _entityAxis->appendEntity(set);
    eventEntitySets[seriesID] = set;

    connect(this, &TimelineChartView::seriesLegendHovered, chart, &EntityChart::seriesKindHovered);
    connect(set, &EntitySet::visibilityChanged, chart, &EntityChart::setVisible);
    connect(set, &EntitySet::closeEntity, this, &TimelineChartView::entitySetClosed);
    connect(set, &EntitySet::hovered, [=] (bool hovered) {
        _timelineChart->setEntityChartHovered(chart, hovered);
    });

    // set the initial visibility state of the chart/axis item
    for (auto& action : _legendActions.values()) {
        auto kind = _legendActions.key(action, TIMELINE_DATA_KIND::DATA);
        if (kind == series->getKind()) {
            chart->setSeriesKindVisible(kind, true);
            chart->setVisible(action->isChecked());
            set->setVisible(action->isChecked());
        }
    }

    if (mainWidget_->isHidden()) {
        mainWidget_->show();
        emptyLabel_->hide();
    }

    return chart;
}


/**
 * @brief TimelineChartView::removeChart
 * This removes the chart with the specified ID from the timeline
 * It also removes the corresponding axis item and contained series from the timeline and their hashes
 * @param ID
 * @param clearing
 */
void TimelineChartView::removeChart(const QString &ID, bool clearing)
{
    // TODO - This needs to change if multiple series are allowed to be displayed in one entity chart
    // NOTE - At the moment there should be a chart per series, hence a chart should only have one series
    // ID is the chart's key, which is also the event series ID of the series it contains

    // remove chart series
    for (auto series : eventSeries) {
        if (series->getEventSeriesID() == ID) {
            auto key = eventSeries.key(series);
            eventSeries.remove(key, series);
            series->deleteLater();
            break;
        }
    }

    // remove chart
    auto chart = eventEntityCharts.value(ID, 0);
    if (chart) {
        _timelineChart->removeEntityChart(chart);
        chart->deleteLater();
        if (!clearing) {
            eventEntityCharts.remove(ID);
        }
    }

    // remove entity set
    auto set = eventEntitySets.value(ID, 0);
    if (set) {
        auto childrenSets = set->getChildrenEntitySets();
        if (!childrenSets.isEmpty()) {
            // remove/delete entity set's children sets
            auto childItr = childrenSets.begin();
            while (childItr != childrenSets.end()) {
                (*childItr)->deleteLater();
                childItr = childrenSets.erase(childItr);
            }
        }
        _entityAxis->removeEntity(set);
        eventEntitySets.remove(ID);
        set->deleteLater();
    }

    if (!clearing) {
        // check if the timeline range needs updating
        auto expRunID = chart->getExperimentRunID();
        if (experimentRunSeriesCount_.contains(expRunID)) {
            experimentRunSeriesCount_[expRunID]--;
            removedDataFromExperimentRun(expRunID);
        }
        // if there are no more charts, show empty label
        if (eventEntityCharts.isEmpty()) {
            mainWidget_->hide();
            emptyLabel_->show();
        }
    }

    // clear the timeline chart's hovered rect
    _timelineChart->setEntityChartHovered(0, false);
}


/**
 * @brief TimelineChartView::updateRangeForExperimentRun
 * This is called when new experiment data is received
 * It checks if the timeline range needs updating
 * @param experimentRunID
 * @param startTime
 * @param lastUpdatedTime
 */
void TimelineChartView::updateRangeForExperimentRun(const quint32 experimentRunID, const qint64 startTime, const qint64 lastUpdatedTime)
{
    experimentRunTimeRange_[experimentRunID] = {startTime, lastUpdatedTime};

    auto duration = lastUpdatedTime - startTime;
    if (duration > longestExperimentRunDuration_.second) {
        longestExperimentRunDuration_= {experimentRunID, duration};
    }

    totalTimeRange_ = {qMin(startTime, totalTimeRange_.first), qMax(lastUpdatedTime, totalTimeRange_.second)};
    updateTimelineRange(); // TODO - only update the range when it's been changed
}


/**
 * @brief TimelineChartView::removedDataFromExperimentRun
 * This is called when experiment data is removed
 * If the last series from the specified experiment run is removed, it recalculates the timeline's range
 * @param experimentRunID
 */
void TimelineChartView::removedDataFromExperimentRun(const quint32 experimentRunID)
{
    auto seriesCount =  experimentRunSeriesCount_.value(experimentRunID, 0);
    if (seriesCount > 0)
        return;

    experimentRunSeriesCount_.remove(experimentRunID);
    experimentRunTimeRange_.remove(experimentRunID);

    // recalculate the longest experiment run duration and the total range
    auto min = INT64_MAX;
    auto max = INT64_MIN;
    auto updateLongestDuration = experimentRunID != longestExperimentRunDuration_.first;
    if (updateLongestDuration) {
        longestExperimentRunDuration_.second = INT64_MIN;
    }

    for (auto id : experimentRunTimeRange_.keys()) {
        auto val = experimentRunTimeRange_.value(id);
        min = qMin(min, val.first);
        max = qMax(max, val.second);
        if (updateLongestDuration) {
            auto range = val.second - val.first;
            if (range > longestExperimentRunDuration_.second) {
                longestExperimentRunDuration_ = {id, range};
            }
        }
    }

    totalTimeRange_ = {min, max};
    updateTimelineRange();
}


/**
 * @brief TimelineChartView::updateTimelineRange
 * This updates the timeline's range based on the time display mode
 * @param updateDisplayRange
 */
void TimelineChartView::updateTimelineRange(bool updateDisplayRange)
{
    /*
     * TODO - Refactor so that the total range can be changed without affecting the display range
     */
    auto startTime = totalTimeRange_.first;
    auto duration = totalTimeRange_.second - totalTimeRange_.first;

    switch (timeDisplayFormat_) {
    case TIME_DISPLAY_FORMAT::ELAPSED_TIME: {
        startTime = 0.0;
        duration = longestExperimentRunDuration_.second;
        break;
    }
    default:
        break;
    }

    for (auto chart : eventEntityCharts) {
        chart->updateRange(startTime, duration);
    }

    _dateTimeAxis->setRange(startTime, startTime + duration, updateDisplayRange);
}


/**
 * @brief TimelineChartView::setupLayout
 */
void TimelineChartView::setupLayout()
{
    /*
     * CHART/AXES - Note: The axis lines are on by default for both axes.
     * The timeline chart can draw its own axis lines but is off by default.
     */
    _entityAxis = new EntityAxis(this);
    _entityAxis->setAxisLineVisible(false);
    connect(_entityAxis, &EntityAxis::sizeChanged, this, &TimelineChartView::entityAxisSizeChanged);

    _dateTimeAxis = new AxisWidget(Qt::Horizontal, Qt::AlignBottom, this, VALUE_TYPE::DATE_TIME);
    _dateTimeAxis->setZoomFactor(ZOOM_FACTOR);

    connect(_dateTimeAxis, &AxisWidget::minRatioChanged, this, &TimelineChartView::minSliderMoved);
    connect(_dateTimeAxis, &AxisWidget::maxRatioChanged, this, &TimelineChartView::maxSliderMoved);

    _timelineChart = new TimelineChart(this);
    _timelineChart->setAxisWidth(AXIS_LINE_WIDTH);
    _timelineChart->setAxisYVisible(true);

    connect(_timelineChart, &TimelineChart::panning, _dateTimeAxis, &AxisWidget::setPanning);
    connect(_timelineChart, &TimelineChart::hoverLineUpdated, _dateTimeAxis, &AxisWidget::hoverLineUpdated);

    connect(_timelineChart, &TimelineChart::zoomed, this, &TimelineChartView::timelineZoomed);
    connect(_timelineChart, &TimelineChart::panned, this, &TimelineChartView::timelinePanned);
    connect(_timelineChart, &TimelineChart::rubberbandUsed, this, &TimelineChartView::timelineRubberbandUsed);
    connect(_timelineChart, &TimelineChart::entityChartHovered, this, &TimelineChartView::entityChartHovered);
    if (HOVER_DISPLAY_ON) {
        connect(_timelineChart, &TimelineChart::hoverLineUpdated, this, &TimelineChartView::updateHoverDisplay);
    }

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
    _hoverWidget->setStyleSheet("background: rgba(0,0,0,0);");

    QVBoxLayout* hoverLayout = new QVBoxLayout(_hoverWidget);
    hoverLayout->setSpacing(SPACING * 2);
    hoverLayout->setMargin(SPACING);

    _hoverDisplay = new HoverPopup(this);
    _hoverDisplay->setWidget(_hoverWidget);

    /*
     * HOVER AND LEGEND TIMELINE_DATA_KIND WIDGETS
     */
    for (auto kind : GET_TIMELINE_DATA_KINDS()) {
        if (kind == TIMELINE_DATA_KIND::DATA)
            continue;

        // construct legend widgets
        QAction* action = _legendToolbar->addAction(GET_TIMELINE_DATA_KIND_STRING(kind));
        _legendActions[kind] = action;
        action->setToolTip("Show/Hide " + action->text() + " Series");
        action->setCheckable(true);
        action->setChecked(true);
        //action->setVisible(false);
        action->setProperty("TIMELINE_DATA_KIND", (uint)kind);
        connect(action, &QAction::toggled, this, &TimelineChartView::toggledSeriesLegend);

        QWidget* actionWidget = _legendToolbar->widgetForAction(action);
        actionWidget->setProperty("TIMELINE_DATA_KIND", (uint)kind);
        actionWidget->installEventFilter(this);

        // construct hover display widgets
        QPushButton* button = new QPushButton(this);
        button->setStyleSheet("QPushButton{ text-align: left; }");
        hoverLayout->addWidget(button);
        _hoverDisplayButtons[kind] = button;
    }

    /*
     * MID (SCROLL AREA) LAYOUT
     */
    QWidget* scrollWidget = new QWidget(this);
    QHBoxLayout* scrollLayout = new QHBoxLayout(scrollWidget);
    scrollLayout->setMargin(0);
    scrollLayout->setSpacing(0);
    scrollLayout->addWidget(_timelineChart, 1);
    scrollLayout->addWidget(_entityAxis);

    _scrollArea = new QScrollArea(this);
    _scrollArea->setWidget(scrollWidget);
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
    //emptyLabel_ = new QLabel("<i>No Charts To Display</i>", this);
    emptyLabel_ = new QLabel("No Charts To Display", this);
    emptyLabel_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    emptyLabel_->setAlignment(Qt::AlignCenter);

    mainWidget_ = new QWidget(this);
    mainWidget_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainWidget_->setVisible(false);
    mainWidget_->setObjectName("MAIN_WIDGET");

    QVBoxLayout* mainLayout = new QVBoxLayout(mainWidget_);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(SPACING, SPACING, SPACING, SPACING);
    mainLayout->addLayout(topLayout);
    mainLayout->addSpacerItem(new QSpacerItem(0, SPACING));
    mainLayout->addWidget(_scrollArea, 1);
    mainLayout->addLayout(bottomLayout);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(mainWidget_);
    layout->addWidget(emptyLabel_);

    _scrollArea->verticalScrollBar()->setTracking(true);
    connect(_scrollArea->verticalScrollBar(), &QScrollBar::valueChanged, [=]() {
        verticalScrollValue = _scrollArea->verticalScrollBar()->value();
    });

    auto minTimeAxisWidth = fontMetrics().width(QDateTime::fromMSecsSinceEpoch(0).toString(TIME_FORMAT));
    setMinimumWidth(_entityAxis->minimumWidth() + minTimeAxisWidth + SPACING * 2);
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
