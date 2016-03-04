#include "medeawindow.h"
#include "Controller/controller.h"
#include "GUI/codeeditor.h"
#include "CUTS/GUI/cutsexecutionwidget.h"

//Testing a new Include..
#include <QDebug>

#include <QFileDialog>
#include <QDebug>
#include <QObject>
#include <QImage>
#include <QFileDialog>
#include <QClipboard>
#include <QMessageBox>
#include <QApplication>
#include <QHeaderView>
#include <QScrollBar>
#include <QSettings>
#include <QTemporaryFile>
#include <QPicture>
#include "GUI/actionbutton.h"
#include "GUI/shortcutdialog.h"
#include <QToolButton>
#include <QToolBar>
#include <QDesktopServices>


#define THREADING true

#define RIGHT_PANEL_WIDTH 230.0
#define SPACER_SIZE 10

#define MIN_WIDTH 1000
#define MIN_HEIGHT (480 + SPACER_SIZE * 3)
//#define MIN_WIDTH 1280
//#define MIN_HEIGHT (720 + SPACER_SIZE*3)

#define TOOLBAR_BUTTON_WIDTH 46
#define TOOLBAR_BUTTON_HEIGHT 40
#define TOOLBAR_GAP 5

#define RECENT_PROJECT_SIZE 5

#define SEARCH_DIALOG_MIN_WIDTH ((MIN_WIDTH * 2.0) / 3.0)
#define SEARCH_DIALOG_MIN_HEIGHT ((MIN_HEIGHT * 2.0) / 3.0)

//#define NOTIFICATION_TIME 2000
#define NOTIFICATION_TIME 2000

#define SEARCH_VIEW_ASPECTS 0
#define SEARCH_NODE_KINDS 1
#define SEARCH_DATA_KEYS 2

#define GRAPHML_FILE_EXT "GraphML Documents (*.graphml)"
#define GRAPHML_FILE_SUFFIX ".graphml"
#define GME_FILE_EXT "GME Documents (*.xme)"
#define GME_FILE_SUFFIX ".xme"

#define GITHUB_URL "https://github.com/cdit-ma/MEDEA/"
// USER SETTINGS

/**
 * @brief MedeaWindow::MedeaWindow
 * @param graphMLFile
 * @param parent
 */
MedeaWindow::MedeaWindow(QString graphMLFile, QWidget *parent) :
    QMainWindow(parent)
{
    setupApplication();

    nodeView = 0;
    nodeView = 0;
    controller = 0;
    fileDialog = 0;
    leftOverTime = 0;
    controllerThread = 0;

    SETTINGS_LOADING = false;
    WINDOW_MAXIMIZED = false;
    WINDOW_FULLSCREEN = false;
    IS_WINDOW_MAXIMIZED = false;
    INITIAL_SETTINGS_LOADED = false;
    maximizedSettingInitiallyChanged = false;

    CURRENT_THEME = VT_NORMAL_THEME;

    //Initialize classes.
    initialiseSettings();
    initialiseJenkinsManager();
    initialiseCUTSManager();
    initialiseGUI();

    setupConnections();

    resetGUI();
    resetView();

    //Load the Settings
    setupInitialSettings();

    //Show Welcome Screen
    toggleWelcomeScreen(true);

    //Load initial model.
    if(!graphMLFile.isEmpty()){
        openProject(graphMLFile);
    }

    INITIAL_SETTINGS_LOADED = true;
}


/**
 * @brief MedeaWindow::~MedeaWindow
 */
MedeaWindow::~MedeaWindow()
{
    if(loadingMovie){
        delete loadingMovie;
    }

    if (appSettings) {
        saveSettings();
        delete appSettings;
    }

    teardownProject();

    if (nodeView) {
        delete nodeView;
    }

    if(jenkinsManager){
        delete jenkinsManager;
    }
}

void MedeaWindow::projectRequiresSaving(bool requiresSave)
{
    setWindowModified(requiresSave);
}


/**
 * @brief MedeaWindow::toolbarSettingChanged
 * @param keyName
 * @param value
 */
void MedeaWindow::toolbarSettingChanged(QString keyName, QVariant value)
{
    bool isBool = false;
    bool boolValue = false;
    if(value == "true" || value == "false"){
        isBool = true;
        if(value == "true"){
            boolValue = true;
        }
    }
    if(!isBool){
        return;
    }

    if(toolbarActionLookup.contains(keyName)){
        QAction* action = toolbarActionLookup[keyName];

        if(action){
            action->setVisible(boolValue);
        }
        updateToolbar();
    }

    if (boolValue) {
        nodeView->updateActionsEnabledStates();
    }
}


/**
 * @brief MedeaWindow::enableTempExport
 * @param enable
 */
void MedeaWindow::enableTempExport(bool enable)
{
    //Disable the actions which use Temporary exporting!
    if(jenkins_ExecuteJob){
        jenkins_ExecuteJob->setEnabled(enable);
    }
    if(model_validateModel){
        model_validateModel->setEnabled(enable);
    }
}


/**
 * @brief MedeaWindow::setApplicationEnabled
 * This enables/disables the window, the view and all top level widgets.
 * @param enable
 */
void MedeaWindow::setApplicationEnabled(bool enable)
{
    emit window_SetViewVisible(enable);
    setEnabled(enable);
}


/**
 * @brief MedeaWindow::setViewWidgetsEnabled
 * This enables/disables all widgets/actions that depend on the model being ready.
 * @param enable
 */
void MedeaWindow::setViewWidgetsEnabled(bool enable)
{
    // TODO - Can't seem to hide the minimap!
    minimap->setEnabled(enable);
    //minimap->setVisible(false);
    //minimap->hide();

    // project title widgets
    projectName->setVisible(enable);
    closeProjectButton->setVisible(enable);
    updateWidgetsOnProjectChange(enable);

    // search widgets
    searchBar->setEnabled(enable);
    searchButton->setEnabled(enable);
    searchOptionButton->setEnabled(enable);

    // dock buttons
    partsButton->setVisible(enable);
    hardwareNodesButton->setVisible(enable);

    // aspect toggle buttons
    emit window_SetViewVisible(enable);
    /*foreach(AspectToggleWidget* aspect, aspectToggles){
        aspect->enableToggleButton(enable);
    }*/

    // actions that alter the model
    foreach(QAction* action, modelActions){
        action->setEnabled(enable);
    }
}


/**
 * @brief MedeaWindow::modelReady - Called whenever a new project is run, after the controller has finished setting up the NodeView/Controller/Model
 */
void MedeaWindow::modelReady()
{
    // reset the view - clear history and selection
    resetView();

    //Setup the Docks again.
    populateDocks();

    // re-enable the view widgets and window
    setViewWidgetsEnabled(true);
    //setApplicationEnabled(true);
}

void MedeaWindow::modelDisconnected()
{
    //Disable most of the GUI.
    setViewWidgetsEnabled(false);

    //Clear the Docks
    emit window_clearDocks();

    //Clear toolbar
}


/**
 * @brief MedeaWindow::projectCleared
 */
void MedeaWindow::projectCleared()
{
    emit window_clearDocksSelection();
}

/**
 * @brief MedeaWindow::settingChanged
 * @param groupName
 * @param keyName
 * @param value
 */
void MedeaWindow::settingChanged(QString groupName, QString keyName, QVariant value)
{
    if(groupName==TOOLBAR_SETTINGS){
        toolbarSettingChanged(keyName, value);
    }else if(groupName==WINDOW_SETTINGS && INITIAL_SETTINGS_LOADED){
        //Ignore Window settigns after initial load.
        return;
    }else if(groupName==ASPECT_SETTINGS && !INITIAL_SETTINGS_LOADED){
        //Ignore Aspect Settings on initial load.
        return;
    }

    bool isInt;
    QString strValue = value.toString();
    bool boolValue = value.toBool();
    int intValue = value.toInt(&isInt);

    if(keyName == WINDOW_X && isInt){
        if(intValue >= 0){
            move(intValue, pos().y());
        }
    }else if(keyName == WINDOW_Y && isInt){
        if(intValue >= 0){
            move(pos().x(), intValue);
        }
    }else if(keyName == WINDOW_W && isInt){
        resize(intValue, size().height());
    }else if(keyName == WINDOW_H && isInt){
        resize(size().width(), intValue);
    }else if(keyName == WINDOW_MAX_STATE){
        if(boolValue != IS_WINDOW_MAXIMIZED && SETTINGS_LOADING){
            maximizedSettingInitiallyChanged = true;
            WINDOW_MAXIMIZED = boolValue;
        }
        if(boolValue){
            showMaximized();
        }else{
            showNormal();
        }
    }else if(keyName == WINDOW_FULL_SCREEN){
        setFullscreenMode(boolValue);
    }else if(keyName == WINDOW_STORE_SETTINGS){
        SAVE_WINDOW_SETTINGS = boolValue;
    }else if(keyName == CUTS_CONFIGURE_PATH){
        if(cutsManager){
            cutsManager->setCUTSConfigScriptPath(strValue);
        }
    }else if(keyName == THREAD_LIMIT && isInt){
        if(cutsManager){
            cutsManager->setMaxThreadCount(intValue);
        }
    }else if(keyName == TOGGLE_GRID){
        toggleAndTriggerAction(actionToggleGrid, boolValue);
    }else if(keyName == DOCK_VISIBLE){
        showDocks(!boolValue);
    }else if(keyName == TOOLBAR_VISIBLE){
        setToolbarVisibility(!boolValue);
        settingChanged(groupName,TOOLBAR_EXPANDED, appSettings->getSetting(TOOLBAR_EXPANDED));
        SHOW_TOOLBAR = !boolValue;
    }else if(keyName == TOOLBAR_EXPANDED){
        if(appSettings->getSetting(TOOLBAR_VISIBLE) != "true"){
            toolbarButton->setChecked(boolValue);
            toolbarButton->clicked(boolValue);
        }
    }else if(keyName == ASPECT_I){
        definitionsToggle->setClicked(boolValue);
    }else if(keyName == ASPECT_B){
        workloadToggle->setClicked(boolValue);
    }else if(keyName == ASPECT_A){
        assemblyToggle->setClicked(boolValue);
    }else if(keyName == ASPECT_H){
        hardwareToggle->setClicked(boolValue);
    }else if(keyName == JENKINS_URL){
        if(jenkinsManager){
            jenkinsManager->setURL(strValue);
        }
    }else if(keyName == JENKINS_USER){
        if(jenkinsManager){
            jenkinsManager->setUsername(strValue);
        }
    }else if(keyName == JENKINS_PASS){
        if(jenkinsManager){
            jenkinsManager->setPassword(strValue);
        }
    }else if(keyName == JENKINS_TOKEN){
        if(jenkinsManager){
            jenkinsManager->setToken(strValue);
        }
    }else if(keyName == JENKINS_JOB){
        jenkins_JobName_Changed(strValue);
    }else if(keyName == DEFAULT_DIR_PATH){
        //Set up default path.
        DEFAULT_PATH = strValue;
        if(DEFAULT_PATH == ""){
            //Use application directory
            DEFAULT_PATH = applicationDirectory;
        }
    }
}

void MedeaWindow::loadSettingsFromINI()
{
    if(appSettings){
        SETTINGS_LOADING = true;
        appSettings->loadSettings();
        SETTINGS_LOADING = false;
    }
}


/**
 * @brief MedeaWindow::initialiseGUI
 * Initialise variables, setup widget sizes, organise layout and setup the view, scene and menu.
 */
void MedeaWindow::initialiseGUI()
{
    // set all gui widget fonts to this
    double fontSize = 8.5;
    guiFont = QFont("Verdana", fontSize);

    // initialise variables
    controller = 0;
    controllerThread = 0;

    nodeView = new NodeView();
    nodeView->setApplicationDirectory(applicationDirectory);
    nodeView->setMinimumSize(MIN_WIDTH, MIN_HEIGHT);
    nodeView->viewport()->setMinimumSize(MIN_WIDTH, MIN_HEIGHT);

    delegate = new ComboBoxTableDelegate(0);

    // setup and add dataTable/dataTableBox widget/layout
    dataTableBox = new QGroupBox(this);
    dataTableBox->setFixedWidth(RIGHT_PANEL_WIDTH + 10);
    dataTableBox->setContentsMargins(0,0,0,0);

    dataTable = new QTableView(dataTableBox);
    dataTable->setItemDelegateForColumn(1, delegate);
    dataTable->setFixedWidth(RIGHT_PANEL_WIDTH + 5);
    dataTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    dataTable->setFont(guiFont);
    dataTable->resize(dataTable->width(), 0);

    // setup menu, close project and project name buttons
    menuButton = new QPushButton(getIcon("Actions", "MEDEA_Menu"), "");
    menuButton->setFixedSize(55, 45);
    menuButton->setIconSize(menuButton->size() * 0.75);
    menuButton->setStyleSheet("QPushButton{ background-color: rgb(210,210,210); }"
                              "QPushButton::menu-indicator{ image: none; }");

    projectName = new QPushButton("");
    projectName->setFlat(true);
    projectName->setStyleSheet("QPushButton{ color: black; font-weight: bold; font-size: 16px; text-align: left; }"
                               "QTooltip{ background: white; color: black; }");

    closeProjectButton = new QPushButton(getIcon("Actions", "Close"), "");
    closeProjectButton->setToolTip("Close Current Project");
    closeProjectButton->setFixedSize(menuButton->height()/2.5, menuButton->height()/2.5);
    closeProjectButton->setStyleSheet("QPushButton{ background: rgb(200,200,200); border-radius: 2px; }"
                                      "QPushButton:hover{ background: rgb(240,240,240); }"
                                      "QPushButton:pressed{ background: white; }");

    menuTitleBox = new QGroupBox(this);
    menuTitleBox->setFixedHeight(menuButton->height() + SPACER_SIZE*3);
    menuTitleBox->setMask(QRegion(0, (menuTitleBox->height() - menuButton->height()) / 2,
                                  menuButton->width() + SPACER_SIZE + projectName->width() + closeProjectButton->width(), menuButton->height(),
                                  QRegion::Rectangle));


    // setup aspect toggle buttons
    definitionsToggle = new AspectToggleWidget(VA_INTERFACES, RIGHT_PANEL_WIDTH/2, this);
    workloadToggle = new AspectToggleWidget(VA_BEHAVIOUR, RIGHT_PANEL_WIDTH/2, this);
    assemblyToggle = new AspectToggleWidget(VA_ASSEMBLIES, RIGHT_PANEL_WIDTH/2, this);
    hardwareToggle = new AspectToggleWidget(VA_HARDWARE, RIGHT_PANEL_WIDTH/2, this);

    aspectToggles << definitionsToggle;
    aspectToggles << workloadToggle;
    aspectToggles << assemblyToggle;
    aspectToggles << hardwareToggle;

    QVBoxLayout* tableLayout = new QVBoxLayout();
    tableLayout->setMargin(0);
    tableLayout->setContentsMargins(5,0,0,0);
    tableLayout->addWidget(dataTable);
    dataTableBox->setLayout(tableLayout);

    QHBoxLayout* titleLayout = new QHBoxLayout();
    titleLayout->setMargin(0);
    titleLayout->setSpacing(0);
    titleLayout->addWidget(menuButton);
    titleLayout->addSpacerItem(new QSpacerItem(SPACER_SIZE, 0));
    titleLayout->addWidget(closeProjectButton);
    titleLayout->addWidget(projectName);
    titleLayout->addStretch();
    menuTitleBox->setLayout(titleLayout);

    QHBoxLayout* topHLayout = new QHBoxLayout();
    topHLayout->setMargin(0);
    topHLayout->setSpacing(0);
    topHLayout->addWidget(menuTitleBox);
    topHLayout->addStretch();

    QHBoxLayout* bodyLayout = new QHBoxLayout();
    QVBoxLayout* leftVlayout = new QVBoxLayout();
    leftVlayout->setMargin(0);
    leftVlayout->setSpacing(0);
    leftVlayout->addLayout(topHLayout);
    leftVlayout->addSpacerItem(new QSpacerItem(SPACER_SIZE*2, SPACER_SIZE));
    leftVlayout->addLayout(bodyLayout);
    leftVlayout->addStretch();

    QGridLayout* viewButtonsGrid = new QGridLayout();
    viewButtonsGrid->setSpacing(5);
    viewButtonsGrid->setMargin(0);
    viewButtonsGrid->setContentsMargins(5,0,5,0);

    viewButtonsGrid->addWidget(definitionsToggle, definitionsToggle->getToggleGridPos().x(), definitionsToggle->getToggleGridPos().y());
    viewButtonsGrid->addWidget(workloadToggle, workloadToggle->getToggleGridPos().x(), workloadToggle->getToggleGridPos().y());
    viewButtonsGrid->addWidget(assemblyToggle, assemblyToggle->getToggleGridPos().x(), assemblyToggle->getToggleGridPos().y());
    viewButtonsGrid->addWidget(hardwareToggle, hardwareToggle->getToggleGridPos().x(), hardwareToggle->getToggleGridPos().y());

    searchLayout = new QHBoxLayout();
    minimapBox = new QWidget(this);

    QVBoxLayout* rightVlayout =  new QVBoxLayout();
    rightVlayout->setMargin(0);
    rightVlayout->setContentsMargins(0, SPACER_SIZE, 0, 0);
    rightVlayout->addLayout(searchLayout);
    rightVlayout->addSpacerItem(new QSpacerItem(0, SPACER_SIZE));
    rightVlayout->addLayout(viewButtonsGrid);
    rightVlayout->addSpacerItem(new QSpacerItem(0, SPACER_SIZE));
    rightVlayout->addWidget(dataTableBox);
    rightVlayout->addStretch();
    rightVlayout->addWidget(minimapBox);

    viewLayout = new QHBoxLayout();
    viewLayout->setMargin(0);
    viewLayout->setSpacing(0);
    viewLayout->addLayout(leftVlayout, 4);
    viewLayout->addLayout(rightVlayout, 1);
    viewLayout->setContentsMargins(15, 0, 5, 10);

    viewHolderLayout = new QVBoxLayout();
    viewHolderLayout->setMargin(0);
    viewHolderLayout->setSpacing(0);
    viewHolderLayout->addLayout(viewLayout);
    nodeView->setLayout(viewHolderLayout);

    // set central widget, window size and generic stylesheets
    setCentralWidget(nodeView);
    setMinimumSize(MIN_WIDTH, MIN_HEIGHT);
    setWindowStyleSheet();
    //setStyle(QStyleFactory::create("windows"));


    // setup the menu, dock, search tools, toolbar and information display widgets
    setupMenu();
    setupSearchTools();
    setupToolbar();
    setupMinimap();
    setupDocks(bodyLayout);
    setupInfoWidgets(bodyLayout);
    setupMultiLineBox();
    setupWelcomeScreen();

    updateRecentProjectsWidgets();
}


/**
 * @brief MedeaWindow::setWindowStyleSheet
 */
void MedeaWindow::setWindowStyleSheet()
{
    //QString background = "rgb(70,70,70);";
    //QString textColor = "white;";

    setStyleSheet("QToolBar::separator { width:8px; background-color: rgba(0,0,0,0); }"
                  "QToolButton {"
                  "margin: 0px 1px;"
                  "border-radius: 10px;"
                  "border: 1px solid rgb(160,160,160);"
                  "background-color: rgb(200,200,200);"
                  "}"
                  "QToolButton:hover {"
                  "border: 2px solid rgb(140,140,140);"
                  "background-color: rgb(240,240,240);"
                  "}"
                  "QToolButton:disabled { background-color: rgb(150,150,150);}"
                  "QToolButton:pressed { background-color: white; }"
                  "QToolButton[popupMode=\"1\"] {"
                  "padding-right: 15px;"
                  "}"
                  "QToolButton::menu-button {"
                  "border-left: 1px solid rgb(150,150,150);"
                  "border-top-right-radius: 10px;"
                  "border-bottom-right-radius: 10px;"
                  "width: 15px;"
                  "}"

                  "QCheckBox { padding: 0px 10px 0px 0px; }"
                  "QCheckBox::indicator { width: 25px; height: 25px; }"
                  "QCheckBox:checked { color: green; font-weight: bold; }"

                  /*
                                                                          "QProgressBar {"
                                                                          "border: 2px solid gray;"
                                                                          "border-radius: 10px;"
                                                                          "background: rgb(240,240,240);"
                                                                          "text-align: center;"
                                                                          "color: black;"
                                                                          "}"
                                                                          "QProgressBar::chunk {"
                                                                          "border-radius: 7px;"
                                                                          "background: rgb(0,204,0);"
                                                                          "}"
                                                                          */

                  "QGroupBox {"
                  "background-color: rgba(0,0,0,0);"
                  "border: 0px;"
                  "margin: 0px;"
                  "padding: 0px;"
                  "}"

                  "QMessageBox{background-color:" + palette().color(QWidget::backgroundRole()).name() + ";}"
                  );

}


