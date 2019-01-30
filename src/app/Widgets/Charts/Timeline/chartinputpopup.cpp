#include "chartinputpopup.h"
#include "../../../theme.h"

#include <QScrollArea>
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
    okAction_ = toolbar_->addAction("Ok");

    connect(okAction_, &QAction::triggered, this, &ChartInputPopup::accept);
    connect(cancelAction_, &QAction::triggered, this, &ChartInputPopup::reject);

    QWidget* holderWidget = new QWidget(this);
    QVBoxLayout* mainLayout = constructVBoxLayout(holderWidget, 5, 5);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->addWidget(experimentNameGroupBox_);
    mainLayout->addWidget(experimentRunsGroupBox_, 1);
    mainLayout->addWidget(toolbar_);

    setWidget(holderWidget);
    setVisible(false);
    setModal(true);

    //enableFilters();
    hideGroupBoxes();

    connect(Theme::theme(), &Theme::theme_Changed, this, &ChartInputPopup::themeChanged);
    themeChanged();
}


/**
 * @brief ChartInputPopup::setViewController
 * @param controller
 */
void ChartInputPopup::setViewController(ViewController* controller)
{
    viewController_ = controller;

    if (controller) {
        connect(controller, &ViewController::vc_viewItemsInChart, this, &ChartInputPopup::receivedSelectedViewItems);

        connect(&controller->getAggregationProxy(), &AggregationProxy::setChartUserInputDialogVisible, this, &ChartInputPopup::setPopupVisible);
        connect(&controller->getAggregationProxy(), &AggregationProxy::requestedExperimentRuns, this, &ChartInputPopup::populateExperimentRuns);
        connect(&controller->getAggregationProxy(), &AggregationProxy::requestedExperimentState, this, &ChartInputPopup::receivedExperimentState);

        connect(this, &ChartInputPopup::setExperimentRunID, &controller->getAggregationProxy(), &AggregationProxy::SetRequestExperimentRunID);
        connect(this, &ChartInputPopup::setEventKinds, &controller->getAggregationProxy(), &AggregationProxy::SetRequestEventKinds);

        connect(this, &ChartInputPopup::requestExperimentRuns, &controller->getAggregationProxy(), &AggregationProxy::RequestExperimentRuns);
        connect(this, &ChartInputPopup::requestExperimentState, &controller->getAggregationProxy(), &AggregationProxy::RequestExperimentState);
        connect(this, &ChartInputPopup::requestAllEvents, &controller->getAggregationProxy(), &AggregationProxy::RequestAllEvents);

        connect(this, &ChartInputPopup::requestPortLifecycleEvents, &controller->getAggregationProxy(), &AggregationProxy::RequestPortLifecycleEvents);
        connect(this, &ChartInputPopup::requestWorkloadEvents, &controller->getAggregationProxy(), &AggregationProxy::RequestWorkloadEvents);
        connect(this, &ChartInputPopup::requestCPUUtilisationEvents, &controller->getAggregationProxy(), &AggregationProxy::RequestCPUUtilisationEvents);
        connect(this, &ChartInputPopup::requestMemoryUtilisationEvents, &controller->getAggregationProxy(), &AggregationProxy::RequestMemoryUtilisationEvents);
    }
}


/**
 * @brief ChartInputPopup::enableFilters
 * NOTE - If this is called before setWidget() has been called, the groupboxes aren't put in the right layout`
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
    toolbar_->setStyleSheet(theme->getToolBarStyleSheet());

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
    }

    // NOTE - the default position of the popup is in the center of the screen
    /*if (originalCenterPos_.isNull())
        originalCenterPos_ = geometry().center();*/
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

    if (filterAction_)
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
                // we only need to request the experiment state if the filter widgets are enabled
                if (filtersEnabled_) {
                    emit requestExperimentState(ID);
                }
                selectedExperimentRunID_ = ID;
                emit setExperimentRunID(ID);
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

    if (!filtersEnabled_)
        return;

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
 * @brief ChartInputPopup::receivedSelectedViewItems
 * @param selectedItems
 * @param dataKinds
 */
