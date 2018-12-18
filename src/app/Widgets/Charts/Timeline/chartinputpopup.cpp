#include "chartinputpopup.h"
#include "../../../theme.h"

#include <QVBoxLayout>
#include <QRadioButton>
#include <QDateTime>
#include <QApplication>
#include <QWindow>
#include <QScreen>

/**
 * @brief ChartInputPopup::ChartInputPopup
 * @param parent
 */
ChartInputPopup::ChartInputPopup(QWidget* parent)
    : HoverPopup(parent)
{
    /*
     * Setup Widgets
     */

    experimentNameLineEdit_ = new QLineEdit(this);
    experimentNameLineEdit_->setFixedHeight(40);
    experimentNameLineEdit_->setMinimumWidth(400);
    experimentNameLineEdit_->setFont(QFont(font().family(), 10, QFont::ExtraLight));
    experimentNameLineEdit_->setPlaceholderText("Enter experiment name...");
    experimentNameLineEdit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    experimentNameLineEdit_->setAttribute(Qt::WA_MacShowFocusRect, false);

    connect(experimentNameLineEdit_, &QLineEdit::textChanged, [=]() {
        emit requestExperimentRuns(experimentNameLineEdit_->text().trimmed());
    });

    experimentNameGroupBox_ = new QGroupBox("Visualise Events For Experiment:", this);
    QVBoxLayout* topLayout = new QVBoxLayout(experimentNameGroupBox_);
    topLayout->setMargin(0);
    topLayout->setContentsMargins(1, 5, 1, 1);
    topLayout->addWidget(experimentNameLineEdit_);

    experimentRunsGroupBox_ = new QGroupBox("Select Experiment Run:", this);
    QVBoxLayout* midLayout = new QVBoxLayout(experimentRunsGroupBox_);
    midLayout->setMargin(0);
    midLayout->setSpacing(2);
    midLayout->setContentsMargins(1, 5, 1, 1);

    /*QLineEdit* filterLineEdit = new QLineEdit(this);
    filterLineEdit->setFixedHeight(40);
    filterLineEdit->setMinimumWidth(400);
    filterLineEdit->setFont(QFont(font().family(), 10, QFont::ExtraLight));
    filterLineEdit->setPlaceholderText("Enter component name...");
    filterLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    filterLineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);

    connect(filterLineEdit, &QLineEdit::returnPressed, [=]() {
        sendEventsRequest();
    });

    filtersGroupBox = new QGroupBox("Filter Events By Component(s):", this);
    QVBoxLayout* bottomLayout = new QVBoxLayout(filtersGroupBox);
    bottomLayout->setMargin(0);
    bottomLayout->setContentsMargins(1, 5, 1, 1);
    bottomLayout->addWidget(filterLineEdit);
    filtersGroupBox->hide();*/

    QWidget* spacerWidget = new QWidget(this);
    spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    toolbar_ = new QToolBar(this);
    toolbar_->setMinimumWidth(400);
    toolbar_->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolbar_->addWidget(spacerWidget);
    cancelAction_ = toolbar_->addAction("Cancel");
    okAction_ = toolbar_->addAction("Ok");

    connect(okAction_, &QAction::triggered, this, &ChartInputPopup::accept);
    connect(cancelAction_, &QAction::triggered, this, &ChartInputPopup::reject);

    QWidget* holderWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(holderWidget);
    mainLayout->setMargin(5);
    mainLayout->setSpacing(5);
    mainLayout->addWidget(experimentNameGroupBox_);
    mainLayout->addWidget(experimentRunsGroupBox_, 1);
    mainLayout->addWidget(toolbar_);

    setWidget(holderWidget);
    setVisible(false);
    setModal(true);

    connect(Theme::theme(), &Theme::theme_Changed, this, &ChartInputPopup::themeChanged);
    themeChanged();
}


/**
 * @brief ChartInputPopup::themeChanged
 */
