#include "medeawindow.h"
#include "Controller/controller.h"
#include "GUI/codeeditor.h"
#include "CUTS/GUI/cutsexecutionwidget.h"


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
#include <QToolButton>
#include <QToolBar>

#define THREADING true


#define RIGHT_PANEL_WIDTH 230.0
#define SPACER_HEIGHT 30

#define MIN_WIDTH 1000
#define MIN_HEIGHT (480 + SPACER_HEIGHT*3)

#define TOOLBAR_BUTTON_WIDTH 46
#define TOOLBAR_BUTTON_HEIGHT 40

#define NOTIFICATION_TIME 2000


// USER SETTINGS
#define LOG_DEBUGGING "00-01-Log_Debug_Information"
#define THREAD_LIMIT "00-02-Thread_Limit"
#define CUTS_CONFIGURE_PATH "00-03-CUTS_Configure_Script_Path"

#define WINDOW_X "01-01-Position_X"
#define WINDOW_Y "01-02-Position_Y"
#define WINDOW_W "01-03-Width"
#define WINDOW_H "01-04-Height"
#define WINDOW_MAX_STATE "01-05-Maximized"
#define AUTO_CENTER_VIEW "02-01-Auto_Center_View"
#define SELECT_ON_CREATION "02-02-Select_Entity_On_Creation"
#define ZOOM_ANCHOR_ON_MOUSE "02-03-Zoom_View_Under_Mouse"
#define TOGGLE_GRID "02-04-Toggle_Grid_Lines"
#define SHOW_MANAGEMENT_COMPONENTS "02-05-Show_Management_Components"
#define SHOW_LOCAL_NODE "02-06-Show_Local_Node"
#define ASPECT_D "03-01-Definitions"
#define ASPECT_W "03-02-Workload"
#define ASPECT_A "03-03-Assembly"
#define ASPECT_H "03-04-Hardware"
#define DOCK_VISIBLE "04-01-Hide_Dock"
#define TOOLBAR_VISIBLE "05-00-00-Hide_Toolbar"
#define TOOLBAR_EXPANDED "05-00-01-Expand_Toolbar"
#define JENKINS_URL "06-01-URL"
#define JENKINS_USER "06-02-Username"
#define JENKINS_PASS "06-03-Password"
#define JENKINS_JOB "06-04-MEDEA_Jobname"
#define JENKINS_TOKEN "06-05-API_Token"

#define TOOLBAR_SETTINGS "05-Toolbar_Settings"
#define TOOLBAR_CONTEXT "05-01-Context_Toolbar"
#define TOOLBAR_UNDO "05-02-Undo"
#define TOOLBAR_REDO "05-03-Redo"
#define TOOLBAR_CUT "05-04-Cut"
#define TOOLBAR_COPY "05-05-Copy"
#define TOOLBAR_PASTE "05-06-Paste"
#define TOOLBAR_REPLICATE "05-07-Replicate"
#define TOOLBAR_DELETE_ENTITIES "05-08-Delete_Entities"
#define TOOLBAR_POPUP_SUBVIEW "05-09-Popup_SubView"
#define TOOLBAR_GRID_LINES "05-10-Grid_Lines"
#define TOOLBAR_FIT_TO_SCREEN "05-11-Fit_To_Screen"
#define TOOLBAR_CENTER_ON_ENTITY "05-12-Center_On_Entity"
#define TOOLBAR_ZOOM_TO_FIT "05-13-Zoom_To_Fit"
#define TOOLBAR_SORT "05-14-Sort"
#define TOOLBAR_VERT_ALIGN "05-15-Vertical_Align_Entities"
#define TOOLBAR_HORIZ_ALIGN "05-16-Horizontal_Align_Entities"
#define TOOLBAR_BACK "05-17-Back"
#define TOOLBAR_FORWARD "05-18-Forward"




/**
 * @brief MedeaWindow::MedeaWindow
 * @param graphMLFile
 * @param parent
 */
MedeaWindow::MedeaWindow(QString graphMLFile, QWidget *parent) :
    QMainWindow(parent)
{
    launchFilePathArg = graphMLFile;
    loadLaunchedFile = launchFilePathArg != "";


    modelCleared = false;

    // this needs to happen before the menu is set up and connected

    applicationDirectory = QApplication::applicationDirPath() + "/";
    MEDEA_VERSION = QApplication::applicationVersion();

    appSettings = new AppSettings(this, applicationDirectory);
    appSettings->setModal(true);
    connect(appSettings, SIGNAL(settingChanged(QString,QString,QString)), this, SLOT(settingChanged(QString, QString, QString)));

    controllerThread = 0;
    controller = 0;
    tempExport = false;
    validate_TempExport = false;
    cuts_TempExport = false;
    jenkins_TempExport = false;
    leftOverTime = 0;
    isWindowMaximized = false;
    settingsLoading = false;
    maximizedSettingInitiallyChanged = false;



    initialiseJenkinsManager();
    initialiseCUTSManager();

    initialiseGUI();
    makeConnections();


    newProject();
}


/**
 * @brief MedeaWindow::~MedeaWindow
 */
MedeaWindow::~MedeaWindow()
{
    if (appSettings) {
        saveSettings();
        appSettings->deleteLater();
    }

    if (controller) {
        controller->deleteLater();
    }
    if (nodeView) {
        nodeView->deleteLater();
    }


    if(jenkinsManager){
        jenkinsManager->deleteLater();
    }

    /* REMOVED TO STOP UBUNTU CRASH LOGGING
    if(controllerThread){
        controllerThread->deleteLater();
    }*/

}

void MedeaWindow::toolbarSettingChanged(QString keyName, QString value)
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
    }
}

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
 * @brief MedeaWindow::modelReady - Called whenever a new project is run, after the controller has finished setting up the NodeView/Controller/Model
 */
void MedeaWindow::modelReady()
{
    //Reset the View.
    resetView();
    //Reset the initial settings
    setupInitialSettings();

    //Load loadLaunchedFile
    if(loadLaunchedFile){
        QStringList files;
        files << launchFilePathArg;
        importProjects(files);
        loadLaunchedFile = false;
    }
    //Make the nodeView visable agian!
    if(nodeView){
        nodeView->setEnabled(true);
        //nodeView->fitToScreen();
    }
    //cuts_runGeneration->trigger();
}

void MedeaWindow::projectCleared()
{
    clearDocks();
}

/**
 * @brief MedeaWindow::settingChanged
 * @param groupName
 * @param keyName
 * @param value
 */
void MedeaWindow::settingChanged(QString groupName, QString keyName, QString value)
{
    if(groupName==TOOLBAR_SETTINGS){
        toolbarSettingChanged(keyName, value);
    }

    bool isBool = false;
    bool boolValue = false;
    bool isInt = false;
    int intValue = value.toInt(&isInt);
    if(value == "true" || value == "false"){
        isBool = true;
        if(value == "true"){
            boolValue = true;
        }
    }

    if(keyName == WINDOW_X && isInt){
        move(intValue, pos().y());
    }else if(keyName == WINDOW_Y && isInt){
        move(pos().x(), intValue);
    }else if(keyName == WINDOW_W && isInt){
        resize(intValue, size().height());
    }else if(keyName == WINDOW_H && isInt){
        resize(size().width(), intValue);
    }else if(keyName == WINDOW_MAX_STATE && isBool){
        if(boolValue != isWindowMaximized && settingsLoading){
            maximizedSettingInitiallyChanged = true;
        }
        if(boolValue){
            setWindowState(Qt::WindowMaximized);
        }else{
            setWindowState(Qt::WindowNoState);
        }
    }else if(keyName == LOG_DEBUGGING && isBool){
        if(nodeView){
            emit nodeView->view_EnableDebugLogging(boolValue, applicationDirectory);
        }
    }else if(keyName == CUTS_CONFIGURE_PATH){
        if(cutsManager){
            cutsManager->setCUTSConfigScriptPath(value);
        }
    }else if(keyName == THREAD_LIMIT && isInt){
        if(cutsManager){
            cutsManager->setMaxThreadCount(intValue);
        }
    }else if(keyName == AUTO_CENTER_VIEW && isBool){
        nodeView->autoCenterAspects(boolValue);
    }else if(keyName == SELECT_ON_CREATION && isBool){
        nodeView->selectNodeOnConstruction(boolValue);
    }else if(keyName == ZOOM_ANCHOR_ON_MOUSE && isBool){
        nodeView->toggleZoomAnchor(boolValue);
    }else if(keyName == TOGGLE_GRID && isBool){
        toggleAndTriggerAction(actionToggleGrid, boolValue);
    }else if(keyName == DOCK_VISIBLE && isBool){
        showDocks(!boolValue);
    }else if(keyName == TOOLBAR_VISIBLE && isBool){
        setToolbarVisibility(!boolValue);
        settingChanged(groupName,TOOLBAR_EXPANDED, appSettings->getSetting(TOOLBAR_EXPANDED));
    }else if(keyName == TOOLBAR_EXPANDED && isBool){
        if(appSettings->getSetting(TOOLBAR_VISIBLE) != "true"){
            toolbarButton->setChecked(boolValue);
            toolbarButton->clicked(boolValue);
        }
    }else if(keyName == ASPECT_D && isBool){
        definitionsToggle->setClicked(boolValue);
        definitionsToggle->aspectToggle_clicked(boolValue, 0);
    }else if(keyName == ASPECT_W && isBool){
        workloadToggle->setClicked(boolValue);
        workloadToggle->aspectToggle_clicked(boolValue, 0);
    }else if(keyName == ASPECT_A && isBool){
        assemblyToggle->setClicked(boolValue);
        assemblyToggle->aspectToggle_clicked(boolValue, 0);
    }else if(keyName == ASPECT_H && isBool){
        hardwareToggle->setClicked(boolValue);
        hardwareToggle->aspectToggle_clicked(boolValue, 0);
    }else if(keyName == JENKINS_URL){
        if(jenkinsManager){
            jenkinsManager->setURL(value);
        }
    }else if(keyName == JENKINS_USER){
        if(jenkinsManager){
            jenkinsManager->setUsername(value);
        }
    }else if(keyName == JENKINS_PASS){
        if(jenkinsManager){
            jenkinsManager->setPassword(value);
        }
    }else if(keyName == JENKINS_TOKEN){
        if(jenkinsManager){
            jenkinsManager->setToken(value);
        }
    }else if(keyName == JENKINS_JOB){
        jenkins_JobName_Changed(value);
    }else if(keyName == SHOW_MANAGEMENT_COMPONENTS && isBool){
        if(nodeView){
            nodeView->showManagementComponents(boolValue);
        }
    }else if(keyName == SHOW_LOCAL_NODE && isBool){
        if(nodeView){
            nodeView->showLocalNode(boolValue);
        }
    }


}


/**
 * @brief MedeaWindow::initialiseGUI
 * Initialise variables, setup widget sizes, organise layout
 * and setup the view, scene and menu.
 */
