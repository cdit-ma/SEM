#include "chartinputpopup.h"

#include <QScrollArea>
#include <QScrollBar>
#include <QRadioButton>
#include <QToolButton>
#include <QFutureWatcher>
#include <QKeyEvent>

#define MIN_WIDTH 425
#define PADDING 50
#define FILTER "filter"
#define GROUPBOX_ITEM_SPACING 5
#define GROUPBOX_MAX_HEIGHT 400

/**
 * @brief ChartInputPopup::ChartInputPopup
 * @param parent
 */
ChartInputPopup::ChartInputPopup(QWidget* parent)
    : HoverPopup(parent)
{
    setupLayout();
    setModal(true);

    // initially hide all filter group boxes
    for (const auto& filter : getFilterKeys()) {
        setGroupBoxVisible(filter, false);
    }

    // This ensures that this widget and its children has a proper font and palette
    // It fixes the size/theme bug that previously only got fixed by manually changing the theme
    ensurePolished();

    connect(Theme::theme(), &Theme::theme_Changed, this, &ChartInputPopup::themeChanged);
    themeChanged();

    // The signal below is emitted when done(), accept() or reject() has been called or triggered by the user
    // This makes sure that the charts/pulse checkboxes are visible for when the ChartInputPopup is shown again
    connect(this, &ChartInputPopup::finished, [&checkboxes_hidden = force_charts_only_, &checkboxes = checkbox_group_]() {
        // When force_charts_only_ is true, it means that the checkboxes are currently invisible; set them back to visible when the popup has finished
        if (checkboxes_hidden) {
            checkboxes->setVisible(true);
            checkboxes_hidden = false;
        }
    });
}

/**
 * @brief ChartInputPopup::enforceChartsOnly
 * This is called when the popup is shown by the context (right-click) menu, that is only used for displaying filtered charts data
 */
void ChartInputPopup::enforceChartsOnly()
{
    force_charts_only_ = true;

    // Only show charts data
    charts_checkbox_->setChecked(true);
    pulse_checkbox_->setChecked(false);

    // Hide the checkboxes to stop the user from changing the checked states
    checkbox_group_->setVisible(false);
}

/**
 * @brief ChartInputPopup::enableFilters
 * NOTE: Call this after setWidget(), otherwise the groupboxes won't be put in the right layout
 */
void ChartInputPopup::enableFilters()
{
    filtersEnabled_ = true;
    setupFilterWidgets();
}

/**
 * @brief ChartInputPopup::eventFilter
 * @param watched
 * @param event
 * @return
 */
bool ChartInputPopup::eventFilter(QObject* watched, QEvent* event)
{
   if (event->type() == QEvent::KeyPress) {
       auto ke = dynamic_cast<QKeyEvent*>(event);
       if (ke->key() == Qt::Key_Escape) {
           experimentNameLineEdit_->setText(typedExperimentName_);
       }
   }
   return QDialog::eventFilter(watched, event);
}

/**
 * @brief ChartInputPopup::themeChanged
 */
void ChartInputPopup::themeChanged()
{
    Theme* theme = Theme::theme();

    auto&& tooltipStyle = "QToolTip{ background: white; color: black; }";
    toolbar_->setIconSize(theme->getIconSize());
    toolbar_->setStyleSheet(theme->getToolBarStyleSheet() + tooltipStyle);

    auto checkbox_style = "QCheckBox{color:" + theme->getTextColorHex() + ";}";
    charts_checkbox_->setStyleSheet(checkbox_style);
    charts_checkbox_->setIcon(theme->getIcon("Icons", "barChart"));
    pulse_checkbox_->setStyleSheet(checkbox_style);
    pulse_checkbox_->setIcon(theme->getIcon("Icons", "wave"));

    okAction_->setIcon(theme->getIcon("Icons", "tick"));
    cancelAction_->setIcon(theme->getIcon("Icons", "cross"));

    if (filterAction_) {
        filterAction_->setIcon(theme->getIcon("Icons", "triangleDown"));
    }

    for (auto& radioButton : findChildren<QRadioButton*>()) {
        radioButton->setStyleSheet("color:" + theme->getTextColorHex() + ";");
    }

    auto groupBoxStyle = theme->getGroupBoxStyleSheet() +
                         "QGroupBox{color:" + theme->getTextColorHex() + "; margin-top: 15px; border: 1px solid " + theme->getAltTextColorHex() + "}" +
                         "QGroupBox::title{ subcontrol-origin: margin; }";

    experimentNameGroupBox_->setStyleSheet(groupBoxStyle);
    experimentRunsGroupBox_->setStyleSheet(groupBoxStyle);

    experimentRunsScrollArea_->setStyleSheet("QScrollArea{ background: rgba(0,0,0,0); border: 0px; }");
    experimentRunsScrollArea_->verticalScrollBar()->setStyleSheet(theme->getScrollBarStyleSheet());

    experimentNameLineEdit_->setStyleSheet(theme->getLineEditStyleSheet());

    experimentsCompleter_->popup()->verticalScrollBar()->setStyleSheet(theme->getScrollBarStyleSheet());
    experimentsCompleter_->popup()->setStyleSheet(theme->getAbstractItemViewStyleSheet() +
                                                  "QAbstractItemView::item{ padding: 2px 0px; }"
                                                  "QAbstractItemView::item::selected{ background:red; }");

    live_splitter_widget_->setStyleSheet("QFrame{ color:" + theme->getAltTextColorHex() + ";}"
										 "QLabel{ color:" + theme->getAltTextColorHex() + ";}");
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
        experimentNameChanged(typedExperimentName_);
    }
}

