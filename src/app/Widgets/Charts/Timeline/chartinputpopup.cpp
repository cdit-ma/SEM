#include "chartinputpopup.h"
#include "../../../theme.h"

#include <QScrollArea>
#include <QRadioButton>
#include <QToolButton>
#include <QDateTime>
#include <QFutureWatcher>

#define MIN_WIDTH 400
#define FILTER "filter"
#define GROUPBOX_ITEM_SPACING 5
#define GROUPBOX_MAX_HEIGHT 300


/**
 * @brief ChartInputPopup::ChartInputPopup
 * @param parent
 */
ChartInputPopup::ChartInputPopup(ViewController* vc, QWidget* parent)
    : HoverPopup(parent)
{
    viewController_ = vc;

    setupLayout();
    setVisible(false);
    setModal(true);

    //enableFilters();
    hideGroupBoxes();

    connect(Theme::theme(), &Theme::theme_Changed, this, &ChartInputPopup::themeChanged);
    themeChanged();
}


/**
 * @brief ChartInputPopup::enableFilters
 * NOTE - Call this after setWidget(), otherwise the groupboxes won't be put in the right layout
 */
void ChartInputPopup::enableFilters()
{
    filtersEnabled_ = true;
    setupFilterWidgets();
}


/**
 * @brief ChartInputPopup::themeChanged
 */
void ChartInputPopup::themeChanged()
{
    Theme* theme = Theme::theme();

    toolbar_->setIconSize(theme->getIconSize());
    toolbar_->setStyleSheet(theme->getToolBarStyleSheet() + "QToolTip{ background: white; color: black; }");

    okAction_->setIcon(theme->getIcon("Icons", "tick"));
    cancelAction_->setIcon(theme->getIcon("Icons", "cross"));

    if (filterAction_)
        filterAction_->setIcon(theme->getIcon("Icons", "triangleDown"));

    experimentNameLineEdit_->setStyleSheet(theme->getLineEditStyleSheet());

    for (auto radioButton : findChildren<QRadioButton*>()) {
        radioButton->setStyleSheet("color:" + Theme::theme()->getTextColorHex() + ";");
    }

    auto groupBoxStyle = theme->getGroupBoxStyleSheet() +
                         "QGroupBox{color: lightGray; margin-top: 15px;}" +
                         "QGroupBox::title{subcontrol-origin: margin;}";

    auto scrollbarStyle =  "QScrollArea{ background: rgba(0,0,0,0); border: 0px; }"
                           "QScrollBar::handle:active{ background: " + theme->getHighlightColorHex() + ";}";

    setStyleSheet(styleSheet() + groupBoxStyle + scrollbarStyle);
}


/**
 * @brief ChartInputPopup::requestExperimentRuns
 */
void ChartInputPopup::requestExperimentRuns()
{
    resizePopup(); // remove this when auto-complete is implemented

    auto future = viewController_->getAggregationProxy().RequestExperimentRuns(experimentNameLineEdit_->text().trimmed());
    auto futureWatcher = new QFutureWatcher<QVector<ExperimentRun>>(this);

    connect(futureWatcher, &QFutureWatcher<QVector<ExperimentRun>>::finished, [=]() {
        try {
            populateExperimentRuns(futureWatcher->result().toList());
        } catch(const std::exception& ex) {
            NotificationManager::manager()->AddNotification("Failed to request experiment runs - " + QString::fromStdString(ex.what()),
                                                            "Icons", "chart",
                                                            Notification::Severity::ERROR,
                                                            Notification::Type::APPLICATION,
                                                            Notification::Category::NONE);
        }
    });

    futureWatcher->setFuture(future);
}


/**
 * @brief ChartInputPopup::setPopupVisible
 * @param visible
 */
void ChartInputPopup::setPopupVisible(bool visible)
{
    setVisible(visible);

    if (visible) {
        if (originalCenterPos_.isNull()) {
            originalCenterPos_ = pos() + QPointF(sizeHint().width()/2.0, sizeHint().height()/2.0);
        }
        activateWindow();
        experimentNameLineEdit_->setFocus();
        experimentNameLineEdit_->selectAll();
        requestExperimentRuns();
    }
}


/**
 * @brief ChartInputPopup::filterMenuTriggered
 * This should only be called when there is a selected experiment run
 * The filter action/menu should be disabled other wise
 * @param action
 */
