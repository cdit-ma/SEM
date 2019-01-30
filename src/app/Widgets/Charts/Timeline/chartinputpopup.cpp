#include "chartinputpopup.h"
#include "../../../theme.h"

#include <QScrollArea>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QToolButton>
#include <QDateTime>

#define MIN_WIDTH 400
#define FILTER "filter"
#define GROUPBOX_ITEM_SPACING 3
#define GROUPBOX_MAX_HEIGHT 300


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
    experimentNameLineEdit_->setMinimumWidth(MIN_WIDTH);
    experimentNameLineEdit_->setFont(QFont(font().family(), 10, QFont::ExtraLight));
    experimentNameLineEdit_->setPlaceholderText("Enter experiment name...");
    experimentNameLineEdit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    experimentNameLineEdit_->setAttribute(Qt::WA_MacShowFocusRect, false);

    connect(experimentNameLineEdit_, &QLineEdit::textEdited, [=](QString text) {
        hideGroupBoxes();
        emit requestExperimentRuns(text.trimmed());
    });

    experimentNameGroupBox_ = new QGroupBox("Visualise Events For Experiment:", this);
    QVBoxLayout* topLayout = new QVBoxLayout(experimentNameGroupBox_);
    topLayout->setMargin(0);
    topLayout->setContentsMargins(1, 5, 1, 1);
    topLayout->addWidget(experimentNameLineEdit_);

    experimentRunsGroupBox_ = constructFilterWidgets(FILTER_KEY::RUNS_FILTER, "Runs", false, true);
    experimentRunsGroupBox_->setTitle("Select Experiment Run:");

    QWidget* spacerWidget = new QWidget(this);
    spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    toolbar_ = new QToolBar(this);
    toolbar_->setMinimumWidth(MIN_WIDTH);
    toolbar_->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    filterMenu_ = new QMenu(this);
    connect(filterMenu_, &QMenu::triggered, this, &ChartInputPopup::filterMenuTriggered);

    nodesGroupBox_ = constructFilterWidgets(FILTER_KEY::NODE_FILTER, "Nodes");
    componentsGroupBox_ = constructFilterWidgets(FILTER_KEY::COMPONENT_FILTER, "Components");
    workersGroupBox_ = constructFilterWidgets(FILTER_KEY::WORKER_FILTER, "Workers");

    filterAction_ = toolbar_->addAction("Filters");
    filterAction_->setMenu(filterMenu_);
    filterAction_->setEnabled(false);

    QToolButton* button = (QToolButton*) toolbar_->widgetForAction(filterAction_);
    button->setPopupMode(QToolButton::InstantPopup);
    button->setStyleSheet("QToolButton::menu-indicator{ image:none; }");

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
    mainLayout->addWidget(nodesGroupBox_, 1);
    mainLayout->addWidget(componentsGroupBox_, 1);
    mainLayout->addWidget(workersGroupBox_, 1);
    mainLayout->addWidget(toolbar_);

    hideGroupBoxes();

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
        emit requestExperimentRuns(experimentNameLineEdit_->text().trimmed());

        // TODO - Only request experiments here if the search string has changed
        // or when a refresh is required (a new experiment/run has been added)
        /*if (experimentNameLineEdit_->text().isEmpty())
            emit requestExperimentRuns();*/
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
    // clear previous experimentRun widgets
    clearGroupBox(FILTER_KEY::RUNS_FILTER);

    // hiding it first, resizes the widget immediately
    resizePopup();
    filterAction_->setEnabled(false);

    if (runs.isEmpty()) {
        recenterPopup();
        return;
    }

    for (auto run : runs) {
        auto ID = run.experiment_run_id;
        QString text = run.experiment_name + "[" + QString::number(ID) + "] - started at " +
                       QDateTime::fromMSecsSinceEpoch(run.start_time).toString("MMM d, hh:mm:ss.zzz");

        QRadioButton* button = new QRadioButton(text, this);
        button->setProperty("ID", ID);
        button->setStyleSheet("color:" + Theme::theme()->getTextColorHex() + ";");
        groupBoxLayouts[FILTER_KEY::RUNS_FILTER]->addWidget(button);
        connect(button, &QRadioButton::toggled, [=](bool checked) {
            if (checked) {
                //experimentName_ = run.experiment_name + "[" + QString::number(ID) + "]";
                selectedExperimentRunID_ = ID;
                emit requestExperimentState(ID);
            }
        });
    }

    experimentRunsGroupBox_->show();
    resizePopup();
    recenterPopup();
}


