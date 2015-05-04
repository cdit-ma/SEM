#include "medeawindow.h"
#include "Controller/controller.h"

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
#include <QPicture>

#include <QToolButton>
#include <QToolBar>

#include <QSettings>


#define THREADING false
#define MIN_WIDTH 1300
#define MIN_HEIGHT 800
#define RIGHT_PANEL_WIDTH 230


/**
 * @brief MedeaWindow::MedeaWindow
 * @param parent
 */
MedeaWindow::MedeaWindow(QString graphMLFile, QWidget *parent) :
    QMainWindow(parent)
{

    applicationDirectory = QApplication::applicationDirPath() + "/";
    //Critical() << applicationDirectory;
    // this needs to happen before the menu is set up and connected
    appSettings = new AppSettings(applicationDirectory, this);

    setupJenkinsSettings();

    // initialise gui and connect signals and slots
    initialiseGUI();
    makeConnections();
    newProject();

    //on_SearchTextChanged("");

    /*
    // this is used for when a file is dragged and dropped on top of this tool's icon
    if(graphMLFile.length() != 0){
        QStringList files;
        files.append(graphMLFile);
        importGraphMLFiles(files);
    }
    */
}


/**
 * @brief MedeaWindow::~MedeaWindow
 */
MedeaWindow::~MedeaWindow()
{
    if(appSettings){
        saveSettings();
        delete appSettings;
    }
    if (controller) {
        delete controller;
    }
    if (nodeView) {
        delete nodeView;
    }
}


/**
 * @brief MedeaWindow::initialiseGUI
 * Initialise variables, setup widget sizes, organise layout
 * and setup the view, scene and menu.
 */
