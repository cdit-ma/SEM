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
    setMinimumWidth(800);

    displayFormat_ = TIME_DISPLAY_FORMAT::DATE_TIME;
}


/**
 * @brief ChartDialog::setChartView
 * @param view
 */
void ChartDialog::setChartView(TimelineChartView *view)
{
    if (!view)
        return;

    if (layout()) {
        layout()->addWidget(view);
    } else {
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->setMargin(0);
        layout->setSpacing(0);
        layout->addWidget(view);
    }
    chartView_ = view;
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
    if (!chartView_)
        return;

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
    if (!chartView_)
        return;

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
    if (chartView_)
        chartView_->clearTimelineChart();
}