/**
 * @brief MedeaWindow::setupMenu
 * Initialise and setup menus and their actions.
 */
void MedeaWindow::setupMenu()
{
    menu = new QMenu(this);

    file_menu = menu->addMenu(getIcon("Actions", "Menu"), "File");
    edit_menu = menu->addMenu(getIcon("Actions", "Edit"), "Edit");

    menu->addSeparator();

    view_menu = menu->addMenu(getIcon("Actions", "MenuView"), "View");
    model_menu = menu->addMenu(getIcon("Actions", "MenuModel"), "Model");
    jenkins_menu = menu->addMenu(getIcon("Actions", "Jenkins_Icon"), "Jenkins");

    menu->addSeparator();

    settings_changeAppSettings = menu->addAction(getIcon("Actions", "Settings"), "Settings");
    settings_changeAppSettings->setShortcut(QKeySequence(Qt::Key_F10));
    help_menu = menu->addMenu(getIcon("Actions", "Help"), "Help");

    exit = menu->addAction(getIcon("Actions", "Power"), "Exit");

    file_newProject = file_menu->addAction(getIcon("Actions", "New"), "New Project");
    file_newProject->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));

    //file_openProject = file_menu->addAction(getIcon("Actions", "Import"), "Open Project");
    file_openProject = file_menu->addAction(getIcon("Actions", "Open"), "Open Project");
    file_openProject->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));

    file_recentProjectsMenu = file_menu->addMenu(getIcon("Actions", "Timer"), "Recent Projects");

    file_recentProjects_clearHistory = file_recentProjectsMenu->addAction(getIcon("Actions", "Clear"), "Clear History");

    file_menu->addSeparator();

    file_saveProject = file_menu->addAction(getIcon("Actions", "Save"), "Save Project");
    file_saveProject->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));

    file_saveAsProject = file_menu->addAction(getIcon("Actions", "Save"), "Save Project As");
    file_saveAsProject->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S));

    file_closeProject = file_menu->addAction(getIcon("Actions", "Close"), "Close Project");

    file_menu->addSeparator();

    file_importGraphML = file_menu->addAction(getIcon("Actions", "Import"), "Import");
    file_importGraphML->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
    file_importSnippet = file_menu->addAction(getIcon("Actions", "ImportSnippet"), "Import Snippet");
    file_importXME = file_menu->addAction(QIcon(":/GME.ico"), "Import XME File");

    file_menu->addSeparator();

    file_exportSnippet = file_menu->addAction(getIcon("Actions", "ExportSnippet"), "Export Snippet");

    file_menu->addSeparator();

    edit_undo = edit_menu->addAction(getIcon("Actions", "Undo"), "Undo");
    edit_undo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z));
    edit_redo = edit_menu->addAction(getIcon("Actions", "Redo"), "Redo");
    edit_redo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Y));
    edit_menu->addSeparator();
    edit_cut = edit_menu->addAction(getIcon("Actions", "Cut"), "Cut");
    edit_cut->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_X));
    edit_copy = edit_menu->addAction(getIcon("Actions", "Copy"), "Copy");
    edit_copy->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
    edit_paste = edit_menu->addAction(getIcon("Actions", "Paste"), "Paste");
    edit_paste->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_V));
    edit_replicate = edit_menu->addAction(getIcon("Actions", "Replicate"), "Replicate");
    edit_replicate->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));
    edit_menu->addSeparator();
    edit_search = edit_menu->addAction(getIcon("Actions", "Search"), "Search");
    edit_search->setShortcut(QKeySequence(Qt::Key_F3));
    edit_delete= edit_menu->addAction(getIcon("Actions", "Delete"), "Delete Selection");

    view_fitToScreen = view_menu->addAction(getIcon("Actions", "FitToScreen"), "Fit To Screen");
    view_fitToScreen->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Space));
    view_menu->addSeparator();
    view_goToDefinition = view_menu->addAction(getIcon("Actions", "Definition"), "Go To Definition");
    view_goToDefinition->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_D));
    view_goToImplementation = view_menu->addAction(getIcon("Actions", "Implementation"), "Go To Implementation");
    view_goToImplementation->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_I));
    view_menu->addSeparator();
    view_showConnectedNodes = view_menu->addAction(getIcon("Actions", "Connections"), "View Connections");
    view_menu->addSeparator();
    view_fullScreenMode = view_menu->addAction(getIcon("Actions", "Fullscreen"), "Start Fullscreen Mode");
    view_fullScreenMode->setShortcut(QKeySequence(Qt::Key_F11));
    view_fullScreenMode->setCheckable(true);
    view_printScreen = view_menu->addAction(getIcon("Actions", "PrintScreen"), "Print Screen");
    view_printScreen->setShortcut(QKeySequence(Qt::Key_F12));

    view_menu->addSeparator();
    view_showMinimap = view_menu->addAction(getIcon("Actions", "Minimap"), "Show Minimap");
    view_showMinimap->setEnabled(false);

    model_clearModel = model_menu->addAction(getIcon("Actions", "Clear"), "Clear Model");
    model_menu->addSeparator();
    model_validateModel = model_menu->addAction(getIcon("Actions", "Validate"), "Validate Model");
    model_validateModel->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_V));

    model_ExecuteLocalJob = model_menu->addAction(getIcon("Actions", "Job_Build"), "Launch: Local Deployment");
    model_ExecuteLocalJob->setEnabled(true);
    model_ExecuteLocalJob->setToolTip("Requires Valid CUTS and Windows");

    //Setup Jenkins Menu
    QString jenkinsJobName = appSettings->getSetting(JENKINS_JOB).toString();

    //Generic Jenkins Functionality.
    jenkins_ImportNodes = jenkins_menu->addAction(getIcon("Actions", "Computer"), "Import Jenkins Nodes");
    jenkins_ImportNodes->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_J));

    jenkins_ExecuteJob = jenkins_menu->addAction(getIcon("Actions", "Job_Build"), "Launch: " + jenkinsJobName);

    help_Shortcuts = help_menu->addAction(getIcon("Actions", "Keyboard"), "App Shortcuts");
    help_Shortcuts->setShortcut(QKeySequence(Qt::Key_F1));
    help_ReportBug = help_menu->addAction(getIcon("Actions", "BugReport"), "Report Bug");
    help_Wiki = help_menu->addAction(getIcon("Actions", "Wiki"), "Wiki");
    help_menu->addSeparator();
    help_AboutMedea = help_menu->addAction(getIcon("Actions", "Info"), "About MEDEA");
    help_AboutQt = help_menu->addAction(QIcon(":/Qt.ico"), "About Qt");

    if(!jenkinsManager){
        jenkins_menu->setEnabled(false);
    }
    if(jenkinsJobName == ""){
        jenkins_ExecuteJob->setEnabled(false);
    }

    menu->setFont(guiFont);
    file_menu->setFont(guiFont);
    edit_menu->setFont(guiFont);
    view_menu->setFont(guiFont);
    model_menu->setFont(guiFont);

    menuButton->setMenu(menu);


    actionSort = new QAction(getIcon("Actions", "Sort"), "Sort", this);
    actionSort->setToolTip("Sort Selection");

    actionCenter = new QAction(getIcon("Actions", "Crosshair"), "Center Entity", this);
    actionCenter->setToolTip("Center On Entity");

    actionZoomToFit = new QAction(getIcon("Actions", "ZoomToFit"), "Zoom to Fit Selection", this);
    actionZoomToFit->setToolTip("Center selection and zoom in to fit");

    actionFitToScreen = new QAction(getIcon("Actions", "FitToScreen"), "Fit Model to Screen", this);
    actionFitToScreen->setToolTip("Show Entire Model");

    actionAlignVertically = new QAction(getIcon("Actions", "Align_Vertical"), "Align Selection Vertically", this);
    actionAlignVertically->setToolTip("Align Selection Vertically");

    actionAlignHorizontally = new QAction(getIcon("Actions", "Align_Horizontal"), "Align Selection Horizontally", this);
    actionAlignHorizontally->setToolTip("Align Selection Horizontally");

    actionPopupSubview = new QAction(getIcon("Actions", "Popup"), "Show Selection in New Window", this);
    actionPopupSubview->setToolTip("Show Selection In New Window");

    actionBack = new QAction(getIcon("Actions", "Backward"), "Navigate Back", this);
    actionBack->setToolTip("Navigate Back");

    actionForward = new QAction(getIcon("Actions", "Forward"), "Navigate Forward", this);
    actionForward->setToolTip("Navigate Forward");

    actionContextMenu = new QAction(getIcon("Actions", "Toolbar"), "Show Context Toolbar", this);
    actionContextMenu->setToolTip("Show Context Toolbar");

    actionToggleGrid = new QAction(getIcon("Actions", "Grid_On"), "Toggle Grid Lines", this);
    actionToggleGrid->setToolTip("Turn Off Grid");
    actionToggleGrid->setCheckable(true);

    //Model Actions
    modelActions << file_closeProject;
    modelActions << file_saveProject;
    modelActions << file_saveAsProject;
    modelActions << file_importGraphML;
    modelActions << file_importXME;
    modelActions << file_importSnippet;
    modelActions << file_exportSnippet;

    modelActions << view_menu->actions();
    modelActions << edit_menu->actions();

    modelActions << view_menu->actions();
    modelActions << model_menu->actions();
    modelActions << jenkins_menu->actions();

    modelActions.removeAll(view_fullScreenMode);
    modelActions.removeAll(view_showMinimap);
}


/**
 * @brief MedeaWindow::setupDocks
 * Initialise and setup dock widgets.
 * @param layout
 */
void MedeaWindow::setupDocks(QHBoxLayout *layout)
{
    dockStandAloneDialog = new QDialog(this);
    docksArea = new QGroupBox(this);
    dockButtonsBox = new QGroupBox();
    dockButtonsBox->setStyle(QStyleFactory::create("windows"));

    dockLayout = new QVBoxLayout();
    QVBoxLayout* dockDialogLayout = new QVBoxLayout();
    QVBoxLayout* dockAreaLayout = new QVBoxLayout();
    QHBoxLayout* dockButtonsHlayout = new QHBoxLayout();

    partsButton = new DockToggleButton(PARTS_DOCK, this);
    hardwareNodesButton = new DockToggleButton(HARDWARE_DOCK, this);

    partsDock = new PartsDockScrollArea(PARTS_DOCK, nodeView, partsButton);
    definitionsDock = new DefinitionsDockScrollArea(DEFINITIONS_DOCK, nodeView);
    functionsDock = new FunctionsDockScrollArea(FUNCTIONS_DOCK, nodeView);
    hardwareDock = new HardwareDockScrollArea(HARDWARE_DOCK, nodeView, hardwareNodesButton);

    // width of the containers are fixed
    int dockPadding = 5;
    boxWidth = (partsButton->getWidth() - dockPadding) * 2 + 19;

    // set buttonBox's size and get rid of its border
    QSize buttonsBoxSize(boxWidth + 1, partsButton->getHeight() + dockPadding);
    dockButtonsBox->setStyleSheet("margin: 0px; border: 0px; padding: 0px;");
    dockButtonsBox->setFixedSize(buttonsBoxSize);

    // set dock's size
    partsDock->setFixedWidth(boxWidth);
    definitionsDock->setFixedWidth(boxWidth);
    hardwareDock->setFixedWidth(boxWidth);
    functionsDock->setFixedWidth(boxWidth);

    // set dock's font
    partsDock->widget()->setFont(guiFont);
    definitionsDock->widget()->setFont(guiFont);
    hardwareDock->widget()->setFont(guiFont);
    functionsDock->widget()->setFont(guiFont);

    // set size policy for buttons
    partsButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    hardwareNodesButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // remove extra space in layouts
    dockButtonsHlayout->setMargin(0);
    dockButtonsHlayout->setSpacing(1);
    dockAreaLayout->setMargin(0);
    dockAreaLayout->setSpacing(0);
    dockDialogLayout->setMargin(8);
    dockDialogLayout->setSpacing(0);
    dockLayout->setMargin(0);
    dockLayout->setSpacing(0);

    // add widgets to/and layouts
    dockButtonsHlayout->addWidget(partsButton);
    dockButtonsHlayout->addWidget(hardwareNodesButton);
    dockButtonsBox->setLayout(dockButtonsHlayout);

    dockBackButtonBox = new QGroupBox(this);
    dockBackButtonBox->setStyleSheet("QGroupBox {"
                                     "background-color: rgba(0,0,0,0);"
                                     "border: 0px;"
                                     "margin: 0px;"
                                     "padding: 0px;"
                                     "}");

    dockHeaderBox = new QGroupBox(this);
    dockHeaderBox->setStyleSheet("QGroupBox {"
                                 "border-left: 1px solid rgb(125,125,125);"
                                 "border-right: 1px solid rgb(125,125,125);"
                                 "border-top: none;"
                                 "border-bottom: none;"
                                 "background-color: rgba(250,250,250,240);"
                                 "padding: 10px 0px 0px 0px; }");

    dockActionLabel = new QLabel("Describe action here", this);
    dockActionLabel->setAlignment(Qt::AlignCenter);
    dockActionLabel->setStyleSheet("border: none; background-color: rgba(0,0,0,0); padding: 10px 5px;");

    QPushButton* dockBackButton = new QPushButton(QIcon(":/Actions/Backward.png"), "", this);
    dockBackButton->setFixedSize(boxWidth, 35);
    dockBackButton->setToolTip("Go back to the Parts list");
    dockBackButton->setStyleSheet("QPushButton{"
                                  "background-color: rgba(130,130,130,120);"
                                  "}"
                                  "QPushButton:hover{"
                                  "background-color: rgba(180,180,180,150);"
                                  "}");
    connect(dockBackButton, SIGNAL(clicked(bool)), this, SLOT(dockBackButtonTriggered()));

    openedDockLabel = new QLabel("Parts", this);
    openedDockLabel->setFixedWidth(boxWidth);
    //openedDockLabel->setAlignment(Qt::AlignCenter);
    openedDockLabel->setFont(QFont("Helvetica", 11));
    openedDockLabel->setStyleSheet("border: none; background-color: rgba(0,0,0,0); padding: 0px 8px 5px 8px;");

    QVBoxLayout* dockBackButtonLayout = new QVBoxLayout();
    dockBackButtonLayout->setMargin(0);
    dockBackButtonLayout->setSpacing(0);
    dockBackButtonLayout->addWidget(dockBackButton, 0, Qt::AlignCenter);
    dockBackButtonLayout->addSpacerItem(new QSpacerItem(0, SPACER_SIZE));
    dockBackButtonBox->setLayout(dockBackButtonLayout);

    QVBoxLayout* dockHeaderLayout = new QVBoxLayout();
    dockHeaderLayout->setMargin(0);
    dockHeaderLayout->setSpacing(0);
    dockHeaderLayout->addWidget(openedDockLabel);
    dockHeaderLayout->addWidget(dockActionLabel);
    dockHeaderLayout->addWidget(dockBackButtonBox);
    dockHeaderBox->setLayout(dockHeaderLayout);

    QVBoxLayout* innerDockLayout = new QVBoxLayout();
    innerDockLayout->setMargin(0);
    innerDockLayout->setSpacing(0);
    innerDockLayout->addWidget(dockHeaderBox);
    innerDockLayout->addWidget(partsDock);
    innerDockLayout->addWidget(definitionsDock);
    innerDockLayout->addWidget(functionsDock);
    innerDockLayout->addWidget(hardwareDock);
    innerDockLayout->addStretch();

    dockGroupBox = new QGroupBox(this);
    dockGroupBox->setLayout(innerDockLayout);

    dockLayout->addWidget(dockButtonsBox);
    dockLayout->addWidget(dockGroupBox);

    dockAreaLayout->addLayout(dockLayout);
    docksArea->setLayout(dockAreaLayout);
    docksArea->setFixedWidth(boxWidth);
    docksArea->setMask(QRegion(0, 0, boxWidth, dockButtonsBox->height(), QRegion::Rectangle));
    docksArea->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    layout->addWidget(docksArea, 1);

    dockStandAloneDialog->setVisible(false);
    dockStandAloneDialog->setLayout(dockDialogLayout);
    dockStandAloneDialog->setFixedWidth(boxWidth + 15);
    dockStandAloneDialog->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    dockStandAloneDialog->setStyleSheet("QDialog{ background-color: rgba(175,175,175,255); }");
    dockStandAloneDialog->setWindowTitle("MEDEA - Dock");

    // initially hide the header section of the dock
    dockHeaderBox->hide();
    openedDockLabel->hide();
    dockBackButtonBox->hide();
    dockActionLabel->hide();
}


/**
 * @brief MedeaWindow::setupSearchTools
 * @param layout
 */