void MedeaWindow::initialiseGUI()
{    
    // initialise variables
    thread = 0;
    myProcess = 0;
    controller = 0;

    nodeView = new NodeView();
    toolbar = new QToolBar();

    prevPressedButton = 0;
    firstTableUpdate = true;

    progressBar = new QProgressBar(this);
    progressLabel = new QLabel(this);

    notificationsBar = new QLabel("Hello", this);
    notificationTimer = new QTimer(this);

    dataTable = new QTableView();
    dataTableBox = new QGroupBox();
    delegate = new ComboBoxTableDelegate(0);

    projectName = new QPushButton("Model");
    assemblyButton = new QPushButton("Assembly");
    hardwareButton = new QPushButton("Hardware");
    workloadButton = new QPushButton("Behaviour");
    definitionsButton = new QPushButton("Interface");

    QPushButton *menuButton = new QPushButton(QIcon(":/Resources/Icons/menuIcon.png"), "");

    // set central widget and window size
    this->setCentralWidget(nodeView);
    this->setMinimumSize(MIN_WIDTH, MIN_HEIGHT);
    nodeView->setMinimumSize(MIN_WIDTH, MIN_HEIGHT);
    nodeView->viewport()->setMinimumSize(MIN_WIDTH, MIN_HEIGHT);

    // set the size for the right panel where the view buttons and data table are located
    int rightPanelWidth = RIGHT_PANEL_WIDTH;

    // setup widgets
    menuButton->setFixedSize(50,45);
    menuButton->setIconSize(menuButton->size());
    menuButton->setStyleSheet("QPushButton{ background-color: rgba(220,220,220,0.5); }"
                              "QPushButton::menu-indicator{ image: none; }");
    projectName->setFlat(true);
    projectName->setFixedWidth(rightPanelWidth-menuButton->width()-10);
    projectName->setStyleSheet("font-size: 16px; text-align: left;");

    assemblyButton->setFixedSize(rightPanelWidth/2.05, rightPanelWidth/2.5);
    hardwareButton->setFixedSize(rightPanelWidth/2.05, rightPanelWidth/2.5);
    definitionsButton->setFixedSize(rightPanelWidth/2.05, rightPanelWidth/2.5);
    workloadButton->setFixedSize(rightPanelWidth/2.05, rightPanelWidth/2.5);

    /*
    QString border = "border-right: 2px; border-bottom: 3px; border-left: 1px; border-top: 1px;";
    QString checkedBorder = "border-right: 1px; border-bottom: 1px; border-left: 2px; border-top: 3px;";
    QString borderStyle = "border-style: solid; border-color: rgb(100,100,100); border-radius: 8px;";
    QString font = "font: 13px;";

    assemblyButton->setStyleSheet("QPushButton{ background-color: rgb(230,130,130);"
                                  + border + borderStyle + font + "}"
                                  "QPushButton:checked{ background-color:"
                                  "qlineargradient(x1: 0, y1: 0, x2: 0, y2: 0.5, x3: 0 y3: 1.0,"
                                  "stop: 0 rgb(255,200,200), stop: 0.5 rgb(255,200,200),"
                                  "stop: 0.5 rgb(250,160,160), stop: 1.0  rgb(240,140,140));"
                                  + checkedBorder + borderStyle + "}");
    hardwareButton->setStyleSheet("QPushButton{ background-color: rgb(80,140,190);"
                                  + border + borderStyle + font + "}"
                                  "QPushButton:checked{ background-color:"
                                  "qlineargradient(x1: 0, y1: 0, x2: 0, y2: 0.5, x3: 0 y3: 1.0,"
                                  "stop: 0 rgb(115,200,250), stop: 0.5 rgb(115,200,250),"
                                  "stop: 0.5 rgb(110,170,220), stop: 1.0  rgb(90,150,200));"
                                  + checkedBorder + borderStyle + "}");
    definitionsButton->setStyleSheet("QPushButton{ background-color: rgb(80,180,180);"
                                     + border + borderStyle + font + "}"
                                     "QPushButton:checked{ background-color:"
                                     "qlineargradient(x1: 0, y1: 0, x2: 0, y2: 0.5, x3: 0 y3: 1.0,"
                                     "stop: 0 rgb(125,240,240), stop: 0.5 rgb(125,240,240),"
                                     "stop: 0.5 rgb(110,210,210), stop: 1.0  rgb(90,190,190));"
                                     + checkedBorder + borderStyle + "}");
    workloadButton->setStyleSheet("QPushButton{ background-color: rgb(224,154,96);"
                                  + border + borderStyle + font + "}"
                                  "QPushButton:checked{ background-color:"
                                  "qlineargradient(x1: 0, y1: 0, x2: 0, y2: 0.5, x3: 0 y3: 1.0,"
                                  "stop: 0 rgb(255,224,166), stop: 0.5 rgb(255,224,166),"
                                  "stop: 0.5 rgb(250,190,130), stop: 1.0  rgb(234,164,106));"
                                  + checkedBorder + borderStyle + "}");
                                  */

    assemblyButton->setStyleSheet("background-color: rgb(230,130,130);");
    hardwareButton->setStyleSheet("background-color: rgb(80,140,190);");
    definitionsButton->setStyleSheet("background-color: rgb(80,180,180);");
    workloadButton->setStyleSheet("background-color: rgb(224,154,96);");

    // setup the progress bar
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
    notificationsBar->setStyleSheet("background-color: rgba(250,250,250,0.25);"
                                    "color: rgb(50,50,50);"
                                    "border-radius: 10px;"
                                    "padding: 0px 15px;"
                                    "font: 16px;");
    //"font-weight: bold;");

    QVBoxLayout *progressLayout = new QVBoxLayout();
    progressLayout->addStretch(3);
    progressLayout->addWidget(progressLabel);
    progressLayout->addWidget(progressBar);
    progressLayout->addStretch(4);
    progressLayout->addWidget(notificationsBar);

    // setup and add dataTable/dataTableBox widget/layout
    dataTable->setItemDelegateForColumn(2, delegate);
    dataTable->setFixedWidth(rightPanelWidth);
    dataTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QVBoxLayout *tableLayout = new QVBoxLayout();
    tableLayout->setMargin(0);
    tableLayout->setContentsMargins(0,0,0,0);
    tableLayout->addWidget(dataTable);

    dataTableBox->setFixedWidth(rightPanelWidth);
    dataTableBox->setLayout(tableLayout);
    dataTableBox->setStyleSheet("QGroupBox {"
                                "background-color: rgba(0,0,0,0);"
                                "border: 0px;"
                                "}");


    // setup mini map
    minimap = new NodeViewMinimap();
    minimap->setScene(nodeView->scene());

    minimap->scale(.002,.002);
    minimap->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    minimap->setVerticalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
    minimap->setInteractive(false);

    minimap->setFixedWidth(rightPanelWidth);
    minimap->setFixedHeight(rightPanelWidth/1.6);
    minimap->setStyleSheet("background-color: rgba(125,125,125,225);");

    // layouts
    QHBoxLayout *mainHLayout = new QHBoxLayout();
    QHBoxLayout *topHLayout = new QHBoxLayout();
    QVBoxLayout *leftVlayout = new QVBoxLayout();
    QVBoxLayout *rightVlayout =  new QVBoxLayout();
    QHBoxLayout *titleLayout = new QHBoxLayout();
    QHBoxLayout *bodyLayout = new QHBoxLayout();
    QGridLayout *viewButtonsGrid = new QGridLayout();
    searchLayout = new QHBoxLayout();

    // setup layouts for widgets
    titleLayout->setMargin(0);
    titleLayout->setSpacing(0);
    titleLayout->addWidget(menuButton, 1);
    titleLayout->addSpacerItem(new QSpacerItem(10, 0));
    titleLayout->addWidget(projectName, 1);
    titleLayout->addStretch();

    topHLayout->setMargin(0);
    topHLayout->setSpacing(0);
    topHLayout->addLayout(titleLayout);
    topHLayout->addStretch(1);
    topHLayout->addWidget(toolbar);
    topHLayout->addStretch(1);

    leftVlayout->addLayout(topHLayout, 10);
    leftVlayout->addStretch(1);
    leftVlayout->addLayout(bodyLayout, 50);

    viewButtonsGrid->addWidget(definitionsButton, 1, 1);
    viewButtonsGrid->addWidget(workloadButton, 1, 2);
    viewButtonsGrid->addWidget(assemblyButton, 2, 1);
    viewButtonsGrid->addWidget(hardwareButton, 2, 2);

    rightVlayout->addLayout(searchLayout, 1);
    rightVlayout->addSpacerItem(new QSpacerItem(20, 30));
    rightVlayout->addLayout(viewButtonsGrid);
    rightVlayout->addSpacerItem(new QSpacerItem(20, 30));
    rightVlayout->addWidget(dataTableBox, 4);
    rightVlayout->addSpacerItem(new QSpacerItem(20, 30));
    rightVlayout->addStretch();
    rightVlayout->addSpacerItem(new QSpacerItem(20, 30));
    rightVlayout->addWidget(minimap);

    mainHLayout->setMargin(0);
    mainHLayout->setSpacing(0);
    mainHLayout->addLayout(leftVlayout, 4);
    mainHLayout->addLayout(rightVlayout, 1);
    mainHLayout->setContentsMargins(25, 25, 25, 25);
    nodeView->setLayout(mainHLayout);

    // other settings
    assemblyButton->setCheckable(true);
    hardwareButton->setCheckable(true);
    definitionsButton->setCheckable(true);
    workloadButton->setCheckable(true);

    // setup the menu, dock, search tools and toolbar
    setupMenu(menuButton);
    setupDock(bodyLayout);
    setupSearchTools();
    setupToolbar();

    // add progress bar layout to the body layout after the dock has been set up
    bodyLayout->addStretch(4);
    bodyLayout->addLayout(progressLayout);
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
    file_menu = menu->addMenu(QIcon(":/Resources/Icons/file_menu.png"), "File");
    edit_menu = menu->addMenu(QIcon(":/Resources/Icons/edit_menu.png"), "Edit");
    menu->addSeparator();
    view_menu = menu->addMenu(QIcon(":/Resources/Icons/view.png"), "View");
    model_menu = menu->addMenu(QIcon(":/Resources/Icons/model.png"), "Model");
    menu->addSeparator();
    settings_Menu = menu->addMenu(QIcon(":/Resources/Icons/settings.png"), "Settings");
    exit = menu->addAction(QIcon(":/Resources/Icons/exit.png"), "Exit");

    file_newProject = file_menu->addAction(QIcon(":/Resources/Icons/new_project.png"), "New Project");
    file_newProject->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
    file_importGraphML = file_menu->addAction(QIcon(":/Resources/Icons/import.png"), "Import");
    file_importGraphML->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
    file_exportGraphML = file_menu->addAction(QIcon(":/Resources/Icons/export.png"), "Export");
    file_exportGraphML->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
    file_menu->addSeparator();

    // only create the menu item if JENKINS_ADDRESS is not null
    if (JENKINS_ADDRESS != "") {
        file_importJenkinsNodes = file_menu->addAction(QIcon(":/Resources/Icons/jenkins.png"), "Import Jenkins Nodes");
        file_importJenkinsNodes->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_J));
    } else {
        file_importJenkinsNodes = 0;
    }

    edit_undo = edit_menu->addAction(QIcon(":/Resources/Icons/undo.png"), "Undo");
    edit_undo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z));
    edit_redo = edit_menu->addAction(QIcon(":/Resources/Icons/redo.png"), "Redo");
    edit_redo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Y));
    edit_menu->addSeparator();
    edit_cut = edit_menu->addAction(QIcon(":/Resources/Icons/cut.png"), "Cut");
    edit_cut->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_X));
    edit_copy = edit_menu->addAction(QIcon(":/Resources/Icons/copy.png"), "Copy");
    edit_copy->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
    edit_paste = edit_menu->addAction(QIcon(":/Resources/Icons/paste.png"), "Paste");
    edit_paste->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_V));

    view_fitToScreen = view_menu->addAction(QIcon(":/Resources/Icons/fitToScreen.png"), "Fit To Screen");
    view_fitToScreen->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Space));
    view_menu->addSeparator();
    view_snapToGrid = view_menu->addAction(QIcon(":/Resources/Icons/snapToGrid.png"), "Snap Selection To Grid");
    view_snapChildrenToGrid = view_menu->addAction(QIcon(":/Resources/Icons/snapChildrenToGrid.png"), "Snap Selection's Children To Grid");
    view_menu->addSeparator();
    view_goToDefinition = view_menu->addAction(QIcon(":/Resources/Icons/definition.png"), "Go To Definition");
    //view_goToDefinition->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_D));
    view_goToImplementation = view_menu->addAction(QIcon(":/Resources/Icons/implementation.png"), "Go To Implementation");
    //view_goToImplementation->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_I));
    view_menu->addSeparator();
    view_showConnectedNodes = view_menu->addAction(QIcon(":/Resources/Icons/connections.png"), "View Connections");
    view_showManagementComponents = view_menu->addAction("View Management Components");
    //view_showManagementComponents->setShortcut(QKeySequence(Qt::CTRL +Qt::Key_M));

    model_clearModel = model_menu->addAction(QIcon(":/Resources/Icons/clear.png"), "Clear Model");
    //model_clearModel->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    model_sortModel = model_menu->addAction(QIcon(":/Resources/Icons/sort.png"), "Sort Model");
    //model_sortModel->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
    model_menu->addSeparator();
    model_validateModel = model_menu->addAction(QIcon(":/Resources/Icons/validate.png"), "Validate Model");

    settings_displayWindowToolbar = settings_Menu->addAction("Display Toolbar");
    settings_Menu->addSeparator();
    settings_showGridLines = settings_Menu->addAction("Use Grid Lines");
    //settings_showGridLines->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_G));
    settings_selectOnConstruction = settings_Menu->addAction("Select Node On Construction");
    settings_Menu->addSeparator();
    settings_autoCenterView = settings_Menu->addAction("Automatically Center Views");
    settings_viewZoomAnchor = settings_Menu->addAction("Zoom View Under Mouse");
    settings_Menu->addSeparator();
    settings_ChangeSettings = settings_Menu->addAction("More Settings...");

    button->setMenu(menu);

    // setup toggle actions
    settings_displayWindowToolbar->setCheckable(true);
    settings_showGridLines->setCheckable(true);
    settings_selectOnConstruction->setCheckable(true);
    settings_autoCenterView->setCheckable(true);
    settings_viewZoomAnchor->setCheckable(true);
    view_showManagementComponents->setCheckable(true);

    // initially disable model & goto menu actions
    view_goToDefinition->setEnabled(false);
    view_goToImplementation->setEnabled(false);
    view_showConnectedNodes->setEnabled(false);
}


