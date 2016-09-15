#include "medeamainwindow.h"
#include "medeaviewdockwidget.h"
#include "medeatooldockwidget.h"
#include "selectioncontroller.h"
#include "medeanodeviewdockwidget.h"
#include "viewmanagerwidget.h"

#include "../../View/theme.h"
#include "../../Controller/settingscontroller.h"
#include "../../Controller/settingscontroller.h"

#include <QDebug>
#include <QHeaderView>
#include <QPushButton>
#include <QMenuBar>
#include <QDateTime>
#include <QApplication>
#include <QStringBuilder>
#include <QStringListModel>
#include <QTabWidget>

#define TOOLBAR_HEIGHT 32


/**
 * @brief MedeaMainWindow::MedeaMainWindow
 * @param vc
 * @param parent
 */
MedeaMainWindow::MedeaMainWindow(ViewController *vc, QWidget* parent):MedeaWindowNew(parent, MedeaWindowNew::MAIN_WINDOW)
{
    qint64 timeStart = QDateTime::currentDateTime().toMSecsSinceEpoch();
    SettingsController::initializeSettings();
    connect(SettingsController::settings(), SIGNAL(settingChanged(SETTING_KEY,QVariant)), this, SLOT(settingChanged(SETTING_KEY,QVariant)));

    initializeApplication();

    applicationToolbar = 0;
    jenkinsManager = 0;
    cutsManager = 0;
    viewController = vc;

    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    resize(1000, 600);

    setupTools();
    setupInnerWindow();
    setupJenkinsManager();
    setupCUTSManager();


    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    connect(MedeaWindowManager::manager(), SIGNAL(activeViewDockWidgetChanged(MedeaViewDockWidget*,MedeaViewDockWidget*)), this, SLOT(activeViewDockWidgetChanged(MedeaViewDockWidget*, MedeaViewDockWidget*)));
    setViewController(vc);

    themeChanged();
    qint64 time2 = QDateTime::currentDateTime().toMSecsSinceEpoch();
    show();
    qint64 timeFinish = QDateTime::currentDateTime().toMSecsSinceEpoch();

    toggleWelcomeScreen(true);

    qCritical() << "MedeaMainWindow in: " <<  time2 - timeStart << "MS";
    qCritical() << "MedeaMainWindow->show() in: " <<  timeFinish - time2 << "MS";
    setModelTitle("");
}


/**
 * @brief MedeaMainWindow::~MedeaMainWindow
 */
MedeaMainWindow::~MedeaMainWindow()
{
    qCritical() << "~MedeaMainWindow()";
    //emit cutsManager->initiateTeardown();
    cutsManager->deleteLater();
    //cutsManager = 0;
    SettingsController::teardownSettings();
    Theme::teardownTheme();
}


/**
 * @brief MedeaMainWindow::setViewController
 * @param vc
 */
void MedeaMainWindow::setViewController(ViewController *vc)
{
    viewController = vc;
    SelectionController* controller = vc->getSelectionController();
    ActionController* actionController = vc->getActionController();

    connect(controller, &SelectionController::itemActiveSelectionChanged, tableWidget, &TableWidget::itemActiveSelectionChanged);

    connect(vc, &ViewController::mc_projectModified, this, &MedeaMainWindow::setWindowModified);
    connect(vc, &ViewController::vc_projectPathChanged, this, &MedeaMainWindow::setModelTitle);

    connect(vc, &ViewController::mc_projectModified, this, &MedeaMainWindow::setWindowModified);
    connect(actionController, &ActionController::recentProjectsUpdated, this, &MedeaMainWindow::recentProjectsUpdated);

    if (vc->getActionController()) {
        connect(vc->getActionController()->getRootAction("Root_Search"), SIGNAL(triggered(bool)), this, SLOT(popupSearch()));
    }
}


/**
 * @brief MedeaMainWindow::showCompletion
 * @param list
 */
void MedeaMainWindow::updateSearchSuggestions(QStringList list)
{
    searchCompleterModel->setStringList(list);
}


/**
 * @brief MedeaMainWindow::searchEntered
 */
void MedeaMainWindow::searchEntered()
{
    QString query = searchBar->text();
    if (!query.isEmpty()) {
        qint64 timeStart = QDateTime::currentDateTime().toMSecsSinceEpoch();
        searchDialog->searchResults(query, viewController->getSearchResults(query));
        qint64 timeFinish = QDateTime::currentDateTime().toMSecsSinceEpoch();
        searchDialog->show();
        qCritical() << "searchEntered in: " <<  timeFinish - timeStart << "MS";
    }
}


/**
 * @brief MedeaMainWindow::showNotification
 * @param title
 * @param message
 */