void ChartInputPopup::filterMenuTriggered(QAction* action)
{
    if (!action)
        return;

    auto filter = (FILTER_KEY) action->property(FILTER).toUInt();
    if (action->isChecked()) {
        auto groupBox = getFilterGroupBox(filter);
        if (!groupBox)
            return;
        // hiding then showing the groupbox here stops the glitching
        groupBox->hide();
        clearGroupBox(filter);
        populateGroupBox(filter);
        groupBox->show();
    }

    resizePopup();
    recenterPopup();
}


/**
 * @brief ChartInputPopup::accept
 */
void ChartInputPopup::accept()
{
    if (selectedExperimentRunID_ != -1) {
        emit requestEventsForExperimentRun(selectedExperimentRun_);
    }

    resetPopup();
    PopupWidget::accept();
}


/**
 * @brief ChartInputPopup::reject
 */
void ChartInputPopup::reject()
{
    resetPopup();
    PopupWidget::reject();
}


/**
 * @brief ChartInputPopup::experimentRunSelected
 * @param experimentRun
 */
void ChartInputPopup::experimentRunSelected(ExperimentRun experimentRun)
{
    // we only need to request the experiment state if the filter widgets are enabled
    /*if (filtersEnabled_) {
        emit requestExperimentState(ID);
    }*/

    selectedExperimentRun_ = experimentRun;
    selectedExperimentRunID_ = experimentRun.experiment_run_id;
}


/**
 * @brief ChartInputPopup::populateExperimentRuns
 * @param runs
 */
void ChartInputPopup::populateExperimentRuns(QList<ExperimentRun> runs)
{
    experimentRunsGroupBox_->hide();

    // clear previous experimentRun widgets
    clearGroupBox(FILTER_KEY::RUNS_FILTER);

    // hiding it first, resizes the widget immediately
    resizePopup();

    if (filterAction_)
        filterAction_->setEnabled(false);

    if (runs.isEmpty()) {
        recenterPopup();
        return;
    }

    for (auto run : runs) {
        auto ID = run.experiment_run_id;
        //QString text = "[" + QString::number(ID) + "] " + run.experiment_name +
        QString text = run.experiment_name + " [" + QString::number(ID) + "] - started at " +
                       QDateTime::fromMSecsSinceEpoch(run.start_time).toString("MMM d, hh:mm:ss.zzz");

        QRadioButton* button = new QRadioButton(text, this);
        button->setProperty("ID", ID);
        button->setStyleSheet("color:" + Theme::theme()->getTextColorHex() + ";");
        groupBoxLayouts[FILTER_KEY::RUNS_FILTER]->addWidget(button);
        connect(button, &QRadioButton::toggled, [=](bool checked) {
            if (checked) { experimentRunSelected(run); }
        });
    }

    experimentRunsGroupBox_->show();
    resizePopup();
    recenterPopup();
}


/**
 * @brief ChartInputPopup::populateGroupBox
 * @param filter
 */
void ChartInputPopup::populateGroupBox(ChartInputPopup::FILTER_KEY filter)
{
    QGroupBox* groupBox = getFilterGroupBox(filter);
    if (!groupBox)
        return;

    auto filterList = getFilterList(filter);
    for (auto text : filterList) {
        QRadioButton* button = new QRadioButton(text, this);
        groupBoxLayouts[filter]->addWidget(button);
        button->setProperty("ID", text);
        button->setStyleSheet("color:" + Theme::theme()->getTextColorHex() + ";");
        connect(button, &QRadioButton::toggled, [=](bool checked) {
            if (checked)
                getSelectedFilter(filter) = button->text();
        });
    }

    resizePopup();
}


/**
 * @brief ChartInputPopup::clearGroupBox
 * @param filter
 */
void ChartInputPopup::clearGroupBox(ChartInputPopup::FILTER_KEY filter)
{
    QGroupBox* groupBox = getFilterGroupBox(filter);
    if (!groupBox)
        return;

    switch (filter) {
    case FILTER_KEY::RUNS_FILTER:
        selectedExperimentRunID_ = -1;
        break;
    default:
        getSelectedFilter(filter) = "";
        break;
    }

    auto layout = groupBoxLayouts.value(filter, 0);
    if (!layout)
        return;

    auto buttons = groupBox->findChildren<QRadioButton*>();
    while (!buttons.isEmpty()) {
        auto button = buttons.takeFirst();
        layout->removeWidget(button);
        button->deleteLater();
    }
}


