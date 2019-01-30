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

    chartView_ = new TimelineChartView(this);
    chartView_->setActiveEventKinds({TIMELINE_DATA_KIND::PORT_LIFECYCLE, TIMELINE_DATA_KIND::WORKLOAD, TIMELINE_DATA_KIND::CPU_UTILISATION, TIMELINE_DATA_KIND::MEMORY_UTILISATION});

    if (vc) {
        connect(&vc->getAggregationProxy(), &AggregationProxy::receivedEvents, chartView_, &TimelineChartView::receivedRequestedEvents);
        connect(&vc->getAggregationProxy(), &AggregationProxy::clearPreviousEvents, chartView_, &TimelineChartView::clearSeriesEvents);
        connect(&vc->getAggregationProxy(), &AggregationProxy::receivedAllEvents, chartView_, &TimelineChartView::updateTimelineChart);
    } else {
        qWarning("ChartDialog - ViewController is null");
    }

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(chartView_);

    connect(Theme::theme(), &Theme::theme_Changed, this, &ChartDialog::themeChanged);
    themeChanged();
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