void MedeaMainWindow::showNotification(QString title, QString message)
{
    /*
    notificationLabel->setText(message);

    QFontMetrics fm = notificationLabel->fontMetrics();
    int maxWidth = qMin(innerWindow->width() - 10, fm.width(message) + 20);

    notificationPopup->setSize(maxWidth, notificationLabel->sizeHint().height());
    notificationPopup->show();
    moveWidget(notificationPopup, Qt::AlignBottom);
    */

    QMessageBox::critical(this, title, message, QMessageBox::Ok, 0);
}


/**
 * @brief MedeaMainWindow::showProgressBar
 * @param show
 * @param description
 */
void MedeaMainWindow::showProgressBar(bool show, QString description)
{
    progressLabel->setText(description + ". Please wait...");
    progressPopup->setVisible(show);
    if (show) {
        QWidget* cw = centralWidget();
        if (cw) {
            QPointF cwCenter = pos() + cw->pos() + cw->rect().center();
            cwCenter -= QPointF(progressPopup->width()/2, progressPopup->height()/2);
            progressPopup->move(cwCenter.x(), cwCenter.y());
        }
    } else {
        //Reset back to 0
        progressBar->reset();
    }
}


/**
 * @brief MedeaMainWindow::updateProgressBar
 * @param value
 */
void MedeaMainWindow::updateProgressBar(int value)
{
    if (progressPopup->isVisible()) {
        if (value == -1) {
            progressBar->setRange(0,0);
        } else {
            progressBar->setRange(0,100);
            progressBar->setValue(value);
        }
    }
}


/**
 * @brief MedeaMainWindow::resetToolDockWidgets
 */
void MedeaMainWindow::resetToolDockWidgets()
{
    foreach (MedeaDockWidget* child, getDockWidgets()) {
        child->setVisible(true);
    }
}


/**
 * @brief MedeaMainWindow::themeChanged
 */
void MedeaMainWindow::themeChanged()
{
    Theme* theme = Theme::theme();

    QString menuStyle = theme->getMenuStyleSheet();
    viewController->getActionController()->menu_file->setStyleSheet(menuStyle);
    viewController->getActionController()->menu_file_recentProjects->setStyleSheet(menuStyle);
    viewController->getActionController()->menu_edit->setStyleSheet(menuStyle);
    viewController->getActionController()->menu_view->setStyleSheet(menuStyle);
    viewController->getActionController()->menu_model->setStyleSheet(menuStyle);
    viewController->getActionController()->menu_jenkins->setStyleSheet(menuStyle);
    viewController->getActionController()->menu_help->setStyleSheet(menuStyle);
    viewController->getActionController()->menu_window->setStyleSheet(menuStyle);
    viewController->getActionController()->menu_options->setStyleSheet(menuStyle);

    searchCompleter->popup()->setStyleSheet(theme->getAbstractItemViewStyleSheet() % theme->getScrollBarStyleSheet() % "QAbstractItemView::item{ padding: 2px 0px; }");
    searchPopup->setStyleSheet(theme->getPopupWidgetStyleSheet());
    searchToolbar->setStyleSheet(theme->getToolBarStyleSheet());
    searchBar->setStyleSheet(theme->getLineEditStyleSheet());
    searchButton->setIcon(theme->getIcon("Actions", "Search"));

    progressPopup->setStyleSheet(theme->getPopupWidgetStyleSheet());
    progressBar->setStyleSheet(theme->getProgressBarStyleSheet());
    progressLabel->setStyleSheet("background: rgba(0,0,0,0); border: 0px; color:" + theme->getTextColorHex() + ";");

    notificationPopup->setStyleSheet(theme->getPopupWidgetStyleSheet());
    notificationLabel->setStyleSheet("background: rgba(0,0,0,0); border: 0px; color:" + theme->getTextColorHex() + ";");

    restoreAspectsButton->setIcon(theme->getIcon("Actions", "MenuView"));
    restoreToolsButton->setIcon(theme->getIcon("Actions", "Build"));
    restoreToolsAction->setIcon(theme->getIcon("Actions", "Refresh"));

    interfaceButton->setStyleSheet(theme->getAspectButtonStyleSheet(VA_INTERFACES));
    behaviourButton->setStyleSheet(theme->getAspectButtonStyleSheet(VA_BEHAVIOUR));
    assemblyButton->setStyleSheet(theme->getAspectButtonStyleSheet(VA_ASSEMBLIES));
    hardwareButton->setStyleSheet(theme->getAspectButtonStyleSheet(VA_HARDWARE));

    minimap->setStyleSheet(theme->getNodeViewStyleSheet());
}


/**
 * @brief MedeaMainWindow::activeViewDockWidgetChanged
 * @param viewDock
 * @param prevDock
 */