/**
 * @brief MedeaWindow::setupDock
 * Initialise and setup dock widgets.
 * @param layout
 */
void MedeaWindow::setupDock(QHBoxLayout *layout)
{
    QVBoxLayout *dockLayout = new QVBoxLayout();
    QGroupBox* buttonsBox = new QGroupBox();
    QHBoxLayout *dockButtonsHlayout = new QHBoxLayout();

    partsButton = new DockToggleButton("P", this);
    hardwareNodesButton = new DockToggleButton("H", this);
    compDefinitionsButton = new DockToggleButton("D", this);

    partsDock = new PartsDockScrollArea("Parts", nodeView, partsButton);
    definitionsDock = new DefinitionsDockScrollArea("Definitions", nodeView, compDefinitionsButton);
    hardwareDock = new HardwareDockScrollArea("Hardware Nodes", nodeView, hardwareNodesButton);

    // width of the containers are fixed
    boxWidth = (partsButton->getWidth()*3) + 30;

    // set buttonBox's size and get rid of its border
    buttonsBox->setStyleSheet("border: 0px;");
    buttonsBox->setFixedSize(boxWidth, 60);

    // set dockScrollAreas sizes
    partsDock->setMaximumWidth(boxWidth);
    definitionsDock->setMaximumWidth(boxWidth);
    hardwareDock->setMaximumWidth(boxWidth);

    // set size policy for buttons
    partsButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    hardwareNodesButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    compDefinitionsButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // set keyboard shortcuts for dock buttons
    partsButton->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
    hardwareNodesButton->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_H));
    compDefinitionsButton->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));

    // add widgets to/and layouts
    dockButtonsHlayout->addWidget(partsButton);
    dockButtonsHlayout->addWidget(compDefinitionsButton);
    dockButtonsHlayout->addWidget(hardwareNodesButton);
    buttonsBox->setLayout(dockButtonsHlayout);

    dockLayout->addWidget(buttonsBox);
    dockLayout->addWidget(partsDock);
    dockLayout->addWidget(definitionsDock);
    dockLayout->addWidget(hardwareDock);
    dockLayout->addStretch();

    layout->addLayout(dockLayout, 1);
}


/**
 * @brief MedeaWindow::setupSearchTools
 * @param layout
 */
void MedeaWindow::setupSearchTools()
{
    searchBarDefaultText = "Search Here...";
    searchBar = new QLineEdit(searchBarDefaultText, this);
    searchButton = new QPushButton(QIcon(":/Resources/Icons/search_icon.png"), "");
    searchOptionButton = new QPushButton(QIcon(":/Resources/Icons/settings.png"), "");
    searchOptionMenu = new QMenu(searchOptionButton);
    searchSuggestions = new QListView(searchButton);
    searchResults = new QDialog(this);

    QVBoxLayout* resultsMainLayout = new QVBoxLayout();
    resultsLayout = new QVBoxLayout();
    int rightPanelWidth = RIGHT_PANEL_WIDTH;
    int searchBarHeight = 28;

    QHBoxLayout* headerLayout = new QHBoxLayout();
    QLabel* objectLabel = new QLabel("Object Name:", this);
    QLabel* parentLabel = new QLabel("Parent Name:", this);
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

    searchBar->setFixedSize(rightPanelWidth - (searchButton->width()*2) - 5, searchBarHeight-3);
    searchBar->setStyleSheet("background-color: rgb(230,230,230);");

    searchSuggestions->setViewMode(QListView::ListMode);
    searchSuggestions->setVisible(false);

    searchResults->setLayout(resultsMainLayout);
    searchResults->setMinimumWidth(rightPanelWidth + 110);
    searchResults->setWindowTitle("Search Results");
    searchResults->setVisible(false);

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
    viewAspectsButton = new QPushButton(QIcon(":/Resources/Icons/menu_down_arrow.png"), "");
    viewAspectsMenu = new QMenu(viewAspectsButton);

    aspectsLabel->setFixedWidth(50);
    aspectsLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    viewAspectsButton->setFixedSize(20, 20);
    viewAspectsButton->setIconSize(viewAspectsButton->size()*0.6);
    viewAspectsButton->setCheckable(true);
    viewAspectsBar->setFixedWidth(rightPanelWidth - viewAspectsButton->width() - aspectsLabel->width() - 30);
    viewAspectsBar->setToolTip("Search Aspects: " + viewAspectsBarDefaultText);
    viewAspectsBar->setEnabled(false);
    viewAspectsMenu->setFixedWidth(viewAspectsBar->width() + viewAspectsButton->width());

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
    nodeKindsButton = new QPushButton(QIcon(":/Resources/Icons/menu_down_arrow.png"), "");
    nodeKindsMenu = new QMenu(nodeKindsButton);

    kindsLabel->setFixedWidth(50);
    kindsLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    nodeKindsButton->setFixedSize(20, 20);
    nodeKindsButton->setIconSize(nodeKindsButton->size()*0.6);
    nodeKindsButton->setCheckable(true);
    nodeKindsBar->setFixedWidth(rightPanelWidth - nodeKindsButton->width() - kindsLabel->width() - 30);
    nodeKindsBar->setToolTip("Search Kinds: " + nodeKindsDefaultText);
    nodeKindsBar->setEnabled(false);
    nodeKindsMenu->setFixedWidth(nodeKindsBar->width() + nodeKindsButton->width());

    kindsLayout->setMargin(5);
    kindsLayout->setSpacing(3);
    kindsLayout->addWidget(kindsLabel);
    kindsLayout->addWidget(nodeKindsBar);
    kindsLayout->addWidget(nodeKindsButton);

    kindsGroup->setLayout(kindsLayout);
    kindsAction->setDefaultWidget(kindsGroup);

    // add widget actions and their menus to the main search option menu
    searchOptionMenu->addAction(aspectsAction);
    searchOptionMenu->addAction(kindsAction);
    searchOptionMenu->setVisible(false);
}


/**
 * @brief MedeaWindow::setupToolbar
 * Initialise and setup toolbar widgets.
 */
