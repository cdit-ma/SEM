#include "chartdialog.h"
#include "../../../theme.h"

#include <QVBoxLayout>
#include <QFileDialog>
#include <QStandardPaths>
#include <QImageWriter>
#include <QMessageBox>
#include <QLayout>


/**
 * @brief ChartDialog::ChartDialog
 * @param parent
 */
ChartDialog::ChartDialog(QWidget *parent)
    : QFrame(parent)
{
    connect(Theme::theme(), &Theme::theme_Changed, this, &ChartDialog::themeChanged);
    themeChanged();

    setMinimumWidth(900);
    setupChartView();

    displayFormat_ = TIME_DISPLAY_FORMAT::DATE_TIME;
}


/**
 * @brief ChartDialog::getChartView
 * @return
 */
TimelineChartView& ChartDialog::getChartView()
{
    return chartView_;
}


/**
 * @brief ChartDialog::setupChartView
 */
void ChartDialog::setupChartView()
{
    if (layout()) {
        layout()->addWidget(&chartView_);
    } else {
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->setMargin(0);
        layout->setSpacing(0);
        layout->addWidget(&chartView_);
    }
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
        break;
    case TIME_DISPLAY_FORMAT::ELAPSED_TIME:
        displayFormat_ = TIME_DISPLAY_FORMAT::DATE_TIME;
        break;
    default:
        break;
    }
    chartView_.setTimeDisplayFormat(displayFormat_);
}


/**
 * @brief ChartDialog::snapShot
 */
void ChartDialog::snapShot()
{
    QPixmap widgetPixmap = chartView_.grab();
    QString initialPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    const QString format = "png";
    if (initialPath.isEmpty()) {
        initialPath = QDir::currentPath();
    }
    initialPath += tr("/untitled.") + format;

    QFileDialog fileDialog(this, tr("Save As"), initialPath);
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setFileMode(QFileDialog::AnyFile);
    fileDialog.setDirectory(initialPath);

    QStringList mimeTypes;
    foreach (const QByteArray &bf, QImageWriter::supportedMimeTypes()) {
        mimeTypes.append(QLatin1String(bf));
    }

    fileDialog.setMimeTypeFilters(mimeTypes);
    fileDialog.selectMimeTypeFilter("image/" + format);
    fileDialog.setDefaultSuffix(format);
    if (fileDialog.exec() != QDialog::Accepted) {
        return;
    }

    const QString& fileName = fileDialog.selectedFiles().first();
    if (!widgetPixmap.save(fileName)) {
        QMessageBox::warning(this, tr("Save Error"), tr("The image could not be saved to \"%1\".")
                             .arg(QDir::toNativeSeparators(fileName)));
    }
}


/**
 * @brief ChartDialog::clear
 */
void ChartDialog::clear()
{
    chartView_.clearChartList();
}