void MedeaMainWindow::activeViewDockWidgetChanged(MedeaViewDockWidget *viewDock, MedeaViewDockWidget *prevDock)
{
    if(viewDock && viewDock->isNodeViewDock()){
        MedeaNodeViewDockWidget* nodeViewDock = (MedeaNodeViewDockWidget*) viewDock;
        NodeViewNew* view = nodeViewDock->getNodeView();

        if(prevDock && prevDock->isNodeViewDock()){
            MedeaNodeViewDockWidget* prevNodeViewDock = (MedeaNodeViewDockWidget*) prevDock;
            NodeViewNew* prevView = prevNodeViewDock->getNodeView();
            if(prevView){
                disconnect(minimap, SIGNAL(minimap_Pan(QPointF)), prevView, SLOT(minimap_Pan(QPointF)));
                disconnect(minimap, SIGNAL(minimap_Panning(bool)), prevView, SLOT(minimap_Panning(bool)));
                disconnect(minimap, SIGNAL(minimap_Zoom(int)), prevView, SLOT(minimap_Zoom(int)));

                disconnect(prevView, &NodeViewNew::sceneRectChanged, minimap, &NodeViewMinimap::sceneRectChanged);

                disconnect(prevView, SIGNAL(viewportChanged(QRectF, qreal)), minimap, SLOT(viewportRectChanged(QRectF, qreal)));
            }
        }

        if(view){
            minimap->setBackgroundColor(view->getBackgroundColor());
            minimap->setScene(view->scene());

            connect(minimap, SIGNAL(minimap_Pan(QPointF)), view, SLOT(minimap_Pan(QPointF)));
            connect(minimap, SIGNAL(minimap_Panning(bool)), view, SLOT(minimap_Panning(bool)));
            connect(minimap, SIGNAL(minimap_Zoom(int)), view, SLOT(minimap_Zoom(int)));
            connect(view, SIGNAL(viewportChanged(QRectF, qreal)), minimap, SLOT(viewportRectChanged(QRectF, qreal)));
            connect(view, &NodeViewNew::sceneRectChanged, minimap, &NodeViewMinimap::sceneRectChanged);


            view->viewportChanged();
        }
    }
}


/**
 * @brief MedeaMainWindow::popupSearch
 */
void MedeaMainWindow::popupSearch()
{
    emit requestSuggestions();
    moveWidget(searchPopup);
    searchPopup->show();
    searchBar->setFocus();
    //showNotification("", "This is a testcdsce wcefcercrcrerr evrbtybdvftrhtynrb"); // wcefcercrcrerr evrbtybdvftrhtynrb wcefcercrcrerr evrbtybdvftrhtynrb!!!");
}


/**
 * @brief MedeaMainWindow::toolbarChanged
 * @param area
 */
void MedeaMainWindow::toolbarChanged(Qt::DockWidgetArea area)
{
    if (area == Qt::TopDockWidgetArea || area == Qt::BottomDockWidgetArea) {
        applicationToolbar->setOrientation(Qt::Horizontal);
        applicationToolbar->setFixedHeight(QWIDGETSIZE_MAX);
        applicationToolbar->setFixedWidth(QWIDGETSIZE_MAX);
    } else {
        applicationToolbar->setOrientation(Qt::Vertical);
        resizeEvent(0);
    }
}


/**
 * @brief MedeaMainWindow::toolbarTopLevelChanged
 * @param undocked
 */
void MedeaMainWindow::toolbarTopLevelChanged(bool undocked)
{
    if (undocked) {
        if (applicationToolbar->orientation() == Qt::Vertical) {
            applicationToolbar->setOrientation(Qt::Horizontal);
            applicationToolbar->setFixedHeight(QWIDGETSIZE_MAX);
        }
        applicationToolbar->parentWidget()->resize(applicationToolbar->sizeHint() +  QSize(12,0));
    }
}


/**
 * @brief MedeaMainWindow::hideWelcomeScreen
 * @param action
 */
void MedeaMainWindow::hideWelcomeScreen(QAction* action)
{
    if (action->text() != "Settings") {
        toggleWelcomeScreen(false);
    }
}


/**
 * @brief MedeaMainWindow::setModelTitle
 * @param modelTitle
 */
void MedeaMainWindow::setModelTitle(QString modelTitle)
{
    if(!modelTitle.isEmpty()){
        modelTitle = "- " % modelTitle;
    }
    QString title = "MEDEA " % modelTitle % "[*]";
    setWindowTitle(title);

    searchDialog->searchResults("ment", viewController->getSearchResults("ment"));
}


/**
 * @brief MedeaMainWindow::settingChanged
 * @param setting
 * @param value
 */