void MedeaWindow::setupSearchTools()
{
    searchBarDefaultText = "Search Here...";
    searchBar = new QLineEdit(searchBarDefaultText, this);
    searchSuggestions = new SearchSuggestCompletion(searchBar);
    searchButton = new QPushButton(getIcon("Actions", "Search"), "");
    searchOptionButton = new QPushButton(getIcon("Actions", "Settings"), "");
    searchOptionMenu = new QMenu(searchOptionButton);
    searchResults = new QDialog(this);
    searchDialog = new SearchDialog(QSize(SEARCH_DIALOG_MIN_WIDTH, SEARCH_DIALOG_MIN_HEIGHT), this);

    QVBoxLayout* layout = new QVBoxLayout();
    QWidget* scrollableWidget = new QWidget(this);
    QScrollArea* scrollableSearchResults = new QScrollArea(this);

    QVBoxLayout* resultsMainLayout = new QVBoxLayout();
    resultsLayout = new QVBoxLayout();

    int searchBarHeight = 28;
    float searchItemMinWidth = 500.0;

    resultsMainLayout->addLayout(resultsLayout);
    resultsMainLayout->addStretch();

    searchButton->setFixedSize(30, searchBarHeight);
    searchButton->setIconSize(searchButton->size()*0.65);

    searchOptionButton->setFixedSize(30, searchBarHeight);
    searchOptionButton->setIconSize(searchButton->size()*0.7);
    searchOptionButton->setCheckable(true);
    //searchOptionButton->setStyle(QStyleFactory::create("windows"));

    searchBar->setPlaceholderText(searchBarDefaultText);
    searchBar->setFixedSize(RIGHT_PANEL_WIDTH - (searchButton->width()*2) + 10, searchBarHeight - 3);
    searchBar->setStyleSheet("QLineEdit{ background-color: rgb(230,230,230); }"
                             "QLineEdit:focus{ border: 1px solid; border-color: blue; background: rgb(250,250,250) }");

    searchSuggestions->setSize(searchBar->width(), height(), 2);

    scrollableWidget->setLayout(resultsMainLayout);
    scrollableSearchResults->setWidget(scrollableWidget);
    scrollableSearchResults->setWidgetResizable(true);
    layout->addWidget(scrollableSearchResults);

    searchResults->setLayout(layout);
    searchResults->setMinimumWidth(searchItemMinWidth + 200);
    searchResults->setMinimumHeight(height() / 2);
    searchResults->setWindowTitle("Search Results");
    searchResults->setVisible(false);

    searchLayout->setContentsMargins(10,0,8,0);
    searchLayout->addWidget(searchBar, 3);
    searchLayout->addWidget(searchButton, 1);
    searchLayout->addWidget(searchOptionButton, 1);

    // setup search option widgets and menu for view aspects
    QWidgetAction* aspectsAction = new QWidgetAction(this);
    QLabel* aspectsLabel = new QLabel("Aspect(s):", this);
    QGroupBox* aspectsGroup = new QGroupBox(this);
    QHBoxLayout* aspectsLayout = new QHBoxLayout();

    aspectsLabel->setMinimumWidth(50);
    aspectsLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    viewAspectsBarDefaultText = "Entire Model";
    viewAspectsBar = new QLineEdit(viewAspectsBarDefaultText, this);
    viewAspectsButton = new QPushButton(getIcon("Actions", "Arrow_Down"), "");
    viewAspectsMenu = new QMenu(viewAspectsButton);

    viewAspectsButton->setFixedSize(20, 20);
    viewAspectsButton->setIconSize(viewAspectsButton->size()*0.6);
    viewAspectsButton->setCheckable(true);
    viewAspectsBar->setFixedWidth(RIGHT_PANEL_WIDTH - viewAspectsButton->width() - aspectsLabel->width() - 30);
    viewAspectsBar->setToolTip("Search Aspects: " + viewAspectsBarDefaultText);
    viewAspectsBar->setEnabled(false);
    viewAspectsMenu->setMinimumWidth(viewAspectsBar->width() + viewAspectsButton->width());

    aspectsLayout->setMargin(5);
    aspectsLayout->setSpacing(3);
    aspectsLayout->addWidget(aspectsLabel);
    aspectsLayout->addWidget(viewAspectsBar);
    aspectsLayout->addWidget(viewAspectsButton);

    aspectsGroup->setLayout(aspectsLayout);
    aspectsAction->setDefaultWidget(aspectsGroup);

    // populate view aspects menu
    QStringList aspects = GET_ASPECT_NAMES();
    aspects.sort();
    foreach (QString aspect, aspects) {
        QWidgetAction* action = new QWidgetAction(this);
        QCheckBox* checkBox = new QCheckBox(aspect, this);
        checkBox->setFont(guiFont);
        connect(checkBox, SIGNAL(clicked()), this, SLOT(updateSearchLineEdits()));
        action->setDefaultWidget(checkBox);
        viewAspectsMenu->addAction(action);
    }

    // setup search option widgets and menu for view aspects
    QWidgetAction* kindsAction = new QWidgetAction(this);
    QLabel* kindsLabel = new QLabel("Kind(s):", this);
    QGroupBox* kindsGroup = new QGroupBox(this);
    QHBoxLayout* kindsLayout = new QHBoxLayout();
    nodeKindsDefaultText = "All Kinds";
    nodeKindsBar = new QLineEdit(nodeKindsDefaultText, this);
    nodeKindsButton = new QPushButton(getIcon("Actions", "Arrow_Down"), "");
    nodeKindsMenu = new QMenu(nodeKindsButton);

    kindsLabel->setMinimumWidth(50);
    kindsLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    nodeKindsButton->setFixedSize(20, 20);
    nodeKindsButton->setIconSize(nodeKindsButton->size()*0.6);
    nodeKindsButton->setCheckable(true);
    nodeKindsBar->setFixedWidth(RIGHT_PANEL_WIDTH - nodeKindsButton->width() - kindsLabel->width() - 30);
    nodeKindsBar->setToolTip("Search Kinds: " + nodeKindsDefaultText);
    nodeKindsBar->setEnabled(false);
    nodeKindsMenu->setMinimumWidth(nodeKindsBar->width() + nodeKindsButton->width());

    kindsLayout->setMargin(5);
    kindsLayout->setSpacing(3);
    kindsLayout->addWidget(kindsLabel);
    kindsLayout->addWidget(nodeKindsBar);
    kindsLayout->addWidget(nodeKindsButton);

    kindsGroup->setLayout(kindsLayout);
    kindsAction->setDefaultWidget(kindsGroup);

    // setup search option widgets and menu for data keys
    QWidgetAction* keysAction = new QWidgetAction(this);
    QLabel* keysLabel = new QLabel("Data Key(s):", this);
    QGroupBox* keysGroup = new QGroupBox(this);
    QHBoxLayout* keysLayout = new QHBoxLayout();

    keysLabel->setMinimumWidth(50);
    keysLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    dataKeys = QStringList() << "label" << "type" << "worker" << "description" << "topicName" << "kind";
    dataKeys.sort();

    foreach (QString key, dataKeys) {
        dataKeysDefaultText += key + ", ";
    }
    dataKeysDefaultText.truncate(dataKeysDefaultText.length() - 2);

    dataKeysBar = new QLineEdit(dataKeysDefaultText, this);
    dataKeysButton = new QPushButton(getIcon("Actions", "Arrow_Down"), "");
    dataKeysMenu = new QMenu(dataKeysButton);

    dataKeysButton->setFixedSize(20, 20);
    dataKeysButton->setIconSize(dataKeysButton->size()*0.6);
    dataKeysButton->setCheckable(true);
    dataKeysBar->setFixedWidth(RIGHT_PANEL_WIDTH - dataKeysButton->width() - keysLabel->width() - 30);
    dataKeysBar->setToolTip("Search Data Keys: " + dataKeysDefaultText);
    dataKeysBar->setCursorPosition(0);
    dataKeysBar->setEnabled(false);
    dataKeysMenu->setMinimumWidth(dataKeysBar->width() + dataKeysButton->width());

    keysLayout->setMargin(5);
    keysLayout->setSpacing(3);
    keysLayout->addWidget(keysLabel);
    keysLayout->addWidget(dataKeysBar);
    keysLayout->addWidget(dataKeysButton);

    keysGroup->setLayout(keysLayout);
    keysAction->setDefaultWidget(keysGroup);

    // populate data attribute keys menu
    foreach (QString key, dataKeys) {
        QWidgetAction* action = new QWidgetAction(this);
        QCheckBox* checkBox = new QCheckBox(key, this);
        checkBox->setFont(guiFont);
        connect(checkBox, SIGNAL(clicked()), this, SLOT(updateSearchLineEdits()));
        action->setDefaultWidget(checkBox);
        dataKeysMenu->addAction(action);
    }

    searchBar->setFont(guiFont);
    viewAspectsBar->setFont(guiFont);
    nodeKindsBar->setFont(guiFont);
    dataKeysBar->setFont(guiFont);
    aspectsLabel->setFont(guiFont);
    kindsLabel->setFont(guiFont);
    keysLabel->setFont(guiFont);

    // add widget actions and their menus to the main search option menu
    searchOptionMenu->addAction(aspectsAction);
    searchOptionMenu->addAction(kindsAction);
    searchOptionMenu->addAction(keysAction);
    searchOptionMenu->setVisible(false);
}


/**
 * @brief MedeaWindow::setupInfoWidgets
 * @param layout
 */
void MedeaWindow::setupInfoWidgets(QHBoxLayout* layout)
{
    QFont biggerFont = QFont(guiFont.family(), 11);

    // setup progress bar
    progressBar = new QProgressBar(this);
    progressBar->setFixedHeight(20);
    progressBar->setStyleSheet("border: 2px solid gray;"
                               "border-radius: 5px;"
                               "background: rgb(240,240,240);"
                               "text-align: center;"
                               "color: black;");

    // setup progress label
    progressLabel = new QLabel(this);
    progressLabel->setAlignment(Qt::AlignCenter);
    progressLabel->setFixedHeight(30);
    progressLabel->setFont(biggerFont);
    progressLabel->setStyleSheet("background: rgba(0,0,0,0); padding: 0px; color: white;");

    QVBoxLayout* progressLayout = new QVBoxLayout();
    progressLayout->addWidget(progressLabel);
    progressLayout->addWidget(progressBar);

    QWidget* progressWidget = new QWidget(this);
    progressWidget->setLayout(progressLayout);
    progressWidget->setFixedSize(RIGHT_PANEL_WIDTH*2, progressBar->height() + progressLabel->height() + SPACER_SIZE*3);
    progressWidget->setStyleSheet("QWidget{ padding: 0px; border-radius: 5px; }");

    // setup progress dialog
    progressDialog = new QDialog();
    progressDialog->setModal(true);
    progressDialog->setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    progressDialog->setAttribute(Qt::WA_NoSystemBackground, true);
    progressDialog->setAttribute(Qt::WA_TranslucentBackground, true);
    //progressDialog->setStyleSheet("background-color: rgba(250,250,250,0.85);");
    progressDialog->setStyleSheet("background-color: rgba(50,50,50,0.85);");
    progressDialogVisible = false;

    QVBoxLayout* innerLayout = new QVBoxLayout();
    innerLayout->setMargin(0);
    innerLayout->setSpacing(0);
    innerLayout->addWidget(progressWidget);
    progressDialog->setLayout(innerLayout);

    // setup notification bar and timer
    notificationTimer = new QTimer(this);
    notificationsBar = new QLabel("", this);
    notificationsBar->setFixedHeight(40);
    notificationsBar->setFont(biggerFont);
    notificationsBar->setAlignment(Qt::AlignCenter);
    notificationsBar->setStyleSheet("background-color: rgba(250,250,250,0.85);"
                                    "color: rgb(30,30,30);"
                                    "border-radius: 5px;"
                                    "padding: 0px 15px;");

    // setup loading gif and widgets
    loadingLabel = new QLabel("Loading...", this);
    loadingLabel->setFont(biggerFont);
    loadingLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    loadingMovie = new QMovie(":/Actions/Loading.gif");
    loadingMovie->setBackgroundColor(Qt::white);
    loadingMovie->setScaledSize(QSize(TOOLBAR_BUTTON_WIDTH*1.25, TOOLBAR_BUTTON_HEIGHT*1.25));
    loadingMovie->start();

    loadingMovieLabel = new QLabel(this);
    loadingMovieLabel->setMovie(loadingMovie);

    QHBoxLayout* loadingLayout = new QHBoxLayout();
    loadingLayout->setMargin(0);
    loadingLayout->setSpacing(0);
    loadingLayout->addStretch();
    loadingLayout->addWidget(loadingMovieLabel);
    loadingLayout->addWidget(loadingLabel);
    loadingLayout->addStretch();

    loadingBox = new QGroupBox(this);
    loadingBox->setFixedHeight(TOOLBAR_BUTTON_HEIGHT);
    loadingBox->setLayout(loadingLayout);

    // add widgets to layout
    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->addStretch();
    vLayout->addWidget(notificationsBar);
    vLayout->setAlignment(notificationsBar, Qt::AlignCenter);
    vLayout->addWidget(loadingBox, 1, Qt::AlignHCenter);

    layout->addStretch();
    layout->addLayout(vLayout);
    layout->addStretch();
}


/**
 * @brief MedeaWindow::setupToolbar
 * Initialise and setup toolbar widgets.
 */