void MedeaWindow::initialiseGUI()
{    

    // set all gui widget fonts to this
    guiFont = QFont("Verdana");
    guiFont.setPointSizeF(8.5);

    // initialise variables
    controller = 0;
    prevPressedButton = 0;
    controllerThread = 0;

    nodeView = new NodeView();

    progressBar = new QProgressBar(this);
    progressLabel = new QLabel(this);

    notificationsBar = new QLabel("", this);
    notificationTimer = new QTimer(this);

    dataTableBox = new QGroupBox(this);
    dataTable = new QTableView(dataTableBox);
    delegate = new ComboBoxTableDelegate(0);

    menuTitleBox = new QGroupBox(this);
    projectName = new QPushButton("Model");

    // set central widget and window size
    setCentralWidget(nodeView);
    setMinimumSize(MIN_WIDTH, MIN_HEIGHT);

    nodeView->setMinimumSize(MIN_WIDTH, MIN_HEIGHT);
    nodeView->viewport()->setMinimumSize(MIN_WIDTH, MIN_HEIGHT);

    // set the size for the right panel where the view buttons and data table are located
    double rightPanelWidth = RIGHT_PANEL_WIDTH;

    // setup widgets
    QPushButton* menuButton = new QPushButton(getIcon("Actions", "MEDEA_Menu"), "");
    menuButton->setFixedSize(55, 45);
    menuButton->setIconSize(menuButton->size() * 0.75);
    menuButton->setStyleSheet("QPushButton{ background-color: rgb(210,210,210); }"
                              "QPushButton::menu-indicator{ image: none; }");

    projectName->setFlat(true);
    projectName->setStyleSheet("font-size: 16px; text-align: left; padding: 8px;");
    projectName->setFixedWidth(200);

    definitionsToggle = new AspectToggleWidget("Definitions", rightPanelWidth/2, this);
    workloadToggle = new AspectToggleWidget("Workload", rightPanelWidth/2, this);
    assemblyToggle = new AspectToggleWidget("Assembly", rightPanelWidth/2, this);
    hardwareToggle = new AspectToggleWidget("Hardware", rightPanelWidth/2, this);

    // setup progress bar
    progressBar->setVisible(false);
    progressBar->setFixedSize(rightPanelWidth*2, 20);
    progressBar->setStyleSheet("QProgressBar {"
                               "border: 2px solid gray;"
                               "border-radius: 10px;"
                               "background: rgb(240,240,240);"
                               "text-align: center;"
                               "color: black;"
                               "}"

                               "QProgressBar::chunk {"
                               "border-radius: 7px;"
                               "background: rgb(0,204,0);"
                               "}");

    progressLabel->setVisible(false);
    progressLabel->setFixedSize(rightPanelWidth*2, 40);
    progressLabel->setAlignment(Qt::AlignCenter);
    progressLabel->setStyleSheet("color: black; font: 14px;");

    notificationsBar->setVisible(false);
    notificationsBar->setFixedHeight(40);
    notificationsBar->setAlignment(Qt::AlignCenter);
    notificationsBar->setStyleSheet("background-color: rgba(250,250,250,0.85);"
                                    "color: rgb(30,30,30);"
                                    "border-radius: 10px;"
                                    "padding: 0px 15px;"
                                    "font: 14px;");

    QVBoxLayout *progressLayout = new QVBoxLayout();
    progressLayout->addStretch(3);
    progressLayout->addWidget(progressLabel);
    progressLayout->addWidget(progressBar);
    progressLayout->addStretch(4);
    progressLayout->addWidget(notificationsBar);
    progressLayout->setAlignment(notificationsBar, Qt::AlignCenter);

    // setup and add dataTable/dataTableBox widget/layout
    dataTable->setItemDelegateForColumn(2, delegate);
    dataTable->setFixedWidth(rightPanelWidth + 5);
    dataTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    dataTable->setFont(guiFont);

    QVBoxLayout *tableLayout = new QVBoxLayout();
    tableLayout->setMargin(0);
    tableLayout->setContentsMargins(5,0,0,0);
    tableLayout->addWidget(dataTable);

    dataTableBox->setFixedWidth(rightPanelWidth + 10);
    dataTableBox->setContentsMargins(0,0,0,0);
    dataTableBox->setLayout(tableLayout);
    dataTableBox->setStyleSheet("QGroupBox {"
                                "background-color: rgba(0,0,0,0);"
                                "border: 0px;"
                                "margin: 0px;"
                                "padding: 0px;"
                                "}");

    // setup mini map
    minimap = new NodeViewMinimap();
    minimap->setScene(nodeView->scene());

    minimap->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    minimap->setVerticalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
    minimap->setInteractive(false);

    minimap->setFixedSize(rightPanelWidth + 10, rightPanelWidth/1.6);
    minimap->setStyleSheet("background-color: rgba(180,180,180,230);"
                           "border: 2px solid;"
                           "border-color: rgb(80,80,80);"
                           "border-radius: 8px;");
    minimap->centerView();

    // layouts
    QHBoxLayout *mainHLayout = new QHBoxLayout();
    QHBoxLayout *topHLayout = new QHBoxLayout();
    QVBoxLayout *leftVlayout = new QVBoxLayout();
    QVBoxLayout *rightVlayout =  new QVBoxLayout();
    QHBoxLayout *titleLayout = new QHBoxLayout();
    QHBoxLayout *bodyLayout = new QHBoxLayout();
    QVBoxLayout* toolbarContainerLayout = new QVBoxLayout();
    QGridLayout *viewButtonsGrid = new QGridLayout();
    searchLayout = new QHBoxLayout();

    // setup layouts for widgets
    titleLayout->setMargin(0);
    titleLayout->setSpacing(0);
    titleLayout->addWidget(menuButton, 1);
    titleLayout->addSpacerItem(new QSpacerItem(10, 0));
    titleLayout->addWidget(projectName, 1);
    titleLayout->addStretch();

    toolbarContainerLayout->setMargin(0);
    toolbarContainerLayout->setSpacing(0);
    toolbarContainerLayout->setContentsMargins(0, 3, 0, 0);

    menuTitleBox->setLayout(titleLayout);
    menuTitleBox->setFixedHeight(menuButton->height() + 30);
    menuTitleBox->setStyleSheet("QGroupBox{ border: none; background-color: rgba(0,0,0,0); }");
    menuTitleBox->setMask(QRegion(0, (menuTitleBox->height() - menuButton->height()) / 2,
                                  menuButton->width() + projectName->width(), menuButton->height(),
                                  QRegion::Rectangle));

    topHLayout->setMargin(0);
    topHLayout->setSpacing(0);
    topHLayout->addWidget(menuTitleBox, 1);
    topHLayout->addStretch(5);
    topHLayout->addLayout(toolbarContainerLayout);
    topHLayout->addStretch(5);

    leftVlayout->setMargin(0);
    leftVlayout->setSpacing(0);
    leftVlayout->addLayout(topHLayout);
    leftVlayout->addSpacerItem(new QSpacerItem(20, 10));
    leftVlayout->addLayout(bodyLayout);
    leftVlayout->addStretch();

    viewButtonsGrid->setSpacing(5);
    viewButtonsGrid->setContentsMargins(5,0,5,0);
    viewButtonsGrid->addWidget(definitionsToggle, 1, 1);
    viewButtonsGrid->addWidget(workloadToggle, 1, 2);
    viewButtonsGrid->addWidget(assemblyToggle, 2, 1);
    viewButtonsGrid->addWidget(hardwareToggle, 2, 2);

    rightVlayout->setMargin(0);
    rightVlayout->setSpacing(0);
    rightVlayout->setContentsMargins(0, 10, 0, 0);
    rightVlayout->addLayout(searchLayout, 1);
    rightVlayout->addSpacerItem(new QSpacerItem(20, SPACER_HEIGHT));
    rightVlayout->addLayout(viewButtonsGrid);
    rightVlayout->addSpacerItem(new QSpacerItem(20, SPACER_HEIGHT));
    rightVlayout->addWidget(dataTableBox, 4);
    rightVlayout->addSpacerItem(new QSpacerItem(20, SPACER_HEIGHT));
    rightVlayout->addStretch();
    rightVlayout->addWidget(minimap);

    mainHLayout->setMargin(0);
    mainHLayout->setSpacing(0);
    mainHLayout->addLayout(leftVlayout, 4);
    mainHLayout->addLayout(rightVlayout, 1);
    mainHLayout->setContentsMargins(15, 0, 20, 25);
    nodeView->setLayout(mainHLayout);

    // setup the menu, dock, search tools and toolbar
    setupMenu(menuButton);
    setupDock(bodyLayout);
    setupSearchTools();
    setupToolbar(toolbarContainerLayout);
    setupMultiLineBox();

    // add progress bar layout to the body layout after the dock has been set up
    bodyLayout->addStretch(4);
    bodyLayout->addLayout(progressLayout);
    //bodyLayout->setAlignment(progressLayout, Qt::AlignBottom);
    bodyLayout->addStretch(3);
}


/**
 * @brief MedeaWindow::setupMenu
 * Initialise and setup menus and their actions.
 * @param button
 */
