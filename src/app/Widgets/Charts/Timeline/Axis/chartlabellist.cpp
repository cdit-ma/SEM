#include "chartlabellist.h"
#include "chartlabel.h"
#include "../../../../theme.h"

#include <QDebug>

using namespace MEDEA;

/**
 * @brief ChartLabelList::ChartLabelList
 * @param parent
 */
ChartLabelList::ChartLabelList(QWidget* parent)
    : QWidget(parent)
{
    mainLayout_ = new QVBoxLayout(this);
    mainLayout_->setSpacing(0);
    mainLayout_->setMargin(0);
    mainLayout_->setContentsMargins(5, 0, 0, 0);
    mainLayout_->setAlignment(Qt::AlignCenter);

    connect(Theme::theme(), &Theme::theme_Changed, this, &ChartLabelList::themeChanged);
    themeChanged();

    setMinimumWidth(140);
}


/**
 * @brief ChartLabelList::appendChartLabel
 * @param chartLabel
 * @return
 */
int ChartLabelList::appendChartLabel(ChartLabel* chartLabel)
{
    int index = -1;
    if (chartLabel) {
        chartLabel->setAxisLineVisible(axisLineVisible_);
        chartLabel->setTickVisible(tickVisible_);
        mainLayout_->addWidget(chartLabel);
        index = mainLayout_->indexOf(chartLabel);
    }
    return index;
}


/**
 * @brief ChartLabelList::insertChartLabel
 * @param prevChartLabel
 * @param chartLabel
 * @return
 */
int ChartLabelList::insertChartLabel(ChartLabel* prevChartLabel, ChartLabel* chartLabel)
{
    int index = -1;
    if (prevChartLabel && chartLabel) {
        chartLabel->setAxisLineVisible(axisLineVisible_);
        chartLabel->setTickVisible(tickVisible_);
        int prevIndex = mainLayout_->indexOf(prevChartLabel);
        if (prevIndex != -1) {
            index = prevIndex + prevChartLabel->getAllDepthChildrenCount();
            mainLayout_->insertWidget(index, chartLabel);
        }
    }
    return index;
}


/**
 * @brief ChartLabelList::removeChartLabel
 * @param chartLabel
 * @return
 */
int ChartLabelList::removeChartLabel(ChartLabel* chartLabel)
{
    int index = -1;
    if (chartLabel) {
        index = mainLayout_->indexOf(chartLabel);
        mainLayout_->removeWidget(chartLabel);
    }
    return index;
}


/**
 * @brief ChartLabelList::getChartLabelIndex
 * @param chartLabel
 * @return
 */
int ChartLabelList::getChartLabelIndex(ChartLabel* chartLabel) const
{
    return mainLayout_->indexOf(chartLabel);
}


/**
 * @brief ChartLabelList::setAxisLineVisible
 * @param visible
 */
void ChartLabelList::setAxisLineVisible(bool visible)
{
    QList<ChartLabel*> childrenLabels = findChildren<ChartLabel*>();
    for (ChartLabel* label : childrenLabels) {
        label->setAxisLineVisible(visible);
    }
    axisLineVisible_ = visible;
}


/**
 * @brief ChartLabelList::setTickVisible
 * @param visible
 */
void ChartLabelList::setTickVisible(bool visible)
{
    QList<ChartLabel*> childrenLabels = findChildren<ChartLabel*>();
    for (ChartLabel* label : childrenLabels) {
        label->setTickVisible(visible);
    }
    tickVisible_ = visible;
}


/**
 * @brief ChartLabelList::themeChanged
 */
void ChartLabelList::themeChanged()
{
    Theme* theme = Theme::theme();
    QList<ChartLabel*> childrenLabels = findChildren<ChartLabel*>();
    for (ChartLabel* label : childrenLabels) {
        label->themeChanged(theme);
    }
}


/**
 * @brief ChartLabelList::resizeEvent
 * @param event
 */
void ChartLabelList::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    emit sizeChanged(size());
}