/**
 * @brief ChartInputPopup::hideGroupBoxes
 */
void ChartInputPopup::hideGroupBoxes()
{
    // hide all the filter groupboxes whenever this popup is closed or the search has changed
    for (auto key : getFilterKeys()) {
        auto groupBox = getFilterGroupBox(key);
        if (groupBox)
            groupBox->hide();
    }
    updateGeometry();
}


/**
 * @brief ChartInputPopup::recenterPopup
 */
void ChartInputPopup::recenterPopup()
{
    // re-center position
    auto newSize = sizeHint() / 2.0;
    move(originalCenterPos_.x() - newSize.width(), originalCenterPos_.y() - newSize.height());
}


/**
 * @brief ChartInputPopup::resizePopup
 */
void ChartInputPopup::resizePopup()
{
    experimentRunsGroupBox_->setFixedHeight(qMin(GROUPBOX_MAX_HEIGHT, experimentRunsScrollWidget_->sizeHint().height()) + 45);
    adjustChildrenSize("", Qt::FindDirectChildrenOnly);
    updateGeometry();
}


/**
 * @brief ChartInputPopup::reset
 */
void ChartInputPopup::resetPopup()
{
    selectedExperimentRunID_ = -1;
    hideGroupBoxes();
}


/**
 * @brief ChartInputPopup::setupLayout
 */
void ChartInputPopup::setupLayout()
{
    /*
     * Setup Widgets
     */

    experimentNameLineEdit_ = new QLineEdit(this);
    experimentNameLineEdit_->setFixedHeight(40);
    experimentNameLineEdit_->setMinimumWidth(MIN_WIDTH);
    experimentNameLineEdit_->setFont(QFont(font().family(), 10, QFont::ExtraLight));
    experimentNameLineEdit_->setPlaceholderText("Enter experiment name...");
    experimentNameLineEdit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    experimentNameLineEdit_->setAttribute(Qt::WA_MacShowFocusRect, false);
    connect(experimentNameLineEdit_, &QLineEdit::textEdited, this, &ChartInputPopup::requestExperimentRuns);

    experimentNameGroupBox_ = new QGroupBox("Visualise Events For Experiment:", this);
    QVBoxLayout* topLayout = constructVBoxLayout(experimentNameGroupBox_);
    topLayout->addWidget(experimentNameLineEdit_);

    // EXPERIMENT RUNS GROUP BOX
    {
        experimentRunsScrollWidget_ = new QWidget(this);
        experimentRunsScrollWidget_->setStyleSheet("background: rgba(0,0,0,0);");
        experimentRunsScrollWidget_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
        groupBoxLayouts[FILTER_KEY::RUNS_FILTER] = constructVBoxLayout(experimentRunsScrollWidget_, GROUPBOX_ITEM_SPACING);

        QScrollArea* scroll = new QScrollArea(this);
        scroll->setWidget(experimentRunsScrollWidget_);
        scroll->setWidgetResizable(true);
        scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        experimentRunsGroupBox_ = new QGroupBox("Select Experiment Run:", this);
        experimentRunsGroupBox_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

        QVBoxLayout* layout = constructVBoxLayout(experimentRunsGroupBox_);
        layout->addWidget(scroll);
    }

    toolbar_ = new QToolBar(this);
    toolbar_->setMinimumWidth(MIN_WIDTH);
    toolbar_->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    QWidget* spacerWidget = new QWidget(this);
    spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolbar_->addWidget(spacerWidget);

    cancelAction_ = toolbar_->addAction("Cancel");
    connect(cancelAction_, &QAction::triggered, this, &ChartInputPopup::reject);

    okAction_ = toolbar_->addAction("Ok");
    connect(okAction_, &QAction::triggered, this, &ChartInputPopup::accept);

    QWidget* holderWidget = new QWidget(this);
    holderWidget->setStyleSheet("background: rgba(0,0,0,0);");

    QVBoxLayout* mainLayout = constructVBoxLayout(holderWidget, 5, 5);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->addWidget(experimentNameGroupBox_);
    mainLayout->addWidget(experimentRunsGroupBox_, 1);
    mainLayout->addWidget(toolbar_);

    setWidget(holderWidget);
}


/**
 * @brief ChartInputPopup::setupFilterWidgets
 */
