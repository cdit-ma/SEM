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

    if (vc) {
        connect(vc, &ViewController::vc_viewItemsInChart, this, &ChartInputPopup::receivedSelectedViewItems);
        connect(vc, &ViewController::vc_showChartPopup, this, &ChartInputPopup::setPopupVisible);
    }

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
            toastRequestError("Failed to request experiment runs - " + QString::fromStdString(ex.what()), "Icons", "chart");
        }
    });
    futureWatcher->setFuture(future);
}

void ChartInputPopup::requestExperimentState(quint32 experiment_run_id)
{
    auto future = viewController_->getAggregationProxy().RequestExperimentState(experiment_run_id);
    auto futureWatcher = new QFutureWatcher<ExperimentState>(this);
    connect(futureWatcher, &QFutureWatcher<ExperimentState>::finished, [=]() {
        try {
            auto state = futureWatcher->result();
            qDebug() << "[Experiment State Request]";
            qDebug() << "nodes#: " << state.nodes.size();
            qDebug() << "components#: " << state.components.size();
            qDebug() << "workers#: " << state.workers.size();
            qDebug() << "last-updated-time: " << QDateTime::fromMSecsSinceEpoch(state.last_updated_time).toString("MMM d, hh:mm:ss.zzz");
            qDebug() << "end-time: " << QDateTime::fromMSecsSinceEpoch(state.end_time).toString("MMM d, hh:mm:ss.zzz");
            qDebug() << "-----------------------------------------------------------";
        } catch (const std::exception& ex) {
            toastRequestError("Failed to request experiment state - " + QString::fromStdString(ex.what()), "Icons", "chart");
        }
    });
    futureWatcher->setFuture(future);
}


/**
 * @brief ChartInputPopup::requestEvents
 * @param experimentRunID
 */
void ChartInputPopup::requestEvents(quint32 experiment_run_id)
{

}

void ChartInputPopup::requestPortLifecycleEvents(const quint32 experiment_run_id, const QVector<qint64> &time_intervals, const QVector<QString> &component_instance_ids, const QVector<QString> &port_ids)
{
    auto future = viewController_->getAggregationProxy().RequestPortLifecycleEvents(experiment_run_id, time_intervals, component_instance_ids, port_ids);
    auto futureWatcher = new QFutureWatcher<QVector<PortLifecycleEvent*>>(this);
    connect(futureWatcher, &QFutureWatcher<QVector<PortLifecycleEvent*>>::finished, [=]() {
        try {
            auto events = futureWatcher->result();
            qDebug() << "[PortLifecycle Request] Result size#: " << events.size();
            emit receivedPortLifecycleResponse(events);
            qDebug() << "-----------------------------------------------------------";
        } catch (const std::exception& ex) {
            toastRequestError("Failed to request port lifecycle events - " + QString::fromStdString(ex.what()), "Icons", "plug");
        }
    });
    futureWatcher->setFuture(future);

}

void ChartInputPopup::requestWorkloadEvents(const quint32 experiment_run_id, const QVector<qint64> &time_intervals, const QVector<QString> &component_instance_ids, const QVector<QString> &worker_ids)
{
    auto future = viewController_->getAggregationProxy().RequestWorkloadEvents(experiment_run_id, time_intervals, component_instance_ids, worker_ids);
    auto futureWatcher = new QFutureWatcher<QVector<WorkloadEvent*>>(this);
    connect(futureWatcher, &QFutureWatcher<QVector<WorkloadEvent*>>::finished, [=]() {
        try {
            auto events = futureWatcher->result();
            qDebug() << "[Workload Request] Result size#: " << events.size();
            emit receivedWorkloadResponse(events);
            qDebug() << "-----------------------------------------------------------";
        } catch (const std::exception& ex) {
            toastRequestError("Failed to request workload events - " + QString::fromStdString(ex.what()), "Icons", "spanner");
        }
    });
    futureWatcher->setFuture(future);

}

void ChartInputPopup::requestCPUUtilisationEvents(const quint32 experiment_run_id, const QVector<qint64> &time_intervals, const QVector<QString> &graphml_ids)
{
    auto future = viewController_->getAggregationProxy().RequestCPUUtilisationEvents(experiment_run_id, time_intervals, graphml_ids);
    auto futureWatcher = new QFutureWatcher<QVector<CPUUtilisationEvent*>>(this);
    connect(futureWatcher, &QFutureWatcher<QVector<CPUUtilisationEvent*>>::finished, [=]() {
        try {
            auto events = futureWatcher->result();
            qDebug() << "[CPUUtilisation Request] Result size#: " << events.size();
            emit receivedCPUUtilisationResponse(events);
            qDebug() << "-----------------------------------------------------------";
        } catch (const std::exception& ex) {
            toastRequestError("Failed to request cpu utilisation events - " + QString::fromStdString(ex.what()), "Icons", "cpu");
        }
    });
    futureWatcher->setFuture(future);
}

void ChartInputPopup::requestMemoryUtilisationEvents(const quint32 experiment_run_id, const QVector<qint64> &time_intervals, const QVector<QString> &graphml_ids)
{
    auto future = viewController_->getAggregationProxy().RequestMemoryUtilisationEvents(experiment_run_id, time_intervals, graphml_ids);
    auto futureWatcher = new QFutureWatcher<QVector<MemoryUtilisationEvent*>>(this);
    connect(futureWatcher, &QFutureWatcher<QVector<MemoryUtilisationEvent*>>::finished, [=]() {
        try {
            auto events = futureWatcher->result();
            qDebug() << "[MemoryUtilisation Request] Result size#: " << events.size();
            emit receivedMemoryUtilisationResponse(events);
            qDebug() << "-----------------------------------------------------------";
        } catch (const std::exception& ex) {
            toastRequestError("Failed to request memory utilisation events - " + QString::fromStdString(ex.what()), "Icons", "memoryCard");
        }
    });
    futureWatcher->setFuture(future);

}


