#include "timelinechartview.h"
#include "../Axis/axiswidget.h"

#include <QScrollBar>

#define MIN_ENTITY_HEIGHT 50
#define SCROLLBAR_WIDTH 20
#define AXIS_LINE_WIDTH 2
#define ZOOM_FACTOR 1.025
#define SPACING 5
#define OPACITY 0.2

#define HOVER_DISPLAY_ON true
#define HOVER_DISPLAY_ITEM_COUNT 10

#define CHART_DATA_KIND "ChartDataKind"

using namespace MEDEA;

const ChartDataKind TimelineChartView::no_data_kind_ = ChartDataKind::DATA;

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
        if (!watched->property("checked").toBool()) {
            return false;
        }
        auto kind = no_data_kind_;
        if (event->type() == QEvent::HoverEnter) {
            kind = static_cast<MEDEA::ChartDataKind>(watched->property(CHART_DATA_KIND).toUInt());
        }
        emit seriesLegendHovered(kind);
        return true;
    }
    return QWidget::eventFilter(watched, event);
}

/**
 * @brief TimelineChartView::addChart
 * @param series
 * @param exp_run_data
 * @throws std::invalid_argument
 */
void TimelineChartView::addChart(const QPointer<const MEDEA::EventSeries>& series, const MEDEA::ExperimentRunData& exp_run_data)
{
    if (series.isNull()) {
        throw std::invalid_argument("TimelineChartView::addChart - Series is null.");
    }

    // If a chart already exists for the series, simply replace the series within the chart
    auto chart = charts_.value(series->getID(), nullptr);
    if (chart) {
        // TODO: If the charts don't get updated live, and the exp run was live when the chart was constructed,
        //  we will need to update the stored range for the exp run and then update the timeline range
        chart->removeSeries(series->getKind());
        chart->addSeries(series);
        return;
    }

    const auto exp_run_id = exp_run_data.experiment_run_id();
    const auto exp_start_time = exp_run_data.start_time();
    const auto& series_id = series->getID();
    const auto& series_label = series->getLabel();

    experimentRunSeriesCount_[exp_run_id]++;
    setTimeRangeForExperimentRun(exp_run_id, exp_start_time, exp_run_data.last_updated_time());

    connect(series, &MEDEA::EventSeries::eventAdded, [this]{ update(); });
    connect(&exp_run_data, &MEDEA::ExperimentRunData::dataUpdated, [this]{ update(); });

    auto exp_run_info = "Experiment name:\t" + exp_run_data.experiment_name() +
                        "\nJob number#:\t" + QString::number(exp_run_data.job_num()) +
                        "\nStarted at:\t" + QDateTime::fromMSecsSinceEpoch(exp_start_time).toString(DATE_TIME_FORMAT);

    chart = new Chart(exp_run_id, exp_start_time, this);
    chart->addSeries(series);
    chartList_->addChart(chart);
    charts_[series_id] = chart;

    auto chart_label = new ChartLabel(series_label, this);
    chart_label->setMinimumHeight(MIN_ENTITY_HEIGHT);
    chart_label->themeChanged(Theme::theme());
    chart_label->setToolTip(exp_run_info);
    chartLabelList_->appendChartLabel(chart_label);
    chartLabels_[series_id] = chart_label;

    connect(this, &TimelineChartView::seriesLegendHovered, chart, &Chart::seriesKindHovered);
    connect(chart_label, &ChartLabel::visibilityChanged, chart, &Chart::setVisible);
    connect(chart_label, &ChartLabel::closeChart, this, &TimelineChartView::chartClosed);
    connect(chart_label, &ChartLabel::hovered, [=] (bool hovered) {
        chartList_->setChartHovered(chart, hovered);
    });

    // Set the initial visibility state of the chart & chart label
    const auto series_kind = series->getKind();
    const auto legend_action = legendActions_.value(series_kind, nullptr);
    if (legend_action) {
        bool series_kind_visible = legend_action->isChecked();
        chart->setVisible(series_kind_visible);
        chart_label->setVisible(series_kind_visible);
    }

    // Make sure the main widget is visible
    if (mainWidget_->isHidden()) {
        mainWidget_->show();
        emptyLabel_->hide();
    }

    updateTimelineRange();
}

/**
 * @brief TimelineChartView::removeChart
 * @param id
 * @param clearing_chart_list
 */