void MedeaMainWindow::settingChanged(SETTING_KEY setting, QVariant value)
{
    /*
    bool boolValue = value.toBool();
    //Handle stuff.
    switch(setting){
case SK_WINDOW_INTERFACES_VISIBLE:{
        nodeView_Interfaces->parentWidget()->setVisible(boolValue);
        break;
    }
case SK_WINDOW_BEHAVIOUR_VISIBLE:{
        nodeView_Behaviour->parentWidget()->setVisible(boolValue);
        break;
    }
case SK_WINDOW_ASSEMBLIES_VISIBLE:{
        nodeView_Assemblies->parentWidget()->setVisible(boolValue);
        break;
    }
case SK_WINDOW_HARDWARE_VISIBLE:{
        nodeView_Hardware->parentWidget()->setVisible(boolValue);
        break;
    }
case SK_WINDOW_TABLE_VISIBLE:
case SK_WINDOW_MINIMAP_VISIBLE:
case SK_WINDOW_BROWSER_VISIBLE:
case SK_WINDOW_TOOLBAR_VISIBLE:
    default:
        break;
}*/

}


/**
 * @brief MedeaMainWindow::initializeApplication
 */
void MedeaMainWindow::initializeApplication()
{
    //Allow Drops
    setAcceptDrops(true);

    //Set QApplication information.
    QApplication::setApplicationName("MEDEA");
    QApplication::setApplicationVersion(APP_VERSION);
    QApplication::setOrganizationName("Defence Information Group");
    QApplication::setOrganizationDomain("http://blogs.adelaide.edu.au/dig/");
    QApplication::setWindowIcon(Theme::theme()->getIcon("Actions", "MEDEA"));

    //Set Font.
    int opensans_FontID = QFontDatabase::addApplicationFont(":/Resources/Fonts/OpenSans-Regular.ttf");
    QString opensans_fontname = QFontDatabase::applicationFontFamilies(opensans_FontID).at(0);
    //QFont font = QFont(opensans_fontname);
    QFont font("Verdana");
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPointSizeF(8.5);
    QApplication::setFont(font);
}


/**
 * @brief MedeaMainWindow::connectNodeView
 * @param nodeView
 */
void MedeaMainWindow::connectNodeView(NodeViewNew *nodeView)
{
    if(nodeView && viewController){
        nodeView->setViewController(viewController);
    }
}


/**
 * @brief MedeaMainWindow::toggleWelcomeScreen
 * @param on
 */
void MedeaMainWindow::toggleWelcomeScreen(bool on)
{
    if (welcomeScreenOn == on) {
        return;
    }

    if (on) {
        holderLayout->removeWidget(welcomeScreen);
        holderLayout->addWidget(innerWindow);
        setCentralWidget(welcomeScreen);
    } else {
        holderLayout->removeWidget(innerWindow);
        holderLayout->addWidget(welcomeScreen);
        setCentralWidget(innerWindow);
    }

    // show/hide the menu bar and close all dock widgets
    menuBar->setVisible(!on);
    foreach (QDockWidget* dw, findChildren<QDockWidget*>()) {
        if (!on && dw->windowTitle() == "QOS Browser") {
            continue;
        }
        dw->setVisible(!on);
    }

    welcomeScreenOn = on;
}


/**
 * @brief MedeaMainWindow::setupTools
 */
void MedeaMainWindow::setupTools()
{
    setupWelcomeScreen();
    setupMenuBar();
    setupSearchBar();
    setupProgressBar();
    setupNotificationBar();
    setupDock();
    setupToolBar();
    setupDataTable();
    setupWindowManager();
    setupMinimap();
}


/**
 * @brief MedeaMainWindow::setupInnerWindow
 */