/**
 * @brief ChartInputPopup::toastRequestError
 * @param description
 * @param iconPath
 * @param iconName
 */
void ChartInputPopup::toastRequestError(QString description, QString iconPath, QString iconName)
{
    NotificationManager::manager()->AddNotification(description, iconPath, iconName, Notification::Severity::ERROR, Notification::Type::APPLICATION, Notification::Category::NONE);
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
 * @brief ChartInputPopup::receivedSelectedViewItems
 * This slot is called when the user has selected an entity(s) to view chart data for
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
    resetPopup();

    hasSelection_ = !selectedItems.isEmpty();
    eventKinds_ = dataKinds;

    // send a separate filtered request per selected node item
    for (auto item : selectedItems) {

        if (!item || !item->isNode())
            continue;

        auto nodeItem = (NodeViewItem*) item;
        auto nodeItemID = nodeItem->getID();
        auto label = getItemLabel(nodeItem);

        switch (nodeItem->getNodeKind()) {
        case NODE_KIND::COMPONENT:
        case NODE_KIND::COMPONENT_IMPL:
            // can send port/workload requests
            compNames_.append(label);
            break;
        case NODE_KIND::COMPONENT_INSTANCE:
            // can send port/workload requests
            //compInstPaths_.append(getItemLabel(nodeItem->getParentItem()) + ".%/" + label);
            compInstIDs_.append(QString::number(nodeItem->getParentID()));
            break;
        case NODE_KIND::PORT_REPLIER_IMPL:
        case NODE_KIND::PORT_REQUESTER_IMPL:
        case NODE_KIND::PORT_PUBLISHER_IMPL:
        case NODE_KIND::PORT_SUBSCRIBER_IMPL:
            nodeItemID = viewController_->getNodeDefinitionID(nodeItemID);
        case NODE_KIND::PORT_REPLIER:
        case NODE_KIND::PORT_REQUESTER:
        case NODE_KIND::PORT_PUBLISHER:
        case NODE_KIND::PORT_SUBSCRIBER:
        case NODE_KIND::PORT_PERIODIC:
            // can send port requests
            if (dataKinds.contains(TIMELINE_DATA_KIND::PORT_LIFECYCLE)) {
                for (auto instID : viewController_->getNodeInstanceIDs(nodeItemID)) {
                    portIDs_.append(QString::number(instID) + "_0");
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
                    for (auto instID : viewController_->getNodeInstanceIDs(nodeItemID)) {
                        workerInstIDs_.append(QString::number(instID));
                    }
                } else if (parentNodeKind == NODE_KIND::COMPONENT_INSTANCE) {
                    workerInstIDs_.append(QString::number(nodeItemID));
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

    // show the popup so that the user can select which experiment run they want to display data from
    setPopupVisible(true);
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

    qDebug() << "-----------------------------------------------------------";

    if (hasSelection_) {
        for (auto kind : eventKinds_) {
            switch (kind) {
            case TIMELINE_DATA_KIND::PORT_LIFECYCLE:
                requestPortLifecycleEvents(selectedExperimentRunID_, {}, compInstIDs_, portIDs_);
                break;
            case TIMELINE_DATA_KIND::WORKLOAD:
                requestWorkloadEvents(selectedExperimentRunID_, {}, compInstIDs_, workerInstIDs_);
                break;
            case TIMELINE_DATA_KIND::CPU_UTILISATION:
                requestCPUUtilisationEvents(selectedExperimentRunID_, {}, nodeIDs_);
                break;
            case TIMELINE_DATA_KIND::MEMORY_UTILISATION:
                requestMemoryUtilisationEvents(selectedExperimentRunID_, {}, nodeIDs_);
                break;
            default:
                NotificationManager::manager()->AddNotification("No chart data for selection", "Icons", "chart", Notification::Severity::INFO, Notification::Type::APPLICATION, Notification::Category::NONE);
                break;
            }
        }
    } else {
        // if there is no selection, request all events for the selected experiment run
        //requestEvents(selectedExperimentRunID_);

        requestPortLifecycleEvents(selectedExperimentRunID_, {}, compInstIDs_, portIDs_);
        requestWorkloadEvents(selectedExperimentRunID_, {}, compInstIDs_, workerInstIDs_);
        requestCPUUtilisationEvents(selectedExperimentRunID_, {}, nodeIDs_);
        requestMemoryUtilisationEvents(selectedExperimentRunID_, {}, nodeIDs_);

    }

    requestExperimentState(selectedExperimentRunID_);
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
            if (checked) {
                // we only need to request the experiment state if the filter widgets are enabled
                if (filtersEnabled_) {
                    //emit requestExperimentState(ID);
                }
                selectedExperimentRunID_ = ID;
                emit selectedExperimentRunID(ID);
                emit selectedExperimentRun(run);
            }
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
    hasSelection_ = false;
    hideGroupBoxes();

    // reset filters
    eventKinds_.clear();
    compNames_.clear();
    compInstPaths_.clear();
    compInstIDs_.clear();
    portPaths_.clear();
    portIDs_.clear();
    workerInstPaths_.clear();
    workerInstIDs_.clear();
    nodeHostnames_.clear();
    nodeIDs_.clear();
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