void MedeaWindow::setupMenu(QPushButton *button)
{   
    // menu buttons/actions
    menu = new QMenu();
    file_menu = menu->addMenu(getIcon("Actions", "Menu"), "File");
    edit_menu = menu->addMenu(getIcon("Actions", "Edit"), "Edit");
    menu->addSeparator();
    view_menu = menu->addMenu(getIcon("Actions", "MenuView"), "View");
    model_menu = menu->addMenu(getIcon("Actions", "MenuModel"), "Model");
    jenkins_menu = menu->addMenu(getIcon("Actions", "Jenkins_Icon"), "Jenkins");
    menu->addSeparator();

    settings_changeAppSettings = menu->addAction(getIcon("Actions", "Settings"), "Settings");
    file_AboutMedea = menu->addAction(getIcon("Actions", "Info"),"About MEDEA");
    exit = menu->addAction(getIcon("Actions", "Power"), "Exit");

    menu->setFont(guiFont);
    file_menu->setFont(guiFont);
    edit_menu->setFont(guiFont);
    view_menu->setFont(guiFont);
    model_menu->setFont(guiFont);

    file_newProject = file_menu->addAction(getIcon("Actions", "New"), "New Project");
    file_newProject->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
    file_menu->addSeparator();
    file_importGraphML = file_menu->addAction(getIcon("Actions", "Import"), "Import");
    file_importSnippet = file_menu->addAction(getIcon("Actions", "ImportSnippet"), "Import Snippet");
    file_importGraphML->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
    file_menu->addSeparator();
    file_exportGraphML = file_menu->addAction(getIcon("Actions", "Export"), "Export");
    file_exportSnippet = file_menu->addAction(getIcon("Actions", "ExportSnippet"), "Export Snippet");
    file_exportGraphML->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
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
    edit_menu->addSeparator();
    edit_delete = edit_menu->addAction(getIcon("Actions", "Delete"), "Delete Selection");

    view_fitToScreen = view_menu->addAction(getIcon("Actions", "FitToScreen"), "Fit To Screen");
    view_fitToScreen->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Space));
    view_menu->addSeparator();
    view_snapToGrid = view_menu->addAction(getIcon("Actions", "AlignToGrid"), "Snap Selection To Grid");
    view_snapChildrenToGrid = view_menu->addAction(getIcon("Actions", "GridLayout"), "Snap Selection's Children To Grid");
    view_menu->addSeparator();
    view_goToDefinition = view_menu->addAction(getIcon("Actions", "Definition"), "Go To Definition");
    view_goToDefinition->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_D));
    view_goToImplementation = view_menu->addAction(getIcon("Actions", "Implementation"), "Go To Implementation");
    view_goToImplementation->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_I));
    view_menu->addSeparator();
    view_showConnectedNodes = view_menu->addAction(getIcon("Actions", "Connections"), "View Connections");


    model_clearModel = model_menu->addAction(getIcon("Actions", "Clear"), "Clear Model");
    model_menu->addSeparator();
    model_validateModel = model_menu->addAction(getIcon("Actions", "Validate"), "Validate Model");




    button->setMenu(menu);


    //Setup Jenkins Menu

    QString jenkinsJobName = appSettings->getSetting(JENKINS_JOB);
    //Generic Jenkins Functionality.
    jenkins_ImportNodes = jenkins_menu->addAction(getIcon("Actions", "Computer"), "Import Nodes");
    jenkins_ImportNodes->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_J));

    cuts_runGeneration = jenkins_menu->addAction(getIcon("Actions", "Job_Build"), "Launch: Local Deployment");

    jenkins_ExecuteJob = jenkins_menu->addAction(getIcon("Actions", "Job_Build"), "Launch: " + jenkinsJobName);

    if(!jenkinsManager){
        jenkins_menu->setEnabled(false);
    }
    if(jenkinsJobName == ""){
        jenkins_ExecuteJob->setEnabled(false);
    }

    // initially disable these actions
    view_goToDefinition->setEnabled(false);
    view_goToImplementation->setEnabled(false);
    view_showConnectedNodes->setEnabled(false);



    actionSort = new QAction(getIcon("Actions", "Sort"), "Sort", this);
    actionSort->setToolTip("Sort model/selection");

    actionCenter = new QAction(getIcon("Actions", "Crosshair"), "Center Entity", this);
    actionCenter->setToolTip("Center entity without zooming");

    actionZoomToFit = new QAction(getIcon("Actions", "ZoomToFit"), "Zoom to Fit Selection", this);
    actionZoomToFit->setToolTip("Center selection and zoom in to fit");

    actionFitToScreen = new QAction(getIcon("Actions", "FitToScreen"), "Fit Model to Screen", this);
    actionFitToScreen->setToolTip("Show entire Model");

    actionAlignVertically = new QAction(getIcon("Actions", "Align_Vertical"), "Align Selection Vertically", this);
    actionAlignVertically->setToolTip("Align selection vertically");

    actionAlignHorizontally = new QAction(getIcon("Actions", "Align_Horizontal"), "Align Selection Horizontally", this);
    actionAlignHorizontally->setToolTip("Align selection horizontally");

    actionPopupSubview = new QAction(getIcon("Actions", "Popup"), "Show Selection in New Window", this);
    actionPopupSubview->setToolTip("Show selection in new window");

    actionBack = new QAction(getIcon("Actions", "Backward"), "Navigate Back", this);
    actionBack->setToolTip("Navigate back");

    actionForward = new QAction(getIcon("Actions", "Forward"), "Navigate Forward", this);
    actionForward->setToolTip("Navigate forward");

    actionContextMenu = new QAction(getIcon("Actions", "Toolbar"), "Show Context Toolbar", this);
    actionContextMenu->setToolTip("Show context toolbar");

    actionToggleGrid = new QAction(getIcon("Actions", "Grid_On"), "Toggle Grid Lines", this);
    actionToggleGrid->setToolTip("Turn Grid Off");
    actionToggleGrid->setCheckable(true);
    connect(actionToggleGrid, SIGNAL(triggered()), this, SLOT(toggleGridLines()));
}


/**
 * @brief MedeaWindow::setupDock
 * Initialise and setup dock widgets.
 * @param layout
 */
void MedeaWindow::setupDock(QHBoxLayout *layout)
{
    dockStandAloneDialog = new QDialog(this);
    docksArea = new QGroupBox(this);
    dockButtonsBox = new QGroupBox();
    dockButtonsBox->setStyle(QStyleFactory::create("windows"));

    dockLayout = new QVBoxLayout();
    QVBoxLayout* dockDialogLayout = new QVBoxLayout();
    QVBoxLayout* dockAreaLayout = new QVBoxLayout();
    QHBoxLayout* dockButtonsHlayout = new QHBoxLayout();

    partsButton = new DockToggleButton("P", this);
    hardwareNodesButton = new DockToggleButton("H", this);

    definitionsButton = new DockToggleButton("D", this);

    partsDock = new PartsDockScrollArea("Parts", nodeView, partsButton);
    definitionsDock = new DefinitionsDockScrollArea("Definitions", nodeView, definitionsButton);
    hardwareDock = new HardwareDockScrollArea("Nodes", nodeView, hardwareNodesButton);

    // width of the containers are fixed
    boxWidth = (partsButton->getWidth()*3) + 30;

    // set buttonBox's size and get rid of its border
    dockButtonsBox->setStyleSheet("border: 0px; padding: 0px 7px;");
    dockButtonsBox->setFixedSize(boxWidth, 60);

    // set dock's size
    partsDock->setFixedWidth(boxWidth);
    definitionsDock->setFixedWidth(boxWidth);
    hardwareDock->setFixedWidth(boxWidth);

    // set dock's font
    partsDock->widget()->setFont(guiFont);
    definitionsDock->widget()->setFont(guiFont);
    hardwareDock->widget()->setFont(guiFont);

    // set size policy for buttons
    partsButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    hardwareNodesButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    definitionsButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // remove extra space in layouts
    dockButtonsHlayout->setMargin(0);
    dockButtonsHlayout->setSpacing(0);
    dockAreaLayout->setMargin(0);
    dockAreaLayout->setSpacing(0);
    dockDialogLayout->setMargin(8);
    dockDialogLayout->setSpacing(0);

    // add widgets to/and layouts
    dockButtonsHlayout->addWidget(partsButton);
    dockButtonsHlayout->addWidget(definitionsButton);
    dockButtonsHlayout->addWidget(hardwareNodesButton);
    dockButtonsHlayout->setAlignment(Qt::AlignHCenter);
    dockButtonsBox->setLayout(dockButtonsHlayout);

    dockLayout->addWidget(dockButtonsBox);
    dockLayout->addWidget(partsDock);
    dockLayout->addWidget(definitionsDock);
    dockLayout->addWidget(hardwareDock);
    dockLayout->addStretch();

    dockAreaLayout->addLayout(dockLayout);
    docksArea->setLayout(dockAreaLayout);
    docksArea->setFixedWidth(boxWidth);
    docksArea->setStyleSheet("border: none; padding: 0px; margin: 0px; background-color: rgba(0,0,0,0);");
    docksArea->setMask(QRegion(0, 0, boxWidth, dockButtonsBox->height(), QRegion::Rectangle));
    docksArea->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    layout->addWidget(docksArea, 1);

    dockStandAloneDialog->setVisible(false);
    dockStandAloneDialog->setLayout(dockDialogLayout);
    dockStandAloneDialog->setFixedWidth(boxWidth + 15);
    dockStandAloneDialog->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    dockStandAloneDialog->setStyleSheet("QDialog{ background-color: rgba(175,175,175,255); }");
    dockStandAloneDialog->setWindowTitle("MEDEA - Dock");
}


/**
 * @brief MedeaWindow::setupSearchTools
 * @param layout
 */