void MedeaMainWindow::setupInnerWindow()
{
    innerWindow = MedeaWindowManager::constructCentralWindow("Main Window");
    setCentralWidget(innerWindow);

    nodeView_Interfaces = new NodeViewNew();
    nodeView_Behaviour = new NodeViewNew();
    nodeView_Assemblies = new NodeViewNew();
    nodeView_Hardware = new NodeViewNew();

    nodeView_Interfaces->setContainedViewAspect(VA_INTERFACES);
    nodeView_Behaviour->setContainedViewAspect(VA_BEHAVIOUR);
    nodeView_Assemblies->setContainedViewAspect(VA_ASSEMBLIES);
    nodeView_Hardware->setContainedViewAspect(VA_HARDWARE);

    MedeaDockWidget *dwInterfaces = MedeaWindowManager::constructNodeViewDockWidget("Interface", Qt::TopDockWidgetArea);
    dwInterfaces->setWidget(nodeView_Interfaces);
    dwInterfaces->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);

    MedeaDockWidget *dwBehaviour = MedeaWindowManager::constructNodeViewDockWidget("Behaviour", Qt::TopDockWidgetArea);
    dwBehaviour->setWidget(nodeView_Behaviour);
    dwBehaviour->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);

    MedeaDockWidget *dwAssemblies = MedeaWindowManager::constructNodeViewDockWidget("Assemblies", Qt::BottomDockWidgetArea);
    dwAssemblies->setWidget(nodeView_Assemblies);
    dwAssemblies->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);

    MedeaDockWidget *dwHardware = MedeaWindowManager::constructNodeViewDockWidget("Hardware", Qt::BottomDockWidgetArea);
    dwHardware->setWidget(nodeView_Hardware);
    dwHardware->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);

    MedeaDockWidget *qosDockWidget = MedeaWindowManager::constructViewDockWidget("QOS Browser");
    qosBrowser = new QOSBrowser(viewController, this);
    qosDockWidget->setWidget(qosBrowser);
    qosDockWidget->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    qosDockWidget->setVisible(false);

    dwInterfaces->setProtected(true);
    dwBehaviour->setProtected(true);
    dwAssemblies->setProtected(true);
    dwHardware->setProtected(true);
    qosDockWidget->setProtected(true);

    SettingsController* settings = SettingsController::settings();
    dwInterfaces->setVisible(settings->getSetting(SK_WINDOW_INTERFACES_VISIBLE).toBool());
    dwBehaviour->setVisible(settings->getSetting(SK_WINDOW_BEHAVIOUR_VISIBLE).toBool());
    dwAssemblies->setVisible(settings->getSetting(SK_WINDOW_ASSEMBLIES_VISIBLE).toBool());
    dwHardware->setVisible(settings->getSetting(SK_WINDOW_HARDWARE_VISIBLE).toBool());

    innerWindow->addDockWidget(Qt::TopDockWidgetArea, dwInterfaces);
    innerWindow->addDockWidget(Qt::TopDockWidgetArea, dwBehaviour);
    innerWindow->addDockWidget(Qt::BottomDockWidgetArea, dwAssemblies);
    innerWindow->addDockWidget(Qt::BottomDockWidgetArea, dwHardware);
    innerWindow->addDockWidget(Qt::TopDockWidgetArea, qosDockWidget);

    // NOTE: Apparently calling innerWindow's createPopupMenu crashes the
    // application if it's called before the dock widgets are added above
    // This function needs to be called after the code above and before the connections below
    setupMainDockWidgetToggles();

    connectNodeView(nodeView_Interfaces);
    connectNodeView(nodeView_Behaviour);
    connectNodeView(nodeView_Assemblies);
    connectNodeView(nodeView_Hardware);

    // connect aspect toggle buttons
    connect(dwInterfaces, SIGNAL(visibilityChanged(bool)), interfaceButton, SLOT(setChecked(bool)));
    connect(dwBehaviour, SIGNAL(visibilityChanged(bool)), behaviourButton, SLOT(setChecked(bool)));
    connect(dwAssemblies, SIGNAL(visibilityChanged(bool)), assemblyButton, SLOT(setChecked(bool)));
    connect(dwHardware, SIGNAL(visibilityChanged(bool)), hardwareButton, SLOT(setChecked(bool)));
    connect(interfaceButton, SIGNAL(clicked(bool)), dwInterfaces, SLOT(setVisible(bool)));
    connect(behaviourButton, SIGNAL(clicked(bool)), dwBehaviour, SLOT(setVisible(bool)));
    connect(assemblyButton, SIGNAL(clicked(bool)), dwAssemblies, SLOT(setVisible(bool)));
    connect(hardwareButton, SIGNAL(clicked(bool)), dwHardware, SLOT(setVisible(bool)));
    connect(restoreAspectsButton, SIGNAL(clicked(bool)), innerWindow, SLOT(resetDockWidgets()));

}


/**
 * @brief MedeaMainWindow::setupWelcomeScreen
 */
void MedeaMainWindow::setupWelcomeScreen()
{
    welcomeScreen = new WelcomeScreenWidget(viewController->getActionController(), this);
    welcomeScreenOn = false;

    QWidget* holderWidget = new QWidget(this);
    holderWidget->hide();

    holderLayout = new QVBoxLayout(holderWidget);
    holderLayout->addWidget(welcomeScreen);

    connect(welcomeScreen, &WelcomeScreenWidget::actionTriggered, this, &MedeaMainWindow::hideWelcomeScreen);
    connect(this, &MedeaMainWindow::recentProjectsUpdated, welcomeScreen, &WelcomeScreenWidget::recentProjectsUpdated);
}


/**
 * @brief MedeaMainWindow::setupMenuBar
 */
void MedeaMainWindow::setupMenuBar()
{
    menuBar = new QMenuBar(this);
    menuBar->addMenu(viewController->getActionController()->menu_file);
    menuBar->addMenu(viewController->getActionController()->menu_edit);
    menuBar->addMenu(viewController->getActionController()->menu_view);
    menuBar->addMenu(viewController->getActionController()->menu_model);
    menuBar->addMenu(viewController->getActionController()->menu_jenkins);
    menuBar->addMenu(viewController->getActionController()->menu_window);
    menuBar->addMenu(viewController->getActionController()->menu_options);
    menuBar->addMenu(viewController->getActionController()->menu_help);

    // TODO - Find out how to set the height of the menubar items
    menuBar->setFixedHeight(TOOLBAR_HEIGHT);
    menuBar->setNativeMenuBar(false);
    setMenuBar(menuBar);
}