void TimelineChartView::removeChart(const QString& id, bool clearing_chart_list)
{
    // TODO: This needs to change if multiple series are allowed to be displayed in one chart
    // NOTE: At the moment there should be a chart per series, hence a chart should only have one series

    // Remove the chart
    auto chart = charts_.value(id, nullptr);
    if (chart) {
        if (!clearing_chart_list) {
            decrementSeriesCountForExperimentRun(chart->getExperimentRunID());
            charts_.remove(id);
            if (charts_.isEmpty()) {
                chartsEmptied();
            }
        }
        chartList_->removeChart(chart);
        chart->deleteLater();
    }

    // Remove the chart label
    auto chart_label = chartLabels_.value(id, nullptr);
    if (chart_label) {
        /* TODO: Un-comment this out if ever we decide to group/parent ChartLabels
        auto childrenLabels = chart_label->getChildrenChartLabels();
        if (!childrenLabels.isEmpty()) {
            // remove/delete chart chart_label's children labels
            auto childItr = childrenLabels.begin();
            while (childItr != childrenLabels.end()) {
                (*childItr)->deleteLater();
                childItr = childrenLabels.erase(childItr);
            }
        } */
        chartLabels_.remove(id);
        chartLabelList_->removeChartLabel(chart_label);
        chart_label->deleteLater();
    }
}

/**
 * @brief TimelineChartView::updateExperimentRunLastUpdatedTime
 * @param experimentRunID
 * @param time
 */