/**
 * @brief ChartInputPopup::setExperimentRuns
 * @param experimentName
 * @param experimentRuns
 */
void ChartInputPopup::setExperimentRuns(const QString& experimentName, const QList<AggServerResponse::ExperimentRun>& experimentRuns)
{
    QStringList experimentNames;
    if (experimentName.isEmpty()) {
        for (const auto& run : experimentRuns) {
            const auto& exp_name = run.experiment_name;
            experimentNames.append(exp_name);
            experimentRuns_.insert(exp_name, run);
        }
        experimentNames.removeDuplicates();
    } else {
        for (const auto& run : experimentRuns) {
            experimentRuns_.insert(experimentName, run);
        }
        experimentNames.append(experimentName);
    }
    experimentsModel_->setStringList(experimentNames);
    experimentNameChanged(typedExperimentName_);
}

/**
 * @brief ChartInputPopup::filterMenuTriggered
 * This should only be called when there is a selected experiment run
 * The filter action/menu should be disabled other wise
 * @param action
 */
void ChartInputPopup::filterMenuTriggered(QAction* action)
{
    if (action == nullptr) {
        return;
    }

    // hiding then showing the groupbox stops the glitching
    if (action->isChecked()) {
        auto filter = static_cast<FILTER_KEY>(action->property(FILTER).toUInt());
        resetGroupBox(filter);
        populateGroupBox(filter);
        setGroupBoxVisible(filter, true);
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
        emit visualiseExperimentRunData(selectedExperimentRun_, charts_checkbox_->isChecked(), pulse_checkbox_->isChecked());
    }
    PopupWidget::accept();
    resetPopup();
}

/**
 * @brief ChartInputPopup::reject
 */
void ChartInputPopup::reject()
{
    PopupWidget::reject();
    resetPopup();
}

/**
 * @brief ChartInputPopup::experimentNameChanged
 * @param experimentName
 */
void ChartInputPopup::experimentNameChanged(const QString& experimentName)
{
    auto name = experimentName.trimmed();
    if (experimentsModel_->stringList().contains(name)) {
        experimentNameActivated(name);
    }
}

/**
 * @brief ChartInputPopup::experimentNameActivated
 * @param experimentName
 */
void ChartInputPopup::experimentNameActivated(const QString &experimentName)
{
    typedExperimentName_ = experimentName;

    // hiding before resize does the resize event immediately, returning the expected size hint
    resetGroupBox(FILTER_KEY::RUNS_FILTER);
    resizePopup();

    populateExperimentRuns(experimentRuns_.values(experimentName));
    setGroupBoxVisible(FILTER_KEY::RUNS_FILTER, true);
    resizePopup();
    recenterPopup();
}

/**
 * @brief ChartInputPopup::experimentRunSelected
 * @param experimentRun
 */
void ChartInputPopup::experimentRunSelected(const AggServerResponse::ExperimentRun& experimentRun)
{
    if (filterAction_) {
        filterAction_->setEnabled(false);
    }
    selectedExperimentRun_ = experimentRun;
    selectedExperimentRunID_ = experimentRun.experiment_run_id;
}

/**
 * @brief ChartInputPopup::populateExperimentRuns
 * @param runs
 */