/**
 * @brief MedeaMainWindow::setupToolBar
 */
void MedeaMainWindow::setupToolBar()
{
    applicationToolbar = new QToolBar(this);
    applicationToolbar->setMovable(false);
    applicationToolbar->setFloatable(false);
    applicationToolbar->setIconSize(QSize(20,20));

    QWidget* w1 = new QWidget(this);
    QWidget* w2 = new QWidget(this);
    w1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    w2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    applicationToolbar->addWidget(w1);
    applicationToolbar->addActions(viewController->getActionController()->applicationToolbar->actions());
    applicationToolbar->addWidget(w2);

    MedeaDockWidget* dockWidget = MedeaWindowManager::constructToolDockWidget("Toolbar");
    dockWidget->setTitleBarWidget(applicationToolbar);
    dockWidget->setAllowedAreas(Qt::TopDockWidgetArea);
    //dockWidget->setAllowedAreas(Qt::TopDockWidgetArea | Qt::LeftDockWidgetArea | Qt::BottomDockWidgetArea);

    connect(dockWidget, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), this, SLOT(toolbarChanged(Qt::DockWidgetArea)));
    connect(dockWidget, SIGNAL(topLevelChanged(bool)), this, SLOT(toolbarTopLevelChanged(bool)));

    //Check visibility state.
    dockWidget->setVisible(SettingsController::settings()->getSetting(SK_WINDOW_TOOLBAR_VISIBLE).toBool());
    addDockWidget(Qt::TopDockWidgetArea, dockWidget, Qt::Horizontal);
}


/**
 * @brief MedeaMainWindow::setupSearchBar
 */
void MedeaMainWindow::setupSearchBar()
{
    searchButton = new QToolButton(this);
    searchButton->setToolTip("Submit Search");

    searchCompleterModel = new QStringListModel(this);

    searchCompleter = new QCompleter(this);
    searchCompleter->setModel(searchCompleterModel);
    searchCompleter->setFilterMode(Qt::MatchContains);
    searchCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    searchCompleter->popup()->setItemDelegate(new QStyledItemDelegate(this));
    searchCompleter->popup()->setFont(QFont(font().family(), 10));

    searchBar = new QLineEdit(this);
    searchBar->setFont(QFont(font().family(), 13));
    searchBar->setPlaceholderText("Search Here...");
    searchBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    searchBar->setCompleter(searchCompleter);

    searchToolbar = new QToolBar(this);
    searchToolbar->setIconSize(QSize(24,24));
    searchToolbar->setMovable(false);
    searchToolbar->setFloatable(false);
    searchToolbar->addWidget(searchBar);
    searchToolbar->addWidget(searchButton);

    searchPopup = new PopupWidget(PopupWidget::POPUP, this);
    searchPopup->setWidget(searchToolbar);
    searchPopup->setWidth(300);

    searchDialog = new SearchDialog(this);

    connect(this, &MedeaMainWindow::requestSuggestions, viewController, &ViewController::requestSearchSuggestions);
    connect(viewController, &ViewController::vc_gotSearchSuggestions, this, &MedeaMainWindow::updateSearchSuggestions);
    connect(searchBar, SIGNAL(returnPressed()), searchButton, SLOT(click()));
    connect(searchButton, SIGNAL(clicked(bool)), searchPopup, SLOT(hide()));
    connect(searchButton, SIGNAL(clicked(bool)), this, SLOT(searchEntered()));
    connect(searchDialog, SIGNAL(centerOnViewItem(int)), viewController, SLOT(centerOnID(int)));
    connect(searchDialog, SIGNAL(itemHoverEnter(int)), viewController->getToolbarController(), SLOT(actionHoverEnter(int)));
    connect(searchDialog, SIGNAL(itemHoverLeave(int)), viewController->getToolbarController(), SLOT(actionHoverLeave(int)));
}


/**
 * @brief MedeaMainWindow::setupProgressBar
 */
void MedeaMainWindow::setupProgressBar()
{
    progressLabel = new QLabel("Please wait...", this);
    progressLabel->setFont(QFont(font().family(), 11));
    progressLabel->setFixedHeight(progressLabel->sizeHint().height());
    progressLabel->setAlignment(Qt::AlignCenter);
    progressLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    progressBar = new QProgressBar(this);
    progressBar->setRange(0,100);
    progressBar->setTextVisible(false);

    QWidget* widget = new QWidget(this);
    widget->setStyleSheet("QWidget{ background: rgba(0,0,0,0); border: 0px; }");

    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setSpacing(5);
    layout->setMargin(2);
    layout->addWidget(progressLabel);
    layout->addWidget(progressBar);

    progressPopup = new PopupWidget(PopupWidget::DIALOG, this);
    progressPopup->setWidget(widget);
    progressPopup->setWidth(widget->sizeHint().width() + 200);
    progressPopup->setHeight(progressLabel->sizeHint().height() + 30);
    progressPopup->hide();

    connect(viewController, &ViewController::mc_showProgress, this, &MedeaMainWindow::showProgressBar);
    connect(viewController, &ViewController::mc_progressChanged, this, &MedeaMainWindow::updateProgressBar);
}