void MedeaWindow::setupToolbar()
{
    QSize buttonSize = QSize(46,40);

    toolbarButton = new QToolButton(this);
    toolbarAction = new QWidgetAction(this);
    toolbarAction->setDefaultWidget(toolbarButton);

    toolbarButton->setCheckable(true);
    toolbarButton->setStyleSheet("QToolButton:!checked{ background-color: rgba(180,180,180,225); }"
                                 //"QToolButton{ border-radius: 20px; }"
                                 "QToolButton:hover:!checked{ background-color: rgba(210,210,210,225); }");


    cutButton = new QToolButton(this);
    copyButton = new QToolButton(this);
    pasteButton = new QToolButton(this);
    sortButton = new QToolButton(this);
    centerButton = new QToolButton(this);
    zoomToFitButton = new QToolButton(this);
    fitToScreenButton = new QToolButton(this);
    duplicateButton = new QToolButton(this);
    undoButton = new QToolButton(this);
    redoButton = new QToolButton(this);

    toolbarButton->setIcon(QIcon(":/Resources/Icons/toolbar.png"));
    cutButton->setIcon(QIcon(":/Resources/Icons/cut.png"));
    copyButton->setIcon(QIcon(":/Resources/Icons/copy.png"));
    pasteButton->setIcon(QIcon(":/Resources/Icons/paste.png"));
    sortButton->setIcon(QIcon(":/Resources/Icons/sort.png"));
    centerButton->setIcon(QIcon(":/Resources/Icons/center.png"));
    zoomToFitButton->setIcon(QIcon(":/Resources/Icons/zoomToFit.png"));
    fitToScreenButton->setIcon(QIcon(":/Resources/Icons/fitToScreen.png"));
    duplicateButton->setIcon(QIcon(":/Resources/Icons/duplicate.png"));
    undoButton->setIcon(QIcon(":/Resources/Icons/undo.png"));
    redoButton->setIcon(QIcon(":/Resources/Icons/redo.png"));

    toolbarButton->setFixedSize(buttonSize);
    cutButton->setFixedSize(buttonSize);
    copyButton->setFixedSize(buttonSize);
    pasteButton->setFixedSize(buttonSize);
    sortButton->setFixedSize(buttonSize);
    centerButton->setFixedSize(buttonSize);
    zoomToFitButton->setFixedSize(buttonSize);
    fitToScreenButton->setFixedSize(buttonSize);
    duplicateButton->setFixedSize(buttonSize);
    undoButton->setFixedSize(buttonSize);
    redoButton->setFixedSize(buttonSize);

    toolbarButton->setToolTip("Expand Toolbar");
    cutButton->setToolTip("Cut");
    copyButton->setToolTip("Copy");
    pasteButton->setToolTip("Paste");
    sortButton->setToolTip("Sort Model/Selection");
    centerButton->setToolTip("Center On Selection");
    zoomToFitButton->setToolTip("Zoom To Fit Selection");
    fitToScreenButton->setToolTip("Fit View To Screen");
    duplicateButton->setToolTip("Replicate");
    undoButton->setToolTip("Undo");
    redoButton->setToolTip("Redo");

    QWidget* spacerWidgetLeft = new QWidget();
    QWidget* spacerWidgetRight = new QWidget();
    QWidget* spacerWidget1 = new QWidget();
    QWidget* spacerWidget2 = new QWidget();
    QWidget* spacerWidget3 = new QWidget();
    QWidget* spacerWidget4 = new QWidget();
    QWidget* spacerWidget5 = new QWidget();
    QWidget* spacerWidget6 = new QWidget();
    int spacerWidth = 3;

    leftSpacerAction = new QWidgetAction(this);
    rightSpacerAction = new QWidgetAction(this);
    leftSpacerAction->setDefaultWidget(spacerWidgetLeft);
    rightSpacerAction->setDefaultWidget(spacerWidgetRight);

    spacerWidgetLeft->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    spacerWidgetRight->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    spacerWidget1->setFixedWidth(spacerWidth);
    spacerWidget2->setFixedWidth(spacerWidth);
    spacerWidget3->setFixedWidth(spacerWidth);
    spacerWidget4->setFixedWidth(spacerWidth);
    spacerWidget5->setFixedWidth(spacerWidth);
    spacerWidget6->setFixedWidth(spacerWidth);

    toolbar->addAction(leftSpacerAction);
    toolbar->addAction(toolbarAction);
    toolbar->addWidget(spacerWidget5);
    toolbar->addSeparator();
    toolbar->addWidget(spacerWidget6);
    toolbar->addWidget(undoButton);
    toolbar->addWidget(redoButton);
    toolbar->addWidget(spacerWidget1);
    toolbar->addSeparator();
    toolbar->addWidget(spacerWidget2);
    toolbar->addWidget(cutButton);
    toolbar->addWidget(copyButton);

    toolbar->addWidget(pasteButton);
    toolbar->addWidget(duplicateButton);
    toolbar->addWidget(spacerWidget3);
    toolbar->addSeparator();
    toolbar->addWidget(spacerWidget4);
    toolbar->addWidget(fitToScreenButton);
    toolbar->addWidget(centerButton);
    toolbar->addWidget(zoomToFitButton);
    toolbar->addWidget(sortButton);
    toolbar->addAction(rightSpacerAction);

    toolbar->setIconSize(buttonSize*0.6);
    toolbar->setFixedSize(toolbar->contentsRect().width(), buttonSize.height()+spacerWidth);
    toolbar->setStyle(QStyleFactory::create("windows"));
    toolbar->setStyleSheet("QToolButton{"
                           "border: 1px solid grey;"
                           "border-radius: 10px;"
                           "background-color: rgba(210,210,210,225);"
                           "margin: 0px 3px 0px 3px;"
                           "}"
                           );


}


/**
 * @brief MedeaWindow::setupController
 */