/**
 * @brief ChartInputPopup::receivedExperimentState
 * @param nodeHostname
 * @param componentName
 * @param workerName
 */
void ChartInputPopup::receivedExperimentState(QStringList nodeHostnames, QStringList componentNames, QStringList workerNames)
{
    nodes_ = nodeHostnames;
    components_ = componentNames;
    workers_ = workerNames;

    // if the corresponding filter section is visible, clear then re-populate it
    for (auto action : filterMenu_->actions()) {
        auto filter = (FILTER_KEY) action->property(FILTER).toUInt();
        bool showAction = !getFilterList(filter).isEmpty();
        action->setVisible(showAction);
        if (showAction) {
            filterAction_->setEnabled(true);
            filterMenuTriggered(action);
        } else if (action->isChecked()) {
            action->toggled(false);
            resizePopup();
        }
    }
}


/**
 * @brief ChartInputPopup::filterMenuTriggered
 * This should onle be called when there is a selected experiment run.
 * The filter action/menu should be disabled other wise.
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
        //emit setChartTitle(experimentName_);
        emit requestEvents(selectedNode_, selectedComponent_, selectedWorker_);
    }
    hideGroupBoxes();
    PopupWidget::accept();
}


/**
 * @brief ChartInputPopup::reject
 */
void ChartInputPopup::reject()
{
    hideGroupBoxes();
    PopupWidget::reject();
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

    switch (filter) {
    case FILTER_KEY::RUNS_FILTER:
        selectedExperimentRunID_ = -1;
        break;
    default:
        getSelectedFilter(filter) = "";
        break;
    }

    if (!groupBox)
        return;

    /*
    auto buttons = groupBox->findChildren<QRadioButton*>();
    while (!buttons.isEmpty()) {
        auto button = buttons.takeFirst();
        groupBox->layout()->removeWidget(button);
        button->deleteLater();
    }
    */

    auto layout = groupBoxLayouts.value(filter, 0);
    if (layout) {
        auto buttons = groupBox->findChildren<QRadioButton*>();
        while (!buttons.isEmpty()) {
            auto button = buttons.takeFirst();
            layout->removeWidget(button);
            button->deleteLater();
        }
    }
}


/**
 * @brief ChartInputPopup::hideGroupBoxes
 */
void ChartInputPopup::hideGroupBoxes()
{
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
    experimentRunsGroupBox_->setFixedHeight(qMin(GROUPBOX_MAX_HEIGHT, scrollWidget->sizeHint().height()) + 38);
    adjustChildrenSize("", Qt::FindDirectChildrenOnly);
    updateGeometry();
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
 * @param addToFilterMenu
 * @param scrollable
 * @return
 */
QGroupBox* ChartInputPopup::constructFilterWidgets(ChartInputPopup::FILTER_KEY filter, QString filterName, bool addToFilterMenu, bool scrollable)
{
    QGroupBox* groupBox = new QGroupBox("Filter Events By " + filterName + ":", this);
    groupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setMargin(0);

    if (scrollable) {

        scrollWidget = new QWidget(this);
        scrollWidget->setStyleSheet("background: rogba(0,0,0,0);");
        scrollWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

        QVBoxLayout* scrollLayout = new QVBoxLayout(scrollWidget);
        scrollLayout->setMargin(0);
        scrollLayout->setSpacing(GROUPBOX_ITEM_SPACING);
        scrollLayout->setContentsMargins(1, 5, 1, 1);

        QScrollArea* scroll = new QScrollArea(this);
        scroll->setWidget(scrollWidget);
        scroll->setWidgetResizable(true);
        scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        qDebug() << "contents margin: " << scroll->contentsMargins();

        layout->addWidget(scroll);
        groupBoxLayouts[filter] = scrollLayout;

    } else {
        layout->setSpacing(GROUPBOX_ITEM_SPACING);
        layout->setContentsMargins(1, 5, 1, 1);
        groupBoxLayouts[filter] = layout;
    }

    if (addToFilterMenu) {
        QAction* action = filterMenu_->addAction(filterName);
        action->setProperty(FILTER, (uint)filter);
        action->setCheckable(true);
        connect(action, &QAction::toggled, groupBox, &QGroupBox::setVisible);
    }

    return groupBox;
}