/**
 * @brief MedeaMainWindow::setupNotificationBar
 */
void MedeaMainWindow::setupNotificationBar()
{
    notificationLabel = new QLabel("This is a notification.", this);
    notificationLabel->setFont(QFont(font().family(), 11));
    notificationLabel->setAlignment(Qt::AlignCenter);
    notificationLabel->setWordWrap(true);

    notificationPopup = new PopupWidget(PopupWidget::TOOL, this);
    notificationPopup->setWidget(notificationLabel);
    notificationPopup->hide();
}


/**
 * @brief MedeaMainWindow::setupDock
 */
void MedeaMainWindow::setupDock()
{
    dockTabWidget = new DockTabWidget(viewController, this);

    MedeaDockWidget* dockWidget = MedeaWindowManager::constructToolDockWidget("Dock");
    dockWidget->setWidget(dockTabWidget);
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea);

    connect(viewController->getActionController()->toggleDock, SIGNAL(triggered(bool)), dockWidget, SLOT(setVisible(bool)));

    //Check visibility state.
    //dockWidget->setVisible(SettingsController::settings()->getSetting(SK_WINDOW_TABLE_VISIBLE).toBool());
    addDockWidget(Qt::LeftDockWidgetArea, dockWidget, Qt::Vertical);
}


/**
 * @brief MedeaMainWindow::setupDataTable
 */
void MedeaMainWindow::setupDataTable()
{
    tableWidget = new TableWidget(viewController, this);

    MedeaDockWidget* dockWidget = MedeaWindowManager::constructToolDockWidget("Table");
    dockWidget->setWidget(tableWidget);
    dockWidget->setAllowedAreas(Qt::RightDockWidgetArea);

    //Check visibility state.
    dockWidget->setVisible(SettingsController::settings()->getSetting(SK_WINDOW_TABLE_VISIBLE).toBool());
    addDockWidget(Qt::RightDockWidgetArea, dockWidget, Qt::Vertical);
}


/**
 * @brief MedeaMainWindow::setupMinimap
 */
void MedeaMainWindow::setupMinimap()
{
    minimap = new NodeViewMinimap(this);
    minimap->setFixedHeight(175);

    MedeaDockWidget* dockWidget = MedeaWindowManager::constructToolDockWidget("Minimap");
    dockWidget->setWidget(minimap);
    dockWidget->setAllowedAreas(Qt::RightDockWidgetArea);

    //Check visibility state.
    dockWidget->setVisible(SettingsController::settings()->getSetting(SK_WINDOW_MINIMAP_VISIBLE).toBool());
    addDockWidget(Qt::RightDockWidgetArea, dockWidget, Qt::Vertical);
}

void MedeaMainWindow::setupWindowManager()
{
    MedeaDockWidget* dockWidget = MedeaWindowManager::constructToolDockWidget("View Manager");
    dockWidget->setWidget(MedeaWindowManager::manager()->getViewManagerGUI());
    dockWidget->setAllowedAreas(Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, dockWidget, Qt::Vertical);
}


/**
 * @brief MedeaMainWindow::setupMainDockWidgetToggles
 * NOTE: This neeeds to be called after the tool dock widgets
 * and both the central and inner windows are constructed.
 */
void MedeaMainWindow::setupMainDockWidgetToggles()
{
    interfaceButton = new QToolButton(this);
    behaviourButton = new QToolButton(this);
    assemblyButton = new QToolButton(this);
    hardwareButton = new QToolButton(this);
    restoreAspectsButton = new QToolButton(this);
    restoreToolsButton = new QToolButton(this);

    /*
    interfaceButton->setText("I");
    behaviourButton->setText("B");
    assemblyButton->setText("A");
    hardwareButton->setText("H");
    */

    interfaceButton->setToolTip("Toggle Interface Aspect");
    behaviourButton->setToolTip("Toggle Behaviour Aspect");
    assemblyButton->setToolTip("Toggle Assembly Aspect");
    hardwareButton->setToolTip("Toggle Hardware Aspect");
    restoreAspectsButton->setToolTip("Restore Main Dock Widgets");
    restoreToolsButton->setToolTip("Restore Tool Dock Widgets");

    interfaceButton->setCheckable(true);
    behaviourButton->setCheckable(true);
    assemblyButton->setCheckable(true);
    hardwareButton->setCheckable(true);

    QMenu* menu = createPopupMenu();
    restoreToolsAction = menu->addAction("Reset Tool Widgets");
    restoreToolsButton->setMenu(menu);
    restoreToolsButton->setPopupMode(QToolButton::InstantPopup);

    if (innerWindow) {
        restoreAspectsButton->setMenu(innerWindow->createPopupMenu());
        restoreAspectsButton->setPopupMode(QToolButton::InstantPopup);
    }

    QToolBar* toolbar = new QToolBar(this);
    toolbar->setIconSize(QSize(20,20));
    toolbar->setFixedHeight(menuBar->height() - 6);
    toolbar->setStyleSheet("QToolButton{ padding: 2px 4px; }");

    toolbar->addWidget(interfaceButton);
    toolbar->addWidget(behaviourButton);
    toolbar->addWidget(assemblyButton);
    toolbar->addWidget(hardwareButton);
    toolbar->addSeparator();
    toolbar->addWidget(restoreAspectsButton);
    toolbar->addWidget(restoreToolsButton);

    menuBar->setCornerWidget(toolbar);
    connect(restoreToolsAction, SIGNAL(triggered(bool)), this, SLOT(resetToolDockWidgets()));

}