void MedeaWindow::setupController()
{
    if (controller) {
        delete controller;
        controller = 0;
    }
    if (thread) {
        delete thread;
        thread = 0;
    }

    //controller = 0;
    //thread = 0;

    controller = new NewController();

    if (THREADING) {
        //IMPLEMENT THREADING!
        thread = new QThread();
        thread->start();
        controller->moveToThread(thread);
    }

    controller->connectView(nodeView);
    controller->initializeModel();
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

    connect(this, SIGNAL(window_AspectsChanged(QStringList)), nodeView, SLOT(setAspects(QStringList)));
    connect(nodeView, SIGNAL(view_GUIAspectChanged(QStringList)), this, SLOT(setViewAspects(QStringList)));
    connect(nodeView, SIGNAL(view_updateGoToMenuActions(QString,bool)), this, SLOT(setGoToMenuActions(QString,bool)));
    connect(nodeView, SIGNAL(view_SetAttributeModel(AttributeTableModel*)), this, SLOT(setAttributeModel(AttributeTableModel*)));
    connect(nodeView, SIGNAL(view_updateProgressStatus(int,QString)), this, SLOT(updateProgressStatus(int,QString)));

    //connect(nodeView, SIGNAL(view_showWindowToolbar()), this, SLOT(showWindowToolbar()));
    connect(nodeView, SIGNAL(view_nodeSelected()), this, SLOT(graphicsItemSelected()));

    connect(nodeView, SIGNAL(customContextMenuRequested(QPoint)), nodeView, SLOT(showToolbar(QPoint)));
    connect(nodeView, SIGNAL(view_ViewportRectChanged(QRectF)), minimap, SLOT(viewportRectChanged(QRectF)));

    connect(notificationTimer, SIGNAL(timeout()), notificationsBar, SLOT(hide()));
    connect(notificationTimer, SIGNAL(timeout()), this, SLOT(checkNotificationsQueue()));
    connect(nodeView, SIGNAL(view_displayNotification(QString)), this, SLOT(displayNotification(QString)));

    connect(projectName, SIGNAL(clicked()), nodeView, SLOT(view_SelectModel()));

    connect(file_newProject, SIGNAL(triggered()), this, SLOT(on_actionNew_Project_triggered()));
    connect(file_importGraphML, SIGNAL(triggered()), this, SLOT(on_actionImport_GraphML_triggered()));
    connect(file_exportGraphML, SIGNAL(triggered()), this, SLOT(on_actionExport_GraphML_triggered()));
    connect(file_importJenkinsNodes, SIGNAL(triggered()), this, SLOT(on_actionImportJenkinsNode()));
    connect(this, SIGNAL(window_ExportProject()), nodeView, SIGNAL(view_ExportProject()));
    connect(this, SIGNAL(window_ImportProjects(QStringList)), nodeView, SIGNAL(view_ImportProjects(QStringList)));

    connect(edit_undo, SIGNAL(triggered()), this, SLOT(menuActionTriggered()));
    connect(edit_redo, SIGNAL(triggered()), this, SLOT(menuActionTriggered()));

    connect(edit_undo, SIGNAL(triggered()), nodeView, SIGNAL(view_Undo()));
    connect(edit_redo, SIGNAL(triggered()), nodeView, SIGNAL(view_Redo()));
    connect(edit_cut, SIGNAL(triggered()), nodeView, SLOT(cut()));
    connect(edit_copy, SIGNAL(triggered()), nodeView, SLOT(copy()));
    connect(edit_paste, SIGNAL(triggered()), this, SLOT(on_actionPaste_triggered()));
    connect(this, SIGNAL(window_PasteData(QString)), nodeView, SLOT(paste(QString)));

    connect(view_fitToScreen, SIGNAL(triggered()), nodeView, SLOT(fitToScreen()));
    connect(view_snapToGrid, SIGNAL(triggered()), nodeView, SLOT(snapSelectionToGrid()));
    connect(view_snapChildrenToGrid, SIGNAL(triggered()), nodeView, SLOT(snapChildrenToGrid()));
    connect(view_goToImplementation, SIGNAL(triggered()), nodeView, SLOT(goToImplementation()));
    connect(view_goToDefinition, SIGNAL(triggered()), nodeView, SLOT(goToDefinition()));
    connect(view_showConnectedNodes, SIGNAL(triggered()), nodeView, SLOT(showConnectedNodes()));
    connect(view_showManagementComponents, SIGNAL(triggered(bool)), nodeView, SLOT(showManagementComponents(bool)));

    connect(model_clearModel, SIGNAL(triggered()), nodeView, SLOT(clearModel()));
    connect(model_sortModel, SIGNAL(triggered()), this, SLOT(on_actionSortNode_triggered()));
    connect(model_validateModel, SIGNAL(triggered()), this, SLOT(on_actionValidate_triggered()));

    connect(settings_displayWindowToolbar, SIGNAL(triggered(bool)), this, SLOT(showWindowToolbar(bool)));
    connect(settings_showGridLines, SIGNAL(triggered()), this, SLOT(menuActionTriggered()));
    connect(settings_showGridLines, SIGNAL(triggered(bool)), nodeView, SLOT(toggleGridLines(bool)));
    connect(settings_selectOnConstruction, SIGNAL(triggered(bool)), nodeView, SLOT(selectNodeOnConstruction(bool)));
    connect(settings_autoCenterView, SIGNAL(triggered(bool)), nodeView, SLOT(autoCenterAspects(bool)));
    connect(settings_viewZoomAnchor, SIGNAL(triggered(bool)), nodeView, SLOT(toggleZoomAnchor(bool)));
    connect(settings_ChangeSettings, SIGNAL(triggered()), appSettings, SLOT(launchSettingsUI()));

    connect(exit, SIGNAL(triggered()), this, SLOT(on_actionExit_triggered()));

    //connect(searchBar, SIGNAL(textChanged(QString)), this, SLOT(on_SearchTextChanged(QString)));
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
    connect(cutButton, SIGNAL(clicked()), nodeView, SLOT(cut()));
    connect(copyButton, SIGNAL(clicked()), nodeView, SLOT(copy()));
    connect(pasteButton, SIGNAL(clicked()), this, SLOT(on_actionPaste_triggered()));
    connect(duplicateButton, SIGNAL(clicked()), nodeView, SLOT(duplicate()));

    connect(fitToScreenButton, SIGNAL(clicked()), nodeView, SLOT(fitToScreen()));
    connect(centerButton, SIGNAL(clicked()), nodeView, SLOT(centerOnItem()));
    connect(zoomToFitButton, SIGNAL(clicked()), this, SLOT(on_actionCenterNode_triggered()));
    connect(sortButton, SIGNAL(clicked()), this, SLOT(on_actionSortNode_triggered()));

    connect(undoButton, SIGNAL(clicked()), nodeView, SIGNAL(view_Undo()));
    connect(redoButton, SIGNAL(clicked()), nodeView, SIGNAL(view_Redo()));

    connect(nodeView, SIGNAL(view_ExportedProject(QString)), this, SLOT(writeExportedProject(QString)));
    connect(nodeView, SIGNAL(view_UndoListChanged(QStringList)), this, SLOT(updateUndoStates(QStringList)));
    connect(nodeView, SIGNAL(view_RedoListChanged(QStringList)), this, SLOT(updateRedoStates(QStringList)));
    connect(nodeView, SIGNAL(view_SetClipboardBuffer(QString)), this, SLOT(setClipboard(QString)));
    connect(nodeView, SIGNAL(view_ProjectNameChanged(QString)), this, SLOT(changeWindowTitle(QString)));

    connect(assemblyButton, SIGNAL(clicked()), this, SLOT(updateViewAspects()));
    connect(hardwareButton, SIGNAL(clicked()), this, SLOT(updateViewAspects()));
    connect(definitionsButton, SIGNAL(clicked()), this, SLOT(updateViewAspects()));
    connect(workloadButton, SIGNAL(clicked()), this, SLOT(updateViewAspects()));

    connect(this, SIGNAL(clearDocks()), hardwareDock, SLOT(clear()));
    connect(this, SIGNAL(clearDocks()), definitionsDock, SLOT(clear()));

    connect(nodeView, SIGNAL(view_NodeDeleted(QString,QString)), partsDock, SLOT(nodeDeleted(QString, QString)));
    connect(nodeView, SIGNAL(view_NodeDeleted(QString,QString)), hardwareDock, SLOT(nodeDeleted(QString, QString)));
    connect(nodeView, SIGNAL(view_NodeDeleted(QString,QString)), definitionsDock, SLOT(nodeDeleted(QString, QString)));

    connect(nodeView, SIGNAL(view_nodeSelected()), partsDock, SLOT(updateCurrentNodeItem()));
    connect(nodeView, SIGNAL(view_nodeSelected()), hardwareDock, SLOT(updateCurrentNodeItem()));
    connect(nodeView, SIGNAL(view_nodeSelected()), definitionsDock, SLOT(updateCurrentNodeItem()));

    connect(nodeView, SIGNAL(view_nodeConstructed(NodeItem*)), partsDock, SLOT(updateDock()));
    connect(nodeView, SIGNAL(view_nodeConstructed(NodeItem*)), hardwareDock, SLOT(nodeConstructed(NodeItem*)));
    connect(nodeView, SIGNAL(view_nodeConstructed(NodeItem*)), definitionsDock, SLOT(nodeConstructed(NodeItem*)));

    connect(nodeView, SIGNAL(view_nodeDestructed(NodeItem*)), partsDock, SLOT(updateDock()));
    connect(nodeView, SIGNAL(view_nodeDestructed(NodeItem*)), hardwareDock, SLOT(refreshDock()));
    connect(nodeView, SIGNAL(view_nodeDestructed(NodeItem*)), definitionsDock, SLOT(nodeDestructed(NodeItem*)));

    connect(nodeView, SIGNAL(view_EdgeDeleted(QString,QString)), hardwareDock, SLOT(edgeDeleted(QString, QString)));

    connect(nodeView, SIGNAL(view_edgeConstructed()), hardwareDock, SLOT(updateDock()));
    connect(nodeView, SIGNAL(view_edgeConstructed()), definitionsDock, SLOT(updateDock()));

    connect(nodeView, SIGNAL(view_edgeDestructed()), hardwareDock, SLOT(refreshDock()));
    connect(nodeView, SIGNAL(view_edgeDestructed()), definitionsDock, SLOT(refreshDock()));

    connect(hardwareDock, SIGNAL(dock_destructEdge(Edge*)), nodeView, SLOT(destructEdge(Edge*)));

    //For mac
    addAction(exit);
    addAction(file_newProject);
    addAction(file_importGraphML);
    addAction(file_exportGraphML);
    addAction(file_importJenkinsNodes);
    addAction(edit_undo);
    addAction(edit_redo);
    addAction(edit_cut);
    addAction(edit_copy);
    addAction(edit_paste);
    addAction(view_fitToScreen);
    addAction(view_snapToGrid);
    addAction(view_snapChildrenToGrid);
    addAction(view_goToDefinition);
    addAction(view_goToImplementation);
    addAction(view_showConnectedNodes);
    addAction(view_showManagementComponents);
    addAction(model_validateModel);
    addAction(model_clearModel);
    addAction(model_sortModel);
    addAction(settings_displayWindowToolbar);
    addAction(settings_autoCenterView);
    addAction(settings_viewZoomAnchor);
    addAction(settings_showGridLines);
    addAction(settings_selectOnConstruction);
    addAction(settings_ChangeSettings);
    // this needs fixing
    //connect(this, SIGNAL(checkDockScrollBar()), partsContainer, SLOT(checkScrollBar()));
}