void ChartInputPopup::setupFilterWidgets()
{
    if (!toolbar_)
        return;

    filterMenu_ = new QMenu(this);
    connect(filterMenu_, &QMenu::triggered, this, &ChartInputPopup::filterMenuTriggered);

    filterAction_ = new QAction("Filters");
    filterAction_->setMenu(filterMenu_);
    filterAction_->setEnabled(false);

    // the first item in the toolbar is a spacer widget
    QAction* spacerAction = toolbar_->actions().at(0);
    toolbar_->insertAction(spacerAction, filterAction_);

    QToolButton* button = (QToolButton*) toolbar_->widgetForAction(filterAction_);
    button->setPopupMode(QToolButton::InstantPopup);
    button->setStyleSheet("QToolButton::menu-indicator{ image:none; }");

    nodesGroupBox_ = constructFilterWidgets(FILTER_KEY::NODE_FILTER, "Nodes");
    componentsGroupBox_ = constructFilterWidgets(FILTER_KEY::COMPONENT_FILTER, "Components");
    workersGroupBox_ = constructFilterWidgets(FILTER_KEY::WORKER_FILTER, "Workers");

    // insert the group boxes above the bottom toolbar
    if (getWidget() && getWidget()->layout()) {
        auto vLayout = (QVBoxLayout*) getWidget()->layout();
        int index = vLayout->indexOf(toolbar_);
        vLayout->insertWidget(index, workersGroupBox_);
        vLayout->insertWidget(index, componentsGroupBox_);
        vLayout->insertWidget(index, nodesGroupBox_);
    }
}


/**
 * @brief ChartInputPopup::getSelectedFilter
 * @param filter
 * @return
 */
QString& ChartInputPopup::getSelectedFilter(ChartInputPopup::FILTER_KEY filter)
{
    switch (filter) {
    case FILTER_KEY::NODE_FILTER:
        return selectedNode_;
    case FILTER_KEY::COMPONENT_FILTER:
        return selectedComponent_;
    case FILTER_KEY::WORKER_FILTER:
        return selectedWorker_;
    default:
        return QString();
    }
}


/**
 * @brief ChartInputPopup::getFilterList
 * @param filter
 * @return
 */
QStringList& ChartInputPopup::getFilterList(ChartInputPopup::FILTER_KEY filter)
{
    switch (filter) {
    case FILTER_KEY::NODE_FILTER:
        return nodes_;
    case FILTER_KEY::COMPONENT_FILTER:
        return components_;
    case FILTER_KEY::WORKER_FILTER:
        return workers_;
    default:
        return QStringList();
    }
}


/**
 * @brief ChartInputPopup::getFilterGroupBox
 * @param filter
 * @return
 */
QGroupBox* ChartInputPopup::getFilterGroupBox(ChartInputPopup::FILTER_KEY filter)
{
    switch (filter) {
    case FILTER_KEY::RUNS_FILTER:
        return experimentRunsGroupBox_;
    case FILTER_KEY::NODE_FILTER:
        return nodesGroupBox_;
    case FILTER_KEY::COMPONENT_FILTER:
        return componentsGroupBox_;
    case FILTER_KEY::WORKER_FILTER:
        return workersGroupBox_;
    default:
        return 0;
    }
}


/**
 * @brief ChartInputPopup::constructFilterWidgets
 * @param filter
 * @param filterName
 * @return
 */
QGroupBox* ChartInputPopup::constructFilterWidgets(ChartInputPopup::FILTER_KEY filter, QString filterName)
{
    QGroupBox* groupBox = new QGroupBox("Filter Events By " + filterName + ":", this);
    groupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    groupBoxLayouts[filter] = constructVBoxLayout(groupBox, GROUPBOX_ITEM_SPACING);

    QAction* action = filterMenu_->addAction(filterName);
    action->setProperty(FILTER, (uint)filter);
    action->setCheckable(true);
    connect(action, &QAction::toggled, groupBox, &QGroupBox::setVisible);

    return groupBox;
}


/**
 * @brief ChartInputPopup::constructVBoxLayout
 * @param widget
 * @return
 */
QVBoxLayout* ChartInputPopup::constructVBoxLayout(QWidget* widget, int spacing, int margin)
{
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setMargin(margin);
    layout->setSpacing(spacing);
    layout->setContentsMargins(1, 5, 1, 1);
    return layout;
}
