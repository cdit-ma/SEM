#include "chartdialog.h"
#include "../../../theme.h"

#include <QVBoxLayout>
#include <QFileDialog>
#include <QStandardPaths>
#include <QImageWriter>
#include <QMessageBox>

/**
 * @brief ChartDialog::ChartDialog
 * @param vc
 * @param parent
 */
ChartDialog::ChartDialog(ViewController *vc, QWidget *parent)
    : QFrame(parent)
{
    // setup widgets
    inputPopup_ = new ChartInputPopup(this);
    inputPopup_->setViewController(vc);
    connect(inputPopup_, &ChartInputPopup::selectedExperimentRun, this, &ChartDialog::experimentRunSelected);
    connect(inputPopup_, &ChartInputPopup::accepted, this, &ChartDialog::receivedData);

    chartView_ = new TimelineChartView(this);
    chartView_->setActiveEventKinds({TIMELINE_DATA_KIND::PORT_LIFECYCLE, TIMELINE_DATA_KIND::WORKLOAD, TIMELINE_DATA_KIND::CPU_UTILISATION, TIMELINE_DATA_KIND::MEMORY_UTILISATION});

    if (vc) {
        connect(&vc->getAggregationProxy(), &AggregationProxy::receivedEvents, chartView_, &TimelineChartView::receivedRequestedEvents);
        connect(&vc->getAggregationProxy(), &AggregationProxy::receivedAllEvents, chartView_, &TimelineChartView::updateTimelineChart);
        connect(&vc->getAggregationProxy(), &AggregationProxy::clearPreviousEvents, chartView_, &TimelineChartView::clearTimelineChart);
        connect(&vc->getAggregationProxy(), &AggregationProxy::setChartUserInputDialogVisible, [=] (bool visible) {
            if (visible)
                hasSelectedExperimentRun_ = false;
        });
        /*connect(&vc->getAggregationProxy(), &AggregationProxy::receivedAllEvents, [=]() {
            qDebug() << "Received ALL events";
            emit receivedData();
            //updateTimelineRange();
        });*/
    } else {
        qWarning("ChartDialog - ViewController is null");
    }

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(chartView_);

    connect(Theme::theme(), &Theme::theme_Changed, this, &ChartDialog::themeChanged);
    themeChanged();

    minTime_ = INT64_MAX;
    maxTime_ = INT64_MIN;
    displayFormat_ = TIME_DISPLAY_FORMAT::DATE_TIME;
}


/**
 * @brief ChartDialog::themeChanged
 */
void ChartDialog::themeChanged()
{
    Theme* theme = Theme::theme();
    setStyleSheet("background:" + theme->getBackgroundColorHex() + "; border: none;");
}


/**
 * @brief ChartDialog::toggleTimelineAxisFormat
 */
void ChartDialog::toggleTimelineAxisFormat()
{
    switch (displayFormat_) {
    case TIME_DISPLAY_FORMAT::DATE_TIME:
        displayFormat_ = TIME_DISPLAY_FORMAT::ELAPSED_TIME;
        //chartView_->setTimelineRange(minTime_, maxTime_);
        break;
    case TIME_DISPLAY_FORMAT::ELAPSED_TIME:
        displayFormat_ = TIME_DISPLAY_FORMAT::DATE_TIME;
        //chartView_->setTimelineRange(0, maxTime_ - minTime_);
        break;
    default:
        break;
    }
    chartView_->toggleTimeDisplay(displayFormat_);
}


/**
 * @brief ChartDialog::snapShot
 */
void ChartDialog::snapShot()
{
    QPixmap widgetPixmap = chartView_->grab();
    QString initialPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    const QString format = "png";
    if (initialPath.isEmpty())
        initialPath = QDir::currentPath();
    initialPath += tr("/untitled.") + format;

    QFileDialog fileDialog(this, tr("Save As"), initialPath);
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setFileMode(QFileDialog::AnyFile);
    fileDialog.setDirectory(initialPath);

    QStringList mimeTypes;
    foreach (const QByteArray &bf, QImageWriter::supportedMimeTypes())
        mimeTypes.append(QLatin1String(bf));

    fileDialog.setMimeTypeFilters(mimeTypes);
    fileDialog.selectMimeTypeFilter("image/" + format);
    fileDialog.setDefaultSuffix(format);

    if (fileDialog.exec() != QDialog::Accepted)
        return;

    const QString fileName = fileDialog.selectedFiles().first();
    if (!widgetPixmap.save(fileName))
        QMessageBox::warning(this, tr("Save Error"), tr("The image could not be saved to \"%1\".")
                             .arg(QDir::toNativeSeparators(fileName)));
}


/**
 * @brief ChartDialog::clear
 */
void ChartDialog::clear()
{
    chartView_->clearTimelineChart();
}


/**
 * @brief ChartDialog::experimentRunSelected
 * @param experimentRun
 */
void ChartDialog::experimentRunSelected(ExperimentRun experimentRun)
{
    qDebug() << "Exepriment run selected: " << experimentRun.experiment_name;
    selectedExperimentRun_ = experimentRun;
    hasSelectedExperimentRun_ = true;
}


/**
 * @brief ChartDialog::updateTimelineRange
 */
void ChartDialog::updateTimelineRange()
{
    if (!hasSelectedExperimentRun_)
        return;

    auto minTime = selectedExperimentRun_.start_time;
    auto maxTime = selectedExperimentRun_.end_time;
    auto rangeChanged = minTime < minTime_ || maxTime > maxTime_;

    qDebug() << "start: " << minTime << ", end: " << maxTime;

    minTime_ = qMin(minTime, minTime_);
    maxTime_ = maxTime == 0 ? minTime_ + 10000 : qMax(maxTime, maxTime_);

    if (rangeChanged)
        chartView_->setTimelineRange(minTime_, maxTime_);

    qDebug() << "Update timeline range";
}