/**
 * @brief MedeaWindow::resizeEvent
 * The width of the scroll areas and group boxes in the dock are fixed.
 * The height changes depending on window size and content.
 * @param event
 */
void MedeaWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    // update the stored view center point and re-center the view
    nodeView->updateViewCenterPoint();
    nodeView->recenterView();

    boxHeight = this->height()*0.77;
    partsDock->setMinimumHeight(boxHeight);
    definitionsDock->setMinimumHeight(boxHeight);
    hardwareDock->setMinimumHeight(boxHeight);

    // update dataTable size
    updateDataTable();
}


void MedeaWindow::editMultiLineData(GraphMLData *data)
{
    //
    if(data){

    }
}


/**
 * @brief MedeaWindow::setupJenkinsSettings
 */
void MedeaWindow::setupJenkinsSettings()
{
    QSettings* settings = appSettings->getSettings();
    settings->beginGroup("Jenkins");
    JENKINS_ADDRESS =  settings->value("url").toString();
    JENKINS_USERNAME =  settings->value("username").toString();
    JENKINS_PASSWORD =  settings->value("password").toString();
    settings->endGroup();
}


/**
 * @brief MedeaWindow::setupInitialSettings
 * This force sorts and centers the definitions containers before they are hidden.
 * It also sets the default values for toggle menu actions and populates the parts
 * dock and search option menu. This is only called once.
 */
void MedeaWindow::setupInitialSettings()
{
    QSettings* settings = appSettings->getSettings();

    // need to set initial toggle action values before triggering them
    settings_displayWindowToolbar->setChecked(true);
    settings_autoCenterView->setChecked(true);
    settings_showGridLines->setChecked(true);
    settings_selectOnConstruction->setChecked(false);
    view_showManagementComponents->setChecked(false);

    settings_displayWindowToolbar->triggered(true);
    settings_autoCenterView->triggered(true);
    settings_showGridLines->triggered(true);
    settings_selectOnConstruction->triggered(false);
    view_showManagementComponents->triggered(false);

    // initially show, but contract the toolbar
    toolbarButton->setChecked(false);
    toolbarButton->clicked(false);

    if (nodeView) {
        nodeView->centerAspects();
    }

    // this only needs to happen once, the whole time the application is open
    partsDock->addDockNodeItems(nodeView->getConstructableNodeKinds());

    // populate view aspects menu  once the nodeView and controller have been
    // constructed and connected - should only need to do this once
    QStringList nodeKinds = nodeView->getConstructableNodeKinds();
    nodeKinds.removeDuplicates();
    nodeKinds.sort();
    foreach (QString kind, nodeKinds) {
        QWidgetAction* action = new QWidgetAction(this);
        QCheckBox* checkBox = new QCheckBox(kind, this);
        checkBox->setStyleSheet("QCheckBox::indicator{ width: 25px; height: 25px; }"
                                "QCheckBox::checked{ color: green; font-weight: bold; }");
        connect(checkBox, SIGNAL(clicked()), this, SLOT(updateSearchLineEdits()));
        action->setDefaultWidget(checkBox);
        nodeKindsMenu->addAction(action);
    }
}


void MedeaWindow::loadSettings()
{
    QPoint pos;
    QSize size;
    QSettings* settings = appSettings->getSettings();
    settings->beginGroup("MainWindow");
    pos.setX(settings->value("x").toInt());
    pos.setY(settings->value("y").toInt());
    size.setWidth(settings->value("width").toInt());
    size.setHeight(settings->value("height").toInt());

    if(settings->value("maximized").toString() == "true"){
        setWindowState(windowState() | Qt::WindowMaximized);
    }

    resize(size);
    move(pos);
    settings->endGroup();
}


void MedeaWindow::saveSettings()
{
    qCritical() << "SAVING SETTINGS";
    //SAVE width/height
    QSettings* settings = appSettings->getSettings();
    settings->beginGroup("MainWindow");
    settings->setValue("width", size().width());
    settings->setValue("height", size().height());
    settings->setValue("x", pos().x());
    settings->setValue("y", pos().y());
    settings->setValue("maximized", isMaximized());
    settings->setValue("aspects", checkedViewAspects);
    settings->endGroup();
}


/**
 * @brief MedeaWindow::on_actionImportJenkinsNode
 */
void MedeaWindow::on_actionImportJenkinsNode()
{
    progressAction = "Importing Jenkins";

    QString groovyScript = "Jenkins_Construct_GraphMLNodesList.groovy";

    QString program = "java -jar jenkins-cli.jar";
    program += " -s " + JENKINS_ADDRESS;
    program += " groovy " + groovyScript;
    program += " --username " + JENKINS_USERNAME;
    program += " --password " + JENKINS_PASSWORD;

    myProcess = new QProcess(this);
    QDir dir;

    myProcess->setWorkingDirectory(applicationDirectory + "/Resources/Scripts/");
    //qCritical() << myProcess->workingDirectory();
    //qCritical() << program;
    //myProcess->setWorkingDirectory(DEPGEN_ROOT + "/scripts");
    connect(myProcess, SIGNAL(finished(int)), this, SLOT(loadJenkinsData(int)));
   qCritical() << applicationDirectory + "/Resources/Scripts/";
    qCritical() << program;
    myProcess->start(program);
}


/**
 * @brief MedeaWindow::on_actionNew_Project_triggered
 * At the moment it olnly allows one project to be opened at a time.
 */