void MedeaWindow::setupSearchTools()
{
    searchBarDefaultText = "Search Here...";
    searchBar = new QLineEdit(searchBarDefaultText, this);
    searchButton = new QPushButton(getIcon("Actions", "Search"), "");
    searchOptionButton = new QPushButton(getIcon("Actions", "Settings"), "");
    searchOptionMenu = new QMenu(searchOptionButton);
    //searchSuggestions = new QListView(searchButton);
    searchResults = new QDialog(this);

    QVBoxLayout* layout = new QVBoxLayout();
    QWidget* scrollableWidget = new QWidget(this);
    scrollableSearchResults = new QScrollArea(this);
    scrollableSearchResults->setWidget(scrollableWidget);

    QVBoxLayout* resultsMainLayout = new QVBoxLayout();
    resultsLayout = new QVBoxLayout();
    int rightPanelWidth = RIGHT_PANEL_WIDTH;
    int searchBarHeight = 28;

    QHBoxLayout* headerLayout = new QHBoxLayout();
    QLabel* objectLabel = new QLabel("Entity Label:", this);
    QLabel* parentLabel = new QLabel("Parent Entity Label:", this);
    QLabel* iconHolder = new QLabel(this);
    int searchItemMinWidth = 300;
    int marginOffset = 8;

    iconHolder->setFixedWidth(42);
    objectLabel->setMinimumWidth(searchItemMinWidth/2 - marginOffset);
    parentLabel->setMinimumWidth(searchItemMinWidth/2 - marginOffset);

    headerLayout->setMargin(2);
    headerLayout->setSpacing(5);
    headerLayout->addWidget(objectLabel);
    headerLayout->addWidget(iconHolder);
    headerLayout->addWidget(parentLabel);

    resultsMainLayout->addLayout(headerLayout);
    resultsMainLayout->addLayout(resultsLayout);
    resultsMainLayout->addStretch();

    searchButton->setFixedSize(30, searchBarHeight);
    searchButton->setIconSize(searchButton->size()*0.65);

    searchOptionButton->setFixedSize(30, searchBarHeight);
    searchOptionButton->setIconSize(searchButton->size()*0.7);
    searchOptionButton->setCheckable(true);

    searchBar->setFixedSize(rightPanelWidth - (searchButton->width()*2), searchBarHeight-3);
    searchBar->setStyleSheet("background-color: rgb(230,230,230);");
    searchBar->setPlaceholderText(searchBarDefaultText);

    //searchSuggestions->setViewMode(QListView::ListMode);
    //searchSuggestions->setVisible(false);

    scrollableWidget->setMinimumWidth(rightPanelWidth + 110);
    scrollableWidget->setLayout(resultsMainLayout);
    scrollableSearchResults->setWidget(scrollableWidget);
    scrollableSearchResults->setWidgetResizable(true);
    layout->addWidget(scrollableSearchResults);

    searchResults->setLayout(layout);
    searchResults->setMinimumWidth(rightPanelWidth + 110);
    searchResults->setMinimumHeight(height() / 2);
    searchResults->setWindowTitle("Search Results");
    searchResults->setVisible(false);

    searchLayout->setContentsMargins(5,0,5,0);
    searchLayout->addWidget(searchBar, 3);
    searchLayout->addWidget(searchButton, 1);
    searchLayout->addWidget(searchOptionButton, 1);

    // setup search option widgets and menu for view aspects
    QWidgetAction* aspectsAction = new QWidgetAction(this);
    QLabel* aspectsLabel = new QLabel("Aspect(s):", this);
    QGroupBox* aspectsGroup = new QGroupBox(this);
    QHBoxLayout* aspectsLayout = new QHBoxLayout();

    viewAspectsBarDefaultText = "Entire Model";
    viewAspectsBar = new QLineEdit(viewAspectsBarDefaultText, this);
    viewAspectsButton = new QPushButton(getIcon("Actions", "Arrow_Down"), "");
    viewAspectsMenu = new QMenu(viewAspectsButton);

    aspectsLabel->setMinimumWidth(50);
    aspectsLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    viewAspectsButton->setFixedSize(20, 20);
    viewAspectsButton->setIconSize(viewAspectsButton->size()*0.6);
    viewAspectsButton->setCheckable(true);
    viewAspectsBar->setFixedWidth(rightPanelWidth - viewAspectsButton->width() - aspectsLabel->width() - 30);
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
    foreach (QString aspect, nodeView->getAllAspects()) {
        QWidgetAction* action = new QWidgetAction(this);
        QCheckBox* checkBox = new QCheckBox(aspect, this);
        checkBox->setFont(guiFont);
        checkBox->setStyleSheet("QCheckBox::indicator{ width: 25px; height: 25px; }"
                                "QCheckBox::checked{ color: green; font-weight: bold; }");
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
    nodeKindsBar->setFixedWidth(rightPanelWidth - nodeKindsButton->width() - kindsLabel->width() - 30);
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

    searchBar->setFont(guiFont);
    viewAspectsBar->setFont(guiFont);
    nodeKindsBar->setFont(guiFont);
    objectLabel->setFont(guiFont);
    parentLabel->setFont(guiFont);
    aspectsLabel->setFont(guiFont);
    kindsLabel->setFont(guiFont);

    // add widget actions and their menus to the main search option menu
    searchOptionMenu->addAction(aspectsAction);
    searchOptionMenu->addAction(kindsAction);
    searchOptionMenu->setVisible(false);
}


/**
 * @brief MedeaWindow::setupToolbar
 * Initialise and setup toolbar widgets.
 */
void MedeaWindow::setupToolbar(QVBoxLayout *layout)
{
    toolbar = new QToolBar();
    toolbarLayout = new QVBoxLayout();

    // stylesheet for QToolButtons and the toolbar's separators
    setStyleSheet("QToolButton{"
                  "border: 1px solid;"
                  "border-color: rgba(160,160,160,225);"
                  "border-radius: 10px;"
                  "background-color: rgba(200,200,200,225);"
                  "margin: 0px 1px;"
                  "}"
                  "QToolButton:hover{"
                  "border: 2px solid;"
                  "border-color: rgba(140,140,140,225);"
                  "background-color: rgba(250,250,250,230);"
                  "}"
                  "QToolButton:disabled{ background-color: rgba(150,150,150,225);}"
                  "QToolButton:pressed{ background-color: rgba(230,230,230,230);}"
                  "QToolBar::separator { background-color: rgba(0,0,0,0); }"
                  "QMessageBox{background-color:" + palette().color(QWidget::backgroundRole()).name() + ";}"
                  );

    toolbarButton = new QToolButton(this);
    toolbarButton->setFixedSize(TOOLBAR_BUTTON_WIDTH, TOOLBAR_BUTTON_HEIGHT / 2);
    toolbarButton->setCheckable(true);
    toolbarButton->setStyleSheet("QToolButton{ background-color: rgba(200,200,200,225); border-radius: 5px; }"
                                 "QToolButton:hover{ background-color: rgba(250,250,250,235); }");




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
    constructToolbarButton(toolbar, actionContextMenu, TOOLBAR_CONTEXT);
    constructToolbarButton(toolbar, edit_delete, TOOLBAR_DELETE_ENTITIES);
    constructToolbarButton(toolbar, actionPopupSubview, TOOLBAR_POPUP_SUBVIEW);

    toolbar->addSeparator();
    constructToolbarButton(toolbar, edit_cut, TOOLBAR_CUT);
    constructToolbarButton(toolbar, edit_copy, TOOLBAR_COPY);
    constructToolbarButton(toolbar, edit_paste, TOOLBAR_PASTE);
    constructToolbarButton(toolbar, edit_replicate, TOOLBAR_REPLICATE);
    toolbar->addSeparator();


    constructToolbarButton(toolbar, actionCenter, TOOLBAR_CENTER_ON_ENTITY);
    constructToolbarButton(toolbar, actionZoomToFit, TOOLBAR_ZOOM_TO_FIT);
    constructToolbarButton(toolbar, actionSort, TOOLBAR_SORT);
    constructToolbarButton(toolbar, actionFitToScreen, TOOLBAR_FIT_TO_SCREEN);
    toolbar->addSeparator();
    constructToolbarButton(toolbar, actionToggleGrid, TOOLBAR_GRID_LINES);

    //toolbar->addSeparator();
    constructToolbarButton(toolbar, actionAlignVertically, TOOLBAR_VERT_ALIGN);
    constructToolbarButton(toolbar, actionAlignHorizontally, TOOLBAR_HORIZ_ALIGN);
    //toolbar->addSeparator();
    constructToolbarButton(toolbar, actionBack, TOOLBAR_BACK);
    constructToolbarButton(toolbar, actionForward, TOOLBAR_FORWARD);


    QHBoxLayout* toolbarHLayout = new QHBoxLayout();
    toolbarHLayout->addStretch();
    toolbarHLayout->addWidget(toolbarButton);
    toolbarHLayout->addStretch();


    layout->addLayout(toolbarHLayout);
    layout->addLayout(toolbarLayout);
    layout->addStretch();
    toolbarLayout->addWidget(toolbar);



    toolbar->setStyle(QStyleFactory::create("windows"));

    //Implement Button.

}

bool MedeaWindow::constructToolbarButton(QToolBar* toolbar, QAction *action, QString actionName)
{
    if(toolbar && action && actionName != ""){
        if(actionName == TOOLBAR_BACK || actionName == TOOLBAR_FORWARD){
            return false;
        }

        QSize buttonSize = QSize(TOOLBAR_BUTTON_WIDTH, TOOLBAR_BUTTON_HEIGHT);
        toolbar->setIconSize(buttonSize*0.6);

        ActionButton* actionButton = new ActionButton(action);
        actionButton->setFixedSize(buttonSize);
        QAction* toolAction = toolbar->addWidget(actionButton);
        if(!toolbarActionLookup.contains(actionName)){
            toolbarActionLookup[actionName] = toolAction;
        }else{
            qCritical() << "Duplicate Actions";
        }
        if(!toolbarButtonLookup.contains(actionName)){
            toolbarButtonLookup[actionName] = actionButton;
        }else{
            qCritical() << "Duplicate Actions";
        }

        return true;
    }
    return false;
}



/**
 * @brief MedeaWindow::setupController
 */
void MedeaWindow::setupController()
{

    if (controller) {
        //controller->deleteLater();
        delete controller;
        controller = 0;
    }
    if (controllerThread) {
        controllerThread->terminate();
        //controllerThread->deleteLater();
        delete controllerThread;
        controllerThread = 0;
    }

    controller = new NewController();

    if (THREADING) {
        //IMPLEMENT THREADING!
        controllerThread = new QThread();
        controllerThread->start();
        controller->moveToThread(controllerThread);

    }
    connect(this, SIGNAL(window_ConnectViewAndSetupModel(NodeView*)), controller, SLOT(connectViewAndSetupModel(NodeView*)));

    emit window_ConnectViewAndSetupModel(nodeView);
}


/**
 * @brief MedeaWindow::resetGUI
 * This is called everytime a new project is created.
 */
void MedeaWindow::resetGUI()
{
    prevPressedButton = 0;
    setupController();
}


/**
 * @brief MedeaWindow::makeConnections
 * Connect signals and slots.
 */
void MedeaWindow::makeConnections()
{
    validateResults.connectToWindow(this);

    connect(partsDock, SIGNAL(dock_openDefinitionsDock()), this, SLOT(forceOpenDefinitionsDock()));
    connect(hardwareNodesButton, SIGNAL(dockButton_dockOpen(bool)), nodeView, SLOT(hardwareDockOpened(bool)));


    connect(nodeView, SIGNAL(view_OpenHardwareDock()), this, SLOT(jenkinsNodesLoaded()));
    connect(nodeView, SIGNAL(view_ModelReady()), this, SLOT(modelReady()));
    connect(nodeView, SIGNAL(view_ImportSnippet(QString)), this, SLOT(importSnippet(QString)));

    connect(this, SIGNAL(window_ImportSnippet(QString,QString)), nodeView, SLOT(importSnippet(QString,QString)));
    connect(this, SIGNAL(window_AspectsChanged(QStringList)), nodeView, SLOT(setAspects(QStringList)));
    connect(this, SIGNAL(window_DisplayMessage(MESSAGE_TYPE,QString,QString)), nodeView, SLOT(showMessage(MESSAGE_TYPE,QString,QString)));
    connect(nodeView, SIGNAL(view_GUIAspectChanged(QStringList)), this, SLOT(setViewAspects(QStringList)));
    connect(nodeView, SIGNAL(view_updateMenuActionEnabled(QString,bool)), this, SLOT(setMenuActionEnabled(QString,bool)));
    connect(nodeView, SIGNAL(view_SetAttributeModel(AttributeTableModel*)), this, SLOT(setAttributeModel(AttributeTableModel*)));
    connect(nodeView, SIGNAL(view_updateProgressStatus(int,QString)), this, SLOT(updateProgressStatus(int,QString)));
    connect(nodeView, SIGNAL(view_ProjectCleared()), this, SLOT(projectCleared()));
    connect(file_importSnippet, SIGNAL(triggered()), nodeView, SLOT(request_ImportSnippet()));
    connect(file_exportSnippet, SIGNAL(triggered()), nodeView, SLOT(exportSnippet()));


    //connect(nodeView, SIGNAL(view_showWindowToolbar()), this, SLOT(showWindowToolbar()));
    //connect(this, SIGNAL(window_highlightDeployment(bool)), nodeView, SLOT(highlightDeployment(bool)));

    connect(nodeView, SIGNAL(customContextMenuRequested(QPoint)), nodeView, SLOT(showToolbar(QPoint)));
    connect(nodeView, SIGNAL(view_ViewportRectChanged(QRectF)), minimap, SLOT(viewportRectChanged(QRectF)));

    connect(minimap, SIGNAL(minimap_Pressed(QMouseEvent*)), nodeView, SLOT(minimapPressed(QMouseEvent*)));
    connect(minimap, SIGNAL(minimap_Moved(QMouseEvent*)), nodeView, SLOT(minimapMoved(QMouseEvent*)));
    connect(minimap, SIGNAL(minimap_Released(QMouseEvent*)), nodeView, SLOT(minimapReleased(QMouseEvent*)));
    connect(minimap, SIGNAL(minimap_Scrolled(int)), nodeView, SLOT(scrollEvent(int)));
    connect(nodeView, SIGNAL(view_ModelSizeChanged()), minimap, SLOT(centerView()));

    connect(notificationTimer, SIGNAL(timeout()), notificationsBar, SLOT(hide()));
    connect(notificationTimer, SIGNAL(timeout()), this, SLOT(checkNotificationsQueue()));
    connect(nodeView, SIGNAL(view_displayNotification(QString,int,int)), this, SLOT(displayNotification(QString,int,int)));

    connect(projectName, SIGNAL(clicked()), nodeView, SLOT(selectModel()));

    connect(file_newProject, SIGNAL(triggered()), this, SLOT(on_actionNew_Project_triggered()));
    connect(file_importGraphML, SIGNAL(triggered()), this, SLOT(on_actionImport_GraphML_triggered()));
    connect(file_exportGraphML, SIGNAL(triggered()), this, SLOT(on_actionExport_GraphML_triggered()));
    connect(file_AboutMedea, SIGNAL(triggered()), this, SLOT(aboutMedea()));

    connect(this, SIGNAL(window_ExportProject()), nodeView, SLOT(exportProject()));
    connect(this, SIGNAL(window_ImportProjects(QStringList)), nodeView, SLOT(importProjects(QStringList)));
    connect(this, SIGNAL(window_LoadJenkinsNodes(QString)), nodeView, SLOT(loadJenkinsNodes(QString)));

    connect(edit_undo, SIGNAL(triggered()), this, SLOT(menuActionTriggered()));
    connect(edit_redo, SIGNAL(triggered()), this, SLOT(menuActionTriggered()));

    connect(edit_undo, SIGNAL(triggered()), nodeView, SLOT(undo()));
    connect(edit_redo, SIGNAL(triggered()), nodeView, SLOT(redo()));
    connect(edit_cut, SIGNAL(triggered()), nodeView, SLOT(cut()));
    connect(edit_copy, SIGNAL(triggered()), nodeView, SLOT(copy()));
    connect(edit_replicate, SIGNAL(triggered()), nodeView, SLOT(replicate()));
    connect(edit_paste, SIGNAL(triggered()), this, SLOT(on_actionPaste_triggered()));
    connect(this, SIGNAL(window_PasteData(QString)), nodeView, SLOT(paste(QString)));

    connect(view_fitToScreen, SIGNAL(triggered()), nodeView, SLOT(fitToScreen()));
    connect(view_snapToGrid, SIGNAL(triggered()), nodeView, SLOT(snapSelectionToGrid()));
    connect(view_snapChildrenToGrid, SIGNAL(triggered()), nodeView, SLOT(snapChildrenToGrid()));
    connect(view_goToImplementation, SIGNAL(triggered()), nodeView, SLOT(goToImplementation()));
    connect(view_goToDefinition, SIGNAL(triggered()), nodeView, SLOT(goToDefinition()));
    connect(view_showConnectedNodes, SIGNAL(triggered()), nodeView, SLOT(showConnectedNodes()));

    connect(model_clearModel, SIGNAL(triggered()), nodeView, SLOT(clearModel()));

    connect(model_validateModel, SIGNAL(triggered()), this, SLOT(on_actionValidate_triggered()));

    //Jenkins Settings
    connect(jenkins_ExecuteJob, SIGNAL(triggered()), this, SLOT(jenkinsExport()));
    connect(cuts_runGeneration, SIGNAL(triggered()), this, SLOT(cutsExport()));

    connect(jenkins_ImportNodes, SIGNAL(triggered()), this, SLOT(on_actionImportJenkinsNode()));

    connect(settings_changeAppSettings, SIGNAL(triggered()), appSettings, SLOT(show()));
    connect(actionToggleGrid, SIGNAL(triggered(bool)), nodeView, SLOT(toggleGridLines(bool)));

    connect(exit, SIGNAL(triggered()), this, SLOT(on_actionExit_triggered()));

    connect(searchBar, SIGNAL(cursorPositionChanged(int,int)), this, SLOT(updateSearchLineEdits()));
    connect(searchBar, SIGNAL(editingFinished()), this, SLOT(updateSearchLineEdits()));
    connect(searchBar, SIGNAL(returnPressed()), this, SLOT(on_actionSearch_triggered()));

    connect(searchButton, SIGNAL(clicked()), this, SLOT(on_actionSearch_triggered()));
    connect(searchOptionButton, SIGNAL(clicked(bool)), this, SLOT(searchMenuButtonClicked(bool)));
    connect(viewAspectsButton, SIGNAL(clicked(bool)), this, SLOT(searchMenuButtonClicked(bool)));
    connect(nodeKindsButton, SIGNAL(clicked(bool)), this, SLOT(searchMenuButtonClicked(bool)));

    connect(searchOptionMenu, SIGNAL(aboutToHide()), this, SLOT(searchMenuClosed()));
    connect(viewAspectsMenu, SIGNAL(aboutToHide()), this, SLOT(searchMenuClosed()));
    connect(nodeKindsMenu, SIGNAL(aboutToHide()), this, SLOT(searchMenuClosed()));

    connect(toolbarButton, SIGNAL(clicked(bool)), this, SLOT(showWindowToolbar(bool)));

    connect(edit_delete, SIGNAL(triggered()), nodeView, SLOT(deleteSelection()));
    connect(actionFitToScreen, SIGNAL(triggered()), nodeView, SLOT(fitToScreen()));
    connect(actionCenter, SIGNAL(triggered()), nodeView, SLOT(centerOnItem()));
    connect(actionSort, SIGNAL(triggered()), nodeView, SLOT(sort()));

    connect(actionPopupSubview, SIGNAL(triggered()), nodeView, SLOT(constructNewView()));

    connect(actionZoomToFit, SIGNAL(triggered()), nodeView, SLOT(centerItem()));

    connect(actionAlignHorizontally, SIGNAL(triggered()), nodeView, SLOT(alignSelectionHorizontally()));
    connect(actionAlignVertically, SIGNAL(triggered()), nodeView, SLOT(alignSelectionVertically()));
    connect(actionContextMenu, SIGNAL(triggered()), nodeView, SLOT(showToolbar()));
    connect(actionBack, SIGNAL(triggered()), nodeView, SLOT(moveViewBack()));
    connect(actionForward, SIGNAL(triggered()), nodeView, SLOT(moveViewForward()));

    connect(nodeView, SIGNAL(view_ExportedProject(QString)), this, SLOT(writeExportedProject(QString)));
    connect(nodeView, SIGNAL(view_ExportedSnippet(QString,QString)), this, SLOT(writeExportedSnippet(QString,QString)));


    connect(nodeView, SIGNAL(view_SetClipboardBuffer(QString)), this, SLOT(setClipboard(QString)));
    connect(nodeView, SIGNAL(view_ProjectNameChanged(QString)), this, SLOT(updateWindowTitle(QString)));

    connect(dockStandAloneDialog, SIGNAL(finished(int)), this, SLOT(detachedDockClosed()));
    // DEMO CHANGE
    //connect(toolbarStandAloneDialog, SIGNAL(finished(int)), this, SLOT(detachedToolbarClosed()));

    connect(this, SIGNAL(clearDocks()), hardwareDock, SLOT(clear()));
    connect(this, SIGNAL(clearDocks()), definitionsDock, SLOT(clear()));

    
    connect(nodeView, SIGNAL(view_nodeDeleted(QString,QString)), partsDock, SLOT(nodeDeleted(QString, QString)));
    connect(nodeView, SIGNAL(view_nodeDeleted(QString,QString)), hardwareDock, SLOT(nodeDeleted(QString, QString)));
    connect(nodeView, SIGNAL(view_nodeDeleted(QString,QString)), definitionsDock, SLOT(nodeDeleted(QString, QString)));

    connect(nodeView, SIGNAL(view_nodeSelected()), this, SLOT(graphicsItemSelected()));

    connect(nodeView, SIGNAL(view_nodeSelected()), partsDock, SLOT(updateCurrentNodeItem()));
    connect(nodeView, SIGNAL(view_nodeSelected()), hardwareDock, SLOT(updateCurrentNodeItem()));
    connect(nodeView, SIGNAL(view_nodeSelected()), definitionsDock, SLOT(updateCurrentNodeItem()));

    connect(nodeView, SIGNAL(view_nodeConstructed(NodeItem*)), partsDock, SLOT(updateDock()));
    connect(nodeView, SIGNAL(view_nodeConstructed(NodeItem*)), hardwareDock, SLOT(nodeConstructed(NodeItem*)));
    connect(nodeView, SIGNAL(view_nodeConstructed(NodeItem*)), definitionsDock, SLOT(nodeConstructed(NodeItem*)));

    
    connect(nodeView, SIGNAL(view_edgeDeleted(QString,QString)), hardwareDock, SLOT(edgeDeleted(QString, QString)));
    connect(nodeView, SIGNAL(view_edgeDeleted(QString,QString)), definitionsDock, SLOT(edgeDeleted(QString, QString)));

	connect(nodeView, SIGNAL(view_edgeConstructed()), hardwareDock, SLOT(updateDock()));
    connect(nodeView, SIGNAL(view_edgeConstructed()), definitionsDock, SLOT(updateDock()));

    connect(dataTable, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(dataTableDoubleClicked(QModelIndex)));

    //For mac
    addAction(exit);
    addAction(file_newProject);
    addAction(file_importGraphML);
    addAction(file_exportGraphML);

    addAction(edit_undo);
    addAction(edit_redo);
    addAction(edit_cut);
    addAction(edit_copy);
    addAction(edit_paste);
    addAction(edit_replicate);
    addAction(view_fitToScreen);
    addAction(view_snapToGrid);
    addAction(view_snapChildrenToGrid);
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
    addAction(file_AboutMedea);
}


/**
 * @brief MedeaWindow::resizeEvent
 * This is called when the main window is resized.
 * It doesn't however, always pick up on maximise/un-maximise events.
 * @param event
 */
void MedeaWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    if(isWindowMaximized != this->isMaximized() && maximizedSettingInitiallyChanged){
        if(nodeView){
            nodeView->fitToScreen();
        }
        maximizedSettingInitiallyChanged = false;
    }
    isWindowMaximized = this->isMaximized();    updateWidgetsOnWindowChanged();

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
        updateWidgetsOnWindowChanged();
    }
}

