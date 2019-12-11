#include "timelinechartview.h"
#include "chartlist.h"
#include "chart.h"
#include "../Axis/axiswidget.h"
#include "../../../../theme.h"

#include "../../Data/Series/portlifecycleeventseries.h"
#include "../../Data/Series/workloadeventseries.h"
#include "../../Data/Series/cpuutilisationeventseries.h"
#include "../../Data/Series/memoryutilisationeventseries.h"
#include "../../Data/Series/markereventseries.h"
#include "../../Data/Series/porteventseries.h"
#include "../../Data/Series/networkutilisationeventseries.h"

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

#define CHART_DATA_KIND "ChartDataKind"
#define EXPERIMENT_RUN_ID "experimentRunID"
#define EXPERIMENT_RUN_START_TIME "experimentRunStartTime"

using namespace MEDEA;

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
        auto kind = MEDEA::ChartDataKind::DATA;
        if (event->type() == QEvent::HoverEnter) {
            kind = static_cast<MEDEA::ChartDataKind>(watched->property(CHART_DATA_KIND).toUInt());
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
 * @throws std::invalid_argument
 */
void TimelineChartView::addPortLifecycleEvents(const AggServerResponse::ExperimentRun& experimentRun, const QVector<PortLifecycleEvent*>& events)
{
    /*
    for (const auto& event : events) {
        const auto& series_id = event->getSeriesID();
        auto series = getSeriesForEventKind(MEDEA::ChartDataKind::PORT_LIFECYCLE, experimentRun, series_id);
        if (series == nullptr) {
            const auto& series_name = event->getSeriesName();
            series = constructSeriesForEventKind(MEDEA::ChartDataKind::PORT_LIFECYCLE, experimentRun, series_id, series_name);
        }
        series->addEvent(event);
    }
    */
    for (const auto& event : events) {
        addEvent(MEDEA::ChartDataKind::PORT_LIFECYCLE, experimentRun, event);
    }
    addedEvents(experimentRun);
}


/**
 * @brief TimelineChartView::addWorkloadEvents
 * @param experimentRun
 * @param events
 * @throws std::invalid_argument
 */
void TimelineChartView::addWorkloadEvents(const AggServerResponse::ExperimentRun& experimentRun, const QVector<WorkloadEvent*>& events)
{
    for (const auto& event : events) {
        addEvent(MEDEA::ChartDataKind::WORKLOAD, experimentRun, event);
    }
    addedEvents(experimentRun);
}


/**
 * @brief TimelineChartView::addCPUUtilisationEvents
 * @param experimentRun
 * @param events
 * @throws std::invalid_argument
 */
void TimelineChartView::addCPUUtilisationEvents(const AggServerResponse::ExperimentRun &experimentRun, const QVector<CPUUtilisationEvent *> &events)
{
    for (const auto& event : events) {
        addEvent(MEDEA::ChartDataKind::CPU_UTILISATION, experimentRun, event);
    }
    addedEvents(experimentRun);
}


/**
 * @brief TimelineChartView::addMemoryUtilisationEvents
 * @param experimentRun
 * @param events
 * @throws std::invalid_argument
 */
void TimelineChartView::addMemoryUtilisationEvents(const AggServerResponse::ExperimentRun &experimentRun, const QVector<MemoryUtilisationEvent *> &events)
{
    for (const auto& event : events) {
        addEvent(MEDEA::ChartDataKind::MEMORY_UTILISATION, experimentRun, event);
    }
    addedEvents(experimentRun);
}


/**
 * @brief TimelineChartView::addMarkerEvents
 * @param experimentRun
 * @param events
 * @throws std::invalid_argument
 */
void TimelineChartView::addMarkerEvents(const AggServerResponse::ExperimentRun &experimentRun, const QVector<MarkerEvent *> &events)
{
    for (const auto& event : events) {
        addEvent(MEDEA::ChartDataKind::MARKER, experimentRun, event);
    }
    addedEvents(experimentRun);
}


/**
 * @brief TimelineChartView::addPortEvents
 * @param experimentRun
 * @param events
 * @throws std::invalid_argument
 */
void TimelineChartView::addPortEvents(const AggServerResponse::ExperimentRun& experimentRun, const QVector<PortEvent*>& events)
{
    for (const auto& event : events) {
        addEvent(MEDEA::ChartDataKind::PORT_EVENT, experimentRun, event);
    }
    addedEvents(experimentRun);
}


/**
 * @brief TimelineChartView::addNetworkUtilisationEvents
 * @param experimentRun
 * @param events
 * @throws std::invalid_argument
 */
void TimelineChartView::addNetworkUtilisationEvents(const AggServerResponse::ExperimentRun& experimentRun, const QVector<NetworkUtilisationEvent*>& events)
{
    for (const auto& event : events) {
        addEvent(MEDEA::ChartDataKind::NETWORK_UTILISATION, experimentRun, event);
    }
    addedEvents(experimentRun);
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
 * @brief TimelineChartView::clearChartList
 * Clear/delete all the axis items and entity charts and reset the timeline range.
 */
void TimelineChartView::clearChartList()
{
    auto chartItr = charts_.begin();
    while (chartItr != charts_.end()) {
        auto chartID = charts_.key(*chartItr, "");
        removeChart(chartID, true);
        chartItr = charts_.erase(chartItr);
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
 * @brief TimelineChartView::updateChartList
 */
void TimelineChartView::updateChartList()
{
    chartList_->update();
}


/**
 * @brief TimelineChartView::setTimeDisplayFormat
 * @param format
 */
void TimelineChartView::setTimeDisplayFormat(const TIME_DISPLAY_FORMAT format)
{
    timelineAxis_->setDisplayFormat(format);
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

    setStyleSheet(theme->getScrollBarStyleSheet());

    legendToolbar_->setFixedHeight(theme->getLargeIconSize().height());
    legendToolbar_->setStyleSheet(theme->getToolTipStyleSheet() +
                                  theme->getToolBarStyleSheet() +
                                  "QToolButton{ border: 0px; color:" + theme->getTextColorHex(ColorRole::DISABLED) + ";}"
                                  "QToolButton::checked:!hover{ color:" + theme->getTextColorHex() + ";}"
                                  "QToolButton:!hover{ background: rgba(0,0,0,0); }");

    for (auto action : legendToolbar_->actions()) {
        auto widget = legendToolbar_->widgetForAction(action);
        widget->setMinimumSize(theme->getLargeIconSize());
    }

    for (auto kind : MEDEA::Event::GetChartDataKinds()) {
        QIcon buttonIcon;
        switch (kind) {
        case MEDEA::ChartDataKind::PORT_EVENT:
        case MEDEA::ChartDataKind::PORT_LIFECYCLE:
            buttonIcon = theme->getIcon("ToggleIcons", "portLifecycleHover");
            break;
        case MEDEA::ChartDataKind::WORKLOAD:
            buttonIcon = theme->getIcon("ToggleIcons", "workloadHover");
            break;
        case MEDEA::ChartDataKind::CPU_UTILISATION:
            buttonIcon = theme->getIcon("ToggleIcons", "utilisationHover");
            break;
        case MEDEA::ChartDataKind::MEMORY_UTILISATION:
            buttonIcon = theme->getIcon("ToggleIcons", "memoryHover");
            break;
        case MEDEA::ChartDataKind::MARKER:
            buttonIcon = theme->getIcon("ToggleIcons", "markerHover");
            break;
        case MEDEA::ChartDataKind::NETWORK_UTILISATION:
            buttonIcon = theme->getIcon("ToggleIcons", "networkHover");
            break;
        default:
            qWarning("TimelineChartView::themeChanged - May be missing an icon for a ChartDataKind.");
            continue;
        }
        auto button = hoverDisplayButtons_.value(kind, nullptr);
        if (button) {
            button->setIcon(buttonIcon);
        }
        auto action = legendActions_.value(kind, nullptr);
        if (action) {
            action->setIcon(theme->getIcon("ToggleIcons", MEDEA::Event::GetChartDataKindString(kind)));
        }
    }

    emptyLabel_->setFont(QFont(theme->getFont().family(), 12));
    emptyLabel_->setStyleSheet("QLabel {"
                               "background:" + theme->getBackgroundColorHex() + ";"
                               "color:" + theme->getTextColorHex(ColorRole::DISABLED) + ";"
                               "border: 1px solid " + theme->getDisabledBackgroundColorHex() + ";}");

    mainWidget_->setStyleSheet("QWidget #MAIN_WIDGET {"
                               "background:" + theme->getBackgroundColorHex() + ";"
                               "border: 1px solid " + theme->getDisabledBackgroundColorHex() + ";}");
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

    auto kind = (MEDEA::ChartDataKind) sender()->property(CHART_DATA_KIND).toUInt();
    for (auto series : seriesList_) {
        if (series->getKind() == kind) {
            auto ID = series->getEventSeriesID();
            auto chart = charts_.value(ID, 0);
            if (chart) {
                chart->setVisible(checked);
            }
            auto chartLabel = chartLabels_.value(ID, 0);
            if (chartLabel) {
                chartLabel->setVisible(checked);
            }
        }
    }

    sender()->setProperty("checked", checked);
    emit seriesLegendHovered(checked ? kind : MEDEA::ChartDataKind::DATA);
}


/**
 * @brief TimelineChartView::chartLabelListSizeChanged
 * This is called when the entity axis is resized
 * It resizes the invisible filler widgets above and below it to match its width
 * @param size
 */
void TimelineChartView::chartLabelListSizeChanged(QSizeF size)
{
    qreal chartHeight = height() - timelineAxis_->height() - legendToolbar_->height() - SPACING * 3;

    if (size.height() > chartHeight) {
        size.setWidth(size.width() + SCROLLBAR_WIDTH);
    }

    topFillerWidget_->setFixedWidth(size.width());
    bottomFillerWidget_->setFixedWidth(size.width());

    //auto minTimeAxisWidth = fontMetrics().width(QDateTime::fromMSecsSinceEpoch(0).toString(TIME_FORMAT));
    //setMinimumWidth(size.width() + minTimeAxisWidth + SPACING * 2);
}


/**
 * @brief TimelineChartView::chartHovered
 * All this slot does is set the corresponding axis item's hovered state to hovered
 * @param chart
 * @param hovered
 */
void TimelineChartView::chartHovered(Chart* chart, bool hovered)
{
    if (!chart)
        return;

    QString path = charts_.key(chart, "");
    MEDEA::ChartLabel* chartLabel = chartLabels_.value(path, 0);
    if (chartLabel)
        chartLabel->setHovered(hovered);
}


/**
 * @brief TimelineChartView::chartClosed
 * This is called when a user has closed a chart by clicking on a chart label's close button
 */
void TimelineChartView::chartClosed()
{
    auto chartLabel = qobject_cast<MEDEA::ChartLabel*>(sender());
    if (chartLabel) {
        removeChart(chartLabels_.key(chartLabel, ""));
    }
}


/**
 * @brief TimelineChartView::updateHoverDisplay
 * This is called whenever the hover line is moved
 * It checks to see which chart is hovered and gets the hovered data from that chart
 */
void TimelineChartView::updateHoverDisplay()
{
    hoverDisplay_->hide();

    if (chartList_->isPanning())
        return;

    QHash<MEDEA::ChartDataKind, QString> hoveredData;

    for (auto entityChart : chartList_->getCharts()) {
        if (!entityChart || !entityChart->isHovered())
            continue;
        const auto& series = entityChart->getSeries();
        auto hoveredKinds = entityChart->getHovereSeriesKinds();
        for (const auto& s : series) {
            if (!s)
                continue;
            const auto& kind = s->getKind();
            const auto& action = legendActions_.value(kind, 0);
            if (!action || !action->isChecked())
                continue;
            if (!hoveredKinds.contains(kind))
                continue;
            //auto dt = QDateTime::currentMSecsSinceEpoch();
            const auto& hoveredRange = entityChart->getHoveredTimeRange(kind);
            const auto& hoveredInfo = s->getHoveredDataString(hoveredRange.first,
                                                              hoveredRange.second,
                                                              HOVER_DISPLAY_ITEM_COUNT,
                                                              getDateTimeDisplayFormat(kind));
            //qDebug() << "Duration: " << (QDateTime::currentMSecsSinceEpoch() - dt) << " ms";
            //qDebug() << "Hovered Info: " << hoveredInfo;
            //qDebug() << "---";
            if (!hoveredInfo.isEmpty())
                hoveredData[kind] += hoveredInfo + "\n";
        }
    }

    if (hoveredData.isEmpty())
        return;

    for (auto kind : hoverDisplayButtons_.keys()) {
        auto button = hoverDisplayButtons_.value(kind, 0);
        if (!button)
            continue;
        bool hasData = hoveredData.contains(kind);
        button->setVisible(hasData);
        if (hasData) {
            auto data = hoveredData.value(kind);
            button->setText(data.trimmed());
        }
    }

    // adjust the hover's size before calculating its position
    hoverDisplay_->adjustChildrenSize();

    auto globalPos = mapToGlobal(pos());
    auto hoverPos = mapTo(this, cursor().pos()) - QPoint(0, hoverDisplay_->height() / 2.0);
    auto bottom = globalPos.y() + height() - timelineAxis_->height();

    // adjust the hover display's position to make sure that it is fully visible
    if (hoverPos.x() >= (globalPos.x() + width() / 2.0)) {
        hoverPos.setX(hoverPos.x() - hoverDisplay_->width() - 25);
    } else {
        hoverPos.setX(hoverPos.x() + 25);
    }
    if ((hoverPos.y() + hoverDisplay_->height()) > bottom) {
        hoverPos.setY(bottom - hoverDisplay_->height());
    } else if (hoverPos.y() < globalPos.y()){
        hoverPos.setY(globalPos.y());
    }

    hoverDisplay_->move(hoverPos);
    hoverDisplay_->show();
}


/**
 * @brief TimelineChartView::minSliderMoved
 * @param ratio
 */
void TimelineChartView::minSliderMoved(const double ratio)
{
    for (auto chart : charts_) {
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
    for (auto chart : charts_) {
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
    timelineAxis_->zoom(factor);
}


/**
 * @brief TimelineChartView::timelinePanned
 * @param dx
 * @param dy
 */
void TimelineChartView::timelinePanned(const double dx, const double dy)
{
    auto displayRange = timelineAxis_->getDisplayedRange();
    auto actualRange = timelineAxis_->getRange();
    auto ratio = (displayRange.second - displayRange.first) / (actualRange.second - actualRange.first);
    timelineAxis_->pan(dx * ratio, dy * ratio);
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
    auto timelineWidth = (double)chartList_->width();
    left = qMax(left, 0.0);
    right = qMin(right, timelineWidth);

    // set the new display min/max
    auto minRatio = left / timelineWidth;
    auto maxRatio = right / timelineWidth;
    auto displayRange = timelineAxis_->getDisplayedRange();
    auto displayDist = displayRange.second - displayRange.first;
    auto min = displayDist * minRatio + displayRange.first;
    auto max = displayDist * maxRatio + displayRange.first;
    timelineAxis_->setDisplayRange(min, max);

    // update the entity charts' display range
    auto actualRange = timelineAxis_->getRange();
    auto actualDist = actualRange.second - actualRange.first;
    minRatio = actualDist > 0 ? (min - actualRange.first) / actualDist : 0.0;
    maxRatio = actualDist > 0 ? (max - actualRange.first) / actualDist : 0.0;
    for (auto chart : charts_) {
        chart->setDisplayRangeRatio(minRatio, maxRatio);
        chart->updateBinnedData();
    }
}


/**
 * @brief TimelineChartView::addEvent
 * @param kind
 * @param experimentRun
 * @param event
 */
void TimelineChartView::addEvent(MEDEA::ChartDataKind kind, const AggServerResponse::ExperimentRun& experimentRun, MEDEA::Event* event)
{
    if (event != nullptr) {
        const auto& series_id = event->getSeriesID();
        auto series = getSeriesForEventKind(kind, experimentRun, series_id);
        if (series == nullptr) {
            const auto& series_name = event->getSeriesName();
            series = constructSeriesForEventKind(kind, experimentRun, series_id, series_name);
        }
        series->addEvent(event);
    }
}


/**
 * @brief TimelineChartView::addedEvents
 * @param experimentRun
 */
void TimelineChartView::addedEvents(const AggServerResponse::ExperimentRun& experimentRun)
{
    auto experimentRunID = static_cast<quint32>(experimentRun.experiment_run_id);
    auto experimentRunStartTime = experimentRun.start_time;
    auto experimentInfo = "Experiment name:\t" + experimentRun.experiment_name +
            "\nJob number#:\t" + QString::number(experimentRun.job_num) +
            "\nStarted at:\t" + QDateTime::fromMSecsSinceEpoch(experimentRun.start_time).toString(DATE_TIME_FORMAT);

    updateRangeForExperimentRun(experimentRunID, experimentRunStartTime, experimentRun.last_updated_time);

    // Set the experiment info as the chart's tooltip
    for (const auto& chart : charts_) {
        if (chart->getExperimentRunID() == experimentRunID) {
            const auto& chart_id = charts_.key(chart);
            auto chart_label = chartLabels_.value(chart_id, nullptr);
            if (chart_label != nullptr) {
                chart_label->setToolTip(experimentInfo);
            }
        }
    }
}


/**
 * @brief TimelineChartView::getSeriesForEventKind
 * @param kind
 * @param experimentRun
 * @param eventSeriesID
 * @return
 */
MEDEA::EventSeries* TimelineChartView::getSeriesForEventKind(MEDEA::ChartDataKind kind, const AggServerResponse::ExperimentRun& experimentRun, const QString& eventSeriesID) const
{
    auto experimentRunID = experimentRun.experiment_run_id;
    auto seriesID = eventSeriesID + QString::number(experimentRunID);
    if (seriesList_.contains(seriesID)) {
        for (const auto& series : seriesList_.values(seriesID)) {
            if (series && series->getKind() == kind) {
                return series;
            }
        }
    }
    return nullptr;
}


/**
 * @brief TimelineChartView::constructSeriesForEventKind
 * @param kind
 * @param experimentRun
 * @param eventSeriesID
 * @param label
 * @return
 */
MEDEA::EventSeries* TimelineChartView::constructSeriesForEventKind(MEDEA::ChartDataKind kind, const AggServerResponse::ExperimentRun& experimentRun, const QString& eventSeriesID, const QString& label)
{
    MEDEA::EventSeries* series = getSeriesForEventKind(kind, experimentRun, eventSeriesID);
    if (series != nullptr) {
        return series;
    }

    auto experimentRunID = experimentRun.experiment_run_id;
    auto seriesID = eventSeriesID + QString::number(experimentRunID);
    auto seriesLabel = label;

    switch (kind) {
    case MEDEA::ChartDataKind::PORT_LIFECYCLE: {
        auto strList = seriesID.split("_");
        seriesLabel += "_" + strList.first();
        series = new PortLifecycleEventSeries(seriesID, this);
        break;
    }
    case MEDEA::ChartDataKind::WORKLOAD: {
        auto strList = seriesID.split("_");
        seriesLabel += "_" + strList.first();
        series = new WorkloadEventSeries(seriesID, this);
        break;
    }
    case MEDEA::ChartDataKind::CPU_UTILISATION:
        series = new CPUUtilisationEventSeries(seriesID, this);
        break;
    case MEDEA::ChartDataKind::MEMORY_UTILISATION:
        series = new MemoryUtilisationEventSeries(seriesID, this);
        break;
    case MEDEA::ChartDataKind::MARKER:
        series = new MarkerEventSeries(seriesID, this);
        break;
    case MEDEA::ChartDataKind::PORT_EVENT: {
        auto strList = seriesID.split("_");
        seriesLabel += "_" + strList.first();
        series = new PortEventSeries(seriesID, this);
        break;
    }
    case MEDEA::ChartDataKind::NETWORK_UTILISATION:
        series = new NetworkUtilisationEventSeries(seriesID, this);
        break;
    default:
        qWarning("TimelineChartView::constructSeriesForEventKind - Series kind not handled");
        return nullptr;
    }

    if (series) {
        // NOTE: This needs to be set before the chart is constructed
        series->setProperty(EXPERIMENT_RUN_ID, experimentRunID);
        series->setProperty(EXPERIMENT_RUN_START_TIME, experimentRun.start_time);
        constructChartForSeries(series, seriesID, seriesLabel + MEDEA::Event::GetChartDataKindStringSuffix(kind));
        seriesList_.insert(seriesID, series);
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
Chart* TimelineChartView::constructChartForSeries(MEDEA::EventSeries *series, const QString &ID, const QString &label)
{
    if (!series)
        return nullptr;

    // NOTE: At the moment, a new entity chart is constructed per series
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

    Chart* chart = new Chart(experimentRunID, experimentRunStartTime, this);
    chart->addSeries(series);
    chartList_->addChart(chart);
    charts_[seriesID] = chart;

    MEDEA::ChartLabel* chartLabel = new ChartLabel(seriesLabel, this);
    chartLabel->setMinimumHeight(MIN_ENTITY_HEIGHT);
    chartLabel->themeChanged(Theme::theme());
    chartLabelList_->appendChartLabel(chartLabel);
    chartLabels_[seriesID] = chartLabel;

    connect(this, &TimelineChartView::seriesLegendHovered, chart, &Chart::seriesKindHovered);
    connect(chartLabel, &ChartLabel::visibilityChanged, chart, &Chart::setVisible);
    connect(chartLabel, &ChartLabel::closeChart, this, &TimelineChartView::chartClosed);
    connect(chartLabel, &ChartLabel::hovered, [=] (bool hovered) {
        chartList_->setChartHovered(chart, hovered);
    });

    // set the initial visibility state of the chart/chart label
    for (auto& action : legendActions_.values()) {
        auto kind = legendActions_.key(action, MEDEA::ChartDataKind::DATA);
        if (kind == series->getKind()) {
            chart->setSeriesKindVisible(kind, true);
            chart->setVisible(action->isChecked());
            chartLabel->setVisible(action->isChecked());
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
 * It also removes the corresponding axis item from the timeline and contained series from their hashes
 * @param ID
 * @param clearing
 */
void TimelineChartView::removeChart(const QString &ID, bool clearing)
{
    // TODO: This needs to change if multiple series are allowed to be displayed in one entity chart
    // NOTE: At the moment there should be a chart per series, hence a chart should only have one series
    // ID is the chart's key, which is also the event series ID of the series it contains

    // remove chart series
    for (auto series : seriesList_) {
        if (series->getEventSeriesID() == ID) {
            auto key = seriesList_.key(series);
            seriesList_.remove(key, series);
            series->deleteLater();
            break;
        }
    }

    // remove chart
    auto chart = charts_.value(ID, 0);
    if (chart) {
        chartList_->removeChart(chart);
        chart->deleteLater();
        if (!clearing) {
            charts_.remove(ID);
        }
    }

    // remove chart label
    auto label = chartLabels_.value(ID, 0);
    if (label) {
        auto childrenLabels = label->getChildrenChartLabels();
        if (!childrenLabels.isEmpty()) {
            // remove/delete chart label's children labels
            auto childItr = childrenLabels.begin();
            while (childItr != childrenLabels.end()) {
                (*childItr)->deleteLater();
                childItr = childrenLabels.erase(childItr);
            }
        }
        chartLabelList_->removeChartLabel(label);
        chartLabels_.remove(ID);
        label->deleteLater();
    }

    if (!clearing) {
        // check if the timeline range needs updating
        auto expRunID = chart->getExperimentRunID();
        if (experimentRunSeriesCount_.contains(expRunID)) {
            experimentRunSeriesCount_[expRunID]--;
            removedDataFromExperimentRun(expRunID);
        }
        // if there are no more charts, show empty label
        if (charts_.isEmpty()) {
            mainWidget_->hide();
            emptyLabel_->show();
        }
    }

    // clear the timeline chart's hovered rect
    chartList_->setChartHovered(nullptr, false);
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
    bool updateLongestDuration = (experimentRunID == longestExperimentRunDuration_.first);
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

    for (auto chart : charts_) {
        chart->updateRange(startTime, duration);
    }

    timelineAxis_->setRange(startTime, startTime + duration, updateDisplayRange);
}


/**
 * @brief TimelineChartView::getDateTimeDisplayFormat
 * @param kind
 * @return
 */
const QString &TimelineChartView::getDateTimeDisplayFormat(const MEDEA::ChartDataKind &kind) const
{
    switch (kind) {
    case MEDEA::ChartDataKind::CPU_UTILISATION:
    case MEDEA::ChartDataKind::MEMORY_UTILISATION:
    case MEDEA::ChartDataKind::NETWORK_UTILISATION:
        return TIME_FORMAT;
    default:
        return DATE_TIME_FORMAT;
    }
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
    chartLabelList_ = new ChartLabelList(this);
    chartLabelList_->setAxisLineVisible(false);
    connect(chartLabelList_, &ChartLabelList::sizeChanged, this, &TimelineChartView::chartLabelListSizeChanged);

    timelineAxis_ = new AxisWidget(Qt::Horizontal, Qt::AlignBottom, VALUE_TYPE::DATE_TIME, this);
    timelineAxis_->setZoomFactor(ZOOM_FACTOR);

    connect(timelineAxis_, &AxisWidget::minRatioChanged, this, &TimelineChartView::minSliderMoved);
    connect(timelineAxis_, &AxisWidget::maxRatioChanged, this, &TimelineChartView::maxSliderMoved);

    chartList_ = new MEDEA::ChartList(this);
    chartList_->setAxisWidth(AXIS_LINE_WIDTH);
    chartList_->setAxisYVisible(true);

    connect(chartList_, &MEDEA::ChartList::panning, timelineAxis_, &AxisWidget::setPanning);
    connect(chartList_, &MEDEA::ChartList::hoverLineUpdated, timelineAxis_, &AxisWidget::hoverLineUpdated);

    connect(chartList_, &MEDEA::ChartList::zoomed, this, &TimelineChartView::timelineZoomed);
    connect(chartList_, &MEDEA::ChartList::panned, this, &TimelineChartView::timelinePanned);
    connect(chartList_, &MEDEA::ChartList::rubberbandUsed, this, &TimelineChartView::timelineRubberbandUsed);
    connect(chartList_, &MEDEA::ChartList::chartHovered, this, &TimelineChartView::chartHovered);
    if (HOVER_DISPLAY_ON) {
        connect(chartList_, &MEDEA::ChartList::hoverLineUpdated, this, &TimelineChartView::updateHoverDisplay);
    }

    /*
     *  TOP (LEGEND) LAYOUT
     */
    topFillerWidget_ = new QWidget(this);
    topFillerWidget_->setStyleSheet("background: rgba(0,0,0,0);");

    legendToolbar_ = new QToolBar(this);
    legendToolbar_->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->setMargin(0);
    topLayout->setSpacing(0);
    topLayout->addWidget(topFillerWidget_);
    topLayout->addWidget(legendToolbar_, 1, Qt::AlignCenter);

    /*
     * HOVER LAYOUT
     */
    hoverWidget_ = new QWidget(this);
    hoverWidget_->setStyleSheet("background: rgba(0,0,0,0);");

    QVBoxLayout* hoverLayout = new QVBoxLayout(hoverWidget_);
    hoverLayout->setSpacing(SPACING * 2);
    hoverLayout->setMargin(SPACING);

    hoverDisplay_ = new HoverPopup(this);
    hoverDisplay_->setWidget(hoverWidget_);

    /*
     * HOVER AND LEGEND CHART DATA KIND WIDGETS
     */
    for (auto kind : MEDEA::Event::GetChartDataKinds()) {
        if (kind == MEDEA::ChartDataKind::DATA)
            continue;

        // construct legend widgets
        QAction* action = legendToolbar_->addAction(MEDEA::Event::GetChartDataKindString(kind));
        legendActions_[kind] = action;
        action->setToolTip("Show/Hide " + action->text() + " Series");
        action->setCheckable(true);
        action->setChecked(true);
        //action->setVisible(false);
        action->setProperty(CHART_DATA_KIND, (uint)kind);
        connect(action, &QAction::toggled, this, &TimelineChartView::toggledSeriesLegend);

        QWidget* actionWidget = legendToolbar_->widgetForAction(action);
        actionWidget->setProperty(CHART_DATA_KIND, (uint)kind);
        actionWidget->installEventFilter(this);

        // construct hover display widgets
        QPushButton* button = new QPushButton(this);
        button->setStyleSheet("QPushButton{ text-align: left; }");
        hoverLayout->addWidget(button);
        hoverDisplayButtons_[kind] = button;
    }

    /*
     * MID (SCROLL AREA) LAYOUT
     */
    QWidget* scrollWidget = new QWidget(this);
    scrollWidget->setStyleSheet("background: rgba(0,0,0,0);");

    QHBoxLayout* scrollLayout = new QHBoxLayout(scrollWidget);
    scrollLayout->setMargin(0);
    scrollLayout->setSpacing(0);
    scrollLayout->addWidget(chartList_, 1);
    scrollLayout->addWidget(chartLabelList_);

    scrollArea_ = new QScrollArea(this);
    scrollArea_->setWidget(scrollWidget);
    scrollArea_->setWidgetResizable(true);
    scrollArea_->setLayoutDirection(Qt::RightToLeft);
    //scrollArea_->setStyleSheet("background: rgba(0,0,0,0);");
    scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea_->verticalScrollBar()->setFixedWidth(SCROLLBAR_WIDTH);

    /*
     * BOTTOM (TIME AXIS) LAYOUT
     */
    bottomFillerWidget_ = new QWidget(this);
    bottomFillerWidget_->setStyleSheet("background: rgba(0,0,0,0);");

    QHBoxLayout* bottomLayout = new QHBoxLayout();
    bottomLayout->setMargin(0);
    bottomLayout->setSpacing(0);
    bottomLayout->addWidget(bottomFillerWidget_);
    bottomLayout->addWidget(timelineAxis_, 1);

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
    mainLayout->addWidget(scrollArea_, 1);
    mainLayout->addLayout(bottomLayout);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(mainWidget_);
    layout->addWidget(emptyLabel_);

    scrollArea_->verticalScrollBar()->setTracking(true);
    connect(scrollArea_->verticalScrollBar(), &QScrollBar::valueChanged, [=]() {
        verticalScrollValue = scrollArea_->verticalScrollBar()->value();
    });

    auto minTimeAxisWidth = fontMetrics().width(QDateTime::fromMSecsSinceEpoch(0).toString(TIME_FORMAT));
    setMinimumWidth(chartLabelList_->minimumWidth() + minTimeAxisWidth + SPACING * 2);
}


/**
 * @brief qHash
 * @param key
 * @param seed
 * @return
 */
inline uint qHash(MEDEA::ChartDataKind key, uint seed)
{
    return ::qHash(static_cast<uint>(key), seed);
}