void ChartInputPopup::populateExperimentRuns(const QList<AggServerResponse::ExperimentRun>& runs)
{
    if (runs.isEmpty()) {
        return;
    }

    bool firstButton = true;
    int maxButtonWidth = 0;

    // Sort the experiment runs with the most recent one at the top
    auto sorted_runs = runs;
    std::sort(sorted_runs.begin(), sorted_runs.end(), [](const AggServerResponse::ExperimentRun& a, const AggServerResponse::ExperimentRun& b) {
       const auto& a_time = a.start_time;
       const auto& b_time = b.start_time;
       return a_time > b_time;
    });

    auto&& runs_layout = qobject_cast<QVBoxLayout*>(groupBoxLayouts.value(FILTER_KEY::RUNS_FILTER, nullptr));
    if (runs_layout == nullptr) {
        return;
    }

    bool show_live_splitter = false;
    int top_index = 0;

    for (const auto& run : sorted_runs) {
        auto ID = run.experiment_run_id;
        QString text = run.experiment_name + " [" + QString::number(ID) + "] - started at " +
                       QDateTime::fromMSecsSinceEpoch(run.start_time).toString("MMM d, hh:mm:ss.zzz");

        auto button = new QRadioButton(text, this);
        button->setProperty("ID", ID);
        button->setStyleSheet("color:" + Theme::theme()->getTextColorHex() + ";");
        connect(button, &QRadioButton::toggled, [=](bool checked) {
            if (checked) { experimentRunSelected(run); }
        });

        if (run.end_time == 0) {
            show_live_splitter = true;
            runs_layout->addWidget(button);
        } else {
            runs_layout->insertWidget(top_index++, button);
        }

        // select the first button by default
        if (firstButton) {
            button->toggle();
            firstButton = false;
        }
        maxButtonWidth = qMax(button->sizeHint().width(), maxButtonWidth);
    }

    live_splitter_widget_->setVisible(show_live_splitter);
    setFixedWidth(qMax(MIN_WIDTH, maxButtonWidth) + PADDING * 2);
}

/**
 * @brief ChartInputPopup::populateGroupBox
 * @param filter
 */