void MedeaWindow::initialiseJenkinsManager()
{
    jenkinsManager = 0;

    QString jenkinsUrl = appSettings->getSetting(JENKINS_URL);
    QString jenkinsUser = appSettings->getSetting(JENKINS_USER);
    QString jenkinsPass = appSettings->getSetting(JENKINS_PASS);
    QString jenkinsToken = appSettings->getSetting(JENKINS_TOKEN);

    if(jenkinsUrl != "" && jenkinsUser != "" && jenkinsPass != ""){
        QString binaryPath = applicationDirectory + "Resources/Binaries/";


        jenkinsManager = new JenkinsManager(binaryPath, jenkinsUrl, jenkinsUser, jenkinsPass, jenkinsToken);
        connect(jenkinsManager, SIGNAL(gotInvalidSettings(QString)), this, SLOT(invalidJenkinsSettings(QString)));
    }
}

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
    cutsManager = new CUTSManager();
    cutsManager->setXalanJPath(xalanJPath);
    cutsManager->setXSLTransformPath(transformPath);
    cutsManager->setMaxThreadCount(threadLimit);
    cutsManager->setScriptsPath(scriptsPath);
}



void MedeaWindow::jenkins_InvokeJob(QString filePath)
{
    QString jenkinsJobName = appSettings->getSetting(JENKINS_JOB);

    if(jenkinsManager){
        JenkinsStartJobWidget* jenkinsSJ = new JenkinsStartJobWidget(this, jenkinsManager);
        jenkinsSJ->requestJob(jenkinsJobName, filePath);
    }
}