void TimelineChartView::updateExperimentRunLastUpdatedTime(const quint32 experimentRunID, const qint64 time)
{
    if (experimentRunTimeRange_.contains(experimentRunID)) {
        setTimeRangeForExperimentRun(experimentRunID, experimentRunTimeRange_[experimentRunID].first, time);
        updateTimelineRange();
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
	chartsEmptied();
}

/**
 * @brief TimelineChartView::setTimeDisplayFormat
 * @param format
 */
void TimelineChartView::setTimeDisplayFormat(const TIME_DISPLAY_FORMAT format)
{
	timelineAxis_->setDisplayFormat(format);
	timeDisplayFormat_ = format;

    // PART OF REFACTOR - TEST THIS!
    updateTimelineRange(false);
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

    auto sb_bgcolor = theme->getDisabledBackgroundColorHex();
    if (theme->getTextColor() == theme->black()) {
        sb_bgcolor = Theme::QColorToHex(theme->white());
    } else {
        sb_bgcolor = Theme::QColorToHex(theme->black());
    }

    // The scrollbar stylesheet needs to be applied directly to the scrollbar
    // Otherwise, it will only apply the stylesheet on the first theme change
    scrollArea_->setStyleSheet("QScrollArea{ background:" + theme->getBackgroundColorHex() + ";}");
    scrollArea_->verticalScrollBar()->setStyleSheet(theme->getScrollBarStyleSheet());

	legendToolbar_->setFixedHeight(theme->getLargeIconSize().height());
	legendToolbar_->setStyleSheet(theme->getToolTipStyleSheet() +
                                  theme->getToolBarStyleSheet() +
                                  "QToolButton{ border: 0px; color:" + theme->getTextColorHex(ColorRole::DISABLED) + ";}"
                                  "QToolButton::checked:!hover{ color:" + theme->getTextColorHex() + ";}"
                                  "QToolButton:!hover{ background: rgba(0,0,0,0); }");

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
                if (kind != no_data_kind_) {
                    qWarning("TimelineChartView::themeChanged - May be missing an icon for a ChartDataKind.");
                }
                continue;
        }
        auto button = hoverDisplayButtons_.value(kind, nullptr);
        if (button) {
            button->setIcon(buttonIcon);
            button->setStyleSheet("color:" + theme->getTextColorHex());
        }
        auto action = legendActions_.value(kind, nullptr);
        if (action) {
            auto widget = legendToolbar_->widgetForAction(action);
            widget->setMinimumSize(theme->getLargeIconSize());
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
    if (sender() == nullptr) {
        return;
    }

    auto kind = (MEDEA::ChartDataKind) sender()->property(CHART_DATA_KIND).toUInt();
    for (auto chart : charts_) {
        const auto& series = chart->getSeries(kind);
        if (!series.isNull()) {
            chart->setVisible(checked);
            auto chart_label = chartLabels_.value(series->getID(), nullptr);
            if (chart_label) {
                chart_label->setVisible(checked);
            }
        }
    }

    sender()->setProperty("checked", checked);
    emit seriesLegendHovered(checked ? kind : no_data_kind_);
}

/**
 * @brief TimelineChartView::chartLabelListSizeChanged
 * This is called when the entity axis is resized
 * It resizes the invisible filler widgets above and below it to match its width
 * @param size
 */
void TimelineChartView::chartLabelListSizeChanged(QSizeF size)
{
    // NOTE: On MacOs, this moves the axis widget too far to the right when the scrollbar is visible
    // TODO: Double check if this is needed on Windows. If not, remove it.
    /*
	qreal chartHeight = height() - timelineAxis_->height() - legendToolbar_->height() - SPACING * 3;
	if (size.height() > chartHeight) {
		size.setWidth(size.width() + SCROLLBAR_WIDTH);
	}
    */
	topFillerWidget_->setFixedWidth(size.width());
	bottomFillerWidget_->setFixedWidth(size.width());
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

    const auto& id = charts_.key(chart, "");
    MEDEA::ChartLabel* chartLabel = chartLabels_.value(id, nullptr);
    if (chartLabel) {
        chartLabel->setHovered(hovered);
    }
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

    if (chartList_->isPanning()) {
        return;
    }

    QHash<MEDEA::ChartDataKind, QString> hoveredData;
    for (const auto& chart : charts_) {
        if (!chart->isHovered()) {
            continue;
        }
        for (const auto& kind : chart->getHoveredSeriesKinds()) {
            const auto& hovered_range = chart->getHoveredTimeRange(kind);
            const auto& series = chart->getSeries(kind);
            if (series.isNull()) {
                continue;
            }
            const auto &hovered_info = series->getHoveredDataString(hovered_range.first,
                                                                    hovered_range.second,
                                                                    HOVER_DISPLAY_ITEM_COUNT,
                                                                    getDateTimeDisplayFormat(kind));
            if (!hovered_info.isEmpty()) {
                hoveredData[kind] += hovered_info + "\n";
            }
        }
    }

    if (hoveredData.isEmpty()) {
        return;
    }

    for (auto button : hoverDisplayButtons_) {
        if (button) {
            const auto kind = hoverDisplayButtons_.key(button);
            const auto& data = hoveredData.value(kind, "");
            button->setText(data.trimmed());
            button->setVisible(!data.isEmpty());
        }
    }

	// adjust the hover's size before calculating its position
	hoverDisplay_->adjustChildrenSize();

    auto globalPos = mapToGlobal(chartList_->pos());
    auto hoverPos = mapTo(this, cursor().pos()) - QPoint(0, hoverDisplay_->height() / 2.0);
    auto bottom = globalPos.y() + height() - timelineAxis_->height();

    // adjust the hover display's position to make sure that it is fully visible
    if (hoverPos.x() >= (globalPos.x() + chartList_->width() / 2.0)) {
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
	}
}

/**
 * @brief TimelineChartView::setTimeRangeForExperimentRun
 * This is called when new experiment data is received
 * It checks if the stored timeline range needs updating
 * @param experimentRunID
 * @param startTime
 * @param lastUpdatedTime
 */
void TimelineChartView::setTimeRangeForExperimentRun(quint32 experimentRunID, qint64 startTime, qint64 lastUpdatedTime)
{
    auto duration = lastUpdatedTime - startTime;
    if (duration > longestExperimentRunDuration_.second) {
        longestExperimentRunDuration_= {experimentRunID, duration};
    }
    totalTimeRange_ = {qMin(startTime, totalTimeRange_.first), qMax(lastUpdatedTime, totalTimeRange_.second)};
    experimentRunTimeRange_[experimentRunID] = {startTime, lastUpdatedTime};
}

/**
 * @brief TimelineChartView::decrementSeriesCountForExperimentRun
 * This is called when experiment data is removed
 * If the last series from the specified experiment run is removed, it recalculates the timeline's range
 * @param experimentRunID
 */
void TimelineChartView::decrementSeriesCountForExperimentRun(quint32 experimentRunID)
{
    if (!experimentRunSeriesCount_.contains(experimentRunID)) {
        return;
    }

    // decrement the series count; if it's not the last one, do nothing
    int seriesCount = --experimentRunSeriesCount_[experimentRunID];
    if (seriesCount > 0) {
        return;
    }

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
        min = qMin(static_cast<long long>(min), val.first);
        max = qMax(static_cast<long long>(max), val.second);
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
	auto startTime = totalTimeRange_.first;
	auto duration = totalTimeRange_.second - totalTimeRange_.first;

    if (timeDisplayFormat_ == TIME_DISPLAY_FORMAT::ELAPSED_TIME) {
        startTime = 0.0;
        duration = longestExperimentRunDuration_.second;
    }

	for (auto chart : charts_) {
		chart->updateRange(startTime, duration);
	}

	timelineAxis_->setRange(startTime, startTime + duration, updateDisplayRange);
	update();
}

/**
 * @brief TimelineChartView::chartsEmptied
 */
void TimelineChartView::chartsEmptied()
{
    // Show empty label
    mainWidget_->hide();
    emptyLabel_->show();

    // Clear stored ranges
    totalTimeRange_ = {INT64_MAX, INT64_MIN};
    longestExperimentRunDuration_ = {0, INT64_MIN};
    experimentRunTimeRange_.clear();
    experimentRunSeriesCount_.clear();

    timelineAxis_->setRange(0, 1);
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

	auto topLayout = new QHBoxLayout();
	topLayout->setMargin(0);
	topLayout->setSpacing(0);
	topLayout->addWidget(topFillerWidget_);
	topLayout->addWidget(legendToolbar_, 1, Qt::AlignCenter);

	/*
	 * HOVER LAYOUT
	 */
	hoverWidget_ = new QWidget(this);
	hoverWidget_->setStyleSheet("background: rgba(0,0,0,0);");

	auto hoverLayout = new QVBoxLayout(hoverWidget_);
	hoverLayout->setSpacing(SPACING * 2);
	hoverLayout->setMargin(SPACING);

	hoverDisplay_ = new HoverPopup(this);
	hoverDisplay_->setWidget(hoverWidget_);

    /*
     * HOVER AND LEGEND CHART DATA KIND WIDGETS
     */
    for (auto kind : MEDEA::Event::GetChartDataKinds()) {
        if (kind == no_data_kind_) {
            continue;
        }
        // construct legend widgets
        QAction* action = legendToolbar_->addAction(MEDEA::Event::GetChartDataKindString(kind));
        legendActions_[kind] = action;
        action->setToolTip("Show/Hide " + action->text() + " Series");
        action->setCheckable(true);
        action->setChecked(true);
        action->setProperty(CHART_DATA_KIND, (uint)kind);
        connect(action, &QAction::toggled, this, &TimelineChartView::toggledSeriesLegend);

		QWidget* actionWidget = legendToolbar_->widgetForAction(action);
		actionWidget->setProperty(CHART_DATA_KIND, (uint)kind);
		actionWidget->installEventFilter(this);

        // construct hover display widgets
        QPushButton* button = new QPushButton(this);
        button->setStyleSheet("QPushButton{ text-align: left; background: rgba(0,0,0,0); }");
        hoverLayout->addWidget(button);
        hoverDisplayButtons_[kind] = button;
    }

	/*
	 * MID (SCROLL AREA) LAYOUT
	 */
	auto scrollWidget = new QWidget(this);
	scrollWidget->setStyleSheet("background: rgba(0,0,0,0);");

	auto scrollLayout = new QHBoxLayout(scrollWidget);
	scrollLayout->setMargin(0);
	scrollLayout->setSpacing(0);
	scrollLayout->addWidget(chartList_, 1);
	scrollLayout->addWidget(chartLabelList_);

    scrollArea_ = new QScrollArea(this);
    scrollArea_->setWidget(scrollWidget);
    scrollArea_->setWidgetResizable(true);
    scrollArea_->setLayoutDirection(Qt::RightToLeft);
    scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea_->verticalScrollBar()->setFixedWidth(SCROLLBAR_WIDTH);
    scrollArea_->verticalScrollBar()->setTracking(true);

	/*
	 * BOTTOM (TIME AXIS) LAYOUT
	 */
	bottomFillerWidget_ = new QWidget(this);
	bottomFillerWidget_->setStyleSheet("background: rgba(0,0,0,0);");

	auto bottomLayout = new QHBoxLayout();
	bottomLayout->setMargin(0);
	bottomLayout->setSpacing(0);
	bottomLayout->addWidget(bottomFillerWidget_);
	bottomLayout->addWidget(timelineAxis_, 1);

	/*
	 * MAIN LAYOUT
	 */
	emptyLabel_ = new QLabel("No Charts To Display", this);
	emptyLabel_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	emptyLabel_->setAlignment(Qt::AlignCenter);

	mainWidget_ = new QWidget(this);
	mainWidget_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	mainWidget_->setVisible(false);
	mainWidget_->setObjectName("MAIN_WIDGET");

	auto mainLayout = new QVBoxLayout(mainWidget_);
	mainLayout->setMargin(0);
	mainLayout->setSpacing(0);
	mainLayout->setContentsMargins(SPACING, SPACING, SPACING, SPACING);
	mainLayout->addLayout(topLayout);
	mainLayout->addSpacerItem(new QSpacerItem(0, SPACING));
	mainLayout->addWidget(scrollArea_, 1);
	mainLayout->addLayout(bottomLayout);

	auto layout = new QVBoxLayout(this);
	layout->setContentsMargins(0,0,0,0);
	layout->addWidget(mainWidget_);
	layout->addWidget(emptyLabel_);

    auto minTimeAxisWidth = fontMetrics().width(QDateTime::fromMSecsSinceEpoch(0).toString(TIME_FORMAT));
    setMinimumWidth(chartLabelList_->minimumWidth() + minTimeAxisWidth + SPACING * 2);
}