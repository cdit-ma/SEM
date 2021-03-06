#include "windowmanager.h"

#include <QDialog>
#include <QPushButton>
#include <QBoxLayout>
#include <QObject>
#include <QApplication>
#include <QWidgetAction>
#include <QDoubleSpinBox>

#include "../../Widgets/Windows/basewindow.h"
#include "../../Widgets/Windows/centralwindow.h"
#include "../../Widgets/Windows/mainwindow.h"
#include "../../Widgets/Windows/subwindow.h"
#include "../../Widgets/Windows/invisiblewindow.h"

#include "../../Widgets/DockWidgets/defaultdockwidget.h"
#include "../../Widgets/DockWidgets/viewdockwidget.h"
#include "../../Widgets/DockWidgets/tooldockwidget.h"
#include "../../Widgets/DockWidgets/invisibledockwidget.h"

#include "../../Widgets/ViewManager/viewmanagerwidget.h"
#include "../../Widgets/ViewManager/dockreparenterpopup.h"

#include "../../Widgets/Panel/overlaysplitter.h"
#include "../../Widgets/Panel/panelwidget.h"

WindowManager* WindowManager::managerSingleton = nullptr;

WindowManager* WindowManager::manager()
{
    if(!managerSingleton){
        qRegisterMetaType<Qt::Alignment>("Qt::Alignment");
        managerSingleton = new WindowManager();
    }
    return managerSingleton;
}

bool WindowManager::Sort(const BaseDockWidget *a, const BaseDockWidget *b)
{
    auto dock_type_a = QString(a->metaObject()->className());
    auto dock_type_b = QString(b->metaObject()->className());
    if(dock_type_a == dock_type_b){
        if(a->isProtected() == b->isProtected()){
            return a->getTitle() < b->getTitle();
        }else{
            return a->isProtected();
        }
    }else{
        if(dock_type_a == "ViewDockWidget"){
            return true;
        }else if(dock_type_b == "ViewDockWidget"){
            return false;
        }else{
            return dock_type_a < dock_type_b;
        }
    }
}

bool WindowManager::isViewDockWidget(BaseDockWidget* base_dock_widget)
{
    if(base_dock_widget && base_dock_widget->getBaseDockType() == BaseDockType::DOCK){
        auto dock_widget = (DefaultDockWidget*) base_dock_widget;
        if(dock_widget->getDefaultDockType() == DefaultDockType::VIEW){
            return true;
        }
    }
    return false;
};

void WindowManager::ShowDockWidget(BaseDockWidget* widget)
{
    if(widget){
        auto window = widget->window();
        widget->show();
        if(window){
            if (window->isMinimized()) {
                window->showNormal();
            }
            // raise and activate the dock's parent window
            window->raise();
        }
        // make sure that the search panel dock widget is visible and that its window is raised
        widget->activateWindow();
        // raise the dock widget - this brings the dock widget to the front if it's tabbed
        widget->raise();
    }
}

BaseWindow* WindowManager::constructMainWindow(ViewController* vc)
{
    if(!mainWindow){
        mainWindow = new MainWindow(vc);
        addWindow(mainWindow);
    }
    return mainWindow;
}

BaseWindow* WindowManager::constructSubWindow(const QString& title, BaseWindow* parent_window)
{
    auto window = new SubWindow(parent_window);
    window->setWindowTitle(title);
    addWindow(window);
    return window;
}

BaseWindow* WindowManager::constructInvisibleWindow(const QString& title, BaseWindow* parent_window)
{
    auto window = new InvisibleWindow(parent_window);
    window->setWindowTitle(title);
    addWindow(window);
    return window;
}

BaseWindow* WindowManager::constructCentralWindow(const QString& title, BaseWindow* parent_window)
{
    if(!centralWindow){
        auto window = new CentralWindow(parent_window);
        window->setWindowTitle(title);
        centralWindow = window;
        addWindow(window);
    }
    return centralWindow;
}

ViewDockWidget* WindowManager::constructViewDockWidget(const QString& title, QWidget* parent, Qt::DockWidgetArea area)
{
    auto dock = new ViewDockWidget(title, parent, area);
    addDockWidget(dock);
    return dock;
}

DefaultDockWidget* WindowManager::constructDockWidget(const QString& title, QWidget* parent, Qt::DockWidgetArea area)
{
    auto dock = new DefaultDockWidget(title, parent, area);
    addDockWidget(dock);
    return dock;
}