void MedeaMainWindow::setupJenkinsManager()
{
    if(!jenkinsManager){
        jenkinsManager = new JenkinsManager(this);
        connect(jenkinsManager, &JenkinsManager::settingsValidationComplete, viewController, &ViewController::jenkinsManager_SettingsValidated);

        connect(viewController->getActionController()->jenkins_importNodes, &QAction::triggered, jenkinsManager, &JenkinsManager::getJenkinsNodes);

        connect(jenkinsManager, &JenkinsManager::gotJenkinsNodeGraphml, viewController, &ViewController::jenkinsManager_GotJenkinsNodesList);
        connect(jenkinsManager, &JenkinsManager::jenkinsReady, viewController, &ViewController::vc_JenkinsReady);

        connect(viewController, &ViewController::vc_executeJenkinsJob, jenkinsManager, &JenkinsManager::executeJenkinsJob);




        jenkinsManager->validateSettings();
    }
}

void MedeaMainWindow::setupCUTSManager()
{
    if(!cutsManager ){
        cutsManager  = new CUTSManager();
        xmiImporter = new XMIImporter(cutsManager, this);

        //connect(cutsManager, &CUTSManager::localDeploymentOkay, viewController, &ViewController::cutsManager_DeploymentOkay);
        connect(viewController, &ViewController::vc_getCodeForComponent, cutsManager, &CUTSManager::getCPPForComponent);
        connect(viewController, &ViewController::vc_importXMEProject, cutsManager, &CUTSManager::executeXMETransform);


        connect(viewController, &ViewController::vc_validateModel, cutsManager, &CUTSManager::executeXSLValidation);
        connect(viewController, &ViewController::vc_launchLocalDeployment, cutsManager, &CUTSManager::showLocalDeploymentGUI, Qt::DirectConnection);


        connect(cutsManager, &CUTSManager::gotCodeForComponent, viewController, &ViewController::showCodeViewer);
        connect(cutsManager, &CUTSManager::gotXMETransform, viewController, &ViewController::importGraphMLFile);
        connect(cutsManager, &CUTSManager::executedXSLValidation, viewController, &ViewController::modelValidated);

        connect(viewController, &ViewController::vc_importXMIProject, xmiImporter, &XMIImporter::importXMI);
        connect(xmiImporter, &XMIImporter::loadingStatus, this, &MedeaMainWindow::showProgressBar);
        connect(xmiImporter, &XMIImporter::gotXMIGraphML, viewController, &ViewController::importGraphMLExtract);
    }
}



/**
 * @brief MedeaMainWindow::moveWidget
 * @param widget
 * @param alignment
 */
void MedeaMainWindow::moveWidget(QWidget* widget, Qt::Alignment alignment)
{
    QWidget* cw = centralWidget();
    if (cw && widget) {
        QPointF widgetPos = pos() + cw->pos() + cw->rect().center();
        switch (alignment) {
        case Qt::AlignBottom:
            widgetPos += QPoint(0, cw->rect().height()/2);
            break;
        default:
            break;
        }
        widgetPos -= QPointF(widget->width()/2, widget->height()/2);
        widget->move(widgetPos.x(), widgetPos.y());
    }
}


/**
 * @brief MedeaMainWindow::resizeEvent
 * @param e
 */
void MedeaMainWindow::resizeEvent(QResizeEvent* e)
{
    if(e){
        QMainWindow::resizeEvent(e);
    }
    if(applicationToolbar && applicationToolbar->orientation() == Qt::Vertical){
        applicationToolbar->setFixedHeight(centralWidget()->rect().height());
    }
}

void MedeaMainWindow::closeEvent(QCloseEvent *event)
{
    if(viewController){
        viewController->closeMEDEA();
        event->ignore();
    }
}
