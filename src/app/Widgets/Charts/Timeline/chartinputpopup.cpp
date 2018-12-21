#include "chartinputpopup.h"
#include "../../../theme.h"

#include <QVBoxLayout>
#include <QRadioButton>
#include <QToolButton>
#include <QDateTime>

#define FILTER "filter"


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

    connect(experimentNameLineEdit_, &QLineEdit::textEdited, [=](QString text) {
        emit requestExperimentRuns(text.trimmed());
    });

    experimentNameGroupBox_ = new QGroupBox("Visualise Events For Experiment:", this);
    QVBoxLayout* topLayout = new QVBoxLayout(experimentNameGroupBox_);
    topLayout->setMargin(0);
    topLayout->setContentsMargins(1, 5, 1, 1);
    topLayout->addWidget(experimentNameLineEdit_);

    experimentRunsGroupBox_ = new QGroupBox("Select Experiment Run:", this);
    QVBoxLayout* runsLayout = new QVBoxLayout(experimentRunsGroupBox_);
    runsLayout->setMargin(0);
    runsLayout->setSpacing(2);
    runsLayout->setContentsMargins(1, 5, 1, 1);

    nodesGroupBox_ = new QGroupBox("Filter Events By Node(s):", this);
    QVBoxLayout* nodesLayout = new QVBoxLayout(nodesGroupBox_);
    nodesLayout->setMargin(0);
    nodesLayout->setSpacing(2);
    nodesLayout->setContentsMargins(1, 5, 1, 1);

    componentsGroupBox_ = new QGroupBox("Filter Events By Component(s):", this);
    QVBoxLayout* compLayout = new QVBoxLayout(componentsGroupBox_);
    compLayout->setMargin(0);
    compLayout->setSpacing(2);
    compLayout->setContentsMargins(1, 5, 1, 1);

    workersGroupBox_ = new QGroupBox("Filter Events By Worker(s):", this);
    QVBoxLayout* workerLayout = new QVBoxLayout(workersGroupBox_);
    workerLayout->setMargin(0);
    workerLayout->setSpacing(2);
    workerLayout->setContentsMargins(1, 5, 1, 1);

    QWidget* spacerWidget = new QWidget(this);
    spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    toolbar_ = new QToolBar(this);
    toolbar_->setMinimumWidth(400);
    toolbar_->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    filterMenu_ = new QMenu(this);
    filterMenu_->addAction("Nodes")->setProperty(FILTER, (uint)NODE_FILTER);
    filterMenu_->addAction("Components")->setProperty(FILTER, (uint)COMPONENT_FILTER);
    filterMenu_->addAction("Workers")->setProperty(FILTER, (uint)WORKER_FILTER);
    connect(filterMenu_, &QMenu::triggered, this, &ChartInputPopup::filterMenuTriggered);

    for (auto action : filterMenu_->actions()) {
        action->setCheckable(true);
    }

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

    experimentRunsGroupBox_->hide();
    nodesGroupBox_->hide();
    componentsGroupBox_->hide();
    workersGroupBox_->hide();

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

    auto groupBoxStyle = theme->getGroupBoxStyleSheet() +
                         "QGroupBox{color: lightGray; margin-top: 15px;}" +
                         "QGroupBox::title{subcontrol-origin: margin;}";

    experimentNameGroupBox_->setStyleSheet(groupBoxStyle);
    experimentRunsGroupBox_->setStyleSheet(groupBoxStyle);
    nodesGroupBox_->setStyleSheet(groupBoxStyle);
    componentsGroupBox_->setStyleSheet(groupBoxStyle);
    workersGroupBox_->setStyleSheet(groupBoxStyle);

    for (auto radioButton : findChildren<QRadioButton*>()) {
        radioButton->setStyleSheet("color:" + Theme::theme()->getTextColorHex() + ";");
    }
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

    if (runs.isEmpty()) {
        recenterPopup();
        return;
    }

    // clear experimentStateResponse lists
    nodes_.clear();
    components_.clear();
    workers_.clear();

    for (auto run : runs) {
        auto ID = run.experiment_run_id;
        QString text = run.experiment_name + "[" + QString::number(ID) + "] - started at " +
                       QDateTime::fromMSecsSinceEpoch(run.start_time).toString("MMM d, hh:mm:ss:zzz");

        QRadioButton* button = new QRadioButton(text, this);
        button->setProperty("ID", ID);
        button->setStyleSheet("color:" + Theme::theme()->getTextColorHex() + ";");
        experimentRunsGroupBox_->layout()->addWidget(button);
        connect(button, &QRadioButton::toggled, [=](bool checked) {
            filterAction_->setEnabled(true);
            if (checked) {
                selectedExperimentRunID_ = ID;
                emit requestExperimentState(ID);
            }
        });
    }

    experimentRunsGroupBox_->show();
    updateGeometry();
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
}


/**
 * @brief ChartInputPopup::filterMenuTriggered
 * @param action
 */