ToolDockWidget* WindowManager::constructToolDockWidget(const QString& title, QWidget* parent)
{
    auto dock = new ToolDockWidget(title, parent);
    addDockWidget(dock);
    return dock;
}

InvisibleDockWidget* WindowManager::constructInvisibleDockWidget(const QString& title, QWidget* parent)
{
    auto dock = new InvisibleDockWidget(title, parent);
    addDockWidget(dock);
    return dock;
}

DefaultDockWidget* WindowManager::constructChartDockWidget(const QString& title, ChartDialog* dialog, QWidget *parent, Qt::DockWidgetArea area)
{
    if (!dialog)
        return nullptr;

    auto dockWidget = new DefaultDockWidget(title, parent, area);
    dockWidget->setWidget(dialog);
    addDockWidget(dockWidget);

    // add actions to the dock widget's title bar for clearAll, toggle time display and screenshot
    QAction* clearChartsAction = dockWidget->addAction("Clear Charts", "Icons", "clearList", Qt::AlignCenter);
    QAction* timeAction = dockWidget->addAction("Toggle Time Display", "ToggleIcons", "axisTimeToggle", Qt::AlignCenter);
    QAction* snapShotAction = dockWidget->addAction("Take Snapshot", "Icons", "camera", Qt::AlignCenter);

    timeAction->setCheckable(true);
    timeAction->setChecked(false);

    connect(clearChartsAction, &QAction::triggered, dialog, &ChartDialog::clear);
    connect(timeAction, &QAction::triggered, dialog, &ChartDialog::toggleTimelineAxisFormat);
    connect(snapShotAction, &QAction::triggered, dialog, &ChartDialog::snapShot);

    return dockWidget;
}

DefaultDockWidget* WindowManager::constructPulseDockWidget(const QString& title, DataflowDialog* dialog, QWidget* parent, Qt::DockWidgetArea area)
{
    if (!dialog) {
        return nullptr;
    }

    auto dockWidget = new DefaultDockWidget(title, parent, area);
    dockWidget->setWidget(dialog);
    addDockWidget(dockWidget);

    // Add actions to the dock widget's title bar for changing the playback speed and displaying live mode status
    Theme* theme = Theme::theme();
    QIcon live_icon = theme->getImage("Icons", "circleRadio", QSize(), Qt::red);
    live_icon.addPixmap(theme->getImage("Icons", "circleRadio", QSize(), Qt::red), QIcon::Disabled);

    QAction* live_status_action = dockWidget->addAction("Live Experiment", "Icons", "circleRadio", Qt::AlignCenter);
    live_status_action->setIcon(live_icon);
    live_status_action->setEnabled(false);
    live_status_action->setVisible(false);
    connect(dialog, &DataflowDialog::updateLiveStatus, live_status_action, &QAction::setVisible);

    double min_playback_speed = 0.25;
    double max_playback_speed = 3.0;

    auto playback_speed_spinbox = new QDoubleSpinBox(dialog);
    playback_speed_spinbox->setRange(min_playback_speed, max_playback_speed);
    playback_speed_spinbox->setSingleStep(min_playback_speed);
    playback_speed_spinbox->setValue(1.0);
    playback_speed_spinbox->setSuffix("x");
    playback_speed_spinbox->setStyleSheet(theme->getSpinBoxStyleSheet("QDoubleSpinBox"));
    connect(playback_speed_spinbox, SIGNAL(valueChanged(double)), dialog, SLOT(playbackSpeedChanged(double)));

    QWidgetAction* widget_action = new QWidgetAction(dialog);
    widget_action->setDefaultWidget(playback_speed_spinbox);

    QAction* speed_settings_action = dockWidget->addAction("Change Playback Speed", "Icons", "speed", Qt::AlignCenter);
    speed_settings_action->setMenu(new QMenu(dialog));
    speed_settings_action->menu()->addAction(widget_action);

    // Connect the theme_Changed signal to the actions
    connect(theme, &Theme::theme_Changed, [speed_settings_action, playback_speed_spinbox, theme]() {
        speed_settings_action->menu()->setStyleSheet(theme->getMenuBarStyleSheet());
        playback_speed_spinbox->setStyleSheet(theme->getSpinBoxStyleSheet("QDoubleSpinBox"));
    });

    const auto& titlebar = dockWidget->getTitleBar();
    auto tool_button = qobject_cast<QToolButton*>(titlebar->widgetForAction(speed_settings_action));
    if (tool_button) {
        tool_button->setPopupMode(QToolButton::InstantPopup);
        tool_button->setStyleSheet("QToolButton::menu-indicator{ image:none; }");
    }

    return dockWidget;
}