void MedeaWindow::on_actionNew_Project_triggered()
{
    // ask user if they want to save current project before closing it
    QMessageBox::StandardButton saveProject = QMessageBox::question(this,
                                                                    "Creating A New Project",
                                                                    "Current project will be closed.\nSave changes?",
                                                                    QMessageBox::Yes | QMessageBox::No);
    if (saveProject == QMessageBox::Yes) {
        // if failed to export, do nothing
        if (!exportProject()) {
            return;
        }
    }else{
        return;
    }

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

    // clear item selection
    nodeView->clearSelection();
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
 * @brief MedeaWindow::on_clearModel_triggered
 * When the model is cleared or the new project menu is triggered,
 * this method resets the model, clears any current selection,
 * disables the dock buttons and clears the dock containers.
 */
void MedeaWindow::on_actionClearModel_triggered()
{
    progressAction = "Clearing Model";

    if (nodeView) {
        nodeView->clearSelection();
        nodeView->resetModel();

        clearDocks();

        /*
        // disconnected partsDock from the clearDocks signal
        // it doesn't need to be cleared the whole time the application is open
        // moved initial populating of partsDock to the constructor
        partsDock->addDockNodeItems(nodeView->getConstructableNodeKinds());
        */
    }
}


/**
 * @brief MedeaWindow::on_actionSortModel_triggered
 * This is called whne the sortNode tool button is triggered.
 * If there is a selected node, recursively sort it.
 * If there isn't, recursively sort the model.
 */
void MedeaWindow::on_actionSortNode_triggered()
{
    progressAction = "Sorting Model";

    nodeView->view_TriggerAction("Medea: Sorting Node");

    if (nodeView->getSelectedNode()){
        nodeView->sortNode(nodeView->getSelectedNode());
    } else {
        nodeView->sortEntireModel();
    }
}


/**
 * @brief MedeaWindow::on_actionValidate_triggered
 * This is called to validate a model, and will display a list of errors.
 */
void MedeaWindow::on_actionValidate_triggered()
{
    progressAction = "Validating Model";

    nodeView->view_TriggerAction("Medea: Validate");

    QDir dir;
    QString scriptPath = applicationDirectory + "/Resources/Scripts";
    //qDebug() << absPath;

    // First export Model to temporary file in scripts directory
    exportFileName = scriptPath + "/tmp.graphml";
    emit window_ExportProject();

    // transform .graphml to report.xml
    // The MEDEA.xsl transform is produced by Schematron/iso_svrl_for_xslt1.xsl
    QString program = "Xalan";
    QStringList arguments;
    QString inputFile = "tmp.graphml";
    QString outputFile = "report.xml";
    arguments << "-o" << outputFile
              << inputFile
              << "MEDEA.xsl";

    QProcess *myProcess = new QProcess(this);
    myProcess->setWorkingDirectory( scriptPath );

    //qDebug() << program << " " << arguments;

    myProcess->start(program, arguments);
    myProcess->waitForFinished();

    // launch window of messages from report.xml
    QString filename = scriptPath + "/report.xml";
    QFile xmlFile(filename);
    if (!xmlFile.exists() || !xmlFile.open(QIODevice::ReadOnly))
        return;

    // Query the graphml to get a list of all Files to process.
    QXmlQuery query;
    query.bindVariable("graphmlFile", &xmlFile);
    const QString queryMessages = QString("declare namespace svrl = \"http://purl.oclc.org/dsdl/svrl\"; doc('file:///%1')//svrl:schematron-output/svrl:failed-assert/string()")
            .arg(xmlFile.fileName());
    query.setQuery(queryMessages);
    QStringList messagesResult;
    query.evaluateTo(&messagesResult);

    xmlFile.close();

    validateResults.setupItemsTable(&messagesResult);
    validateResults.show();

}


/**
 * @brief MedeaWindow::on_actionCenterNode_triggered
 * This is called whne the centerNode tool button is triggered.
 * It zooms into and centers on the selected node.
 */
void MedeaWindow::on_actionCenterNode_triggered()
{
    progressAction = "Centering Node";

    if (nodeView->getSelectedNodeItem()) {
        nodeView->centerItem(nodeView->getSelectedNodeItem());
    }
}


/**
 * @brief MedeaWindow::on_actionPopupNewWindow
 * This is called whne the popupNode tool button is triggered.
 * It pops up the selected node to a new window.
 */
void MedeaWindow::on_actionPopupNewWindow()
{
    progressAction = "Opening New Window";

    if (nodeView->getSelectedNode()) {
        nodeView->constructNewView(nodeView->getSelectedNode());
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

void MedeaWindow::on_SearchTextChanged(QString text)
{
    if (text.length() != 0) {
        searchButton->setEnabled(true);
    } else {
        searchButton->setEnabled(false);
    }

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
        checkedKinds = nodeView->getConstructableNodeKinds();
        checkedKinds.removeDuplicates();
    }

    //progressAction = "Searching Model";

    QString searchText = searchBar->text();
    if (nodeView && searchText != "" && searchText != searchBarDefaultText) {

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
        if (itemsToDisplay.count() == 0) {
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

        // show popup list view
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
void MedeaWindow::on_searchResultItem_clicked(GraphMLItem *clickedItem)
{
    nodeView->selectAndCenter(clickedItem);
    //nodeView->selectAndCenter(0, clickedItem->getGraphML()->getID());
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
        //Try and Open File.
        QFile file(exportFileName);
        bool fileOpened = file.open(QIODevice::WriteOnly | QIODevice::Text);

        if(!fileOpened){
            QMessageBox::critical(this, "File Error", "Unable to open file: '" + exportFileName + "'! Check Permissions and Try Again!", QMessageBox::Ok);
            return;
        }

        //Create stream to write the data.
        QTextStream out(&file);
        out << data;
        file.close();

        QMessageBox::information(this, "Successfully Exported", "GraphML documented successfully exported to: '" + exportFileName +"'!", QMessageBox::Ok);
    }catch(...){
        QMessageBox::critical(this, "Exporting Error", "Unknown Error!", QMessageBox::Ok);
    }
}


/**
 * @brief MedeaWindow::updateUndoStates
 * @param list
 */
void MedeaWindow::updateUndoStates(QStringList list)
{
    if (list.size() == 0) {
        edit_undo->setEnabled(false);
    } else {
        edit_undo->setEnabled(true);
    }

    // TODO: Use undo states for the progress bar label

    /*
    edit_undo->setText(QString("Undo [%1]").arg(list.size()));

    QMenu* menu = edit_undo->menu();
    if (menu) {
        menu->clear();
    } else {
        menu = new QMenu();
        edit_undo->setMenu(menu);
    }
    foreach (QString item, list) {
        QAction* newUndo = new QAction(item, menu);
        newUndo->setEnabled(false);
        menu->addAction(newUndo);
    }
    */
}


/**
 * @brief MedeaWindow::updateRedoStates
 * @param list
 */
void MedeaWindow::updateRedoStates(QStringList list)
{
    if (list.size() == 0) {
        edit_redo->setEnabled(false);
    } else {
        edit_redo->setEnabled(true);
    }

    // TODO: Use redo states for the progress bar label

    /*
    edit_redo->setText(QString("Redo [%1]").arg(list.size()));

    QMenu* menu = edit_redo->menu();
    if (menu) {
        menu->clear();
    } else {
        menu = new QMenu();
        edit_redo->setMenu(menu);
    }
    foreach (QString item, list) {
        QAction* newRedo = new QAction(item, menu);
        newRedo->setEnabled(false);
        menu->addAction(newRedo);
    }
    */
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
 * @brief MedeaWindow::updateProjectName
 * @param label
 */
void MedeaWindow::changeWindowTitle(QString label)
{
    setWindowTitle("MEDEA - " + label);
    projectName->setText(label);
}


/**
 * @brief MedeaWindow::updateViewAspects
 * When a view aspect button is clicked, add/remove the corresponding
 * view aspect to/from the checkedViewAspects list.
 */
void MedeaWindow::updateViewAspects()
{
    QStringList newAspects = checkedViewAspects;
    QPushButton *sourceButton = qobject_cast<QPushButton*>(QObject::sender());

    if (sourceButton) {
        QString view = sourceButton->text();
        if (view == "Interface") {
            view = "Definitions";
        } else if (view == "Behaviour") {
            view = "Workload";
        }

        if (sourceButton->isChecked() && !newAspects.contains(view)) {
            newAspects.append(view);
        } else {
            newAspects.removeAll(view);
        }
    }

    if (newAspects != checkedViewAspects) {
        window_AspectsChanged(newAspects);
    }
}



/**
 * @brief MedeaWindow::setViewAspects
 * @param aspects
 */
void MedeaWindow::setViewAspects(QStringList aspects)
{
    definitionsButton->setChecked(aspects.contains("Definitions"));
    workloadButton->setChecked(aspects.contains("Workload"));
    assemblyButton->setChecked(aspects.contains("Assembly"));
    hardwareButton->setChecked(aspects.contains("Hardware"));
    checkedViewAspects = aspects;
}


/**
 * @brief MedeaWindow::setGoToMenuActions
 * This gets called everytime a node is selected.
 * It enables/disables the menu's goTo functions depending on the selected node.
 * @param action
 * @param node
 */
void MedeaWindow::setGoToMenuActions(QString action, bool enable)
{
    if (action == "definition") {
        view_goToDefinition->setEnabled(enable);
    } else if (action == "implementation") {
        view_goToImplementation->setEnabled(enable);
    }
}


/**
 * @brief MedeaWindow::showWindowToolbar
 * If the signal came from the menu, show/hide the toolbar.
 * Otherwise if it came from toolbarButton, expand/contract the toolbar.
 */
void MedeaWindow::showWindowToolbar(bool checked)
{
    QAction* senderAction = qobject_cast<QAction*>(QObject::sender());
    if (senderAction) {
        toolbar->setVisible(checked);
        return;
    }

    QToolButton* senderButton = qobject_cast<QToolButton*>(QObject::sender());
    if (senderButton) {
        if (checked) {
            toolbarButton->setToolTip("Contract Toolbar");
        } else {
            // NOTE: hover/focus doesn't leave the button until you move the mouse
            toolbarButton->setToolTip("Expand Toolbar");
        }
        // show/hide all tool buttons except for toolbarButton and the left/right spacer widgets
        foreach (QAction* action, toolbar->actions()) {
            if (action != toolbarAction && action != leftSpacerAction && action != rightSpacerAction) {
                action->setVisible(checked);
            }
        }
    }
}


/**
 * @brief MedeaWindow::menuActionTriggered
 * This method is used to update the displayed text in the progress bar
 * and to update the enabled state of the snap to grid functions.
 */
void MedeaWindow::menuActionTriggered()
{
    QAction* action = qobject_cast<QAction*>(QObject::sender());
    if (action->text().contains("Undo")) {
        progressAction = "Undoing Action";
    } else if (action->text().contains("Redo")) {
        progressAction = "Redoing Action";
    } else if (action->text().contains("Grid Lines")) {
        // in case the grid lines are turned on, this will check if there
        // is a selected item before it turns the snap to grid functions on
        graphicsItemSelected();
    }
}


/**
 * @brief MedeaWindow::resetView
 * This is called everytime a new project is created.
 * It resets the view's turned on aspects to th default.
 */
void MedeaWindow::resetView()
{
    if (nodeView) {
        nodeView->setDefaultAspects();
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

    resetGUI();
    on_actionClearModel_triggered();
    nodeView->view_ClearHistory();
    resetView();
}


/**
 * @brief MedeaWindow::exportGraphML
 * @return
 */
bool MedeaWindow::exportProject()
{
    QString filename = QFileDialog::getSaveFileName(this,
                                                    "Export .graphML",
                                                    "",
                                                    "GraphML Documents (*.graphML *.xml)");

    if (filename != "") {
        if(filename.toLower().endsWith(".graphml") || filename.toLower().endsWith(".xml")){
            exportFileName = filename;
            emit window_ExportProject();
            return true;
        }else{
            QMessageBox::critical(this, "Error", "You must Export using the either .graphML or .xml extensions.", QMessageBox::Ok);
            //CALL AGAIN IF WE Don't get a a .graphML file or a .xml File
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
 * This method makes sure that the groupbox attached to the dock button
 * that was pressed is the only groupbox being currently displayed.
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
        b = compDefinitionsButton;
    }

    // if the previously activated groupbox is still on display, hide it
    if (prevPressedButton != 0 && prevPressedButton != b) {
        prevB = b;
        prevPressedButton->hideContainer();
        b = prevB;
    }

    prevPressedButton = b;
}


/**
 * @brief MedeaWindow::updateProgressStatus
 * This updates the progress bar values.
 */
void MedeaWindow::updateProgressStatus(int value, QString status)
{
    // if something's in progress, show progress bar
    if (!progressBar->isVisible()) {
        progressLabel->setVisible(true);
        progressBar->setVisible(true);
    }

    // update displayed text
    progressLabel->setText(progressAction + "...");

    // update value
    progressBar->setValue(value);

    // once we reach 100%, reset the progress bar then hide it
    if (value == 100) {
        progressLabel->setVisible(false);
        progressBar->setVisible(false);
        progressBar->reset();
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
    // if the call came from the search bar, check if it either needs to be cleared or reset
    QLineEdit* searchEdit = qobject_cast<QLineEdit*>(QObject::sender());
    if (searchEdit && searchEdit == searchBar) {
        if (searchBar->hasFocus()) {
            if (searchBar->text() == searchBarDefaultText){
                searchBar->clear();
            }
        } else {
            if (searchBar->text().length() == 0) {
                searchBar->setText(searchBarDefaultText);
            }
        }
        return;
    }

    // if it came from a checkbox, check if the displayed text of the corresponding
    // line edit needs to be updated or reset back to the default text
    QCheckBox* checkBox = qobject_cast<QCheckBox*>(QObject::sender());
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
        } else {
            qDebug() << "MedeaWindow::updateSearchLineEdits - Not checking for this menu.";
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
 */
void MedeaWindow::displayNotification(QString notification)
{
    // add new notification to the queue
    if (!notification.isEmpty()) {
        notificationsQueue.enqueue(notification);
    }

    if (!notificationTimer->isActive()) {
        notification = notificationsQueue.dequeue();
        notificationsBar->setText(notification);
        notificationsBar->show();
        notificationTimer->start(2000);
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
    if (nodeView->getSelectedNode()) {
        view_showConnectedNodes->setEnabled(true);
        view_snapToGrid->setEnabled(settings_showGridLines->isChecked());
        view_snapChildrenToGrid->setEnabled(settings_showGridLines->isChecked());
    } else {
        view_showConnectedNodes->setEnabled(false);
        view_snapToGrid->setEnabled(false);
        view_snapChildrenToGrid->setEnabled(false);
    }
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

    if(tableModel){
        dataTable->setVisible(true);
    }else{
        dataTable->setVisible(false);
        return;
    }

    int rowCount = tableModel->rowCount();
    int vOffset = dataTable->horizontalHeader()->size().height();
    if (!firstTableUpdate) {
        vOffset += 2; // this check is only added to get rid of the initial extra gap in height
    }

    // calculate the required height
    int height = 0;
    for (int i = 0; i < rowCount; i++) {
        height += dataTable->rowHeight(i);
    }

    int maxHeight = dataTableBox->height();
    int newHeight = height + vOffset;

    // check if the datatable should be hidden or if its height needs restricting
    if (maxHeight == 0) {
        dataTable->setVisible(false);
    } else if (newHeight > maxHeight) {
        dataTable->resize(dataTable->width(), maxHeight);
    } else {
        dataTable->resize(dataTable->width(), newHeight);
    }

    dataTable->repaint();
    firstTableUpdate = false;

    int w = dataTable->width();
    int h = dataTable->height();

    // update the visible region of the groupbox to fit the dataTable
    if (w == 0 || h == 0) {
        dataTableBox->setAttribute(Qt::WA_TransparentForMouseEvents);
    } else {
        dataTableBox->setAttribute(Qt::WA_TransparentForMouseEvents, false);
        dataTableBox->setMask(QRegion(0, 0, w, h, QRegion::Rectangle));
    }

    // center the contents of the "Values" column
    dataTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    dataTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    dataTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
}


/**
 * @brief MedeaWindow::loadJenkinsData
 * @param code
 */
void MedeaWindow::loadJenkinsData(int code)
{

    if(code == 0){
        QStringList files;
        files << myProcess->readAll();

        window_ImportProjects(files);

        // this selects the Jenkins hardware cluster, opens the hardware dock
        // and show the Deployment view aspects (Assembly & Hardware)
        showImportedHardwareNodes();

        // center view aspects
        nodeView->centerAspects();

    }else{
        QMessageBox::critical(this, "Jenkins Error", "Unable to request Jenkins Data", QMessageBox::Ok);
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
                QMessageBox::critical(this, "File Error", "Unable to open file: '" + fileName + "'! Check Permissions and Try Again!", QMessageBox::Ok);
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
        nodeView->centerAspects();
    }
}


/**
 * @brief MedeaWindow::closeEvent
 * @param e
 */
void MedeaWindow::closeEvent(QCloseEvent * e)
{
    //return;
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "MEDEA",
                                                                tr("Are you sure?\n"),
                                                                QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes) {
        e->ignore();
    } else {
        e->accept();
        delete this;
    }
}


/**
 * @brief MedeaWindow::showImportedHardwareNodes
 * This is called after the Jenkins nodes are imported.
 * It selects the Jenkins hardware cluster, opens the hardware dock and display
 * the Assembly and Hardware view aspects if they aren't already turned on.
 */
void MedeaWindow::showImportedHardwareNodes()
{
    // make sure that the aspects for Deployment are turned on
    nodeView->viewDeploymentAspect();

    // select the Jenkins hardware cluster after construction
    Model* model = controller->getModel();
    if (model) {
        QList<Node*> hardwareClusters = model->getChildrenOfKind("HardwareCluster");
        if (hardwareClusters.count() > 0) {
            // at the moment, this method assumes that the only cluster is the Jenkins cluster
            nodeView->clearSelection(true, false);
            nodeView->appendToSelection(hardwareClusters.at(0));
            nodeView->snapSelectionToGrid();
        }
    }

    // if the hardware dock isn't already open, open it
    if (hardwareNodesButton->isEnabled() && !hardwareNodesButton->getSelected()) {
        hardwareNodesButton->pressed();
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