void MedeaWindow::cuts_runDeployment(QString filePath)
{
    if(cutsManager){
        CUTSExecutionWidget* cWidget = new CUTSExecutionWidget(this, cutsManager);
        cWidget->setGraphMLPath(filePath);
        cWidget->show();
    }

}

void MedeaWindow::validate_Exported(QString tempModelPath)
{
    QString xalanJPath = applicationDirectory + "/Resources/Binaries/";
    QString scriptPath = applicationDirectory + "/Resources/Scripts/";
    QString transformsPath = applicationDirectory + "/Resources/Transforms/";

    // transform .graphml to report.xml
    // The MEDEA.xsl transform is produced by Schematron/iso_svrl_for_xslt1.xsl
    QString program = "java";
    QStringList arguments;

    validation_report_path = QDir::tempPath() + "/" + projectName->text() + "_ValidateReport.xml";

    arguments << "-jar" << xalanJPath + "xalan.jar"
              << "-in" << tempModelPath
              << "-xsl" << transformsPath + "MEDEA.xsl"
              << "-out" << validation_report_path;

    // alternative if using xalan-c
    //  QString program =  "Xalan";
    //  arguments << "-o" << outputFile
    //            << inputFile
    //            << "MEDEA.xsl";

    QProcess *myProcess = new QProcess(this);
    //myProcess->setWorkingDirectory(scriptPath);

    connect(myProcess, SIGNAL(finished(int)), this, SLOT(validationComplete(int)));
    connect(myProcess, SIGNAL(finished(int)), myProcess, SLOT(deleteLater()));
    myProcess->start(program, arguments);
}