void WindowManager::constructInnerDockWidget(ViewController* vc, BaseDockWidget* parentDockWidget, const QString& title)
{
    if (parentDockWidget) {
        QWidget* currentWidget = parentDockWidget->widget();
        if (currentWidget) {
            auto panel = new PanelWidget(parentDockWidget);
            panel->setViewController(vc);

            auto splitter = new OverlaySplitter(parentDockWidget);
            splitter->setWidget(panel);

            // need this connection for the view to get the hover events
            if (parentDockWidget->getBaseDockType() == BaseDockType::DOCK) {
                auto d = (DefaultDockWidget*) parentDockWidget;
                if (d->getDefaultDockType() == DefaultDockType::VIEW) {
                    auto vd = (ViewDockWidget*) d;
                    if (vd->getNodeView())
                        connect(splitter, &OverlaySplitter::mouseEventReceived, vd->getNodeView(), &NodeView::receiveMouseMove);
                }
            }

            auto layout = new QVBoxLayout(currentWidget);
            layout->setMargin(0);
            layout->addWidget(splitter, 1);

            // add an action to toggle the inner dock widget's visibility
            QAction* action = parentDockWidget->addAction("Show/Hide Panel", "ToggleIcons", "chartToggle", Qt::AlignCenter);
            if (action) {
                action->setCheckable(true);
                action->setChecked(true);
                connect(action, &QAction::triggered, panel, &PanelWidget::setVisible);
                connect(panel, &PanelWidget::closeTriggered, [=](){ action->setChecked(false); });
                // initially hide the panel
                //action->setChecked(false);
                //panel->setVisible(false);
            }
        }
    }
}

void WindowManager::destructWindow(BaseWindow *window)
{
    if(window){
        auto window_id = window->getID();

        if(window != mainWindow && window != centralWindow){
            removeWindow(window);
            window->deleteLater();
        }
    }
}

void WindowManager::destructDockWidget(BaseDockWidget *widget)
{
    if(widget){
        removeDockWidget(widget);
        widget->deleteLater();
    }
}

void WindowManager::teardown()
{
    if (managerSingleton) {
        delete managerSingleton;
        managerSingleton = nullptr;
    }
}

BaseWindow *WindowManager::getMainWindow()
{
    return managerSingleton->mainWindow;
}

WindowManager::WindowManager()
	: QObject(nullptr)
{
    viewManagerWidget = new ViewManagerWidget(this);

    auto a = (QApplication*) QApplication::instance();
    connect(a, &QApplication::focusChanged, this, &WindowManager::focusChanged);
}

void WindowManager::focusChanged(QWidget* prev, QWidget* now)
{
	Q_UNUSED(prev);
    if(now){
        //Check to see if the widget is a child of one of the view docks
        for(auto id : view_dock_ids){
            auto dock = dockWidgets.value(id, nullptr);
            if(dock && dock->isAncestorOf(now)){
                //We should set
                setActiveViewDockWidget((ViewDockWidget*)dock);
                return;
            }
        }
    }
}

WindowManager::~WindowManager()
{
    if(centralWindow){
        auto ID = centralWindow->getID();
        windows.remove(ID);
        window_ids.remove(ID);
    }
    if(mainWindow){
        auto ID = mainWindow->getID();
        windows.remove(ID);
        window_ids.remove(ID);
    }

    //Delete all subwindows.
    while(!windows.isEmpty()){
        auto id = windows.keys().first();
        auto window = windows[id];
        if(window){
            destructWindow(window);
        }
    }

    if(centralWindow){
        auto window = centralWindow;
        centralWindow = nullptr;
        destructWindow(window);
    }
    if(mainWindow){
        auto window = mainWindow;
        mainWindow = nullptr;
        destructWindow(window);
    }
}

BaseWindow *WindowManager::getCentralWindow()
{
    return centralWindow;
}