void MedeaWindow::setupToolbar()
{
    // TODO - Group separators with tool buttons; hide them accordingly
    toolbar = new QToolBar(this);
    toolbarLayout = new QVBoxLayout();

    toolbarButton = new QToolButton(this);
    toolbarButton->setFixedSize(TOOLBAR_BUTTON_WIDTH, TOOLBAR_BUTTON_HEIGHT / 2);
    toolbarButton->setCheckable(true);
    toolbarButton->setStyleSheet("QToolButton{ background-color: rgb(200,200,200); border-radius: 5px; }"
                                 "QToolButton:hover{ background-color: rgb(240,240,240); }");

    QImage expandImage(":/Actions/Arrow_Down");
    QImage contractImage(":/Actions/Arrow_Up");
    expandImage = expandImage.scaled(toolbarButton->width(), toolbarButton->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    contractImage = contractImage.scaled(toolbarButton->width(), toolbarButton->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    expandPixmap = QPixmap::fromImage(expandImage);
    contractPixmap = QPixmap::fromImage(contractImage);

    toolbarButtonLabel = new QLabel(this);
    toolbarButtonLabel->setPixmap(expandPixmap);
    QVBoxLayout* labelLayout = new QVBoxLayout();
    labelLayout->setMargin(0);
    labelLayout->addWidget(toolbarButtonLabel);
    labelLayout->setAlignment(toolbarButtonLabel, Qt::AlignCenter);
    toolbarButton->setLayout(labelLayout);

    constructToolbarButton(toolbar, edit_undo, TOOLBAR_UNDO);
    constructToolbarButton(toolbar, edit_redo, TOOLBAR_REDO);

    toolbar->addSeparator();
    constructToolbarButton(toolbar, edit_cut, TOOLBAR_CUT);
    constructToolbarButton(toolbar, edit_copy, TOOLBAR_COPY);
    constructToolbarButton(toolbar, edit_paste, TOOLBAR_PASTE);
    constructToolbarButton(toolbar, edit_replicate, TOOLBAR_REPLICATE);

    toolbar->addSeparator();
    constructToolbarButton(toolbar, actionFitToScreen, TOOLBAR_FIT_TO_SCREEN);
    constructToolbarButton(toolbar, actionCenter, TOOLBAR_CENTER_ON_ENTITY);
    constructToolbarButton(toolbar, actionZoomToFit, TOOLBAR_ZOOM_TO_FIT);
    constructToolbarButton(toolbar, actionPopupSubview, TOOLBAR_POPUP_SUBVIEW);

    toolbar->addSeparator();
    constructToolbarButton(toolbar, actionSort, TOOLBAR_SORT);
    constructToolbarButton(toolbar, edit_delete, TOOLBAR_DELETE_ENTITIES);

    //toolbar->addSeparator();
    constructToolbarButton(toolbar, actionContextMenu, TOOLBAR_CONTEXT);
    constructToolbarButton(toolbar, actionToggleGrid, TOOLBAR_GRID_LINES);

    //toolbar->addSeparator();
    constructToolbarButton(toolbar, actionAlignVertically, TOOLBAR_VERT_ALIGN);
    constructToolbarButton(toolbar, actionAlignHorizontally, TOOLBAR_HORIZ_ALIGN);

    //toolbar->addSeparator();
    constructToolbarButton(toolbar, actionBack, TOOLBAR_BACK);
    constructToolbarButton(toolbar, actionForward, TOOLBAR_FORWARD);

    toolbar->setStyle(QStyleFactory::create("windows"));
    toolbar->setFloatable(false);
    toolbar->setMovable(false);
}


/**
 * @brief MedeaWindow::constructToolbarButton
 * @param toolbar
 * @param action
 * @param actionName
 * @return
 */
bool MedeaWindow::constructToolbarButton(QToolBar* toolbar, QAction *action, QString actionName)
{
    if (toolbar && action && actionName != "") {

        if (actionName == TOOLBAR_BACK || actionName == TOOLBAR_FORWARD || actionName == TOOLBAR_ZOOM_TO_FIT) {
            return false;
        }

        QSize buttonSize = QSize(TOOLBAR_BUTTON_WIDTH, TOOLBAR_BUTTON_HEIGHT);
        toolbar->setIconSize(buttonSize*0.6);

        ActionButton* actionButton = new ActionButton(action);
        actionButton->setFixedSize(buttonSize);

        QAction* toolAction = toolbar->addWidget(actionButton);
        if (!toolbarActionLookup.contains(actionName)) {
            toolbarActionLookup[actionName] = toolAction;
        } else {
            qCritical() << "Duplicate Actions";
        }
        if (!toolbarButtonLookup.contains(actionName)) {
            toolbarButtonLookup[actionName] = actionButton;
        } else {
            qCritical() << "Duplicate Actions";
        }

        if (!modelActions.contains(action)) {
            modelActions << action;
        }

        connect(this, SIGNAL(window_refreshActions()), actionButton, SLOT(actionChanged()));

        // setup a menu for the replicate button to allow the user to enter the replicate count
        /*
        if (actionName == TOOLBAR_REPLICATE) {
            QMenu* buttonMenu = new QMenu(this);
            QLineEdit* le = new QLineEdit(this);
            QWidgetAction* action = new QWidgetAction(this);
            action->setDefaultWidget(le);
            buttonMenu->addAction(action);
            actionButton->setMenu(buttonMenu);
            actionButton->setPopupMode(QToolButton::MenuButtonPopup);
            actionButton->setFixedWidth(55);
            le->setFixedSize(47,25);
            le->setFont(guiFont);
            connect(le, SIGNAL(returnPressed()), buttonMenu, SLOT(hide()));
            // connect to the replicate slot in the view here
            //connect(le, SIGNAL(returnPressed()), ));
        }
        */

        return true;
    }
    return false;
}


/**
 * @brief MedeaWindow::setupWelcomeScreen
 */
void MedeaWindow::setupWelcomeScreen()
{
    QWidget *containerWidget = new QWidget();
    containerWidget->setFixedHeight(450);
    containerWidget->setFixedWidth(900);
    QVBoxLayout* containerLayout = new QVBoxLayout(containerWidget);

    QPushButton* newProjectButton = new QPushButton("New Project", this);
    QPushButton* openProjectButton = new QPushButton("Open Project", this);
    QPushButton* settingsButton = new QPushButton("Settings", this);
    QPushButton* recentProjectButton = new QPushButton("Recent Projects", this);
    QPushButton* wikiButton = new QPushButton("Wiki", this);
    QPushButton* aboutButton = new QPushButton("About", this);

    settingsButton->setFlat(true);
    settingsButton->setStyleSheet("QPushButton{ color: white; font-size: 16px; text-align: left; }"
                                  "QPushButton:hover{ color: orange; }"
                                  "QTooltip{ background: white; color: black; }");

    openProjectButton->setStyleSheet(settingsButton->styleSheet());
    openProjectButton->setFlat(true);
    newProjectButton->setStyleSheet(settingsButton->styleSheet());
    newProjectButton->setFlat(true);
    recentProjectButton->setFlat(true);
    recentProjectButton->setStyleSheet(settingsButton->styleSheet());
    wikiButton->setFlat(true);
    wikiButton->setStyleSheet(settingsButton->styleSheet() + "QPushButton{ text-align: right; }");
    aboutButton->setFlat(true);
    aboutButton->setStyleSheet(settingsButton->styleSheet() + "QPushButton{ text-align: right; }");


    QLabel* medeaIcon = new QLabel(this);

    QLabel* medeaLabel = new QLabel("MEDEA");
    medeaLabel->setStyleSheet("font-size:32pt;color:white; text-align:center;");
    QLabel* medeaVersionLabel = new QLabel("Version " + MEDEA_VERSION);
    medeaVersionLabel->setStyleSheet("font-size:12pt;color:gray; text-align:center;");

    QPixmap pixMap(":/Actions/MEDEA.png");
    pixMap = pixMap.scaled(QSize(150,150), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    medeaIcon->setPixmap(pixMap);

    newProjectButton->setIcon(getIcon("Actions", "New"));
    openProjectButton->setIcon(getIcon("Actions", "Open"));
    settingsButton->setIcon(getIcon("Actions", "WelcomeSettings"));
    recentProjectButton->setIcon(getIcon("Actions", "Timer"));
    wikiButton->setIcon(getIcon("Actions", "WelcomeWiki"));
    aboutButton->setIcon(getIcon("Actions", "WelcomeHelp"));

    QVBoxLayout* topLayout = new QVBoxLayout();
    topLayout->addWidget(medeaIcon, 0, Qt::AlignCenter);
    topLayout->addWidget(medeaLabel, 0, Qt::AlignCenter);
    topLayout->addWidget(medeaVersionLabel, 0, Qt::AlignCenter);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addStretch();

    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    QVBoxLayout* leftButtonLayout = new QVBoxLayout();
    QVBoxLayout* rightButtonLayout = new QVBoxLayout();

    leftButtonLayout->addLayout(topLayout);
    //leftButtonLayout->setAlignment(topLayout, Qt::AlignHCenter);

    QVBoxLayout* buttonsLayout2 = new QVBoxLayout();
    leftButtonLayout->addSpacerItem(new QSpacerItem(0,10));
    buttonsLayout2->addWidget(newProjectButton,1);
    buttonsLayout2->addSpacerItem(new QSpacerItem(0,5));
    buttonsLayout2->addWidget(openProjectButton,1);
    buttonsLayout2->addSpacerItem(new QSpacerItem(0,5));
    buttonsLayout2->addWidget(settingsButton,1);
    leftButtonLayout->addLayout(buttonsLayout2);
    leftButtonLayout->setAlignment(buttonsLayout2, Qt::AlignHCenter);
    leftButtonLayout->addStretch();

    buttonsLayout->addLayout(leftButtonLayout,1);
    buttonsLayout->addSpacerItem(new QSpacerItem(10,0));
    buttonsLayout->addLayout(rightButtonLayout,2);

    recentProjectsListWidget = new QListWidget(this);
    connect(recentProjectsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(recentProjectItemClicked(QListWidgetItem*)));

    recentProjectsListWidget->setStyleSheet("background:" + GET_VIEW_COLOR_STRING(CURRENT_THEME, DARKER_SHADE) + "; color: white; font-size: 16px; text-align: left;");
    //recentProjectsListWidget->setStyleSheet("background: white; color: white; font-size: 16px; text-align: left;");

    recentProjectButton->setEnabled(false);
    rightButtonLayout->addWidget(recentProjectButton, 0);
    rightButtonLayout->addWidget(recentProjectsListWidget, 1);

    mainLayout->addLayout(buttonsLayout);
    mainLayout->setAlignment(buttonsLayout, Qt::AlignHCenter);

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addStretch();
    bottomLayout->addWidget(wikiButton,0, Qt::AlignRight);
    bottomLayout->addWidget(aboutButton,0, Qt::AlignRight);
    mainLayout->addLayout(bottomLayout);
    mainLayout->addStretch();

    containerLayout->addLayout(mainLayout);

    welcomeLayout = new QHBoxLayout();
    welcomeLayout->addWidget(containerWidget);

    holderLayout = new QVBoxLayout();
    holderLayout->addLayout(welcomeLayout);

    QWidget* holderWidget = new QWidget(this);
    holderWidget->setLayout(holderLayout);
    holderWidget->setMinimumSize(holderLayout->sizeHint());
    holderWidget->hide();

    welcomeScreenOn = false;

    connect(newProjectButton, SIGNAL(clicked(bool)), file_newProject, SIGNAL(triggered(bool)));
    connect(openProjectButton, SIGNAL(clicked(bool)), file_openProject, SIGNAL(triggered(bool)));
    connect(settingsButton, SIGNAL(clicked(bool)), settings_changeAppSettings, SIGNAL(triggered(bool)));
    connect(wikiButton, SIGNAL(clicked(bool)), help_Wiki, SIGNAL(triggered(bool)));
    connect(aboutButton, SIGNAL(clicked(bool)), help_AboutMedea, SIGNAL(triggered(bool)));
}


/**
 * @brief MedeaWindow::setupMinimap
 */
void MedeaWindow::setupMinimap()
{
    // setup minimap
    minimap = new NodeViewMinimap();
    minimap->setScene(nodeView->scene());
    minimap->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    minimap->setVerticalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
    minimap->setInteractive(false);
    minimap->setFixedSize(RIGHT_PANEL_WIDTH + 10, RIGHT_PANEL_WIDTH * 0.6);
    minimap->setStyleSheet("QGraphicsView{ border: 1px solid rgb(50,50,50); }");
    minimap->centerView();

    QLabel* minimapLabel = new QLabel("Minimap", this);
    minimapLabel->setFont(guiFont);
    minimapLabel->setAlignment(Qt::AlignCenter);
    minimapLabel->setFixedSize(RIGHT_PANEL_WIDTH - 10, 20);
    minimapLabel->setStyleSheet("background-color: rgb(210,210,210);"
                                "border: 1px solid rgb(50,50,50);"
                                "border-bottom: none;"
                                "padding-right: 20px;"
                                "font-size: 12px;");

    QPushButton* closeMinimapButton = new QPushButton(getIcon("Actions", "Close"), "");
    closeMinimapButton->setFixedSize(20,20);
    /*closeMinimapButton->setStyleSheet("QPushButton{ background: rgb(200,200,200); }"
                                      "QPushButton:hover{ background: rgb(240,240,240); }"
                                      "QPushButton:pressed{ background: white; }");*/

    QHBoxLayout* minimapHeaderLayout = new QHBoxLayout();
    minimapHeaderLayout->setSpacing(0);
    minimapHeaderLayout->setMargin(0);
    minimapHeaderLayout->setContentsMargins(0,0,0,0);
    minimapHeaderLayout->addWidget(closeMinimapButton);
    minimapHeaderLayout->addWidget(minimapLabel);

    // setup layouts for widgets
    QVBoxLayout* minimapLayout = new QVBoxLayout();
    minimapLayout->setSpacing(0);
    minimapLayout->setMargin(0);
    minimapLayout->setContentsMargins(0,0,0,0);
    minimapLayout->addLayout(minimapHeaderLayout);
    minimapLayout->addWidget(minimap);
    //minimapLayout->setContentsMargins(10,0,8,10);

    minimapBox->setLayout(minimapLayout);
    minimapBox->setFixedWidth(RIGHT_PANEL_WIDTH + 10);
    minimapBox->setFixedHeight(minimapLabel->height() + minimap->height());
    minimapBox->setStyle(QStyleFactory::create("windows"));

    connect(closeMinimapButton, SIGNAL(clicked()), minimapBox, SLOT(hide()));
    connect(closeMinimapButton, SIGNAL(clicked()), this, SLOT(toggleMinimap()));
}


void MedeaWindow::teardownProject()
{
    if (controller) {
        delete controller;
        controller = 0;
        controllerThread = 0;
    }
}

void MedeaWindow::setupProject()
{
    if(!controller && !controllerThread){
        controller = new NewController();
        //Set External Worker Definitions Path.
        controller->setExternalWorkerDefinitionPath(applicationDirectory + "/Resources/WorkerDefinitions/");

        if (THREADING) {
            controllerThread = new QThread();
            controllerThread->start();
            controller->moveToThread(controllerThread);

            connect(controller, SIGNAL(destroyed(QObject*)), controllerThread, SIGNAL(finished()));
        }

        connect(this, SIGNAL(window_ConnectViewAndSetupModel(NodeView*)), controller, SLOT(connectViewAndSetupModel(NodeView*)));

        QEventLoop waitLoop;
        connect(controller, SIGNAL(controller_ModelReady()), &waitLoop, SLOT(quit()));
        emit window_ConnectViewAndSetupModel(nodeView);
        waitLoop.exec();
    }
}


/**
 * @brief MedeaWindow::resetGUI
 * This is called everytime a new project is created.
 */
void MedeaWindow::resetGUI()
{
    updateWidgetsOnWindowChange();

    if (nodeView && !nodeView->hasModel()) {
        modelDisconnected();
    }

    // reset timer
    notificationTimer->stop();
    leftOverTime = 0;

    // initially hide these
    notificationsBar->hide();
    dataTableBox->hide();
    progressDialog->hide();
    loadingBox->hide();

    // shouldn't really need this
    progressDialogVisible = false;

    // clear and reset search bar and search results
    searchBar->clear();
    searchResults->close();
    searchDialog->clear();
    searchDialog->close();

    QLayoutItem* child;
    while (resultsLayout->count() != 0) {
        child = resultsLayout->takeAt(0);
        delete child;
    }

    // reset checked states for all search options menus
    foreach (QAction* action, viewAspectsMenu->actions()) {
        QWidgetAction* widgetAction = qobject_cast<QWidgetAction*>(action);
        QCheckBox* checkBox = qobject_cast<QCheckBox*>(widgetAction->defaultWidget());
        checkBox->setChecked(false);
    }
    /*
    foreach (QAction* action, nodeKindsMenu->actions()) {
        QWidgetAction* widgetAction = qobject_cast<QWidgetAction*>(action);
        QCheckBox* checkBox = qobject_cast<QCheckBox*>(widgetAction->defaultWidget());
        checkBox->setChecked(false);
    }
    */
    foreach (QAction* action, dataKeysMenu->actions()) {
        QWidgetAction* widgetAction = qobject_cast<QWidgetAction*>(action);
        QCheckBox* checkBox = qobject_cast<QCheckBox*>(widgetAction->defaultWidget());
        checkBox->setChecked(false);
    }

    // clear this menu for now because it is getting re-populated every time new project is triggered
    nodeKindsMenu->clear();
}


/**
 * @brief MedeaWindow::resetView
 * This is called everytime a new project is created.
 * It resets the view's turned on aspects to the default
 * and clears all history of the view.
 */
void MedeaWindow::resetView()
{
    // enable the view, reset controller undo/redo states and clear selecttion
    if (nodeView){
        nodeView->view_ClearHistory();
        nodeView->clearSelection();
    }

}


/**
 * @brief MedeaWindow::newProject
 * This is called everytime a new project is created.
 * It clears the model and resets the GUI and view.
 */
void MedeaWindow::newProject()
{
    progressAction = "Setting up New Project";

    toggleWelcomeScreen(false);

    //TODO
    resetGUI();
    setupProject();
}


/**
 * @brief MedeaWindow::makeConnections
 * Connect signals and slots.
 */
void MedeaWindow::setupConnections()
{
    validateResults.connectToWindow(this);
    connect(this, SIGNAL(window_SetViewVisible(bool)), nodeView, SLOT(setVisible(bool)));

    connect(appSettings, SIGNAL(settingChanged(QString,QString,QVariant)), nodeView, SLOT(settingChanged(QString,QString,QVariant)));

    connect(nodeView, SIGNAL(view_LoadSettings()), this, SLOT(loadSettingsFromINI()));

    connect(nodeView, SIGNAL(view_themeChanged(VIEW_THEME)), this, SLOT(themeChanged(VIEW_THEME)));

    connect(this, SIGNAL(window_ProjectSaved(bool,QString)), nodeView, SIGNAL(view_ProjectSaved(bool, QString)));

    connect(nodeView, SIGNAL(view_LaunchWiki(QString)), this, SLOT(showWiki(QString)));
    connect(nodeView, SIGNAL(view_ProjectFileChanged(QString)), this, SLOT(projectFileChanged(QString)));
    connect(nodeView, SIGNAL(view_ProjectNameChanged(QString)), this, SLOT(projectNameChanged(QString)));

    connect(this, SIGNAL(window_toggleAspect(VIEW_ASPECT,bool)), nodeView, SLOT(toggleAspect(VIEW_ASPECT,bool)));
    connect(this, SIGNAL(window_centerAspect(VIEW_ASPECT)), nodeView, SLOT(centerAspect(VIEW_ASPECT)));
    connect(nodeView, SIGNAL(view_toggleAspect(VIEW_ASPECT, bool)), this, SLOT(forceToggleAspect(VIEW_ASPECT,bool)));
    connect(nodeView, SIGNAL(view_ShowCPPForComponent(QString)), this, SLOT(generateCPPForComponent(QString)));
    connect(nodeView, SIGNAL(view_highlightAspectButton(VIEW_ASPECT)), definitionsToggle, SLOT(highlightToggleButton(VIEW_ASPECT)));
    connect(nodeView, SIGNAL(view_highlightAspectButton(VIEW_ASPECT)), workloadToggle, SLOT(highlightToggleButton(VIEW_ASPECT)));
    connect(nodeView, SIGNAL(view_highlightAspectButton(VIEW_ASPECT)), assemblyToggle, SLOT(highlightToggleButton(VIEW_ASPECT)));
    connect(nodeView, SIGNAL(view_highlightAspectButton(VIEW_ASPECT)), hardwareToggle, SLOT(highlightToggleButton(VIEW_ASPECT)));

    connect(nodeView, SIGNAL(view_RefreshDock()), partsDock, SLOT(updateCurrentNodeItem()));

    connect(nodeView, SIGNAL(view_ProjectRequiresSaving(bool)), this, SLOT(projectRequiresSaving(bool)));
    connect(nodeView, SIGNAL(view_ModelDisconnected()), this, SLOT(modelDisconnected()));

    connect(nodeView, SIGNAL(view_SavedProject(QString,QString)), this, SLOT(gotSaveData(QString, QString)));

    connect(nodeView, SIGNAL(view_OpenHardwareDock()), this, SLOT(jenkinsNodesLoaded()));
    connect(nodeView, SIGNAL(view_ModelReady()), this, SLOT(modelReady()));
    connect(nodeView, SIGNAL(view_ModelDisconnected()), this, SLOT(modelDisconnected()));

    connect(this, SIGNAL(window_ImportSnippet(QString,QString)), nodeView, SLOT(importSnippet(QString,QString)));

    connect(this, SIGNAL(window_DisplayMessage(MESSAGE_TYPE,QString,QString)), nodeView, SLOT(showMessage(MESSAGE_TYPE,QString,QString)));
    connect(nodeView, SIGNAL(view_updateProgressStatus(int,QString)), this, SLOT(updateProgressStatus(int,QString)));

    connect(notificationTimer, SIGNAL(timeout()), notificationsBar, SLOT(hide()));
    connect(notificationTimer, SIGNAL(timeout()), this, SLOT(checkNotificationsQueue()));
    connect(nodeView, SIGNAL(view_displayNotification(QString,int,int)), this, SLOT(displayNotification(QString,int,int)));

    connect(nodeView, SIGNAL(view_updateMenuActionEnabled(QString,bool)), this, SLOT(setActionEnabled(QString,bool)));
    connect(nodeView, SIGNAL(view_SetAttributeModel(AttributeTableModel*)), this, SLOT(setAttributeModel(AttributeTableModel*)));
    connect(nodeView, SIGNAL(view_ProjectCleared()), this, SLOT(projectCleared()));

    connect(file_importSnippet, SIGNAL(triggered()), nodeView, SLOT(request_ImportSnippet()));
    connect(file_exportSnippet, SIGNAL(triggered()), nodeView, SLOT(request_ExportSnippet()));

    connect(nodeView, SIGNAL(view_ImportSnippet(QString)), this, SLOT(importSnippet(QString)));
    connect(nodeView, SIGNAL(view_ExportSnippet(QString)), this, SLOT(exportSnippet(QString)));

    connect(file_importXME, SIGNAL(triggered(bool)), this, SLOT(on_actionImport_XME_triggered()));

    //connect(nodeView, SIGNAL(view_showWindowToolbar()), this, SLOT(showWindowToolbar()));
    connect(toolbarButton, SIGNAL(clicked(bool)), this, SLOT(showWindowToolbar(bool)));

    connect(nodeView, SIGNAL(customContextMenuRequested(QPoint)), nodeView, SLOT(showToolbar(QPoint)));

    //Minimap Funcs
    connect(this, SIGNAL(window_SetViewVisible(bool)), minimap, SLOT(setVisible(bool)));
    connect(nodeView, SIGNAL(view_ViewportRectChanged(QRectF)), minimap, SLOT(viewportRectChanged(QRectF)));
    connect(nodeView, SIGNAL(view_ModelSizeChanged()), minimap, SLOT(centerView()));
    connect(minimap, SIGNAL(minimap_Pan()), nodeView, SLOT(minimapPan()));
    connect(minimap, SIGNAL(minimap_Panning(QPointF)), nodeView, SLOT(minimapPanning(QPointF)));
    connect(minimap, SIGNAL(minimap_Panned()), nodeView, SLOT(minimapPanned()));
    connect(minimap, SIGNAL(minimap_Scrolled(int)), nodeView, SLOT(minimapScrolled(int)));

    connect(projectName, SIGNAL(clicked()), nodeView, SLOT(selectModel()));
    connect(closeProjectButton, SIGNAL(clicked()), this, SLOT(on_actionCloseProject_triggered()));

    connect(file_newProject, SIGNAL(triggered()), this, SLOT(on_actionNew_Project_triggered()));
    connect(file_closeProject, SIGNAL(triggered()), this, SLOT(on_actionCloseProject_triggered()));
    connect(file_openProject, SIGNAL(triggered()), this, SLOT(on_actionOpenProject_triggered()));
    connect(file_saveProject, SIGNAL(triggered(bool)), this, SLOT(on_actionSaveProject_triggered()));
    connect(file_saveAsProject, SIGNAL(triggered()), this, SLOT(on_actionSaveProjectAs_triggered()));
    connect(file_recentProjects_clearHistory, SIGNAL(triggered()), this, SLOT(clearRecentProjectsList()));

    connect(file_importGraphML, SIGNAL(triggered()), this, SLOT(on_actionImport_GraphML_triggered()));
    connect(help_AboutMedea, SIGNAL(triggered()), this, SLOT(aboutMedea()));
    connect(help_AboutQt, SIGNAL(triggered()), this, SLOT(aboutQt()));
    connect(help_ReportBug, SIGNAL(triggered()), this, SLOT(reportBug()));
    connect(help_Shortcuts, SIGNAL(triggered()), this, SLOT(showShortcutList()));
    connect(help_Wiki, SIGNAL(triggered(bool)), this, SLOT(showWiki()));

    connect(this, SIGNAL(window_OpenProject(QString,QString)), nodeView, SIGNAL(view_OpenProject(QString,QString)));
    connect(this, SIGNAL(window_ImportProjects(QStringList)), nodeView, SLOT(importProjects(QStringList)));
    connect(this, SIGNAL(window_ImportJenkinsNodes(QString)), nodeView, SLOT(loadJenkinsNodes(QString)));

    connect(edit_undo, SIGNAL(triggered()), this, SLOT(menuActionTriggered()));
    connect(edit_redo, SIGNAL(triggered()), this, SLOT(menuActionTriggered()));

    connect(edit_undo, SIGNAL(triggered()), nodeView, SLOT(undo()));
    connect(edit_redo, SIGNAL(triggered()), nodeView, SLOT(redo()));
    connect(edit_cut, SIGNAL(triggered()), nodeView, SLOT(cut()));
    connect(edit_copy, SIGNAL(triggered()), nodeView, SLOT(copy()));
    connect(edit_replicate, SIGNAL(triggered()), nodeView, SLOT(replicate()));
    connect(edit_paste, SIGNAL(triggered()), this, SLOT(on_actionPaste_triggered()));
    connect(this, SIGNAL(window_Paste(QString)), nodeView, SLOT(paste(QString)));
    connect(edit_search, SIGNAL(triggered()), this, SLOT(search()));

    connect(view_fitToScreen, SIGNAL(triggered()), nodeView, SLOT(fitToScreen()));
    connect(view_goToImplementation, SIGNAL(triggered()), nodeView, SLOT(centerImplementation()));
    connect(view_goToDefinition, SIGNAL(triggered()), nodeView, SLOT(centerDefinition()));
    connect(view_showConnectedNodes, SIGNAL(triggered()), nodeView, SLOT(showConnectedNodes()));
    connect(view_fullScreenMode, SIGNAL(triggered(bool)), this, SLOT(setFullscreenMode(bool)));
    connect(view_printScreen, SIGNAL(triggered()), this, SLOT(screenshot()));
    connect(view_showMinimap, SIGNAL(triggered()), this, SLOT(toggleMinimap()));

    connect(model_clearModel, SIGNAL(triggered()), nodeView, SLOT(clearModel()));

    connect(model_validateModel, SIGNAL(triggered()), this, SLOT(executeProjectValidation()));
    connect(jenkins_ExecuteJob, SIGNAL(triggered()), this, SLOT(executeJenkinsDeployment()));
    connect(model_ExecuteLocalJob, SIGNAL(triggered()), this, SLOT(executeLocalNodeDeployment()));

    connect(jenkins_ImportNodes, SIGNAL(triggered()), this, SLOT(on_actionImportJenkinsNode()));

    connect(settings_changeAppSettings, SIGNAL(triggered()), appSettings, SLOT(show()));
    connect(actionToggleGrid, SIGNAL(triggered(bool)), nodeView, SLOT(toggleGridLines(bool)));

    connect(exit, SIGNAL(triggered()), this, SLOT(on_actionExit_triggered()));

    connect(nodeView, SIGNAL(view_searchFinished(QStringList)), searchSuggestions, SLOT(showCompletion(QStringList)));

    connect(searchBar, SIGNAL(textEdited(QString)), this, SLOT(updateSearchSuggestions()));
    connect(searchBar, SIGNAL(returnPressed()), this, SLOT(on_actionSearch_triggered()));

    connect(searchDialog, SIGNAL(searchDialog_refresh()), this, SLOT(on_actionSearch_triggered()));

    connect(searchButton, SIGNAL(clicked()), this, SLOT(on_actionSearch_triggered()));
    connect(searchOptionButton, SIGNAL(clicked(bool)), this, SLOT(searchMenuButtonClicked(bool)));
    connect(viewAspectsButton, SIGNAL(clicked(bool)), this, SLOT(searchMenuButtonClicked(bool)));
    connect(nodeKindsButton, SIGNAL(clicked(bool)), this, SLOT(searchMenuButtonClicked(bool)));
    connect(dataKeysButton, SIGNAL(clicked(bool)), this, SLOT(searchMenuButtonClicked(bool)));

    connect(searchOptionMenu, SIGNAL(aboutToHide()), this, SLOT(searchMenuClosed()));
    connect(viewAspectsMenu, SIGNAL(aboutToHide()), this, SLOT(searchMenuClosed()));
    connect(nodeKindsMenu, SIGNAL(aboutToHide()), this, SLOT(searchMenuClosed()));
    connect(dataKeysMenu, SIGNAL(aboutToHide()), this, SLOT(searchMenuClosed()));

    connect(edit_delete, SIGNAL(triggered()), nodeView, SLOT(deleteSelection()));
    connect(actionFitToScreen, SIGNAL(triggered()), nodeView, SLOT(fitToScreen()));
    connect(actionCenter, SIGNAL(triggered()), nodeView, SLOT(centerItem()));
    //connect(actionCenter, SIGNAL(triggered()), nodeView, SLOT(centerOnItem()));
    //connect(actionZoomToFit, SIGNAL(triggered()), nodeView, SLOT(centerItem()));
    connect(actionSort, SIGNAL(triggered()), nodeView, SLOT(sort()));
    connect(actionToggleGrid, SIGNAL(triggered()), this, SLOT(toggleGridLines()));
    connect(actionPopupSubview, SIGNAL(triggered()), nodeView, SLOT(constructNewView()));
    connect(actionAlignHorizontally, SIGNAL(triggered()), nodeView, SLOT(alignSelectionHorizontally()));
    connect(actionAlignVertically, SIGNAL(triggered()), nodeView, SLOT(alignSelectionVertically()));
    connect(actionContextMenu, SIGNAL(triggered()), nodeView, SLOT(showToolbar()));
    connect(actionBack, SIGNAL(triggered()), nodeView, SLOT(moveViewBack()));
    connect(actionForward, SIGNAL(triggered()), nodeView, SLOT(moveViewForward()));

    connect(partsDock, SIGNAL(dock_forceOpenDock(QString)), definitionsDock, SLOT(forceOpenDock(QString)));
    connect(partsDock, SIGNAL(dock_forceOpenDock()), functionsDock, SLOT(forceOpenDock()));
    connect(definitionsDock, SIGNAL(dock_forceOpenDock()), partsDock, SLOT(forceOpenDock()));
    connect(functionsDock, SIGNAL(dock_forceOpenDock()), partsDock, SLOT(forceOpenDock()));

    connect(partsDock, SIGNAL(dock_toggled(bool,QString)), this, SLOT(dockToggled(bool,QString)));
    connect(definitionsDock, SIGNAL(dock_toggled(bool,QString)), this, SLOT(dockToggled(bool,QString)));
    connect(functionsDock, SIGNAL(dock_toggled(bool,QString)), this, SLOT(dockToggled(bool,QString)));
    connect(hardwareDock, SIGNAL(dock_toggled(bool,QString)), this, SLOT(dockToggled(bool,QString)));

    connect(nodeView, SIGNAL(view_SetClipboardBuffer(QString)), this, SLOT(setClipboard(QString)));

    connect(dataTable, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(dataTableDoubleClicked(QModelIndex)));

    //For mac
    addAction(exit);
    addAction(file_newProject);
    addAction(file_openProject);
    addAction(file_importGraphML);

    addAction(edit_undo);
    addAction(edit_redo);
    addAction(edit_cut);
    addAction(edit_copy);
    addAction(edit_paste);
    addAction(edit_replicate);
    addAction(view_fitToScreen);
    addAction(view_goToDefinition);
    addAction(view_goToImplementation);
    addAction(view_showConnectedNodes);

    addAction(model_validateModel);
    addAction(model_clearModel);

    addAction(actionSort);
    addAction(actionCenter);
    addAction(actionZoomToFit);
    addAction(actionFitToScreen);
    addAction(actionAlignVertically);
    addAction(actionAlignHorizontally);
    addAction(actionPopupSubview);
    addAction(actionBack);
    addAction(actionForward);
    addAction(actionToggleGrid);
    addAction(actionContextMenu);

    addAction(jenkins_ExecuteJob);
    addAction(jenkins_ImportNodes);

    addAction(actionToggleGrid);
    addAction(settings_changeAppSettings);
    addAction(help_AboutMedea);
    addAction(help_Shortcuts);
}


/**
 * @brief MedeaWindow::resizeEvent
 * This is called when the main window is resized.
 * It doesn't however, always pick up on maximise/un-maximise events.
 * @param event
 */
void MedeaWindow::resizeEvent(QResizeEvent *event)
{
    if(IS_WINDOW_MAXIMIZED != isMaximized() && maximizedSettingInitiallyChanged){
        maximizedSettingInitiallyChanged = false;
    }
    IS_WINDOW_MAXIMIZED = isMaximized();
    updateWidgetsOnWindowChange();

    QWidget::resizeEvent(event);
}



/**
 * @brief MedeaWindow::changeEvent
 * This is called when certain things about the window has changed.
 * Mainly only using it to catch maximise/un-maximise events.
 * @param event
 */
void MedeaWindow::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    if (event->type() == QEvent::WindowStateChange){
        updateWidgetsOnWindowChange();
    }
}

QPixmap MedeaWindow::getDialogPixmap(QString alias, QString image, QSize size)
{
    return getIcon(alias, image).pixmap(size);
}

bool MedeaWindow::openProject(QString fileName)
{
    bool closed = closeProject();

    if(closed){
        newProject();
    }else{
        return false;
    }

    QString fileData = readFile(fileName);
    if(!fileData.isEmpty()){
        nodeView->openProject(fileName, fileData);
        updateRecentProjectsWidgets(fileName);
    }else{
        return false;
    }
    return true;
}

QRect MedeaWindow::getCanvasRect()
{
    QRect canvasRect;
    canvasRect.setHeight(height()-1);
    canvasRect.setWidth(width() - (docksArea->width() + RIGHT_PANEL_WIDTH + 35 ));
    canvasRect.moveTopLeft(QPoint(docksArea->width() + 15, 0));

    return canvasRect;
}

QString MedeaWindow::getTimestamp()
{
    QDateTime currentTime = QDateTime::currentDateTime();
    return currentTime.toString("yyMMdd-hhmmss");
}

QString MedeaWindow::getTempFileName(QString suffix)
{
    if(suffix == ""){
        suffix = ".graphml";
    }
    //Get Timestamp
    return QDir::tempPath() + "/" + getTempTimeName() + suffix;
}

QString MedeaWindow::getTempTimeName()
{
    return getTimestamp() + "-" + projectName->text();
}

bool MedeaWindow::closeProject()
{
    if(nodeView->projectRequiresSaving()){
        //Ask User to confirm save?
        QMessageBox msgBox(QMessageBox::Question, "Save Changes",
                           "Do you want to save the changes made to '" + currentProjectFilePath +"' ?",
                           QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

        msgBox.setIconPixmap(getDialogPixmap("Actions", "Save"));
        msgBox.setButtonText(QMessageBox::Yes, "Save");
        msgBox.setButtonText(QMessageBox::No, "Ignore Changes");

        int buttonPressed = msgBox.exec();

        if(buttonPressed == QMessageBox::Yes){
            bool saveSuccess = saveProject();
            // if failed to save, do nothing
            if(!saveSuccess){
                return false;
            }
        }else if(buttonPressed == QMessageBox::No){
            //Do Nothing
        }else{
            return false;
        }
    }
    teardownProject();

    toggleWelcomeScreen(true);

    return true;
}

bool MedeaWindow::saveProject(bool saveAs)
{
    if(nodeView){
        QString filePath = nodeView->getProjectFileName();

        if(filePath == ""){
            saveAs = true;
        }

        if(saveAs){
            QStringList files = fileSelector("Select a *.graphml file to save project as.", GRAPHML_FILE_EXT, GRAPHML_FILE_SUFFIX, false, false, filePath);

            if(files.size() != 1){
                return false;
            }

            filePath = files.first();
        }

        QString fileData = nodeView->getProjectAsGraphML();

        bool saveSuccess = writeFile(filePath, fileData);

        if(saveSuccess){
            updateRecentProjectsWidgets(filePath);
        }

        emit window_ProjectSaved(saveSuccess, filePath);
        return saveSuccess;
    }
    return false;
}

void MedeaWindow::populateDocks()
{
    // TODO - The following setup only needs to happen once the whole time the application is open
    // It doesn't need to be redone every time new project is called
    QStringList allKinds = nodeView->getAllNodeKinds();
    QStringList guiKinds = nodeView->getGUIConstructableNodeKinds();
    QList<QPair<QString, QString> > functionKinds;
    functionKinds = nodeView->getFunctionsList();

    partsDock->addDockNodeItems(guiKinds);
    functionsDock->addDockNodeItems(functionKinds);

    // populate view aspects menu once the nodeView and controller have been
    // constructed and connected - should only need to do this once
    allKinds.sort();
    foreach (QString kind, allKinds) {
        QWidgetAction* action = new QWidgetAction(this);
        QCheckBox* checkBox = new QCheckBox(kind, this);
        checkBox->setFont(guiFont);
        connect(checkBox, SIGNAL(clicked()), this, SLOT(updateSearchLineEdits()));
        action->setDefaultWidget(checkBox);
        nodeKindsMenu->addAction(action);
    }
}


bool MedeaWindow::canFilesBeDragImported(QList<QUrl> files)
{
    foreach (const QUrl &url, files){
        QFileInfo fileInfo(url.toLocalFile());
        if(fileInfo.isFile()){
            if(fileInfo.fileName().endsWith(".graphml", Qt::CaseInsensitive)){
                //Only Accept *.graphml files
                continue;
            }
        }
        return false;
    }
    return true;
}


void MedeaWindow::setupApplication()
{
    //Allow Drops
    setAcceptDrops(true);

    //Set QApplication information.
    QApplication::setApplicationName("MEDEA");
    QApplication::setApplicationVersion(APP_VERSION);
    QApplication::setOrganizationName("Defence Information Group");
    QApplication::setOrganizationDomain("http://blogs.adelaide.edu.au/dig/");
    QApplication::setWindowIcon(QIcon(":/Actions/MEDEA.png"));

    // this needs to happen before the menu is set up and connected
    applicationDirectory = QApplication::applicationDirPath() + "/";
    MEDEA_VERSION = QApplication::applicationVersion();


    //load the persistantSettings from the state.ini file to load in the history.
    persistantSettings = new QSettings(applicationDirectory + "/Resources/state.ini", QSettings::IniFormat);
    QVariant projectHistory = persistantSettings->value("projectHistory");

    if(!projectHistory.isNull()){
        QStringList history = projectHistory.toStringList();
        while(!history.isEmpty()){
            QString fileName = history.takeLast();
            if(!fileName.isEmpty()){
                //update the stack.
                recentProjectsList.push(fileName);
            }
        }
    }


    projectFileChanged();

    //Set Font.
    int fontID = QFontDatabase::addApplicationFont(":/Resources/Fonts/OpenSans-Regular.ttf");
    QString fontName = QFontDatabase::applicationFontFamilies(fontID).at(0);
    QFont font = QFont(fontName);
    font.setPointSizeF(8.5);
    QApplication::setFont(font);
}

/**
 * @brief MedeaWindow::initialiseJenkinsManager
 */
void MedeaWindow::initialiseJenkinsManager()
{
    jenkinsManager = 0;

    QString jenkinsUrl = appSettings->getSetting(JENKINS_URL).toString();
    QString jenkinsUser = appSettings->getSetting(JENKINS_USER).toString();
    QString jenkinsPass = appSettings->getSetting(JENKINS_PASS).toString();
    QString jenkinsToken = appSettings->getSetting(JENKINS_TOKEN).toString();

    if(jenkinsUrl != "" && jenkinsUser != "" && jenkinsPass != ""){
        QString binaryPath = applicationDirectory + "Resources/Binaries/";


        jenkinsManager = new JenkinsManager(binaryPath, jenkinsUrl, jenkinsUser, jenkinsPass, jenkinsToken);
        connect(jenkinsManager, SIGNAL(gotInvalidSettings(QString)), this, SLOT(invalidJenkinsSettings(QString)));
    }
}

void MedeaWindow::initialiseSettings()
{
    appSettings = new AppSettings(this, applicationDirectory);
    appSettings->setModal(true);
    connect(appSettings, SIGNAL(settingChanged(QString,QString,QVariant)), this, SLOT(settingChanged(QString, QString, QVariant)));
}


/**
 * @brief MedeaWindow::initialiseCUTSManager
 */
void MedeaWindow::initialiseCUTSManager()
{
    cutsManager = 0;

    QString xalanJPath = applicationDirectory + "/Resources/Binaries/";
    QString transformPath = applicationDirectory + "/Resources/Transforms/";
    QString scriptsPath = applicationDirectory + "/Resources/Scripts/";

    //Try Parse Thread limit
    bool isInt;
    int threadLimit = appSettings->getSetting(THREAD_LIMIT).toInt(&isInt);
    if(!isInt){
        threadLimit = 4;
    }

    //Construct and start a new QThread
    QThread* thread = new QThread();
    thread->start();

    connect(this, SIGNAL(destroyed()), thread, SLOT(quit()));

    cutsManager = new CUTSManager();

    cutsManager->setXalanJPath(xalanJPath);
    cutsManager->setXSLTransformPath(transformPath);
    cutsManager->setMaxThreadCount(threadLimit);
    cutsManager->setScriptsPath(scriptsPath);
    cutsManager->moveToThread(thread);
    connect(cutsManager, SIGNAL(localDeploymentOkay()), this, SLOT(localDeploymentOkay()));
    connect(this, SIGNAL(window_GetCPPForComponent(QString,QString)), cutsManager, SLOT(getCPPForComponent(QString,QString)));
    connect(this, SIGNAL(window_ExecuteXSLValidation(QString, QString)), cutsManager, SLOT(executeXSLValidation(QString,QString)));
    connect(cutsManager, SIGNAL(gotCPPForComponent(bool, QString, QString,QString)), this, SLOT(gotCPPForComponent(bool,QString,  QString, QString)));
    connect(cutsManager, SIGNAL(gotXMETransform(bool,QString, QString)), this, SLOT(gotXMETransform(bool, QString, QString)));
    connect(cutsManager, SIGNAL(executedXSLValidation(bool,QString)), this, SLOT(XSLValidationCompleted(bool,QString)));
}


/**
 * @brief MedeaWindow::importXMEProject
 * @param filePath
 */
void MedeaWindow::importXMEProject(QString filePath)
{
    if(cutsManager){
        QFile file(filePath);
        QFileInfo fileInfo = QFileInfo(file);
        QString fileName =  fileInfo.baseName();
        QString outputPath = QDir::tempPath() + "/" + fileName + ".graphml";

        connect(this, SIGNAL(window_ExecuteXMETransform(QString,QString)), cutsManager, SLOT(executeXMETransform(QString,QString)));
        emit window_ExecuteXMETransform(filePath, outputPath);
        disconnect(this, SIGNAL(window_ExecuteXMETransform(QString,QString)), cutsManager, SLOT(executeXMETransform(QString,QString)));
        setEnabled(false);
        updateProgressStatus(-1, "Transforming XME to GraphML");
    }
}




void MedeaWindow::projectFileChanged(QString name)
{
    if(name != ""){
        currentProjectFilePath = name;
        name = " - " + name + "[*]";
    }
    setWindowTitle("MEDEA" + name);

}


/**
 * @brief MedeaWindow::projectNameChanged
 * @param name
 */
void MedeaWindow::projectNameChanged(QString name)
{
    if (projectName && !name.isEmpty()) {
        projectName->setText(name);
        projectName->setToolTip(name);
        projectName->setFixedWidth(projectName->fontMetrics().width(name) + 10);
        updateWidgetsOnProjectChange();
    }
}

void MedeaWindow::gotSaveData(QString filePath, QString fileData)
{
    //Try and write the file.
    bool success = writeFile(filePath, fileData);

    //Tell the View the project saved.
    emit window_ProjectSaved(success, filePath);
}

void MedeaWindow::setFullscreenMode(bool fullscreen)
{
    if (fullscreen) {
        // need to update this here
        WINDOW_MAXIMIZED = isMaximized();
        showFullScreen();
        view_fullScreenMode->setText("Exit Fullscreen Mode");
        view_fullScreenMode->setChecked(true);
        view_fullScreenMode->setIcon(nodeView->getImage("Actions", "Failure"));
    } else {
        if (!SETTINGS_LOADING) {
            if (WINDOW_MAXIMIZED) {
                showMaximized();
            } else {
                showNormal();
            }
        }
        view_fullScreenMode->setText("Set Fullscreen Mode");
        view_fullScreenMode->setChecked(false);
        view_fullScreenMode->setIcon(nodeView->getImage("Actions", "Fullscreen"));
    }
}

void MedeaWindow::gotXMETransform(bool success, QString errorString, QString path)
{
    displayLoadingStatus(false);
    setEnabled(true);
    updateProgressStatus(0,"");
    if(!success){
        QMessageBox::critical(this, "XME Import Error", errorString, QMessageBox::Ok);

    }else{
        QStringList projects;
        projects << path;
        emit importProjects(projects);
    }
}

void MedeaWindow::gotCPPForComponent(bool success, QString errorString, QString componentName, QString code)
{
    setEnabled(true);
    displayLoadingStatus(false);
    if(!success){
        QMessageBox::critical(this, "XSL Transformation for CPP Error", errorString, QMessageBox::Ok);
    }else{
        popupMultiLine->setWindowTitle(componentName + "Impl.cpp");
        txtMultiLine->setPlainText(code);
        popupMultiLine->show();
    }
}


/**
 * @brief MedeaWindow::localDeploymentOkay
 */
void MedeaWindow::localDeploymentOkay()
{
    if(model_ExecuteLocalJob){
        model_ExecuteLocalJob->setEnabled(true);
        model_ExecuteLocalJob->setToolTip("");
    }
}


/**
 * @brief MedeaWindow::toggleGridLines
 */
void MedeaWindow::toggleGridLines()
{
    if(actionToggleGrid){
        if(actionToggleGrid->isChecked()){
            actionToggleGrid->setIcon(nodeView->getImage("Actions", "Grid_On"));
            //actionToggleGrid->setIcon(getIcon("Actions", "Grid_On"));
            actionToggleGrid->setToolTip("Turn Off Grid");
        }else{
            actionToggleGrid->setIcon(nodeView->getImage("Actions", "Grid_Off"));
            //actionToggleGrid->setIcon(getIcon("Actions", "Grid_Off"));
            actionToggleGrid->setToolTip("Turn On Grid");
        }
    }
}


/**
 * @brief MedeaWindow::aboutMedea Constructs the about MEDEA Dialog
 */
void MedeaWindow::aboutMedea()
{
    QString aboutString;
    aboutString += "<b>MEDEA " + MEDEA_VERSION + "</b><hr />";
    aboutString += "Defence Information Group<br />";
    aboutString += "<i>The University of Adelaide</i><br /><br />";
    aboutString += "Developers:";
    aboutString += "<ul>";
    aboutString += "<li>Dan Fraser</li>";
    aboutString += "<li>Cathlyn Aston</li>";
    aboutString += "<li>Marianne Rieckmann</li>";
    aboutString += "<li>Matthew Hart</li>";
    aboutString += "</ul>";
    aboutString += "<a href=\"";
    aboutString += GITHUB_URL;
    aboutString += "\">MEDEA GitHub</a>";
    QMessageBox::about(this, "About MEDEA", aboutString);
}


/**
 * @brief MedeaWindow::aboutQt
 */
void MedeaWindow::aboutQt()
{
    QMessageBox::aboutQt(this);
}

void MedeaWindow::reportBug()
{
    QString URL = GITHUB_URL;
    URL += "issues/";
    QDesktopServices::openUrl(QUrl(URL));
}

void MedeaWindow::showWiki(QString componentName)
{
    QString URL = GITHUB_URL;
    URL += "wiki/";
    if(componentName != ""){
        URL += "SEM-MEDEA-ModelEntities#" + componentName;
    }
    QDesktopServices::openUrl(QUrl(URL));
}


/**
 * @brief MedeaWindow::showShortcutList
 */
void MedeaWindow::showShortcutList()
{
    ShortcutDialog* shortcutDialog = new ShortcutDialog(this);
    shortcutDialog->exec();
}


/**
 * @brief MedeaWindow::invalidJenkinsSettings
 * @param message
 */
void MedeaWindow::invalidJenkinsSettings(QString message)
{
    emit window_DisplayMessage(CRITICAL, "Invalid Jenkins Settings", message);
}


/**
 * @brief MedeaWindow::jenkinsNodesLoaded
 */
void MedeaWindow::jenkinsNodesLoaded()
{
    // if the hardware dock isn't already open, open it
    if (hardwareNodesButton->isEnabled() && !hardwareNodesButton->isSelected()) {
        hardwareNodesButton->pressed();
    }
}


/**
 * @brief MedeaWindow::toggleWelcomeScreen
 * @param show
 */
void MedeaWindow::toggleWelcomeScreen(bool show)
{
    if (welcomeScreenOn == show) {
        return;
    }

    QVBoxLayout* fromLayout;
    QVBoxLayout* toLayout;

    if (show) {
        fromLayout = holderLayout;
        toLayout = viewHolderLayout;
    } else {
        fromLayout = viewHolderLayout;
        toLayout = holderLayout;
    }

    fromLayout->removeItem(welcomeLayout);
    toLayout->addLayout(welcomeLayout);

    toLayout->removeItem(viewLayout);
    fromLayout->addLayout(viewLayout);

    welcomeScreenOn = show;

    if (show) {
        setToolbarVisibility(false);
    } else {
        setToolbarVisibility(SHOW_TOOLBAR);
    }
}


/**
 * @brief MedeaWindow::toggleAndTriggerAction
 * @param action
 * @param value
 */
void MedeaWindow::toggleAndTriggerAction(QAction *action, bool value)
{
    action->setChecked(value);
    action->triggered(value);
}


/**
 * @brief MedeaWindow::updateWidgetsOnWindowChange
 * This is called when the GUI widgets size/pos need to be
 * updated after the window has been changed.
 */
void MedeaWindow::updateWidgetsOnWindowChange()
{   
    QRect canvasRect;
    canvasRect.setHeight(height()-1);
    canvasRect.setWidth(width() - (docksArea->width() + RIGHT_PANEL_WIDTH + 35 ));
    canvasRect.moveTopLeft(QPoint(docksArea->width() + 15, 0));

    /*
    double newHeight = docksArea->height();
    if (dockStandAloneDialog->isVisible()) {
        newHeight *= 2;
    }
    partsDock->parentHeightChanged(newHeight);
    definitionsDock->parentHeightChanged(newHeight);
    hardwareDock->parentHeightChanged(newHeight);
    */

    // update the stored view center point and re-center the view
    if (nodeView) {
        nodeView->visibleViewRectChanged(getCanvasRect());
        nodeView->updateViewCenterPoint();
        nodeView->recenterView();
        nodeView->aspectGraphicsChanged();
    }


    updateWidgetMask(docksArea, dockButtonsBox, true);
    updateDock();
    updateToolbar();
    updateDataTable();
}


/**
 * @brief MedeaWindow::updateWidgetsOnProjectChange
 * @param projectActive
 */
void MedeaWindow::updateWidgetsOnProjectChange(bool projectActive)
{
    if (menuTitleBox) {
        // 55 is the width of the menu button
        int totalWidth = 55;
        if (projectActive) {
            totalWidth += SPACER_SIZE;
            totalWidth += closeProjectButton->width();
            totalWidth += projectName->width();
        }
        menuTitleBox->setFixedWidth(totalWidth);
    }
}


/**
 * @brief MedeaWindow::updateDock
 * This recalculates the size of the area that's available for the dock.
 */
void MedeaWindow::updateDock()
{
    // update widget sizes and mask
    boxHeight = height() - menuTitleBox->height() - dockButtonsBox->height();
    int prevHeight = docksArea->height();
    int newHeight = (boxHeight*2) - dockHeaderBox->height() - (SPACER_SIZE*3);
    if (newHeight != prevHeight) {
        docksArea->setFixedHeight((boxHeight*2) - dockHeaderBox->height() - (SPACER_SIZE*3));
    }
    //dockStandAloneDialog->setFixedHeight(boxHeight + dockButtonsBox->height() + SPACER_SIZE/2);
}


/**
 * @brief MedeaWindow::updateToolbar
 * This re-centralises the toolbar and the toolbar button.
 * It also recalculates the toolbar's size based on the NodeView's visibleViewRect.
 */
void MedeaWindow::updateToolbar()
{
    int totalWidth = 0;
    foreach (QAction* action, toolbar->actions()) {
        if (action->isVisible()) {
            QString actionName = toolbarActionLookup.key(action);
            if (!actionName.isEmpty()) {
                totalWidth += toolbarButtonLookup[actionName]->width();
            } else {
                // if actionName is empty, it means that it's a sepator - 8 is the width of the separator
                totalWidth += 8;
            }
        }
    }

    QSize toolbarSize = QSize(totalWidth, TOOLBAR_BUTTON_HEIGHT);
    toolbar->setFixedSize(toolbarSize + QSize(5, TOOLBAR_GAP));

    if (nodeView) {
        int centerX = nodeView->getVisibleViewRect().center().x();
        toolbarButton->move(centerX  - (TOOLBAR_BUTTON_WIDTH / 2) + 3, TOOLBAR_GAP);
        toolbar->move(centerX - (toolbar->width() / 2), 2 * TOOLBAR_GAP + (TOOLBAR_BUTTON_HEIGHT / 2));
    }
}


/**
 * @brief MedeaWindow::setupInitialSettings
 * This force sorts and centers the definitions containers before they are hidden.
 * It also sets the default values for toggle menu actions and populates the parts
 * dock and search option menu. This is only called once.
 */
void MedeaWindow::setupInitialSettings()
{
    loadSettingsFromINI();

    // calculate the centered view rect and widget masks after the settings has been loaded
    updateWidgetsOnWindowChange();
}




/**
 * @brief MedeaWindow::jenkinsExport
 */
void MedeaWindow::executeJenkinsDeployment()
{
    QString exportFile = writeProjectToTempFile();

    if(exportFile.isEmpty()){
        return;
    }

    QString jobName = "";
    if(appSettings){
        jobName = appSettings->getSetting(JENKINS_JOB).toString();
    }

    if(jenkinsManager){
        JenkinsStartJobWidget* jenkinsSJ = new JenkinsStartJobWidget(this, jenkinsManager);
        jenkinsSJ->requestJob(jobName, exportFile);
    }
}

void MedeaWindow::recentProjectItemClicked(QListWidgetItem *item)
{
    if(item){
        //Open the project with the text from the item.
        openProject(item->text());
    }
}

void MedeaWindow::recentProjectMenuActionClicked()
{
    QAction* action = dynamic_cast<QAction*>(QObject::sender());
    if(action){
        QString fileName = action->text();
        if(!fileName.isEmpty()){
            openProject(fileName);
        }
    }
}

void MedeaWindow::screenshot()
{
    if(appSettings){
        QString screenshotPath = appSettings->getSetting(SCREENSHOT_PATH).toString();
        int screenshotQuality = appSettings->getSetting(SCREENSHOT_QUALITY).toInt();

        QMessageBox msgBox(
                    QMessageBox::Question,"MEDEA",
                    "Please select which type of screenshot to save.",
                    QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

        msgBox.setIconPixmap(getDialogPixmap("Actions", "PrintScreen"));
        msgBox.setButtonText(QMessageBox::Yes, "Entire Model");
        msgBox.setButtonText(QMessageBox::No, "Current Viewport");

        displayLoadingStatus(true, "Rendering screenshot");

        int buttonPressed = msgBox.exec();
        //Hide!
        msgBox.hide();

        if(buttonPressed == QMessageBox::Cancel){
            displayLoadingStatus(false);
            return;
        }
        bool currentViewPort = buttonPressed == QMessageBox::No;




        QImage image = nodeView->renderScreenshot(currentViewPort, screenshotQuality);
        if(!image.isNull()){
            if(!screenshotPath.endsWith("/")){
                screenshotPath += "/";
            }
            qCritical() << screenshotPath;
            QString fileName = screenshotPath + getTempTimeName() + ".png";
            writeQImage(fileName, image, true);
        }
        displayLoadingStatus(false);
    }
}

void MedeaWindow::XSLValidationCompleted(bool success, QString reportPath)
{
    displayLoadingStatus(false);
    if(success){
        QFile xmlFile(reportPath);

        if (!xmlFile.exists() || !xmlFile.open(QIODevice::ReadOnly)){
            displayNotification("XSL validation failed to produce a report.");
            return;
        }

        QXmlQuery query;
        query.bindVariable("graphmlFile", &xmlFile);
        const QString queryMessages = QString("declare namespace svrl = \"http://purl.oclc.org/dsdl/svrl\"; doc('file:///%1')//svrl:schematron-output/svrl:failed-assert/string()").arg(xmlFile.fileName());
        query.setQuery(queryMessages);

        QStringList messagesResult;
        bool result = query.evaluateTo(&messagesResult);
        xmlFile.close();

        if(!result){
            displayNotification("Cannot run QXmlQuery on validation report.");
        }else{
            validateResults.setupItemsTable(messagesResult);
            validateResults.show();
        }
    }else{
        displayNotification("XSL validation failed!");
    }
}

void MedeaWindow::generateCPPForComponent(QString componentName)
{
    displayLoadingStatus(true, "Getting CPP for ComponentImpl");
    QString exportFile = writeProjectToTempFile();
    if(exportFile.isEmpty()){
        return;
    }

    emit window_GetCPPForComponent(exportFile, componentName);
}


void MedeaWindow::executeProjectValidation()
{
    QString exportFile = writeProjectToTempFile();

    if(exportFile.isEmpty()){
        return;
    }

    displayLoadingStatus(true, "Validating Model");

    QString reportPath = getTempFileName("_report.xml");
    emit window_ExecuteXSLValidation(exportFile, reportPath);
}

void MedeaWindow::executeLocalNodeDeployment()
{
    QString exportFile = writeProjectToTempFile();

    if(exportFile.isEmpty()){
        return;
    }

    if(cutsManager){
        QString path = "";
        if(appSettings){
            path = appSettings->getSetting(DEFAULT_DIR_PATH).toString();
        }

        CUTSExecutionWidget* cWidget = new CUTSExecutionWidget(this, cutsManager);
        cWidget->setGraphMLPath(exportFile);
        cWidget->setOutputPath(path);
        cWidget->show();
    }
}


/**
 * @brief MedeaWindow::saveSettings
 */
void MedeaWindow::saveSettings()
{
    //Write Settings on Quit.
    if(appSettings && SAVE_WINDOW_SETTINGS){
        appSettings->setSetting(TOOLBAR_EXPANDED, toolbarButton->isChecked());

        appSettings->setSetting(WINDOW_MAX_STATE, isMaximized());
        appSettings->setSetting(WINDOW_FULL_SCREEN, isFullScreen());

        if(!isMaximized() && !isFullScreen()){
            appSettings->setSetting(WINDOW_W, size().width());
            appSettings->setSetting(WINDOW_H, size().height());
            appSettings->setSetting(WINDOW_X, pos().x());
            appSettings->setSetting(WINDOW_Y, pos().y());
        }
        appSettings->setSetting(DEFAULT_DIR_PATH, DEFAULT_PATH);
    }

    if(persistantSettings){
        QStringList historicList;

        while(!recentProjectsList.isEmpty()){
            historicList.append(recentProjectsList.takeLast());
        }
        //Write the projectHistory to disk
        persistantSettings->setValue("projectHistory", historicList);
        delete persistantSettings;
    }
}


/**
 * @brief MedeaWindow::search
 */
void MedeaWindow::search()
{
    if(searchBar){
        searchBar->setFocus();
        searchBar->selectAll();
    }
}


/**
 * @brief MedeaWindow::gotJenkinsNodeGraphML Called by a JenkinsRequest with the GraphML string which represents the nodes in the Jenkins Server. Data gets imported into the model.
 * @param jenkinsXML The GraphML representation of the Jenkins Nodes List.
 */
void MedeaWindow::gotJenkinsNodeGraphML(QString jenkinsXML)
{
    displayLoadingStatus(false);
    if(jenkinsXML != ""){
        // import Jenkins
        emit window_ImportJenkinsNodes(jenkinsXML);
    }else{
        QMessageBox::critical(this, "Jenkins Error", "Unable to request Jenkins Data", QMessageBox::Ok);
    }
}


/**
 * @brief MedeaWindow::setImportJenkinsNodeEnabled
 * @param enabled
 */
void MedeaWindow::setImportJenkinsNodeEnabled(bool enabled)
{
    if(jenkins_ImportNodes){
        jenkins_ImportNodes->setEnabled(enabled);
    }
}


/**
 * @brief MedeaWindow::on_actionImportJenkinsNode
 */
void MedeaWindow::on_actionImportJenkinsNode()
{
    progressAction = "Importing Jenkins";

    if(jenkinsManager){
        displayLoadingStatus(true, "Import Jenkins Nodes");
        QString groovyScript = applicationDirectory + "Resources/Scripts/Jenkins_Construct_GraphMLNodesList.groovy";

        JenkinsRequest* jenkinsGS = jenkinsManager->getJenkinsRequest(this);
        connect(this, SIGNAL(jenkins_RunGroovyScript(QString)), jenkinsGS, SLOT(runGroovyScript(QString)));
        connect(jenkinsGS, SIGNAL(gotGroovyScriptOutput(QString)), this, SLOT(gotJenkinsNodeGraphML(QString)));
        connect(jenkinsGS, SIGNAL(requestFinished()), this, SLOT(setImportJenkinsNodeEnabled()));

        //Disable the Jenkins Menu Button
        setImportJenkinsNodeEnabled(false);

        emit jenkins_RunGroovyScript(groovyScript);
        disconnect(this, SIGNAL(jenkins_RunGroovyScript(QString)), jenkinsGS, SLOT(runGroovyScript(QString)));
    }
}


/**
 * @brief MedeaWindow::on_actionNew_Project_triggered
 * At the moment it olnly allows one project to be opened at a time.
 */
void MedeaWindow::on_actionNew_Project_triggered()
{
    // ask user if they want to save current project before closing it
    bool closed = closeProject();

    if(closed){
        newProject();
    }
}

void MedeaWindow::on_actionCloseProject_triggered()
{
    //Close the current Project.
    closeProject();


}

void MedeaWindow::on_actionOpenProject_triggered()
{
    QString filePath;
    if(nodeView){
        filePath = nodeView->getProjectFileName();
    }
    QStringList fileNames = fileSelector("Select Project to Open", GRAPHML_FILE_EXT, GRAPHML_FILE_SUFFIX, true, false, filePath);

    if(fileNames.size() == 1){
        openProject(fileNames.first());
    }
}

void MedeaWindow::on_actionSaveProject_triggered()
{
    saveProject();
}

void MedeaWindow::on_actionSaveProjectAs_triggered()
{
    saveProject(true);
}



/**
 * @brief MedeaWindow::on_actionImport_GraphML_triggered
 */
void MedeaWindow::on_actionImport_GraphML_triggered()
{
    progressAction = "Importing GraphML";

    importProjects(fileSelector("Select one or more files to import.", GRAPHML_FILE_EXT, GRAPHML_FILE_SUFFIX, true));
}

void MedeaWindow::on_actionImport_XME_triggered()
{
    progressAction = "Importing XME";

    QStringList files = fileSelector("Select an XME file to import.", GME_FILE_EXT, GME_FILE_SUFFIX, true, false);
    if(files.size() == 1){
        displayLoadingStatus(true, "Transforming XME for import");
        importXMEProject(files.first());
    }
}



/**
 * @brief MedeaWindow::on_actionPaste_triggered
 */
void MedeaWindow::on_actionPaste_triggered()
{
    progressAction = "Pasting Data";

    QClipboard *clipboard = QApplication::clipboard();
    window_Paste(clipboard->text());
}


/**
 * @brief MedeaWindow::on_actionSearch_triggered
 * This is called when the search button is clicked.
 * It pops up a dialog listing the items in the search results,
 * or an information message if there are none.
 */
void MedeaWindow::on_actionSearch_triggered()
{
    QString searchText = searchBar->text();

    if (searchText.isEmpty()) {
        return;
    }

    if (nodeView) {

        QStringList checkedAspects = getCheckedItems(SEARCH_VIEW_ASPECTS);
        QStringList checkedKinds = getCheckedItems(SEARCH_NODE_KINDS);
        QStringList checkedKeys = getCheckedItems(SEARCH_DATA_KEYS);
        QList<GraphMLItem*> searchResultItems = nodeView->search(searchText, checkedAspects, checkedKinds, checkedKeys);

        bool newSearch = searchDialog->addSearchItems(searchResultItems);
        if (newSearch) {
            searchDialog->updateHedearLabels(searchText.trimmed(), checkedAspects, checkedKinds);
        }

        searchDialog->show();
    }
}


/**
 * @brief MedeaWindow::on_actionExit_triggered
 * This is called when the menu's exit action is triggered.
 * It closes the application.
 */
void MedeaWindow::on_actionExit_triggered()
{
    close();
}


/**
 * @brief MedeaWindow::on_searchResultItem_clicked
 * This is called when an item (button) from the search result list is clicked.
 * It tells the view to center on the clicked item.
 */
void MedeaWindow::on_searchResultItem_clicked(int ID)
{
    nodeView->selectAndCenterItem(ID);
}


/**
 * @brief MedeaWindow::on_validationItem_clicked
 * @param ID
 */
void MedeaWindow::on_validationItem_clicked(int ID)
{
    nodeView->selectAndCenterItem(ID);
}



/**
 * @brief MedeaWindow::importSnippet
 * @param parentName
 */
void MedeaWindow::importSnippet(QString snippetType)
{
    if(!nodeView){
        return;
    }

    if(snippetType == ""){
        //Check if we have any importable snippet types.
        snippetType = nodeView->getImportableSnippetKind();
    }

    if(snippetType == ""){
        return;
    }

    QStringList files = fileSelector("Import " + snippetType + ".snippet", "GraphML " + snippetType + " Snippet (*." + snippetType+ ".snippet)", "."+snippetType+".snippet", true, false);

    if(files.size() != 1){
        return;
    }

    QString snippetFileName = files.first();
    if(snippetFileName == ""){
        displayNotification("Snippet file selected has no name.");
        return;
    }

    if(!snippetFileName.endsWith(snippetType + ".snippet")){
        snippetFileName += snippetType + ".snippet";
        displayNotification("Snippet file selected changed to: " + snippetFileName + " To match type.");
    }

    QString fileData = readFile(snippetFileName);

    if(fileData == ""){
        return;
    }

    QFile file(snippetFileName);
    QFileInfo fileInfo(file.fileName());

    emit window_ImportSnippet(fileInfo.fileName(), fileData);
}

void MedeaWindow::exportSnippet(QString snippetType)
{
    if(!nodeView){
        return;
    }

    QStringList files = fileSelector("Export " + snippetType+ ".snippet", "GraphML " + snippetType + " Snippet (*." + snippetType+ ".snippet)","."+snippetType+".snippet", false);

    if(files.size() != 1){
        if(files.size() > 1){
            displayNotification("Only 1 file can be selected to export snippet!");
        }
        return;
    }

    QString snippetFileName = files.first();
    if(snippetFileName == ""){
        displayNotification("Snippet file selected has no name.");
        return;
    }

    if(!snippetFileName.endsWith(snippetType + ".snippet")){
        snippetFileName += "." + snippetType + ".snippet";
        displayNotification("Snippet file selected changed to: " + snippetFileName + " To match type.");
    }

    QString graphmlData = nodeView->getSelectionAsGraphMLSnippet();
    if(graphmlData != ""){
        writeFile(snippetFileName, graphmlData);
    }else{
        displayNotification("Cannot export snippet!");
    }
}

/**
 * @brief MedeaWindow::setClipboard
 * @param value
 */
void MedeaWindow::setClipboard(QString value)
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(value);
}



/**
 * @brief MedeaWindow::setActionEnabled
 * This gets called everytime a node is selected.
 * It enables/disables the specified menu action depending on the selected node.
 * @param action
 * @param node
 */
void MedeaWindow::setActionEnabled(QString action, bool enable)
{
    if (action == "definition") {
        view_goToDefinition->setEnabled(enable);
    } else if (action == "implementation") {
        view_goToImplementation->setEnabled(enable);
    } else if (action == "exportSnippet") {
        file_exportSnippet->setEnabled(enable);
    } else if (action == "importSnippet") {
        file_importSnippet->setEnabled(enable);
    } else if (action == "cut") {
        edit_cut->setEnabled(enable);
    } else if (action == "copy") {
        edit_copy->setEnabled(enable);
    } else if (action == "paste") {
        edit_paste->setEnabled(enable);
    } else if (action == "replicate") {
        edit_replicate->setEnabled(enable);
    } else if (action == "delete") {
        edit_delete->setEnabled(enable);
    } else if (action == "undo") {
        edit_undo->setEnabled(enable);
    } else if (action == "redo") {
        edit_redo->setEnabled(enable);
    } else if (action == "sort"){
        actionSort->setEnabled(enable);
    } else if (action == "singleSelection") {
        actionCenter->setEnabled(enable);
        actionZoomToFit->setEnabled(enable);
    } else if (action == "multipleSelection") {
        actionContextMenu->setEnabled(enable);
    } else if (action == "localdeployment") {
        model_ExecuteLocalJob->setEnabled(enable);
    } else if (action == "subView") {
        actionPopupSubview->setEnabled(enable);
    } else if (action == "align") {
        actionAlignHorizontally->setEnabled(enable);
        actionAlignVertically->setEnabled(enable);
    }
    emit window_refreshActions();
}


/**
 * @brief MedeaWindow::getIcon
 * @param alias
 * @param image
 * @return
 */
QIcon MedeaWindow::getIcon(QString alias, QString image)
{
    if (nodeView) {
        return QIcon(nodeView->getImage(alias, image));
    }
    return QIcon();
}


/**
 * @brief MedeaWindow::showWindowToolbar
 * If the signal came from the menu, show/hide the toolbar.
 * Otherwise if it came from toolbarsetButton, expand/contract the toolbar.
 * @param checked
 */
void MedeaWindow::showWindowToolbar(bool checked)
{
    if (checked) {
        toolbarButton->setToolTip("Contract Toolbar");
        toolbarButtonLabel->setPixmap(contractPixmap);
        toolbar->clearMask();
    } else {
        // NOTE: hover/focus doesn't leave the button until you move the mouse
        toolbarButton->setToolTip("Expand Toolbar");
        toolbarButtonLabel->setPixmap(expandPixmap);
        toolbar->setMask(QRegion(0,0,1,1, QRegion::Ellipse));
    }

    if (appSettings) {
        appSettings->setSetting(TOOLBAR_EXPANDED, checked);
    }

    //toolbar->setVisible(checked);
}


/**
 * @brief MedeaWindow::setToolbarVisibility
 * @param visible
 */
void MedeaWindow::setToolbarVisibility(bool visible)
{
    if (toolbarButton) {
        toolbarButton->setVisible(visible);
    }
    if (toolbar) {
        toolbar->setVisible(visible);
    }
}


/**
 * @brief MedeaWindow::detachWindowToolbar
 * @param checked
 */
void MedeaWindow::detachWindowToolbar(bool checked)
{
    Q_UNUSED(checked);
    // DEMO CHANGE
    /*
    QVBoxLayout* fromLayout;
    QVBoxLayout* toLayout;

    if (checked) {
        fromLayout = (QVBoxLayout*) toolbarArea->layout();
        toLayout = (QVBoxLayout*) toolbarStandAloneDialog->layout();
    } else {
        fromLayout = (QVBoxLayout*) toolbarStandAloneDialog->layout();
        toLayout = (QVBoxLayout*) toolbarArea->layout();
    }

    toolbarLayout->setParent(0);
    fromLayout->removeItem(toolbarLayout);
    toLayout->addLayout(toolbarLayout);

    // if the toolbar is to be detached, expand it before hiding the toolbar button
    bool showDialog = (settings_displayWindowToolbar->isChecked() && checked);
    if (showDialog) {
        toolbarButton->setChecked(true);
        toolbarButton->clicked(true);
    }

    toolbarStandAloneDialog->setVisible(showDialog);
    toolbarArea->setVisible(settings_displayWindowToolbar->isChecked() && !checked);

    // if the toolbar is detached, there's no point allowing the user to expand/contract
    // the toolbar; hide the toolbar button and the spacer to its right
    if (toolbarStandAloneDialog->isVisible()) {
        toolbarAction->setVisible(false);
        midRightSpacer->setVisible(false);
        checkedToolbarSpacers.removeAll(midRightSpacer);
    } else {
        toolbarAction->setVisible(true);
        updateCheckedToolbarActions();
    }
    */
}


/**
 * @brief MedeaWindow::detachedToolbarClosed
 */
void MedeaWindow::detachedToolbarClosed()
{
    //settings_displayWindowToolbar->setChecked(false);
    //settings_displayWindowToolbar->triggered(false);
}


/**
 * @brief MedeaWindow::updateCheckedToolbarActions
 * This updates the list of checked toolbar actions and sets the corresponding
 * toolbutton's visibility depending on checked and if the toolbar is expanded.
 * @param checked
 */
void MedeaWindow::updateCheckedToolbarActions(bool checked)
{
    QCheckBox* cb = dynamic_cast<QCheckBox*>(QObject::sender());
    QHash<QAction*, int> actionGroup;
    QAction* spacerAction = 0;

    if (cb) {

        QAction* action = toolbarActions.key(cb);
        action->setVisible(checked && toolbarButton->isChecked());

        if (checked) {
            checkedToolbarActions.append(action);
        } else {
            checkedToolbarActions.removeAll(action);
        }

        if (leftMostActions.contains(action)) {
            leftMostActions[action] = checked;
            actionGroup = leftMostActions;
            spacerAction = leftMostSpacer;
        } else if (leftMidActions.contains(action)) {
            leftMidActions[action] = checked;
            actionGroup = leftMidActions;
            spacerAction = leftMidSpacer;
        } else if (midLeftActions.contains(action)) {
            midLeftActions[action] = checked;
            actionGroup = midLeftActions;
            spacerAction = midLeftSpacer;
        } else if (midRightActions.contains(action)) {
            midRightActions[action] = checked;
            actionGroup = midRightActions;
            spacerAction = midRightSpacer;
        } else if (rightMidActions.contains(action)) {
            rightMidActions[action] = checked;
            actionGroup = rightMidActions;
            spacerAction = rightMidSpacer;
        } else if (rightMostActions.contains(action)) {
            rightMostActions[action] = checked;
            actionGroup = rightMostActions;
            spacerAction = rightMostSpacer;
        } else {
            // this case is when contextToolbar/delete button is checked/unchecked
            return;
        }

        // DEMO CHANGE
        /*
        // if the toolbar dialog is visible, the midRightSpacer
        // is hidden on purpose; don't change its visibility
        if (toolbarStandAloneDialog->isVisible()) {
            if (spacerAction == midRightSpacer) {
                return;
            }
            toolbarStandAloneDialog->resize(toolbarLayout->sizeHint());
        }
        */

    } else {
        // this happens when the toolbar has been re-attached to the main window
        // it checks if the spacer to the toolbar button's right should be displayed
        actionGroup = midRightActions;
        spacerAction = midRightSpacer;
    }

    // check if any of the spacer actions need to be hidden
    foreach (int visible, actionGroup.values()) {
        if (visible && spacerAction) {
            spacerAction->setVisible(toolbarButton->isChecked());
            checkedToolbarSpacers.append(spacerAction);
            return;
        }
    }

    spacerAction->setVisible(false);
    checkedToolbarSpacers.removeAll(spacerAction);
}


/**
 * @brief MedeaWindow::updateWidgetMask
 * @param widget
 * @param maskWidget
 * @param check
 * @param border
 */
void MedeaWindow::updateWidgetMask(QWidget *widget, QWidget *maskWidget, bool check, QSize border)
{
    if (check && widget->mask().isEmpty()) {
        return;
    }

    QPointF pos = maskWidget->pos();
    widget->clearMask();
    widget->setMask(QRegion(pos.x() - border.width()/2,
                            pos.y() - border.width()/2,
                            maskWidget->width() + border.width(),
                            maskWidget->height() + border.height(),
                            QRegion::Rectangle));

}


/**
 * @brief MedeaWindow::menuActionTriggered
 * This method is used to update the displayed text in the progress bar, update
 * tooltips and update the enabled state of the snap to grid functions.
 */
void MedeaWindow::menuActionTriggered()
{
    QAction* action = qobject_cast<QAction*>(QObject::sender());
    if (action->text().contains("Undo")) {
        progressAction = "Undoing Action";
    } else if (action->text().contains("Redo")) {
        progressAction = "Redoing Action";
    }
}


/**
 * @brief MedeaWindow::setAttributeModel
 * @param model
 */
void MedeaWindow::setAttributeModel(AttributeTableModel *model)
{
    dataTable->clearSelection();
    dataTable->setModel(model);
    updateDataTable();
}


/**
 * @brief MedeaWindow::forceToggleAspect
 * @param aspect
 * @param on
 */
void MedeaWindow::forceToggleAspect(VIEW_ASPECT aspect, bool on)
{
    foreach (AspectToggleWidget* aspectToggle, aspectToggles) {
        if (aspectToggle->getAspect() == aspect) {
            aspectToggle->setClicked(on);
            return;
        }
    }
}


/**
 * @brief MedeaWindow::dockButtonPressed
 * This slot is called whenever a dock toggle button is pressed.
 * It toggles the dock button and shows/hides the attached dock accordingly.
 */
void MedeaWindow::dockButtonPressed()
{
    DockToggleButton* button = qobject_cast<DockToggleButton*>(QObject::sender());
    if (button) {
        emit window_dockButtonPressed(button->getDockType());
    }
}


/**
 * @brief MedeaWindow::dockToggled
 * This is called whenever a dock is opened/closed.
 * If a dock is opened, update the dock header widgets. Otherwise, hide them.
 * It also updates the dock area's mask depending on whether there's an opened dock.
 * @param dockAction
 */
void MedeaWindow::dockToggled(bool opened, QString kindToConstruct)
{
    bool dockLabelVisible = false;
    bool backButtonVisible = false;
    bool actionLabelVisible = false;
    bool headerBoxVisible = false;

    // clear the dock are's mask; this makes sure that the whole area is visible
    docksArea->clearMask();

    if (opened) {

        DockScrollArea* dock = qobject_cast<DockScrollArea*>(QObject::sender());
        DOCK_TYPE dockType = dock->getDockType();

        switch (dockType) {
        case PARTS_DOCK:
        case HARDWARE_DOCK:
            openedDockLabel->setText(GET_DOCK_LABEL(dockType));
            dockLabelVisible = true;
            break;
        case DEFINITIONS_DOCK:
        case FUNCTIONS_DOCK:
            if (!kindToConstruct.isEmpty()) {
                QString action = "Select to construct a <br/>" + kindToConstruct;
                dockActionLabel->setText(action);
                actionLabelVisible = true;
            }
            backButtonVisible = true;
            break;
        default:
            break;
        }

        headerBoxVisible = true;

    } else {
        if (partsDock->isDockOpen() || definitionsDock->isDockOpen() || functionsDock->isDockOpen() || hardwareDock->isDockOpen()) {
            // if any of the docks are open, show the dock header widgets
            headerBoxVisible = true;
        } else {
            // if no docks are open, update the dock area's mask - allow mouse events to pass through it
            updateWidgetMask(docksArea, dockButtonsBox);
        }
    }

    if (dockLabelVisible != openedDockLabel->isVisible()) {
        openedDockLabel->setVisible(dockLabelVisible);
    }
    if (backButtonVisible != dockBackButtonBox->isVisible()) {
        dockBackButtonBox->setVisible(backButtonVisible);
    }
    if (actionLabelVisible != dockActionLabel->isVisible()) {
        dockActionLabel->setVisible(actionLabelVisible);
    }
    if (headerBoxVisible != dockHeaderBox->isVisible()) {
        dockHeaderBox->setVisible(headerBoxVisible);
        if (headerBoxVisible) {
            updateDock();
        }
    }
}


/**
 * @brief MedeaWindow::dockBackButtonTriggered
 */
void MedeaWindow::dockBackButtonTriggered()
{
    if (definitionsDock->isDockOpen()) {
        definitionsDock->setDockOpen(false);
    } else if (functionsDock->isDockOpen()) {
        functionsDock->setDockOpen(false);
    }
    partsDock->forceOpenDock();
}


/**
 * @brief MedeaWindow::displayLoadingStatus
 * @param show
 * @param displayText
 */
void MedeaWindow::displayLoadingStatus(bool show, QString displayText)
{
    if (loadingBox) {
        if (show != loadingBox->isVisible()) {
            loadingBox->setVisible(show);
        }
        if (show && !displayText.isEmpty()) {
            //displayText += "...";
            if (loadingLabel->text() != displayText) {
                loadingLabel->setText(displayText);
                loadingLabel->setFixedWidth(loadingLabel->fontMetrics().width(loadingLabel->text()) + 20);
                loadingBox->setFixedWidth(loadingMovieLabel->width() + loadingLabel->width());
            }
        }
    }
}


/**
 * @brief MedeaWindow::updateProgressStatus
 * This updates the progress bar values.
 */
void MedeaWindow::updateProgressStatus(int value, QString status)
{
    // pause the notification timer before showing the progress dialog
    if (notificationTimer->isActive()) {
        leftOverTime = notificationTimer->remainingTime();
        notificationTimer->stop();
    }

    // show progress dialog
    if (!progressDialogVisible) {
        //QPoint dialogOrigin = pos() + QPoint(width(), height());
        //QPoint dialogOrigin = pos() + getCanvasRect().center();
        //dialogOrigin -= QPoint(progressDialog->width() / 2, progressDialog->height() / 2);
        //progressDialog->move(dialogOrigin);
        progressDialog->show();
        progressDialogVisible = true;
    }

    // update progress text
    if (!status.isEmpty()) {
        progressLabel->setText(status + ". Please wait...");
    }

    if (value == -1) {
        progressBar->setMaximum(0);
        value = 0;
    } else {
        progressBar->setMaximum(100);
        value = qMax(value, 0);
    }

    // update progress value
    progressBar->setValue(value);

    // close the progress dialog and re-display the notification bar if it was previously displayed
    bool finishedLoading = value == progressBar->maximum();
    if (finishedLoading) {
        closeProgressDialog();
    }
}


/**
 * @brief MedeaWindow::closeProgressDialog
 */
void MedeaWindow::closeProgressDialog()
{
    if (leftOverTime > 0) {
        notificationsBar->show();
        notificationTimer->start(leftOverTime);
        leftOverTime = 0;
    }

    progressDialog->close();
    progressLabel->setText("Loading...");
    progressBar->reset();
    progressDialogVisible = false;
}


/**
 * @brief MedeaWindow::searchItemClicked
 * This is called when one of the search results items is clicked.
 */
void MedeaWindow::searchItemClicked()
{
    SearchItem* itemClicked = qobject_cast<SearchItem*>(QObject::sender());
    window_searchItemClicked(itemClicked);
}


/**
 * @brief MedeaWindow::searchMenuButtonClicked
 * This is called when one of the menu buttons in the search options is clicked.
 * This determines which menu was clicked and where to display it.
 */
void MedeaWindow::searchMenuButtonClicked(bool checked)
{
    QPushButton* senderButton = qobject_cast<QPushButton*>(QObject::sender());
    QWidget* widget = 0;
    QMenu* menu = 0;

    if (senderButton == searchOptionButton) {
        widget = searchBar;
        menu = searchOptionMenu;
    } else if (senderButton == viewAspectsButton) {
        widget = viewAspectsBar;
        menu = viewAspectsMenu;
    } else  if (senderButton == nodeKindsButton) {
        widget = nodeKindsBar;
        menu = nodeKindsMenu;
    } else  if (senderButton == dataKeysButton) {
        widget = dataKeysBar;
        menu = dataKeysMenu;
    }

    if (widget && menu) {
        if (checked) {
            menu->popup(widget->mapToGlobal(widget->rect().bottomLeft()));
        } else {
            menu->close();
        }
    }
}


/**
 * @brief MedeaWindow::searchMenuClosed
 * When a search menu is closed, uncheck the button it's attached to.
 */
void MedeaWindow::searchMenuClosed()
{
    QMenu* menu = qobject_cast<QMenu*>(QObject::sender());
    QPushButton* button = qobject_cast<QPushButton*>(menu->parentWidget());
    if (button && button->isChecked()) {

        //close this levels button
        button->setChecked(false);

        //get the point of the cursor and the main SearchOptionMenu
        QPoint topLeft = searchOptionMenu->mapToParent(searchOptionMenu->rect().topLeft());
        QPoint bottomRight = searchOptionMenu->mapToParent(searchOptionMenu->rect().bottomRight());
        QRect menuRect(topLeft, bottomRight);

        //if the mouse is not within the confines of the searchOptionMenu, then close the searchOptionMenu too
        if (!menuRect.contains(QCursor::pos())) {
            searchOptionMenu->close();
        }

        //if user has clicked on button, catch that case to not re-open the menu
        if (searchOptionButton->rect().contains(searchOptionButton->mapFromGlobal(QCursor::pos()))) {
            searchOptionButton->setChecked(true);
        }
    }
}


/**
 * @brief MedeaWindow::updateSearchLineEdits
 * This is called whenever the search bar gains/loses focus and when an item
 * from one of the search menus has been checked/unchecked.
 * It updates what text is displayed on the corresponding line edit.
 */
void MedeaWindow::updateSearchLineEdits()
{
    // check if the displayed text of the corresponding line edit
    // needs to be updated or reset back to the default text
    QCheckBox* checkBox = qobject_cast<QCheckBox*>(QObject::sender());
    if (!checkBox) {
        return;
    }

    QMenu* menu = qobject_cast<QMenu*>(checkBox->parentWidget());
    if (menu) {

        QLineEdit* lineEdit = 0;
        QString textLabel;
        QString defaultText;
        QStringList checkedItemsList;

        if (menu == viewAspectsMenu) {
            lineEdit = viewAspectsBar;
            textLabel = "Search Aspects: ";
            defaultText = viewAspectsBarDefaultText;
            checkedItemsList = getCheckedItems(0);
        } else if (menu == nodeKindsMenu) {
            lineEdit = nodeKindsBar;
            textLabel = "Search Kinds: ";
            defaultText = nodeKindsDefaultText;
            checkedItemsList = getCheckedItems(1);
        } else if (menu == dataKeysMenu) {
            lineEdit = dataKeysBar;
            textLabel = "Search Data Keys: ";
            defaultText = dataKeysDefaultText;
            checkedItemsList = getCheckedItems(2);
            /*
            if (checkedItemsList.isEmpty()) {
                checkedItemsList = dataKeys;
            }
            */
        } else {
            qWarning() << "MedeaWindow::updateSearchLineEdits - Not checking for this menu.";
            return;
        }

        if (checkedItemsList.count() == 0) {
            lineEdit->setText(defaultText);
        } else {
            QString displayText;
            foreach (QString checkedItem, checkedItemsList) {
                displayText += checkedItem + ", ";
            }
            displayText.truncate(displayText.length() - 2);
            lineEdit->setText(displayText);
        }

        // keep the cursor at the front so that the start of the text is always visible
        lineEdit->setCursorPosition(0);
        lineEdit->setToolTip(textLabel + lineEdit->text());
    }
}


/**
 * @brief MedeaWindow::updateSearchSuggestions
 */
void MedeaWindow::updateSearchSuggestions()
{
    if (nodeView && searchBar) {
        nodeView->searchSuggestionsRequested(searchBar->text(), getCheckedItems(0), getCheckedItems(1), getCheckedItems(2));
    }
}


/**
 * @brief MedeaWindow::displayNotification
 * @param notification
 * @param seqNum
 * @param totalNum
 */
void MedeaWindow::displayNotification(QString notification, int seqNum, int totalNum)
{
    if (totalNum > 1) {
        multipleNotification[seqNum] = notification;
        if (multipleNotification.count() == totalNum) {
            notification = "";
            for (int i = 0; i < totalNum; i++) {
                notification += multipleNotification[i] + " ";
            }
            multipleNotification.clear();
        } else {
            return;
        }
    }

    // add new notification to the queue
    if (!notification.isEmpty()) {
        notificationsQueue.enqueue(notification);
    }

    // if there is a notification in the queue, start the timer and show the notification bar
    if (!notificationTimer->isActive() && !notificationsQueue.isEmpty()) {
        notification = notificationsQueue.dequeue();
        notificationsBar->setText(notification);
        notificationsBar->setFixedWidth(notificationsBar->fontMetrics().width(notification) + 30);
        // only show the notifications bar if the progress dialog is not open
        if (!progressDialogVisible) {
            notificationsBar->show();
        }
        notificationTimer->start(NOTIFICATION_TIME);
    }
}


/**
 * @brief MedeaWindow::checkNotificationsQueue
 */
void MedeaWindow::checkNotificationsQueue()
{
    notificationTimer->stop();
    //notificationsBar->hide();

    // if there are still notifications waiting to be displayed, display them in order
    if (notificationsQueue.count() > 0) {
        displayNotification();
    }
}


/**
 * @brief MedeaWindow::showDocks
 * @param checked
 */
void MedeaWindow::showDocks(bool checked)
{
    // DEMO CHANGE
    //dockStandAloneDialog->setVisible(settings_detachDocks->isChecked() && checked);
    //docksArea->setVisible(!settings_detachDocks->isChecked() && checked);
    docksArea->setVisible(checked);
    //settings_detachDocks->setEnabled(checked);
}


/**
 * @brief MedeaWindow::detachDocks
 */
void MedeaWindow::detachDocks(bool checked)
{
    QVBoxLayout* fromLayout;
    QVBoxLayout* toLayout;

    if (checked) {
        fromLayout = (QVBoxLayout*) docksArea->layout();
        toLayout = (QVBoxLayout*) dockStandAloneDialog->layout();
    } else {
        fromLayout = (QVBoxLayout*) dockStandAloneDialog->layout();
        toLayout = (QVBoxLayout*) docksArea->layout();
    }

    dockLayout->setParent(0);
    fromLayout->removeItem(dockLayout);
    toLayout->addLayout(dockLayout);

    docksArea->setVisible(!checked);

    //dockStandAloneDialog->setVisible(settings_displayDocks->isChecked() && checked);
    //docksArea->setVisible(settings_displayDocks->isChecked() && !checked);

    /*
    if (dockStandAloneDialog->isVisible()) {
        double h = dockStandAloneDialog->height();
        dockStandAloneDialog->setFixedHeight(h*2);
        partsDock->parentHeightChanged(dockStandAloneDialog->height());
        definitionsDock->parentHeightChanged(dockStandAloneDialog->height());
        hardwareDock->parentHeightChanged(dockStandAloneDialog->height());
        dockStandAloneDialog->setFixedHeight(h);
    }
    */

    /*
    double newHeight = docksArea->height();
    if (dockStandAloneDialog->isVisible()) {
        newHeight *= 2;
    }
    partsDock->parentHeightChanged(newHeight);
    definitionsDock->parentHeightChanged(newHeight);
    hardwareDock->parentHeightChanged(newHeight);
    */
}


/**
 * @brief MedeaWindow::detachedDockClosed
 */
void MedeaWindow::detachedDockClosed()
{
    //settings_displayDocks->setChecked(false);
    //settings_displayDocks->triggered(false);
}

void MedeaWindow::clearRecentProjectsList()
{
    //Clear the list.
    recentProjectsList.clear();
    //Update the widgets.
    updateRecentProjectsWidgets();
}


/**
 * @brief MedeaWindow::updateDataTable
 * Update the dataTable size whenever a node is selected/deselected,
 * when a new model is loaded and when the window is resized.
 * NOTE: Once maximum size is set, it cannot be reset.
 */
void MedeaWindow::updateDataTable()
{
    QAbstractItemModel* tableModel = dataTable->model();
    bool hasData = tableModel && tableModel->rowCount() > 0;

    if (hasData) {
        dataTableBox->setVisible(true);
        dataTableBox->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    } else {
        dataTable->resize(dataTable->width(), 0);
        dataTableBox->setAttribute(Qt::WA_TransparentForMouseEvents);
        updateWidgetMask(dataTableBox, dataTable);
        dataTableBox->setVisible(false);
        return;
    }

    // calculate the required height
    int newHeight = 0;
    int maxHeight = dataTableBox->height() - SPACER_SIZE;

    for (int i = 0; i < tableModel->rowCount(); i++) {
        newHeight += dataTable->rowHeight(i);
    }

    newHeight += dataTable->horizontalHeader()->size().height();
    newHeight += dataTable->contentsMargins().top() + dataTable->contentsMargins().bottom();

    if (newHeight > maxHeight) {
        dataTable->resize(dataTable->width(), maxHeight);
    } else {
        dataTable->resize(dataTable->width(), newHeight);
    }

    // update the visible region of the groupbox to fit the dataTable
    updateWidgetMask(dataTableBox, dataTable);
    dataTableBox->repaint();

    // align the contents of the datatable
    if(dataTable->horizontalHeader()->count() == 2){
        dataTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
        dataTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    }
}

void MedeaWindow::updateRecentProjectsWidgets(QString topFileName)
{
    if(!topFileName.isEmpty()){
        //Get the index of the filename opened (if it exists)
        int index = recentProjectsList.indexOf(topFileName);
        if(index > 0){
            //Remove it.
            recentProjectsList.remove(index);
        }else if(index == 0){
            return;
        }
        recentProjectsList.prepend(topFileName);
    }

    //Keep the list short.
    while(recentProjectsList.size() > RECENT_PROJECT_SIZE){
        recentProjectsList.removeLast();
    }

    //Get all of the previous QActions for the projects, except for the clear list button.
    QList<QAction*> actionsToRemove;
    foreach(QAction* action, file_recentProjectsMenu->actions()){
        if(action != file_recentProjects_clearHistory){
            actionsToRemove << action;
        }
    }

    //Delete the old actions.
    while(!actionsToRemove.isEmpty()){
        delete actionsToRemove.takeFirst();
    }

    //Clear the Welcome screen widget.
    recentProjectsListWidget->clear();

    //Construct a new item for the List Widget and Menu.
    foreach(QString fileName, recentProjectsList){
        QListWidgetItem* item = new QListWidgetItem(recentProjectsListWidget);
        item->setIcon(getIcon("Actions", "New"));
        item->setText(fileName);
        recentProjectsListWidget->addItem(item);

        QAction* fileAction = new QAction(file_recentProjectsMenu);
        fileAction->setIcon(getIcon("Actions", "New"));
        fileAction->setText(fileName);
        file_recentProjectsMenu->insertAction(file_recentProjects_clearHistory, fileAction);

        //Connect the menu item to the loadRecentProject.
        connect(fileAction, SIGNAL(triggered(bool)), this, SLOT(recentProjectMenuActionClicked()));
    }
    if(recentProjectsList.size() > 0){
        file_recentProjectsMenu->insertSeparator(file_recentProjects_clearHistory);
    }
}


/**
 * @brief MedeaWindow::importProjects
 * @param files
 */
void MedeaWindow::importProjects(QStringList files)
{
    QStringList projects;
    foreach (QString fileName, files) {
        QString file = readFile(fileName);
        if(file != ""){
            projects << file;
        }
    }
    if (projects.size() > 0) {
        window_ImportProjects(projects);
        nodeView->fitToScreen();
    }
}


/**
 * @brief MedeaWindow::jenkins_JobName_Changed
 * @param jobName
 */
void MedeaWindow::jenkins_JobName_Changed(QString jobName)
{
    if(jenkins_ExecuteJob){
        jenkins_ExecuteJob->setText("Launch: " + jobName);
    }
}


/**
 * @brief MedeaWindow::closeEvent
 * @param e
 */
void MedeaWindow::closeEvent(QCloseEvent * e)
{
    if(closeProject()){
        e->accept();
        deleteLater();
    }else{
        e->ignore();
    }
}


/**
 * @brief MedeaWindow::getCheckedItems
 * This returns a list of the checked items from the search sub-menus.
 * @param menu - 0 = viewAspects, 1 = nodeKinds, 2 = dataKeys
 * @return
 */
QStringList MedeaWindow::getCheckedItems(int menu)
{
    QStringList checkedItems;
    bool checkedDataKeys = false;

    QMenu* searchMenu = 0;
    switch (menu) {
    case SEARCH_VIEW_ASPECTS:
        searchMenu = viewAspectsMenu;
        break;
    case SEARCH_NODE_KINDS:
        searchMenu = nodeKindsMenu;
        break;
    case SEARCH_DATA_KEYS:
        searchMenu = dataKeysMenu;
        checkedDataKeys = true;
        break;
    default:
        return checkedItems;
    }

    if (searchMenu) {
        foreach (QAction* action, searchMenu->actions()) {
            QWidgetAction* widgetAction = qobject_cast<QWidgetAction*>(action);
            QCheckBox* checkBox = qobject_cast<QCheckBox*>(widgetAction->defaultWidget());
            if (checkBox->isChecked()) {
                checkedItems.append(checkBox->text());
            }
        }
        if (checkedDataKeys && checkedItems.isEmpty()) {
            checkedItems = dataKeys;
        }
    }

    return checkedItems;
}


/**
 * @brief MedeaWindow::writeTemporaryFile
 * @param data
 * @return
 */
QTemporaryFile* MedeaWindow::writeTemporaryFile(QString data)
{
    QTemporaryFile* tempFile = new QTemporaryFile();
    bool fileOpened = tempFile->open();

    if(!fileOpened){
        QMessageBox::critical(this, "File Error", "Cannot open Temp File to Write.", QMessageBox::Ok);
    }

    QTextStream out(tempFile);
    out << data;
    tempFile->close();

    return tempFile;
}


QString MedeaWindow::readFile(QString fileName)
{
    QString fileData = "";
    try {
        QFile file(fileName);

        bool fileOpened = file.open(QFile::ReadOnly | QFile::Text);

        if (!fileOpened) {
            QMessageBox::critical(this, "File Error", "Unable to open file: '" + fileName + "'! Check permissions and try again.", QMessageBox::Ok);
            return "";
        }

        QTextStream fileStream(&file);
        fileData = fileStream.readAll();
        file.close();
    }catch (...) {
        QMessageBox::critical(this, "Error", "Error reading file: '" + fileName + "'", QMessageBox::Ok);
    }
    return fileData;
}


bool MedeaWindow::writeQImage(QString filePath, QImage image, bool notify)
{
    try {
        if(ensureDirectory(filePath)){
            QFile file(filePath);

            if(image.save(filePath, "PNG")){
                //SUCCESS
            }else{
                QMessageBox::critical(this, "File Error", "Unable to open file to write: '" + filePath + "'! Check permissions and try again.", QMessageBox::Ok);
                return false;
            }
        }
    }catch (...) {
        QMessageBox::critical(this, "File Error", "Unable to open file to write: '" + filePath + "'! Check permissions and try again.", QMessageBox::Ok);
        return false;
    }

    if(notify){
        displayNotification("Image: '" + filePath + "'' written!");
    }
    return true;

}

bool MedeaWindow::ensureDirectory(QString filePath)
{
    QFile file(filePath);
    QFileInfo fileInfo(file);
    QDir dir = fileInfo.dir();
    if (!dir.exists()) {
        if(dir.mkpath(".")){
            displayNotification("Dir: '" + dir.absolutePath() + "'' Constructed!");
        }else{
            QMessageBox::critical(this, "File Error", "Unable to make path: '" + dir.absolutePath() + "'! Check permissions and try again.", QMessageBox::Ok);
            return false;
        }
    }
    return true;
}

bool MedeaWindow::writeFile(QString filePath, QString fileData, bool notify)
{
    try {
        if(ensureDirectory(filePath)){
            QFile file(filePath);

            bool fileOpened = file.open(QFile::WriteOnly | QFile::Text);

            if (!fileOpened) {
                QMessageBox::critical(this, "File Error", "Unable to open file to write: '" + filePath + "'! Check permissions and try again.", QMessageBox::Ok);
                return false;
            }

            //Create stream to write the data.
            QTextStream out(&file);
            out << fileData;
            file.close();
        }
    }catch (...) {
        QMessageBox::critical(this, "File Error", "Unable to open file to write: '" + filePath + "'! Check permissions and try again.", QMessageBox::Ok);
        return false;
    }

    if(notify){
        displayNotification("File: '" + filePath + "'' written!");
    }
    return true;
}

QString MedeaWindow::writeTempFile(QString fileData)
{
    QString tempFilePath = getTempFileName();

    bool success = writeFile(tempFilePath, fileData, false);

    if(!success){
        return "";
    }
    return tempFilePath;
}

QString MedeaWindow::writeProjectToTempFile()
{
    QString data = nodeView->getProjectAsGraphML();
    if(data.isEmpty()){
        return "";
    }

    //Write the data to a temp file.
    QString exportFile = writeTempFile(data);

    return exportFile;
}

void MedeaWindow::dropEvent(QDropEvent *event)
{
    QStringList fileList;
    foreach (const QUrl &url, event->mimeData()->urls()) {
        fileList << url.toLocalFile();
    }
    if(!fileList.isEmpty()){
        if(fileList.size() == 1){
            progressAction = "Importing GraphML via Drag";
            QString fileName = fileList.first();
            if(nodeView->hasModel()){
                importProjects(fileList);
            }else{
                openProject(fileName);
            }
        }
    }
}

void MedeaWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        if(canFilesBeDragImported(event->mimeData()->urls())){
            event->acceptProposedAction();
        }
    }
}


/**
 * @brief MedeaWindow::setupMultiLineBox
 */
void MedeaWindow::setupMultiLineBox()
{
    //QDialog that pops up
    popupMultiLine = new QDialog(this);
    //take focus from the window
    popupMultiLine->setModal(true);
    //remove the '?' from the title bar
    popupMultiLine->setWindowFlags(popupMultiLine->windowFlags() & (~Qt::WindowContextHelpButtonHint));
    popupMultiLine->setWindowIcon(QIcon(":/Actions/getCPP.png"));
    //Sexy Layout Stuff
    QGridLayout *gridLayout = new QGridLayout(popupMultiLine);

    //Text Edit Box
    txtMultiLine = new CodeEditor();
    //make tab width mode civilized
    txtMultiLine->setTabStopWidth(40);

    //Make look purrdy!
    txtMultiLine->setObjectName(QString::fromUtf8("txtMultiline"));
    gridLayout->addWidget(txtMultiLine, 0, 0, 1, 1);
    QDialogButtonBox *buttonBox = new QDialogButtonBox();
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    gridLayout->addWidget(buttonBox, 1, 0, 1, 1);

    //hook up OK/Cancel boxes
    connect(buttonBox,SIGNAL(accepted()),this,SLOT(dialogAccepted()));
    connect(buttonBox,SIGNAL(rejected()),this,SLOT(dialogRejected()));
}


/**
 * @brief MedeaWindow::dataTableDoubleClicked
 * an item in dataTable was double clicked on - Open a multiline text box (if applicable) so the user can put in multi-line data
 * @param QModelIndex: Details about the index that was double clicked on
 */
void MedeaWindow::dataTableDoubleClicked(QModelIndex index)
{
    //find whether we should popup the window
    QVariant needsMultiLine = index.model()->data(index, -2);

    //Only do this if it's in column 2
    if(needsMultiLine == true) {

        popupMultiLine->setWindowTitle("Editing: " + index.model()->data(index, -3).toString());

        QVariant value = index.model()->data(index, Qt::DisplayRole);

        txtMultiLine->setPlainText(value.toString());

        //Show me the box!
        popupMultiLine->show();

        //store the QModelIndex to update the value
        clickedModelIndex = index;
    }
}


/**
 * @brief MedeaWindow::dialogAccepted
 * Update the data in the text fields
 */
void MedeaWindow::dialogAccepted()
{
    //Update the table and close
    dataTable->model()->setData(clickedModelIndex, QVariant(txtMultiLine->toPlainText()), Qt::EditRole);
    popupMultiLine->close();
}


/**
 * @brief MedeaWindow::dialogRejected
 * Close the Multi-Line dialog box
 */
void MedeaWindow::dialogRejected()
{
    popupMultiLine->close();
}

QStringList MedeaWindow::fileSelector(QString title, QString fileString, QString defaultSuffix, bool open, bool allowMultiple, QString fileName)
{
    Q_UNUSED(defaultSuffix);

    QStringList files;

    if(fileName == ""){
        fileName = "/";
    }

    if(!fileDialog){
        fileDialog = new QFileDialog(this);
        fileDialog->setWindowModality(Qt::WindowModal);
    }

    if(fileDialog){
        fileDialog->setWindowTitle(title);
        fileDialog->setNameFilter(fileString);
        fileDialog->setDirectory(DEFAULT_PATH);


        if(open){
            fileDialog->setAcceptMode(QFileDialog::AcceptOpen);

            if(allowMultiple){
                fileDialog->setFileMode(QFileDialog::ExistingFiles);
            }else{
                fileDialog->setFileMode(QFileDialog::ExistingFile);
            }

            fileDialog->setConfirmOverwrite(false);
            fileDialog->selectFile(fileName);

            if (fileDialog->exec()){
                files = fileDialog->selectedFiles();
            }
        }else{
            fileDialog->setAcceptMode(QFileDialog::AcceptSave);
            fileDialog->setFileMode(QFileDialog::AnyFile);

            fileDialog->setConfirmOverwrite(true);
            fileDialog->selectFile(fileName);

            if (fileDialog->exec()){
                files = fileDialog->selectedFiles();
            }
        }


        //Update DEFAULT_PATH!
        if(files.size() > 0){
            DEFAULT_PATH = fileDialog->directory().absolutePath();
            if(!DEFAULT_PATH.endsWith("/")){
                DEFAULT_PATH += "/";
            }
        }
    }


    return files;
}


/**
 * @brief MedeaWindow::themeChanged
 * @param theme
 */
void MedeaWindow::themeChanged(VIEW_THEME theme)
{
    CURRENT_THEME = theme;

    QString lighterViewColor = GET_VIEW_COLOR_STRING(theme, LIGHTER_SHADE) + ";";
    QString darkerViewColor = GET_VIEW_COLOR_STRING(theme, DARKER_SHADE) + ";";
    QString textColor = GET_COLOR_STRING(GET_TEXT_COLOR(theme)) + ";";
    QString highlightTextColor = GET_COLOR_STRING(GET_TEXT_COLOR(theme, true)) + ";";

    loadingLabel->setStyleSheet("QLabel{ color:" + textColor + "}");
    recentProjectsListWidget->setStyleSheet("background:" + darkerViewColor + "color: white; font-size: 16px;");
    projectName->setStyleSheet("QPushButton{ color:" + textColor + "font-size: 16px; text-align: left; }"
                                                                   "QTooltip{ background: white; color: black; }");

    menu->setStyleSheet("QMenu { "
                        "background:" + lighterViewColor +
                        "}"
                        "QMenu::item {"
                        "padding: 1px 20px 1px 45px;"
                        "background:" + lighterViewColor +
                        "color:" + textColor +
                        "border: none;"
                        "}"
                        "QMenu::item:disabled {"
                        "color: gray;"
                        "}"
                        "QMenu::item:selected {"
                        "border: 1px solid gray;"
                        "background:" + GET_COLOR_STRING(lighterViewColor, LIGHTER_SHADE) + ";"
                        "color:" + highlightTextColor +
                        "}");
}


/**
 * @brief MedeaWindow::toggleMinimap
 */
void MedeaWindow::toggleMinimap()
{
    QAction* action = qobject_cast<QAction*>(QObject::sender());
    if (action && action == view_showMinimap) {
        view_showMinimap->setEnabled(false);
        minimapBox->show();
    } else {
        view_showMinimap->setEnabled(true);
    }
}