void MedeaWindow::toggleGridLines()
{
    if(actionToggleGrid){
        if(actionToggleGrid->isChecked()){
            actionToggleGrid->setIcon(nodeView->getImage("Actions", "Grid_On"));
            //actionToggleGrid->setIcon(getIcon("Actions", "Grid_On"));
            actionToggleGrid->setToolTip("Press to Turn Grid Off");
        }else{
            actionToggleGrid->setIcon(nodeView->getImage("Actions", "Grid_Off"));
            //actionToggleGrid->setIcon(getIcon("Actions", "Grid_Off"));
            actionToggleGrid->setToolTip("Press to Turn Grid On");
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



    QMessageBox::about(this, "About MEDEA", aboutString);
}

void MedeaWindow::invalidJenkinsSettings(QString message)
{
    emit window_DisplayMessage(CRITICAL, "Invalid Jenkins Settings", message);
}

void MedeaWindow::jenkinsNodesLoaded()
{
    // if the hardware dock isn't already open, open it
    if (hardwareNodesButton->isEnabled() && !hardwareNodesButton->isSelected()) {
        hardwareNodesButton->pressed();
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
 * @brief MedeaWindow::updateWidgetsOnWindowChanged
 * This is called when the GUI widgets size/pos need to be
 * updated after the window has been changed.
 */
void MedeaWindow::updateWidgetsOnWindowChanged()
{
    // update widget sizes, containers and and masks
    boxHeight = height() - menuTitleBox->height() - dockButtonsBox->height() - SPACER_HEIGHT;
    docksArea->setFixedHeight(boxHeight*2);
    dockStandAloneDialog->setFixedHeight(boxHeight + dockButtonsBox->height() + SPACER_HEIGHT/2);

	updateWidgetMask(docksArea, dockButtonsBox, true);
    updateDataTable();
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
    if (nodeView && controller) {
        nodeView->updateViewCenterPoint();
        nodeView->recenterView();
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
    settingsLoading = true;
    appSettings->loadSettings();
    settingsLoading = false;

    QStringList allKinds = nodeView->getAllNodeKinds();
    QStringList guiKinds = nodeView->getGUIConstructableNodeKinds();

    // this only needs to happen once, the whole time the application is open
    partsDock->addDockNodeItems(guiKinds);

    // populate view aspects menu  once the nodeView and controller have been
    // constructed and connected - should only need to do this once
    foreach (QString kind, allKinds) {
        QWidgetAction* action = new QWidgetAction(this);
        QCheckBox* checkBox = new QCheckBox(kind, this);
        checkBox->setFont(guiFont);
        checkBox->setStyleSheet("QCheckBox::indicator{ width: 25px; height: 25px; }"
                                "QCheckBox::checked{ color: green; font-weight: bold; }");
        connect(checkBox, SIGNAL(clicked()), this, SLOT(updateSearchLineEdits()));
        action->setDefaultWidget(checkBox);
        nodeKindsMenu->addAction(action);
    }

    // hide initial notifications and reset timer
    notificationsBar->hide();
    notificationTimer->stop();

    //Reset time.
    leftOverTime = 0;

    //We have finished loading settings. reset state of Controller undo states.
    nodeView->view_ClearHistory();

    // initially disable all the docks
    nodeView->view_nodeSelected();
}


/**
 * @brief MedeaWindow::aspectToggleClicked
 * @param checked
 * @param state
 */
void MedeaWindow::aspectToggleClicked(bool checked, int state)
{
    AspectToggleWidget* senderAspect = qobject_cast<AspectToggleWidget*>(QObject::sender());

    if (senderAspect) {

        QStringList newAspects = checkedViewAspects;
        QString aspect = senderAspect->getText();

        if (aspect == "Interface") {
            aspect = "Definitions";
        }

        if (!checked) {
            newAspects.removeAll(aspect);
        } else {

            switch (state) {
            case -1:
                break;
            case 0:
                if (!newAspects.contains(aspect)) {
                    newAspects.append(aspect);
                }
                break;
            case 1:
                newAspects.clear();
                newAspects.append(aspect);
                emit window_aspectToggleDoubleClicked(senderAspect);
                break;
            case 2:
                if (!newAspects.contains(aspect)) {
                    newAspects.append(aspect);
                }
                break;
            }
        }

        window_AspectsChanged(newAspects);

        // if state == MIDDLECLICKED, center on the toggled
        // aspect after the list of aspects has been updated
        if (checked && state == 2) {
            nodeView->centerAspect(aspect);
        }
    }
}

void MedeaWindow::jenkinsExport()
{
    jenkins_TempExport = true;
    exportTempFile();
}

void MedeaWindow::cutsExport()
{
    cuts_TempExport = true;
    exportTempFile();
}

void MedeaWindow::validateExport()
{
    validate_TempExport = true;
    exportTempFile();
}

void MedeaWindow::exportTempFile()
{
    tempExport = true;
    //tempFileName = "C:/MEDEA/build-MEDEA-Desktop_Qt_5_3_0_MSVC2010_OpenGL_32bit-Debug2/debug/Resources/Scripts/"+ projectName->text() + ".graphml";
    tempFileName = QDir::tempPath() + "/" + projectName->text() + ".graphml";
    enableTempExport(false);
    emit window_ExportProject();
}


void MedeaWindow::saveSettings()
{
    //Write Settings on Quit.
    if(appSettings){
        appSettings->setSetting(TOOLBAR_EXPANDED, toolbarButton->isChecked());

        if(isMaximized()){
            appSettings->setSetting(WINDOW_MAX_STATE, isMaximized());
        }else{
            appSettings->setSetting(WINDOW_W, size().width());
            appSettings->setSetting(WINDOW_H, size().height());
            appSettings->setSetting(WINDOW_X, pos().x());
            appSettings->setSetting(WINDOW_Y, pos().y());
        }
    }
}


/**
 * @brief MedeaWindow::gotJenkinsNodeGraphML Called by a JenkinsRequest with the GraphML string which represents the nodes in the Jenkins Server. Data gets imported into the model.
 * @param jenkinsXML The GraphML representation of the Jenkins Nodes List.
 */
void MedeaWindow::gotJenkinsNodeGraphML(QString jenkinsXML)
{
    if(jenkinsXML != ""){
        // import Jenkins
        emit window_LoadJenkinsNodes(jenkinsXML);
    }else{
        QMessageBox::critical(this, "Jenkins Error", "Unable to request Jenkins Data", QMessageBox::Ok);
    }
}

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
    QMessageBox::StandardButton saveProject = QMessageBox::question(this,
                                                                    "MEDEA - New Project",
                                                                    "Current project will be closed.\nSave changes?",
                                                                    QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Cancel);

    if (saveProject == QMessageBox::Yes || saveProject == QMessageBox::No) {
        if(saveProject == QMessageBox::Yes){
            // if failed to export, do nothing
            if (!exportProject()) {
                return;
            }
        }
    }else{
        return;

    }

    if(nodeView){
        nodeView->destroySubViews();
        projectCleared();
    }
    //if(model_clearModel){
    //    model_clearModel->trigger();
    //}

    newProject();
}


/**
 * @brief MedeaWindow::on_actionImport_GraphML_triggered
 */
void MedeaWindow::on_actionImport_GraphML_triggered()
{
    progressAction = "Importing GraphML";

    QStringList files = QFileDialog::getOpenFileNames(
                this,
                "Select one or more files to open",
                "",
                "GraphML Documents (*.graphml *.xml)");

    importProjects(files);
}


/**
 * @brief MedeaWindow::on_actionExport_GraphML_triggered
 */
void MedeaWindow::on_actionExport_GraphML_triggered()
{
    progressAction = "Exporting GraphML";
    exportProject();
}



/**
 * @brief MedeaWindow::on_actionValidate_triggered
 * This is called to validate a model and will display a list of errors.
 */
void MedeaWindow::on_actionValidate_triggered()
{
    //progressAction = "Validating Model";
    displayNotification("Running XSL Validation on Model");

    validateExport();
}

void MedeaWindow::validationComplete(int code)
{
    if(code == 0){
        //validation_report_path
        QFile xmlFile(validation_report_path);
        if (!xmlFile.exists() || !xmlFile.open(QIODevice::ReadOnly)){
            displayNotification("XSL Validation failed to produce a report.");
            return;
        }

        // Query the graphml to get a list of all Files to process.
        QXmlQuery query;
        query.bindVariable("graphmlFile", &xmlFile);
        const QString queryMessages = QString("declare namespace svrl = \"http://purl.oclc.org/dsdl/svrl\"; doc('file:///%1')//svrl:schematron-output/svrl:failed-assert/string()")
                .arg(xmlFile.fileName());
        query.setQuery(queryMessages);

        QStringList messagesResult;
        bool result = query.evaluateTo(&messagesResult);

        xmlFile.close();


        if(!result){
            displayNotification("Cannot run QXmlQuery on Validation Report.");
        }else{
            validateResults.setupItemsTable(messagesResult);
            validateResults.show();
        }
    }else{
        displayNotification("XSL Validation failed with error code: " + QString::number(code));
    }

}

/**
 * @brief MedeaWindow::on_actionPaste_triggered
 */
void MedeaWindow::on_actionPaste_triggered()
{
    progressAction = "Pasting Data";

    QClipboard *clipboard = QApplication::clipboard();
    window_PasteData(clipboard->text());
}


/**
 * @brief MedeaWindow::on_actionSearch_triggered
 * This is called when the search button is clicked.
 * It pops up a dialog listing the items in the search results,
 * or an information message if there are none.
 */
void MedeaWindow::on_actionSearch_triggered()
{    
    QStringList checkedViews = getCheckedItems(0);
    QStringList checkedKinds = getCheckedItems(1);

    // if there are no checked view aspects, search entire model
    if (checkedViews.count() == 0) {
        checkedViews = nodeView->getAllAspects();
        checkedViews.removeDuplicates();
    }

    // if there are no checked kinds, search for all kinds
    if (checkedKinds.count() == 0) {
        checkedKinds = nodeView->getAllNodeKinds();
        checkedKinds.removeDuplicates();
    }

    QString searchText = searchBar->text();
    if (nodeView && !searchText.isEmpty() && (searchText != searchBarDefaultText)) {

        QList<GraphMLItem*> returnedItems = nodeView->search(searchText, GraphMLItem::NODE_ITEM);
        QList<GraphMLItem*> itemsToDisplay;

        // filter the list
        foreach (GraphMLItem* guiItem, returnedItems) {
            NodeItem* nodeItem = dynamic_cast<NodeItem*>(guiItem);
            bool isInAspect = true;
            // if the item is hidden, don't show it in the search results
            if (nodeItem->isHidden()) {
                continue;
            }
            // check if the guiItem is in one of the checked view aspects
            foreach (QString aspect, nodeItem->getAspects()) {
                if (!checkedViews.contains(aspect)) {
                    isInAspect = false;
                }
            }
            if (isInAspect) {
                // if it is, check if the guiItem's kind is one of the checked node kinds
                if (checkedKinds.contains(guiItem->getGraphML()->getDataValue("kind"))) {
                    itemsToDisplay.append(guiItem);
                }
            }
        }

        // if no items match the search checked kinds, display message box
        if (itemsToDisplay.isEmpty()) {
            if (searchResults->isVisible()) {
                searchResults->setVisible(false);
            }
            QMessageBox::information(this, "Search Results", "Search Not Found   ", QMessageBox::Ok);
            return;
        }

        // clear the list view and the old search items
        searchItems.clear();
        for (int i = resultsLayout->count()-1; i >= 0; i--) {
            // remove the layout item's widget, then remove the layout item
            if (resultsLayout->itemAt(i)->widget()) {
                delete resultsLayout->itemAt(i)->widget();
            }
        }

        // for each item to display, create a button for it and add it to the results layout
        foreach (GraphMLItem* guiItem, itemsToDisplay) {
            SearchItemButton* searchItem = new SearchItemButton(guiItem, this);
            searchItem->connectToWindow(this);
            resultsLayout->addWidget(searchItem);
        }

        // move the search results dialog to the bottom left of the window
        // so that it doesn't get in the way of centered search items
        searchResults->move(pos() + QPoint(5, height() - searchResults->height()));

        // show search results
        searchResults->show();
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
void MedeaWindow::on_searchResultItem_clicked(QString ID)
{

    nodeView->selectAndCenter(0, ID);
}


/**
 * @brief MedeaWindow::on_validationItem_clicked
 * @param ID
 */
void MedeaWindow::on_validationItem_clicked(QString ID)
{
    nodeView->selectAndCenter(0, ID);
}


/**
 * @brief MedeaWindow::writeExportedProject
 * @param data
 */
void MedeaWindow::writeExportedProject(QString data)
{
    try {
        QString fileName = exportFileName;
        if(tempExport){
            fileName = tempFileName;
        }
        //Try and Open File.
        QFile file(fileName);
        bool fileOpened = file.open(QIODevice::WriteOnly | QIODevice::Text);

        if(!fileOpened){
            QMessageBox::critical(this, "File Error", "Unable to open file: '" + fileName + "'! Check permissions and try again.", QMessageBox::Ok);
            return;
        }

        //Create stream to write the data.
        QTextStream out(&file);
        out << data;
        file.close();

        //QMessageBox::information(this, "Successfully Exported", "GraphML documented successfully exported to: '" + exportFileName +"'!", QMessageBox::Ok);
        if(!tempExport){
            displayNotification("Successfully exported GraphML document.");
        }

        if(tempExport){
            tempExport = false;
            if(jenkins_TempExport){
                //Link to the function!
                jenkins_InvokeJob(fileName);
                jenkins_TempExport = false;
            }
            if(validate_TempExport){
                validate_Exported(fileName);
                validate_TempExport = false;
            }
            if(cuts_TempExport){
                cuts_runDeployment(fileName);
                cuts_TempExport = false;
            }
            enableTempExport(true);

        }

    }catch(...){
        QMessageBox::critical(this, "Exporting Error", "Unknown Error!", QMessageBox::Ok);
    }
}

void MedeaWindow::writeExportedSnippet(QString parentName, QString snippetXMLData)
{
    try {
        //Try and Open File.

        QString exportName = QFileDialog::getSaveFileName(this,
                                                          "Export " + parentName+ ".snippet",
                                                          "",
                                                          "GraphML " + parentName + " Snippet (*." + parentName+ ".snippet)");

        if (exportName == "") {
            return;
        }

        if(!exportName.toLower().endsWith(".snippet")){
            return;
        }

        QFile file(exportName);
        bool fileOpened = file.open(QIODevice::WriteOnly | QIODevice::Text);

        if(!fileOpened){
            QMessageBox::critical(this, "File Error", "Unable to open file: '" + exportName + "'! Check permissions and try again.", QMessageBox::Ok);
            return;
        }

        //Create stream to write the data.
        QTextStream out(&file);
        out << snippetXMLData;
        file.close();

        //QMessageBox::information(this, "Successfully Exported Snippit", "GraphML documented successfully exported to: '" + exportName +"'!", QMessageBox::Ok);
        displayNotification("Successfully exported GraphML Snippet document.");

    }catch(...){
        QMessageBox::critical(this, "Exporting Error", "Unknown Error!", QMessageBox::Ok);
    }
}

void MedeaWindow::importSnippet(QString parentName)
{

    QString snippetFileName = QFileDialog::getOpenFileName(
                this,
                "Import " + parentName+ ".snippet",
                "",
                "GraphML " + parentName + " Snippet (*." + parentName+ ".snippet)");

    if(snippetFileName.isNull()){
        return;
    }
    QFile file(snippetFileName);

    bool fileOpened = file.open(QFile::ReadOnly | QFile::Text);

    if (!fileOpened) {
        QMessageBox::critical(this, "File Error", "Unable to open file: '" + snippetFileName + "'! Check permissions and try again.", QMessageBox::Ok);
        return;
    }

    QTextStream fileStream(&file);
    QString snippetFileData = fileStream.readAll();
    file.close();

    QFileInfo fileInfo(file.fileName());

    window_ImportSnippet(fileInfo.fileName(), snippetFileData);
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
 * @brief MedeaWindow::updateWindowTitle
 * @param newProjectName
 */
void MedeaWindow::updateWindowTitle(QString newProjectName)
{
    setWindowTitle("MEDEA - " + newProjectName);
    projectName->setText(newProjectName);
    //projectName->setFixedSize(200, projectName->height());
}


/**
 * @brief MedeaWindow::setViewAspects
 * @param aspects
 */
void MedeaWindow::setViewAspects(QStringList aspects)
{
    definitionsToggle->setClicked(aspects.contains("Definitions"));
    workloadToggle->setClicked(aspects.contains("Workload"));
    assemblyToggle->setClicked(aspects.contains("Assembly"));
    hardwareToggle->setClicked(aspects.contains("Hardware"));

    checkedViewAspects = aspects;
}


/**
 * @brief MedeaWindow::setMenuActionEnabled
 * This gets called everytime a node is selected.
 * It enables/disables the specified menu action depending on the selected node.
 * @param action
 * @param node
 */
void MedeaWindow::setMenuActionEnabled(QString action, bool enable)
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
    } else if (action == "singleSelection") {
        actionCenter->setEnabled(enable);
        actionZoomToFit->setEnabled(enable);
        actionPopupSubview->setEnabled(enable);
        // added this after the tag was made
        actionContextMenu->setEnabled(enable);
    } else if(action == "multipleSelection"){
        actionSort->setEnabled(enable);
    } else if(action == "localdeployment"){
        cuts_runGeneration->setEnabled(enable);
    }
}

QIcon MedeaWindow::getIcon(QString alias, QString image)
{
    if(nodeView){
        return QIcon(nodeView->getImage(alias, image));
    }
    return QIcon();
}


/**
 * @brief MedeaWindow::showWindowToolbar
 * If the signal came from the menu, show/hide the toolbar.
 * Otherwise if it came from toolbarButton, expand/contract the toolbar.
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

    if(appSettings){
        appSettings->setSetting(TOOLBAR_EXPANDED, checked);
    }

    toolbar->setVisible(checked);
    return;
}

void MedeaWindow::setToolbarVisibility(bool visible)
{
    if(toolbarButton){
        toolbarButton->setVisible(visible);
    }
    if(toolbar){
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
    } else if (action->text().contains("Grid Lines")) {

        // update toggleGridButton's tooltip
        // in case the grid lines are turned on, this will check if there
        // is a selected item before it turns the snap to grid functions on
        graphicsItemSelected();
    }
}


/**
 * @brief MedeaWindow::resetView
 * This is called everytime a new project is created.
 * It resets the view's turned on aspects to the default
 * and clears all history of the view.
 */
void MedeaWindow::resetView()
{
    if (nodeView){
        nodeView->showManagementComponents(true);
        nodeView->view_ClearHistory();
        nodeView->setDefaultAspects();
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
    //Disable NodeView.
    nodeView->setEnabled(false);
    progressAction = "Setting up New Project";

    // clear and reset the search bar
    searchBar->clear();

    foreach (QAction* action, viewAspectsMenu->actions()) {
        QWidgetAction* widgetAction = qobject_cast<QWidgetAction*>(action);
        QCheckBox* checkBox = qobject_cast<QCheckBox*>(widgetAction->defaultWidget());
        checkBox->setChecked(false);
    }

    foreach (QAction* action, nodeKindsMenu->actions()) {
        QWidgetAction* widgetAction = qobject_cast<QWidgetAction*>(action);
        QCheckBox* checkBox = qobject_cast<QCheckBox*>(widgetAction->defaultWidget());
        checkBox->setChecked(false);
    }

    // need to clear search results here
    QLayoutItem* child;
    while (resultsLayout->count() != 0) {
        child = resultsLayout->takeAt(0);
        /*
        if(child->layout()) {
            remove(child->layout());
        } else if (child->widget()) {
            delete child->widget();
        }
        */
        delete child;
    }

    searchResults->close();

    resetGUI();

}


/**
 * @brief MedeaWindow::exportGraphML
 * @return
 */
bool MedeaWindow::exportProject()
{
    QString filename = QFileDialog::getSaveFileName(this,
                                                    "Export .graphml",
                                                    "",
                                                    "GraphML Documents (*.graphml *.xml)");

    if (filename != "") {
        if(filename.toLower().endsWith(".graphml") || filename.toLower().endsWith(".xml")){
            exportFileName = filename;
            emit window_ExportProject();
            return true;
        }else{
            QMessageBox::critical(this, "Error", "You must export using either a .graphml or .xml extension.", QMessageBox::Ok);
            //CALL AGAIN IF WE Don't get a a .graphml file or a .xml File
            return exportProject();
        }
    }
    return false;
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
 * @brief MedeaWindow::dockButtonPressed
 * This slot is called whenever a dock toggle button is pressed.
 * It makes sure that the groupbox attached to the dock button
 * that was pressed is the only groupbox that is being displayed.
 * @param buttonName
 */
void MedeaWindow::dockButtonPressed(QString buttonName)
{
    DockToggleButton *b, *prevB;

    if (buttonName == "P") {
        b = partsButton;
    } else if (buttonName == "H") {
        b = hardwareNodesButton;
    } else if (buttonName == "D") {
        b = definitionsButton;
    }

    // if the previously activated groupbox is still on display, hide it
    if (prevPressedButton != 0 && prevPressedButton != b) {
        prevB = b;
        prevPressedButton->hideContainer();
        b = prevB;
    }

    prevPressedButton = b;

    // this allows mouse events to pass through the dock's hidden
    // groupbox when none of the docks are currently opened
    if (!partsButton->isSelected() &&
            !definitionsButton->isSelected() &&
            !hardwareNodesButton->isSelected()) {
        updateWidgetMask(docksArea, dockButtonsBox);
    } else {
        docksArea->clearMask();
    }
}


/**
 * @brief MedeaWindow::forceOpenDefinitionsDock
 * This slot is called when a DockNodeItem of kind ComponentInstance or ComponentImpl
 * is clicked from the parts dock. It stops the user from being able to construct a
 * ComponentInstance or ComponentImpl that isn't connected to a Component definition.
 */
void MedeaWindow::forceOpenDefinitionsDock()
{
    if (partsButton->isSelected()) {
        definitionsButton->pressed();
    }
}


/**
 * @brief MedeaWindow::updateProgressStatus
 * This updates the progress bar values.
 */
void MedeaWindow::updateProgressStatus(int value, QString status)
{
    if (value < 0) {
        progressBar->setMaximum(0);
        value = 0;
    } else {
        progressBar->setMaximum(100);
    }
    if (notificationTimer->isActive()) {
        leftOverTime = notificationTimer->remainingTime();
        notificationsBar->hide();
        notificationTimer->stop();
    }

    // if something's in progress, show progress bar
    if (!progressBar->isVisible()) {
        progressLabel->setVisible(true);
        progressBar->setVisible(true);
    }

    // update displayed text
    if (status != "") {
        progressLabel->setText(status + "...");
    }

    // update value
    progressBar->setValue(value);

    // once we reach 100%, reset the progress bar then hide it
    if (value == 100) {
        progressLabel->setVisible(false);
        progressBar->setVisible(false);
        progressBar->reset();
        if (leftOverTime > 0) {
            notificationsBar->show();
            notificationTimer->start(leftOverTime);
            leftOverTime = 0;
        }
    }
}


/**
 * @brief MedeaWindow::searchItemClicked
 * This is called when one of the search results items is clicked.
 */
void MedeaWindow::searchItemClicked()
{
    SearchItemButton* itemClicked = qobject_cast<SearchItemButton*>(QObject::sender());
    window_searchItemClicked(itemClicked);
}


/**
 * @brief MedeaWindow::searchMenuButtonClicked
 * This menu is called when one of the menu buttons in the search options is clicked.
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
            textLabel = "Search Kinds: "; defaultText = nodeKindsDefaultText;
            checkedItemsList = getCheckedItems(1);
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

    if (!notificationTimer->isActive() && !notificationsQueue.isEmpty()) {
        notification = notificationsQueue.dequeue();
        notificationsBar->setText(notification);
        notificationsBar->setFixedWidth(notificationsBar->fontMetrics().width(notification) + 30);
        notificationsBar->show();
        notificationTimer->start(NOTIFICATION_TIME);
    }
}

/**
 * @brief MedeaWindow::checkNotificationsQueue
 */
void MedeaWindow::checkNotificationsQueue()
{
    notificationTimer->stop();

    // if there are still notifications waiting to be displayed, display them in order
    if (notificationsQueue.count() > 0) {
        displayNotification();
    }
}


/**
 * @brief MedeaWindow::graphicsItemSelected
 * Added this slot to update anything that needs updating when a graphics item is selected.
 */
void MedeaWindow::graphicsItemSelected()
{
    //if (hardwareNodesButton->getSelected()) {
    //    if(nodeView){
    //        emit window_highlightDeployment();
    //    }
    //}
    /*
    if (nodeView && nodeView->getSelectedNode()) {
        view_showConnectedNodes->setEnabled(true);
        view_snapToGrid->setEnabled(settings_useGridLines->isChecked());
        view_snapChildrenToGrid->setEnabled(settings_useGridLines->isChecked());



    } else {
        view_showConnectedNodes->setEnabled(false);
        view_snapToGrid->setEnabled(false);
        view_snapChildrenToGrid->setEnabled(false);
    }
    */
}


/**
 * @brief MedeaWindow::graphicsItemDeleted
 * This is currently called when a node item is deleted.
 * It stops the data table from displaying an emty white rectangle
 * when the item that was deleted was previously selected.
 */
void MedeaWindow::graphicsItemDeleted()
{
    //updateDataTable();

    // added this here for when edges are deleted
    // we need to check first if there's a selected node before clearing the deployment highlight
    /*
    if (nodeView) {
        qDebug() << "Edge deleted";
        emit window_highlightDeployment(nodeView->getSelectedNode());
    }
    */
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


/**
 * @brief MedeaWindow::updateDataTable
 * Update the dataTable size whenever a node is selected/deselected,
 * when a new model is loaded and when the window is resized.
 * NOTE: Once maximum size is set, it cannot be reset.
 */
void MedeaWindow::updateDataTable()
{
    QAbstractItemModel* tableModel = dataTable->model();

    //Check for Data.
    bool hasData = tableModel && tableModel->rowCount() > 0;

    if(hasData){
        dataTableBox->setVisible(true);
    }else{
        dataTableBox->setVisible(false);
    }

    // calculate the required height

    int newHeight = 0;
    int maxHeight = dataTableBox->height();
    if(hasData){
        for (int i = 0; i < tableModel->rowCount(); i++) {
            newHeight += dataTable->rowHeight(i);
        }

        if(hasData){
            newHeight += dataTable->horizontalHeader()->size().height();
            newHeight += dataTable->contentsMargins().top() + dataTable->contentsMargins().bottom();
        }
    }

    if(newHeight > maxHeight){
        dataTable->resize(dataTable->width(), maxHeight);
    }else{
        dataTable->resize(dataTable->width(), newHeight);
    }

    // update the visible region of the groupbox to fit the dataTable

    updateWidgetMask(dataTableBox, dataTable);

    if(hasData){
        // align the contents of the datatable
        dataTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        dataTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
        dataTable->horizontalHeader()->resizeSection(1, dataTable->width()/4);
        dataTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
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
        try {
            QFile file(fileName);

            bool fileOpened = file.open(QFile::ReadOnly | QFile::Text);

            if (!fileOpened) {
                QMessageBox::critical(this, "File Error", "Unable to open file: '" + fileName + "'! Check permissions and try again.", QMessageBox::Ok);
                return;
            }

            QTextStream fileStream(&file);
            QString projectXML = fileStream.readAll();
            file.close();
            projects << projectXML;
        } catch (...) {
            QMessageBox::critical(this, "Error", "Error reading file: '" + fileName + "'", QMessageBox::Ok);
            return;
        }
    }
    if (projects.size() > 0) {
        window_ImportProjects(projects);
        nodeView->fitToScreen();
    }
}

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
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "MEDEA - Quit Confirmation",
                                                                tr("Are you sure you want to quit Medea?\n"),
                                                                QMessageBox::Cancel | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes) {
        e->ignore();

    } else {
        e->accept();
        deleteLater();
    }
}




/**
 * @brief MedeaWindow::getCheckedItems
 * This returns a list of the checked items from the search sub-menus.
 * @param menu - 0 = viewAspects, 1 = nodeKinds
 * @return
 */
QStringList MedeaWindow::getCheckedItems(int menu)
{
    QStringList checkedKinds;

    QMenu* searchMenu = 0;
    if (menu == 0) {
        searchMenu = viewAspectsMenu;
    } else if (menu == 1) {
        searchMenu = nodeKindsMenu;
    }

    if (searchMenu) {
        foreach (QAction* action, searchMenu->actions()) {
            QWidgetAction* widgetAction = qobject_cast<QWidgetAction*>(action);
            QCheckBox* checkBox = qobject_cast<QCheckBox*>(widgetAction->defaultWidget());
            if (checkBox->isChecked()) {
                checkedKinds.append(checkBox->text());
            }
        }
    }

    return checkedKinds;
}

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