ViewManagerWidget *WindowManager::getViewManagerGUI()
{
    return viewManagerWidget;
}

DockReparenterPopup* WindowManager::getDockPopup()
{
    if(!dock_popup){
        dock_popup = new DockReparenterPopup();
    }
    return dock_popup;
}

BaseWindow *WindowManager::getActiveWindow()
{
    if(activeViewDockWidget){
        return (BaseWindow*) activeViewDockWidget->parentWidget();
    }
    return centralWindow;
}

ViewDockWidget *WindowManager::getActiveViewDockWidget()
{
    return activeViewDockWidget;
}

void WindowManager::setActiveViewDockWidget(ViewDockWidget *view)
{
    if(view != activeViewDockWidget){
        ViewDockWidget* prev_dock = activeViewDockWidget;
        ViewDockWidget* current_dock = nullptr;

        //Set the New.
        if(view){
            if(view->isVisible()){
                current_dock = view;
            }else{
                current_dock = prev_dock;
            }
        }

        if(current_dock != prev_dock){
            if(prev_dock){
                prev_dock->setActive(false);
                disconnect(prev_dock, &BaseDockWidget::visibilityChanged, this, &WindowManager::activeDockWidgetVisibilityChanged);
            }
            if(current_dock){
                connect(current_dock, &BaseDockWidget::visibilityChanged, this, &WindowManager::activeDockWidgetVisibilityChanged);
                current_dock->setActive(true);
            }
            activeViewDockWidget = current_dock;
            emit activeViewDockWidgetChanged(current_dock, prev_dock);
        }
    }
}

QList<BaseWindow*> WindowManager::getWindows()
{
    return windows.values();
}

QList<ViewDockWidget *> WindowManager::getViewDockWidgets()
{
    QList<ViewDockWidget*> views;

    for(auto id : view_dock_ids){
        auto dock_widget = dockWidgets.value(id, nullptr);
        if(dock_widget){
            views.append((ViewDockWidget*)dock_widget);
        }
    }
    return views;
}

ViewDockWidget* WindowManager::getViewDockWidget(ViewItem *item)
{
    for(auto dock : getViewDockWidgets()){
        if(dock->getNodeView()->getContainedViewItem() == item){
            return dock;
        }
    }
    return nullptr;
}

void WindowManager::addWindow(BaseWindow *window)
{
    if(window){
        auto id = window->getID();

        if(!windows.contains(id)){
            windows[id] = window;
            emit windowConstructed(window);
        }else{
            qCritical() << "MedeaWindowManager::addWindow() " << id  << " - Got duplicated MedeaWindow ID.";
        }
    }
}

void WindowManager::removeWindow(BaseWindow *window)
{
    if(window){
        auto id = window->getID();
        emit windowDestructed(id);

        if(windows.contains(id)){
            if(mainWindow == window){
                mainWindow = nullptr;
            }
            if(centralWindow == window){
                centralWindow = nullptr;
            }
            windows.remove(id);
        }
    }
}

void WindowManager::addDockWidget(BaseDockWidget *dockWidget)
{
    if(dockWidget){
        auto id = dockWidget->getID();
        if(!dockWidgets.contains(id)){

            if(isViewDockWidget(dockWidget)){
                view_dock_ids.insert(id);
            }
            
            dockWidget->setObjectName(dockWidget->getTitle());

            dockWidgets[id] = dockWidget;
            connect(dockWidget, &BaseDockWidget::req_PopOut, this, &WindowManager::dockWidget_PopOut);
            connect(dockWidget, &BaseDockWidget::req_Close, this, &WindowManager::dockWidget_Close);
            emit dockWidgetConstructed(dockWidget);

        }else{
            qCritical() << "MedeaWindowManager::addDockWidget() - Got duplicated MedeaDockWidget ID.";
        }
    }
}

void WindowManager::removeDockWidget(BaseDockWidget *dockWidget)
{
    if(dockWidget){
        int id = dockWidget->getID();
        if(dockWidgets.contains(id)){
            if(activeViewDockWidget == dockWidget){
                setActiveViewDockWidget(nullptr);
            }

            emit dockWidgetDestructed(id);

            if(isViewDockWidget(dockWidget)){
                view_dock_ids.remove(id);
            }

            disconnect(dockWidget, &BaseDockWidget::req_PopOut, this, &WindowManager::dockWidget_PopOut);
            disconnect(dockWidget, &BaseDockWidget::req_Close, this, &WindowManager::dockWidget_Close);
            dockWidgets.remove(id);

            BaseWindow* window = dockWidget->getCurrentWindow();
            if(window){
                window->removeDockWidget(dockWidget);
            }
        }else{
            qCritical() << "MedeaWindowManager::removeDockWidget() - Trying to remove non-hashed MedeaDockWidget.";
        }
    }
}