void ChartInputPopup::populateGroupBox(ChartInputPopup::FILTER_KEY filter)
{
    QGroupBox* groupBox = getFilterGroupBox(filter);
    if (groupBox == nullptr) {
        return;
    }

    auto filterList = getFilterList(filter);
    for (const auto& text : filterList) {
        auto button = new QRadioButton(text, this);
        groupBoxLayouts[filter]->addWidget(button);
        button->setProperty("ID", text);
        button->setStyleSheet("color:" + Theme::theme()->getTextColorHex() + ";");
        connect(button, &QRadioButton::toggled, [=](bool checked) {
            if (checked) {
                getSelectedFilter(filter) = button->text();
            }
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
    if (groupBox == nullptr) {
        return;
    }

    if (filter == FILTER_KEY::RUNS_FILTER) {
    	selectedExperimentRunID_ = -1;
    } else {
		getSelectedFilter(filter) = "";
	}

    auto layout = groupBoxLayouts.value(filter, nullptr);
    if (layout == nullptr) {
        return;
    }

    auto buttons = groupBox->findChildren<QRadioButton*>();
    while (!buttons.isEmpty()) {
        auto button = buttons.takeFirst();
        layout->removeWidget(button);
        button->deleteLater();
    }

    groupBox->updateGeometry();
}

/**
 * @brief ChartInputPopup::resetGroupBox
 * @param filter
 */
void ChartInputPopup::resetGroupBox(ChartInputPopup::FILTER_KEY filter)
{
    auto groupBox = getFilterGroupBox(filter);
    if (groupBox) {
        clearGroupBox(filter);
        setGroupBoxVisible(filter, false);
    }
}

/**
 * @brief ChartInputPopup::setGroupBoxVisible
 * @param key
 * @param visible
 */
void ChartInputPopup::setGroupBoxVisible(ChartInputPopup::FILTER_KEY filter, bool visible)
{
    auto groupBox = getFilterGroupBox(filter);
    if (groupBox) {
        groupBox->setVisible(visible);
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
    auto&& min_height = qMin(GROUPBOX_MAX_HEIGHT, experimentRunsScrollArea_->widget()->sizeHint().height());
    experimentRunsGroupBox_->setFixedHeight( min_height + 45);
    adjustChildrenSize("", Qt::FindDirectChildrenOnly);
    updateGeometry();
}

/**
 * @brief ChartInputPopup::resetPopup
 */
void ChartInputPopup::resetPopup()
{
    selectedExperimentRunID_ = -1;
    experimentRuns_.clear();
    experimentsModel_->setStringList(QStringList());

    if (filtersEnabled_) {
        for (auto filter : getFilterKeys()) {
            resetGroupBox(filter);
        }
    } else {
        resetGroupBox(FILTER_KEY::RUNS_FILTER);
    }

    resizePopup();
    recenterPopup();
    setFixedWidth(MIN_WIDTH + PADDING);
}

/**
 * @brief ChartInputPopup::setupLayout
 */
void ChartInputPopup::setupLayout()
{
    /*
     * Setup Auto-Complete
     */

    experimentsModel_ = new QStringListModel(this);
    experimentsCompleter_ = new QCompleter(this);
    experimentsCompleter_->setModel(experimentsModel_);
    experimentsCompleter_->setFilterMode(Qt::MatchContains);
    experimentsCompleter_->setCaseSensitivity(Qt::CaseInsensitive);
    experimentsCompleter_->popup()->setFont(QFont(font().family(), 10));
    experimentsCompleter_->popup()->installEventFilter(this);

    connect(experimentsCompleter_, static_cast<void(QCompleter::*)(const QString &)>(&QCompleter::activated),
        [=](const QString &text){ experimentNameActivated(text);
    });

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
    experimentNameLineEdit_->setCompleter(experimentsCompleter_);

    connect(experimentNameLineEdit_, &QLineEdit::textEdited, [=] (const QString& text) {
        typedExperimentName_ = text;
    });
    connect(experimentNameLineEdit_, &QLineEdit::returnPressed, [=]() {
        experimentNameChanged(typedExperimentName_);
    });

    experimentNameGroupBox_ = new QGroupBox("Visualise Events For Experiment:", this);
    experimentNameGroupBox_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    QVBoxLayout* topLayout = constructVBoxLayout(experimentNameGroupBox_);
    topLayout->addWidget(experimentNameLineEdit_);

    /*
     * Experiment Runs Layouts/Widgets
     */
    {
        QWidget* experimentRunsScrollWidget = new QWidget(this);
        experimentRunsScrollWidget->setStyleSheet("background: rgba(0,0,0,0);");
        experimentRunsScrollWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

        experimentRunsScrollArea_ = new QScrollArea(this);
        experimentRunsScrollArea_->setWidget(experimentRunsScrollWidget);
        experimentRunsScrollArea_->setWidgetResizable(true);
        experimentRunsScrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        experimentRunsGroupBox_ = new QGroupBox("Select Experiment Run:", this);
        experimentRunsGroupBox_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

        QVBoxLayout* layout = constructVBoxLayout(experimentRunsGroupBox_);
        layout->addWidget(experimentRunsScrollArea_);

        /*
         * Live Section Splitter
         */
        auto live_label = new QLabel("Live Experiments:", this);

        auto left_splitter = new QFrame(this);
        left_splitter->setFrameShape(QFrame::HLine);
        left_splitter->setLineWidth(2);

        auto right_splitter = new QFrame(this);
        right_splitter->setFrameShape(QFrame::HLine);
        right_splitter->setLineWidth(2);

        live_splitter_widget_ = new QWidget(this);
        auto live_splitter_layout = new QHBoxLayout(live_splitter_widget_);
        live_splitter_layout->setMargin(0);
        live_splitter_layout->setSpacing(5);
        live_splitter_layout->addWidget(left_splitter, 1);
        live_splitter_layout->addWidget(live_label, 0);
        live_splitter_layout->addWidget(right_splitter, 1);

        auto runs_layout = constructVBoxLayout(experimentRunsScrollWidget, GROUPBOX_ITEM_SPACING);
        runs_layout->addItem(new QSpacerItem(0, 5));
        runs_layout->addWidget(live_splitter_widget_);
        runs_layout->addItem(new QSpacerItem(0, 5));
        groupBoxLayouts[FILTER_KEY::RUNS_FILTER] = runs_layout;
    }

    /*
     * Bottom ToolBar
     */

    toolbar_ = new QToolBar(this);
    toolbar_->setMinimumWidth(MIN_WIDTH);
    toolbar_->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    checkbox_group_ = new QActionGroup(this);

    charts_checkbox_ = new QCheckBox("Charts", this);
    charts_checkbox_->setLayoutDirection(Qt::RightToLeft);
    charts_checkbox_->setChecked(true);
    checkbox_group_->addAction(toolbar_->addWidget(charts_checkbox_));
    toolbar_->addSeparator();

    auto splitter = new QFrame(this);
    splitter->setFrameShape(QFrame::VLine);
    splitter->setLineWidth(1);
    splitter->setStyleSheet("color: gray;");
    checkbox_group_->addAction(toolbar_->addWidget(splitter));

    pulse_checkbox_ = new QCheckBox("Pulse", this);
    pulse_checkbox_->setLayoutDirection(Qt::RightToLeft);
    pulse_checkbox_->setChecked(true);
    checkbox_group_->addAction(toolbar_->addWidget(pulse_checkbox_));
    toolbar_->addSeparator();

    auto spacerWidget = new QWidget(this);
    spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolbar_->addWidget(spacerWidget);

    cancelAction_ = toolbar_->addAction("Cancel");
    connect(cancelAction_, &QAction::triggered, this, &ChartInputPopup::reject);

    okAction_ = toolbar_->addAction("Ok");
    connect(okAction_, &QAction::triggered, this, &ChartInputPopup::accept);

    auto holderWidget = new QWidget(this);
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
    filterMenu_ = new QMenu(this);
    connect(filterMenu_, &QMenu::triggered, this, &ChartInputPopup::filterMenuTriggered);

    filterAction_ = new QAction("Filters");
    filterAction_->setMenu(filterMenu_);
    filterAction_->setEnabled(false);

    // the first item in the toolbar is a spacer widget
    QAction* spacerAction = toolbar_->actions().at(0);
    toolbar_->insertAction(spacerAction, filterAction_);

    auto button = qobject_cast<QToolButton*>(toolbar_->widgetForAction(filterAction_));
    button->setPopupMode(QToolButton::InstantPopup);
    button->setStyleSheet("QToolButton::menu-indicator{ image:none; }");

    nodesGroupBox_ = constructFilterWidgets(FILTER_KEY::NODE_FILTER, "Nodes");
    componentsGroupBox_ = constructFilterWidgets(FILTER_KEY::COMPONENT_FILTER, "Components");
    workersGroupBox_ = constructFilterWidgets(FILTER_KEY::WORKER_FILTER, "Workers");

    // insert the group boxes above the bottom toolbar
    if (getWidget() && getWidget()->layout()) {
        auto vLayout = qobject_cast<QVBoxLayout*>(getWidget()->layout());
        int index = vLayout->indexOf(toolbar_);
        vLayout->insertWidget(index, workersGroupBox_);
        vLayout->insertWidget(index, componentsGroupBox_);
        vLayout->insertWidget(index, nodesGroupBox_);
    }
}

/**
 * @brief ChartInputPopup::getSelectedFilter
 * @param filter
 * @throws std::runtime_error
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
			throw std::runtime_error("ChartInputPopup::getSelectedFilter - Filter key is not handled.");
	}
}

/**
 * @brief ChartInputPopup::getFilterList
 * @param filter
 * @throws std::runtime_error
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
			throw std::runtime_error("ChartInputPopup::getSelectedFilter - Filter key is not handled.");
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
	}
}

/**
 * @brief ChartInputPopup::constructFilterWidgets
 * @param filter
 * @param filterName
 * @return
 */
QGroupBox* ChartInputPopup::constructFilterWidgets(ChartInputPopup::FILTER_KEY filter, const QString& filterName)
{
    auto groupBox = new QGroupBox("Filter Events By " + filterName + ":", this);
    groupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    groupBoxLayouts[filter] = constructVBoxLayout(groupBox, GROUPBOX_ITEM_SPACING);

    QAction* action = filterMenu_->addAction(filterName);
    action->setProperty(FILTER, static_cast<uint>(filter));
    action->setCheckable(true);
    connect(action, &QAction::toggled, groupBox, &QGroupBox::setVisible);

    return groupBox;
}

/**
 * @brief ChartInputPopup::constructVBoxLayout
 * @param widget
 * @param spacing
 * @param margin
 * @return
 */
QVBoxLayout* ChartInputPopup::constructVBoxLayout(QWidget* widget, int spacing, int margin)
{
    auto layout = new QVBoxLayout(widget);
    layout->setMargin(margin);
    layout->setSpacing(spacing);
    layout->setContentsMargins(1, 5, 1, 1);
    return layout;
}