void ChartInputPopup::receivedSelectedViewItems(QVector<ViewItem*> selectedItems, QList<TIMELINE_DATA_KIND>& dataKinds)
{
    // at this point, all selected node items should have a valid chart data kind to show
    // all selected items are from a single aspect - the active aspect

    if (!viewController_ || dataKinds.isEmpty())
        return;

    // clear request filters
    resetFilters();

    hasSelection_ = !selectedItems.isEmpty();
    eventKinds_ = dataKinds;

    // send a separate filtered request per selected node item
    for (auto item : selectedItems) {

        if (!item || !item->isNode())
            continue;

        auto nodeItem = (NodeViewItem*) item;
        auto label = getItemLabel(nodeItem);
        switch (nodeItem->getNodeKind()) {
        case NODE_KIND::COMPONENT:
        case NODE_KIND::COMPONENT_IMPL:
            // can send port/workload requests
            compNames_.append(label);
            break;
        case NODE_KIND::COMPONENT_INSTANCE:
            // can send port/workload requests
            compInstPaths_.append(getItemLabel(nodeItem->getParentItem()) + ".%/" + label);
            break;
        case NODE_KIND::PORT_REPLIER:
        case NODE_KIND::PORT_REPLIER_IMPL:
        case NODE_KIND::PORT_REQUESTER:
        case NODE_KIND::PORT_REQUESTER_IMPL:
        case NODE_KIND::PORT_PUBLISHER:
        case NODE_KIND::PORT_PUBLISHER_IMPL:
        case NODE_KIND::PORT_SUBSCRIBER:
        case NODE_KIND::PORT_SUBSCRIBER_IMPL:
        case NODE_KIND::PORT_PERIODIC:
            // can send port requests
            if (dataKinds.contains(TIMELINE_DATA_KIND::PORT_LIFECYCLE)) {
                for (auto instItem : viewController_->getNodesInstances(item->getID())) {
                    if (instItem) {
                        auto compInstItem = instItem->getParentItem();
                        if (compInstItem)
                            portPaths_.append(getItemLabel(compInstItem->getParentItem()) + ".%/" + getItemLabel(compInstItem) + "/" + label);
                    }
                }
            }
            break;
        case NODE_KIND::PORT_REPLIER_INST:
        case NODE_KIND::PORT_REQUESTER_INST:
        case NODE_KIND::PORT_PUBLISHER_INST:
        case NODE_KIND::PORT_SUBSCRIBER_INST:
        case NODE_KIND::PORT_PERIODIC_INST:
            // can send port requests
            if (dataKinds.contains(TIMELINE_DATA_KIND::PORT_LIFECYCLE)) {
                auto compInstItem = nodeItem->getParentItem();
                if (compInstItem)
                    portPaths_.append(getItemLabel(compInstItem->getParentItem()) + ".%/" + getItemLabel(compInstItem) + "/" + label);
            }
            break;
        case NODE_KIND::CLASS_INSTANCE:
            // can send workload requests
            if (dataKinds.contains(TIMELINE_DATA_KIND::WORKLOAD)) {
                // a ClassInstance can be a child of either a CompImpl or CompInst
                auto parentNodeKind = nodeItem->getParentNodeKind();
                if (parentNodeKind == NODE_KIND::COMPONENT_IMPL) {
                    for (auto instItem : viewController_->getNodesInstances(item->getID())) {
                        if (instItem) {
                            auto compInstItem = instItem->getParentItem();
                            if (compInstItem)
                                workerPaths_.append(getItemLabel(compInstItem->getParentItem()) + ".%/" + getItemLabel(compInstItem) + "/" + label);
                        }
                    }
                } else if (parentNodeKind == NODE_KIND::COMPONENT_INSTANCE) {
                    auto compInstItem = nodeItem->getParentItem();
                    if (compInstItem)
                        workerPaths_.append(getItemLabel(compInstItem->getParentItem()) + ".%/" + getItemLabel(compInstItem) + "/" + label);
                }
            }
            break;
        case NODE_KIND::HARDWARE_NODE:
            // can send cpu/mem requests
            nodeIDs_.append(label);
            break;
        default:
            break;
        }
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
    if (selectedExperimentRunID_ == -1)
        return;

    // if there is no selection, show all the data for the selected experiment run
    if (!hasSelection_) {
        resetFilters();
    }

    if (!eventKinds_.isEmpty()) {

        bool hasValidRequests = true;

        for (auto kind : eventKinds_) {
            switch (kind) {
            case TIMELINE_DATA_KIND::PORT_LIFECYCLE: {
                PortLifecycleRequest request;
                request.experimentRunID = selectedExperimentRunID_;
                request.port_paths = portPaths_;
                request.component_names = compNames_;
                request.component_instance_paths = compInstPaths_;
                emit requestPortLifecycleEvents(request);
                break;
            }
            case TIMELINE_DATA_KIND::WORKLOAD: {
                WorkloadRequest request;
                request.experimentRunID = selectedExperimentRunID_;
                request.worker_paths = workerPaths_;
                request.component_names = compNames_;
                request.component_instance_paths = compInstPaths_;
                emit requestWorkloadEvents(request);
                break;
            }
            case TIMELINE_DATA_KIND::CPU_UTILISATION: {
                CPUUtilisationRequest request;
                request.experimentRunID = selectedExperimentRunID_;
                request.node_ids = nodeIDs_;
                emit requestCPUUtilisationEvents(request);
                break;
            }
            case TIMELINE_DATA_KIND::MEMORY_UTILISATION: {
                MemoryUtilisationRequest request;
                request.experimentRunID = selectedExperimentRunID_;
                request.node_ids = nodeIDs_;
                emit requestMemoryUtilisationEvents(request);
                break;
            }
            default:
                hasValidRequests = false;
                break;
            }
        }

        if (!hasValidRequests) {
            NotificationManager::manager()->AddNotification("No chart data for selection", "Icons", "chart", Notification::Severity::INFO, Notification::Type::APPLICATION, Notification::Category::NONE);
        }

    } else {
        // request all events for all event kinds
        emit requestAllEvents();
    }

    hasSelection_ = false;
    hideGroupBoxes();
    PopupWidget::accept();
}


/**
 * @brief ChartInputPopup::reject
 */
void ChartInputPopup::reject()
{
    hasSelection_ = false;
    hideGroupBoxes();
    PopupWidget::reject();
}


/**
 * @brief ChartInputPopup::getItemLabel
 * @param item
 * @return
 */
QString ChartInputPopup::getItemLabel(ViewItem *item)
{
    if (item) {
        return item->getData("label").toString();
    }
    return "";
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
 * @brief ChartInputPopup::resetFilters
 */
void ChartInputPopup::resetFilters()
{
    eventKinds_.clear();
    compNames_.clear();
    compInstPaths_.clear();
    portPaths_.clear();
    workerPaths_.clear();
    nodeIDs_.clear();
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