void WindowManager::dockWidget_Close(int ID)
{
    auto dock_widget = dockWidgets.value(ID, nullptr);
    if(dock_widget){
        auto sourceWindow = dock_widget->getSourceWindow();
        auto currentWindow = dock_widget->getCurrentWindow();
        
        if(currentWindow){
            //Remove it from the current window
            currentWindow->removeDockWidget(dock_widget);
            dock_widget->setCurrentWindow(nullptr);
        }

        if(sourceWindow == currentWindow || sourceWindow == nullptr){
            //If the source window is the current window we should destruct the dock.
            destructDockWidget(dock_widget);

        }else{
            //Reparent back into source window.
            reparentDockWidget(dock_widget, sourceWindow);
        }
    }
}

void WindowManager::dockWidget_PopOut(int ID)
{
    auto dock_widget = dockWidgets.value(ID, nullptr);
    if(dock_widget){
        reparentDockWidget(dock_widget);
    }
}

void WindowManager::activeDockWidgetVisibilityChanged()
{
    if(activeViewDockWidget){
        auto parent = activeViewDockWidget->getCurrentWindow();

        if(parent && !activeViewDockWidget->isVisible()){
            ViewDockWidget* next_dock = nullptr;
            
            for(auto dock_widget : parent->getDockWidgets()){
                if(dock_widget->isVisible() && isViewDockWidget(dock_widget)){
                    next_dock = qobject_cast<ViewDockWidget*>(dock_widget);
                    break;
                }
            }
            setActiveViewDockWidget(next_dock);
        }
    }
}

BaseWindow* WindowManager::getWindow(int ID)
{
    return windows.value(ID, nullptr);
}

bool WindowManager::reparentDockWidget(BaseDockWidget* dockWidget)
{
    return getDockPopup()->ReparentDockWidget(dockWidget);
}

bool WindowManager::reparentDockWidget(BaseDockWidget* dock_widget, BaseWindow *window)
{
    if(dock_widget && window){
        auto previous_window = dock_widget->getCurrentWindow();
        if(previous_window){
            //Remove it from its previousWindow
            previous_window->removeDockWidget(dock_widget);
        }

        window->addDockWidget(dock_widget);
        emit dock_widget->req_Visible(dock_widget->getID(), true);
        window->raise();
        return true;
    }
    return false;
}

void WindowManager::MoveWidget(QWidget* widget, QWidget* parent_widget, Qt::Alignment alignment)
{
    //Add an event to the event loop to move the widget in the next server tick.
    QMetaObject::invokeMethod(manager(), "MoveWidgetEvent", Qt::QueuedConnection, Q_ARG(QWidget*, widget), Q_ARG(QWidget*, parent_widget), Q_ARG(Qt::Alignment, alignment));
}

void WindowManager::MoveWidgetEvent(QWidget* widget, QWidget* parent_widget, Qt::Alignment alignment)
{
    if(!parent_widget){
        parent_widget = QApplication::activeWindow();
        //Check
        if(!parent_widget || !parent_widget->isWindow()){
            parent_widget = manager()->getActiveWindow();
        }
    }else{
        parent_widget = parent_widget->window();
    }

    if(widget && parent_widget){
        auto pos = parent_widget->mapToGlobal(parent_widget->rect().center());
        //auto widget_size = widget->frameGeometry().size();
        auto widget_size = widget->rect().size();
        switch (alignment) {
        case Qt::AlignBottom:
            //Move to the bottom
            pos.ry() += parent_widget->height() / 2;
            //Offset by the height of the widget
            pos.ry() -= widget_size.height();
            pos.ry() -= 5;
            break;
        default:
            //Offset by the half the height of the widget
            pos.ry() -= widget_size.height() / 2;
            break;
        }
        //Center the widget
        pos.rx() -= widget_size.width() / 2;
        widget->move(pos);
    }
}