void ChartInputPopup::themeChanged()
{
    Theme* theme = Theme::theme();

    toolbar_->setIconSize(theme->getIconSize());
    toolbar_->setStyleSheet(theme->getToolBarStyleSheet());

    okAction_->setIcon(theme->getIcon("Icons", "tick"));
    cancelAction_->setIcon(theme->getIcon("Icons", "cross"));

    experimentNameLineEdit_->setStyleSheet(theme->getLineEditStyleSheet());

    auto groupBoxStyle = theme->getGroupBoxStyleSheet() +
                         "QGroupBox{color: lightGray; margin-top: 15px;}" +
                         "QGroupBox::title{subcontrol-origin: margin;}";
    experimentNameGroupBox_->setStyleSheet(groupBoxStyle);
    experimentRunsGroupBox_->setStyleSheet(groupBoxStyle);
}


/**
 * @brief ChartInputPopup::setPopupVisible
 * @param visible
 */
void ChartInputPopup::setPopupVisible(bool visible)
{
    setVisible(visible);

    if (visible) {
        activateWindow();
        experimentNameLineEdit_->setFocus();
        experimentNameLineEdit_->selectAll();
        emit requestExperimentRuns(experimentNameLineEdit_->text().trimmed());
    }

    if (originalCenterPos_.isNull())
        originalCenterPos_ = geometry().center();
}


/**
 * @brief ChartInputPopup::populateExperimentRuns
 * @param runs
 */
void ChartInputPopup::populateExperimentRuns(QList<ExperimentRun> runs)
{
    if (!experimentRunsGroupBox_->layout())
        return;

    auto buttons = experimentRunsGroupBox_->findChildren<QRadioButton*>();
    while (!buttons.isEmpty()) {
        auto button = buttons.takeFirst();
        experimentRunsGroupBox_->layout()->removeWidget(button);
        button->deleteLater();
    }

    // hiding it first, resizes the widget immediately
    experimentRunsGroupBox_->hide();
    adjustChildrenSize("", Qt::FindDirectChildrenOnly);

    auto halfSize = sizeHint() / 2.0;
    if (originalCenterPos_.isNull()) {
        originalCenterPos_ = pos() + QPointF(halfSize.width(), halfSize.height());
    }

    if (runs.isEmpty()) {
        move(originalCenterPos_.x() - halfSize.width(), originalCenterPos_.y() - halfSize.height());
        return;
    }

    for (auto run : runs) {
        auto ID = run.experiment_run_id;
        QString text = run.experiment_name + "[" + QString::number(ID) + "] - started at " +
                       QDateTime::fromMSecsSinceEpoch(run.start_time).toString("MMM d, hh:mm:ss:zzz");

        QRadioButton* button = new QRadioButton(text, this);
        button->setProperty("ID", ID);
        button->setStyleSheet("color:" + Theme::theme()->getTextColorHex() + ";");
        experimentRunsGroupBox_->layout()->addWidget(button);
        /*connect(button, &QRadioButton::toggled, [=]() {
            if (button->isChecked()) {
                emit requestExperimentState(ID);
            }
        });*/
    }

    experimentRunsGroupBox_->show();
    updateGeometry();

    // re-center popup
    auto newSize = experimentRunsGroupBox_->sizeHint() / 2.0;
    move(originalCenterPos_.x() - newSize.width(), originalCenterPos_.y() - newSize.height());
}


/**
 * @brief ChartInputPopup::accept
 */
void ChartInputPopup::accept()
{
    if (experimentRunsGroupBox_->layout()) {
        auto buttons = experimentRunsGroupBox_->findChildren<QRadioButton*>();
        for (auto button : buttons) {
            if (button->isChecked()) {
                emit requestEvents(button->property("ID").toUInt());
                break;
            }
        }
    }
    PopupWidget::accept();
}


/**
 * @brief ChartInputPopup::reject
 */
void ChartInputPopup::reject()
{
    PopupWidget::reject();
}
