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
    connect(inputPopup_, &ChartInputPopup::receivedRequestResponse, this, &ChartDialog::queryResponseReceived);
    connect(inputPopup_, &ChartInputPopup::accepted, [=]() { hasSelectedExperimentRun_ = false; });
    connect(inputPopup_, &ChartInputPopup::rejected, [=]() { hasSelectedExperimentRun_ = false; });

    chartView_ = new TimelineChartView(this);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(chartView_);

    connect(Theme::theme(), &Theme::theme_Changed, this, &ChartDialog::themeChanged);
    themeChanged();
    setMinimumWidth(700);

    displayFormat_ = TIME_DISPLAY_FORMAT::DATE_TIME;
}


/**
 * @brief ChartDialog::themeChanged
 */
void ChartDialog::themeChanged()
{
    setStyleSheet("background:" + Theme::theme()->getBackgroundColorHex() + "; border: none;");
}


/**
 * @brief ChartDialog::toggleTimelineAxisFormat
 */
void ChartDialog::toggleTimelineAxisFormat()
{
    switch (displayFormat_) {
    case TIME_DISPLAY_FORMAT::DATE_TIME:
        displayFormat_ = TIME_DISPLAY_FORMAT::ELAPSED_TIME;
        break;
    case TIME_DISPLAY_FORMAT::ELAPSED_TIME:
        displayFormat_ = TIME_DISPLAY_FORMAT::DATE_TIME;
        break;
    default:
        break;
    }
    chartView_->setTimeDisplayFormat(displayFormat_);
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

    const QString& fileName = fileDialog.selectedFiles().first();
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
    selectedExperimentRun_ = experimentRun;
    hasSelectedExperimentRun_ = true;
}


/**
 * @brief ChartDialog::queryResponseReceived
 * @param events
 */
void ChartDialog::queryResponseReceived(QList<MEDEA::Event*> events)
{
    if (!events.isEmpty()) {
        chartView_->addChartEvents(selectedExperimentRun_, events);
        emit receivedData();
    }
}