void ChartInputPopup::filterMenuTriggered(QAction* action)
{
    if (!action)
        return;

    auto filter = (FILTER_KEY) action->property(FILTER).toUInt();
    clearGroupBox(filter);

    switch (filter) {
    case FILTER_KEY::NODE_FILTER:
        populateNodes();
        break;
    case FILTER_KEY::COMPONENT_FILTER:
        populateComponents();
        break;
    case FILTER_KEY::WORKER_FILTER:
        populateWorkers();
        break;
    default:
        return;
    }

    updateGeometry();
    recenterPopup();
}


/**
 * @brief ChartInputPopup::accept
 */
void ChartInputPopup::accept()
{
    if (selectedExperimentRunID_ != -1) {
        emit requestEvents(selectedNode_, selectedComponent_, selectedWorker_);
    }
    PopupWidget::accept();

    /*
    if (experimentRunsGroupBox_->layout()) {
        auto buttons = experimentRunsGroupBox_->findChildren<QRadioButton*>();
        for (auto button : buttons) {
            if (button->isChecked()) {
                emit requestEvents(button->property("ID").toUInt()); //, componentNameLineEdit_->text().trimmed());
                break;
            }
        }
    }
    PopupWidget::accept();
    */
}


/**
 * @brief ChartInputPopup::reject
 */
void ChartInputPopup::reject()
{
    PopupWidget::reject();
}


/**
 * @brief ChartInputPopup::populateNodes
 */
void ChartInputPopup::populateNodes()
{
    if (nodes_.isEmpty() || !nodesGroupBox_->layout())
        return;

    for (auto node : nodes_) {
        QRadioButton* button = new QRadioButton(node, this);
        nodesGroupBox_->layout()->addWidget(button);
        button->setProperty("ID", node);
        button->setStyleSheet("color:" + Theme::theme()->getTextColorHex() + ";");
        connect(button, &QRadioButton::toggled, [=](bool checked) {
            if (checked)
                selectedNode_ = button->text();
        });
    }

    nodesGroupBox_->show();
}


/**
 * @brief ChartInputPopup::populateComponents
 */
void ChartInputPopup::populateComponents()
{
    if (components_.isEmpty() || !componentsGroupBox_->layout())
        return;

    for (auto component : components_) {
        QRadioButton* button = new QRadioButton(component, this);
        componentsGroupBox_->layout()->addWidget(button);
        button->setProperty("ID", component);
        button->setStyleSheet("color:" + Theme::theme()->getTextColorHex() + ";");
        connect(button, &QRadioButton::toggled, [=](bool checked) {
            if (checked)
                selectedComponent_ = button->text();
        });
    }

    componentsGroupBox_->show();
}


/**
 * @brief ChartInputPopup::populateWorkers
 */
void ChartInputPopup::populateWorkers()
{
    if (workers_.isEmpty() || !workersGroupBox_->layout())
        return;

    for (auto worker : workers_) {
        QRadioButton* button = new QRadioButton(worker, this);
        workersGroupBox_->layout()->addWidget(button);
        button->setProperty("ID", worker);
        button->setStyleSheet("color:" + Theme::theme()->getTextColorHex() + ";");
        connect(button, &QRadioButton::toggled, [=](bool checked) {
            if (checked)
                selectedWorker_ = button->text();
        });
    }

    workersGroupBox_->show();
}


/**
 * @brief ChartInputPopup::clearGroupBox
 * @param filter
 */
void ChartInputPopup::clearGroupBox(ChartInputPopup::FILTER_KEY filter)
{
    QGroupBox* groupBox = 0;
    switch (filter) {
    case FILTER_KEY::RUNS_FILTER: {
        groupBox = experimentRunsGroupBox_;
        selectedExperimentRunID_ = -1;
        break;
    }
    case FILTER_KEY::NODE_FILTER: {
        groupBox = nodesGroupBox_;
        selectedNode_ = "";
        break;
    }
    case FILTER_KEY::COMPONENT_FILTER: {
        groupBox = componentsGroupBox_;
        selectedComponent_ = "";
        break;
    }
    case FILTER_KEY::WORKER_FILTER: {
        groupBox = workersGroupBox_;
        selectedWorker_ = "";
        break;
    }
    default:
        return;
    }

    auto buttons = groupBox->findChildren<QRadioButton*>();
    while (!buttons.isEmpty()) {
        auto button = buttons.takeFirst();
        groupBox->layout()->removeWidget(button);
        button->deleteLater();
    }

    // hiding it first, resizes the widget immediately
    groupBox->hide();
    adjustChildrenSize("", Qt::FindDirectChildrenOnly);
}


/**
 * @brief ChartInputPopup::recenterPopup
 */
void ChartInputPopup::recenterPopup()
{
    // re-center position
    //auto newSize = experimentRunsGroupBox_->sizeHint() / 2.0;
    auto newSize = sizeHint() / 2.0;
    move(originalCenterPos_.x() - newSize.width(), originalCenterPos_.y() - newSize.height());
}
