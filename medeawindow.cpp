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

#define THREADING false
#define MIN_WIDTH 800
#define MIN_HEIGHT 600
#define RIGHT_PANEL_WIDTH 230


/**
 * @brief MedeaWindow::MedeaWindow
 * @param parent
 */
MedeaWindow::MedeaWindow(QString graphMLFile, QWidget *parent) :
    QMainWindow(parent)
{
    // this needs to happen before the menu is set up and connected
    applicationDirectory = QApplication::applicationDirPath() + "/";
    appSettings = new AppSettings(applicationDirectory, this);

    setupJenkinsSettings();

    // initialise gui and connect signals and slots
    initialiseGUI();
    makeConnections();
    newProject();

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
    prevPressedButton = 0;

    nodeView = new NodeView();

    progressBar = new QProgressBar(this);
    progressLabel = new QLabel(this);

    notificationsBar = new QLabel("Hello", this);
    notificationTimer = new QTimer(this);

    dataTableBox = new QGroupBox(this);
    dataTable = new QTableView(dataTableBox);
    delegate = new ComboBoxTableDelegate(0);

    titleToolbarBox = new QGroupBox(this);
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

    //this->setAttribute(Qt::WA_TranslucentBackground);
    //this->setStyleSheet("MedeaWindow{ background-color: transparent; }");

    // set the size for the right panel where the view buttons and data table are located
    int rightPanelWidth = RIGHT_PANEL_WIDTH;

    // setup widgets
    menuButton->setFixedSize(50,45);
    menuButton->setIconSize(menuButton->size());
    menuButton->setStyleSheet("QPushButton{ background-color: rgb(200,200,200); }"
                              "QPushButton::menu-indicator{ image: none; }");
    projectName->setFlat(true);
    projectName->setStyleSheet("font-size: 16px; text-align: left; padding: 8px;");

    assemblyButton->setFixedSize(rightPanelWidth/2.05, rightPanelWidth/2.5);
    hardwareButton->setFixedSize(rightPanelWidth/2.05, rightPanelWidth/2.5);
    definitionsButton->setFixedSize(rightPanelWidth/2.05, rightPanelWidth/2.5);
    workloadButton->setFixedSize(rightPanelWidth/2.05, rightPanelWidth/2.5);

    assemblyButton->setStyleSheet("background-color: rgb(230,130,130);");
    hardwareButton->setStyleSheet("background-color: rgb(80,140,190);");
    definitionsButton->setStyleSheet("background-color: rgb(80,180,180);");
    workloadButton->setStyleSheet("background-color: rgb(224,154,96);");

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
    notificationsBar->setStyleSheet("background-color: rgba(250,250,250,0.5);"
                                    "color: rgb(30,30,30);"
                                    "border-radius: 10px;"
                                    "padding: 0px 15px;"
                                    "font: 16px;");

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
                                "margin: 0px;"
                                "padding: 0px;"
                                "}");

    // setup mini map
    minimap = new NodeViewMinimap();
    minimap->setScene(nodeView->scene());

    //minimap->scale(.002,.002);
    minimap->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    minimap->setVerticalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
    minimap->setInteractive(false);

    minimap->setFixedSize(rightPanelWidth, rightPanelWidth/1.6);
    minimap->setStyleSheet("background-color: rgba(155,155,155,155);"
                           "border: 2px solid;"
                           "border-color: rgb(100,100,100);"
                           "border-radius: 8px;");

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
    toolbarContainerLayout->setContentsMargins(0, 5, 0, 0);

    titleToolbarBox->setLayout(titleLayout);
    titleToolbarBox->setFixedHeight(menuButton->height() + 30);
    titleToolbarBox->setStyleSheet("QGroupBox{ border: none; background-color: rgba(0,0,0,0); }");
    titleToolbarBox->setMask(QRegion(0, (titleToolbarBox->height() - menuButton->height()) / 2,
                                     menuButton->width() + projectName->width(), menuButton->height(),
                                     QRegion::Rectangle));

    topHLayout->setMargin(0);
    topHLayout->setSpacing(0);
    topHLayout->addWidget(titleToolbarBox);
    topHLayout->addStretch(1);
    topHLayout->addLayout(toolbarContainerLayout);
    topHLayout->addStretch(1);

    leftVlayout->setMargin(0);
    leftVlayout->setSpacing(0);
    leftVlayout->addLayout(topHLayout);
    leftVlayout->addSpacerItem(new QSpacerItem(20, 20));
    leftVlayout->addLayout(bodyLayout);
    leftVlayout->addStretch();

    viewButtonsGrid->addWidget(definitionsButton, 1, 1);
    viewButtonsGrid->addWidget(workloadButton, 1, 2);
    viewButtonsGrid->addWidget(assemblyButton, 2, 1);
    viewButtonsGrid->addWidget(hardwareButton, 2, 2);

    rightVlayout->setMargin(0);
    rightVlayout->setSpacing(0);
    rightVlayout->setContentsMargins(0, 10, 0, 0);
    rightVlayout->addLayout(searchLayout, 1);
    rightVlayout->addSpacerItem(new QSpacerItem(20, 30));
    rightVlayout->addLayout(viewButtonsGrid);
    rightVlayout->addSpacerItem(new QSpacerItem(20, 30));
    rightVlayout->addWidget(dataTableBox, 4);
    rightVlayout->addSpacerItem(new QSpacerItem(20, 30));
    rightVlayout->addStretch();
    rightVlayout->addWidget(minimap);

    mainHLayout->setMargin(0);
    mainHLayout->setSpacing(0);
    mainHLayout->addLayout(leftVlayout, 4);
    mainHLayout->addLayout(rightVlayout, 1);
    mainHLayout->setContentsMargins(15, 0, 25, 25);
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
    setupToolbar(toolbarContainerLayout);
    setupMultiLineBox();

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
    view_goToImplementation = view_menu->addAction(QIcon(":/Resources/Icons/implementation.png"), "Go To Implementation");
    view_menu->addSeparator();
    view_showConnectedNodes = view_menu->addAction(QIcon(":/Resources/Icons/connections.png"), "View Connections");
    view_showManagementComponents = view_menu->addAction("View Management Components");
    //view_showManagementComponents->setShortcut(QKeySequence(Qt::CTRL +Qt::Key_M));

    model_clearModel = model_menu->addAction(QIcon(":/Resources/Icons/clear.png"), "Clear Model");
    model_sortModel = model_menu->addAction(QIcon(":/Resources/Icons/sort.png"), "Sort Model");
    model_menu->addSeparator();
    model_validateModel = model_menu->addAction(QIcon(":/Resources/Icons/validate.png"), "Validate Model");

    settings_showGridLines = settings_Menu->addAction("Use Grid Lines");
    settings_Menu->addSeparator();
    settings_autoCenterView = settings_Menu->addAction("Automatically Center Views");
    settings_viewZoomAnchor = settings_Menu->addAction("Zoom View Under Mouse");
    settings_selectOnConstruction = settings_Menu->addAction("Select Entity On Construction");
    settings_Menu->addSeparator();
    settings_displayDocks = settings_Menu->addAction("Display Dock");
    settings_detachDocks = settings_Menu->addAction("Detach Dock");
    settings_Menu->addSeparator();
    settings_displayWindowToolbar = settings_Menu->addAction("Display Toolbar");
    // DEMO CHANGE
    //settings_detachWindowToolbar = settings_Menu->addAction("Detach Toolbar");
    settings_editWindowToolbar = settings_Menu->addAction("Toolbar Settings");
    settings_Menu->addSeparator();
    settings_ChangeSettings = settings_Menu->addAction("More Settings...");

    button->setMenu(menu);

    // setup toggle actions
    settings_displayDocks->setCheckable(true);
    settings_detachDocks->setCheckable(true);
    settings_displayWindowToolbar->setCheckable(true);
    // DEMO CHANGE
    //settings_detachWindowToolbar->setCheckable(true);
    settings_showGridLines->setCheckable(true);
    settings_selectOnConstruction->setCheckable(true);
    settings_autoCenterView->setCheckable(true);
    settings_viewZoomAnchor->setCheckable(true);
    view_showManagementComponents->setCheckable(true);

    // initially disable these actions
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
    dockStandAloneDialog = new QDialog(this);
    docksArea = new QGroupBox(this);
    dockButtonsBox = new QGroupBox();

    dockLayout = new QVBoxLayout();
    QVBoxLayout* dockDialogLayout = new QVBoxLayout();
    QVBoxLayout* dockAreaLayout = new QVBoxLayout();
    QHBoxLayout* dockButtonsHlayout = new QHBoxLayout();

    partsButton = new DockToggleButton("P", this);
    hardwareNodesButton = new DockToggleButton("H", this);
    compDefinitionsButton = new DockToggleButton("D", this);

    partsDock = new PartsDockScrollArea("Parts", nodeView, partsButton);
    definitionsDock = new DefinitionsDockScrollArea("Component Definitions", nodeView, compDefinitionsButton);
    hardwareDock = new HardwareDockScrollArea("Hardware Nodes", nodeView, hardwareNodesButton);

    // width of the containers are fixed
    boxWidth = (partsButton->getWidth()*3) + 30;

    // set buttonBox's size and get rid of its border
    dockButtonsBox->setStyleSheet("border: 0px; padding: 0px 7px;");
    dockButtonsBox->setFixedSize(boxWidth, 60);

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

    // remove extra space in layouts
    dockButtonsHlayout->setMargin(0);
    dockButtonsHlayout->setSpacing(0);
    dockAreaLayout->setMargin(0);
    dockAreaLayout->setSpacing(0);
    dockDialogLayout->setMargin(8);
    dockDialogLayout->setSpacing(0);

    // add widgets to/and layouts
    dockButtonsHlayout->addWidget(partsButton);
    dockButtonsHlayout->addWidget(compDefinitionsButton);
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
    layout->addWidget(docksArea, 1);

    dockStandAloneDialog->setVisible(false);
    dockStandAloneDialog->setLayout(dockDialogLayout);
    dockStandAloneDialog->setFixedWidth(boxWidth + 15);
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
    searchButton = new QPushButton(QIcon(":/Resources/Icons/search_icon.png"), "");
    searchOptionButton = new QPushButton(QIcon(":/Resources/Icons/settings.png"), "");
    searchOptionMenu = new QMenu(searchOptionButton);
    searchSuggestions = new QListView(searchButton);
    searchResults = new QDialog(this);

    QVBoxLayout* layout = new QVBoxLayout();
    QWidget* scrollableWidget = new QWidget(this);
    scrollableSearchResults = new QScrollArea(this);

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

    searchBar->setFixedSize(rightPanelWidth - (searchButton->width()*2) - 5, searchBarHeight-3);
    searchBar->setStyleSheet("background-color: rgb(230,230,230);");

    searchSuggestions->setViewMode(QListView::ListMode);
    searchSuggestions->setVisible(false);

    scrollableWidget->setMinimumWidth(rightPanelWidth + 110);
    scrollableWidget->setLayout(resultsMainLayout);
    scrollableSearchResults->setWidget(scrollableWidget);
    scrollableSearchResults->setWidgetResizable(true);
    layout->addWidget(scrollableSearchResults);

    /*
    //QPoint dialogPos = searchResults->mapFromParent(QPoint(0, height() / 2));
    //dialogPos = searchResults->mapToParent((dialogPos));
    QPoint dialogPos = this->mapToGlobal(this->pos());
    dialogPos = QPoint(0, dialogPos.y()*2);
    searchResults->move(dialogPos);
    */

    searchResults->setLayout(layout);
    searchResults->setMinimumWidth(rightPanelWidth + 110);
    searchResults->setMinimumHeight(height() / 2);
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
void MedeaWindow::setupToolbar(QVBoxLayout *layout)
{
    // DEMO CHANGE
    /*
    toolbarStandAloneDialog = new QDialog(this);
    QVBoxLayout* toolbarDialogLayout = new QVBoxLayout();
    QVBoxLayout* toolbarAreaLayout = new QVBoxLayout();
    */

    setStyleSheet("QToolButton{"
                  "border: 1px solid;"
                  "border-color: rgba(160,160,160,225);"
                  "border-radius: 10px;"
                  "background-color: rgba(210,210,210,225);"
                  "margin: 0px 1px;"
                  "}"
                  "QToolButton:hover{"
                  "border: 2px solid;"
                  "border-color: rgba(140,140,140,225);"
                  "background-color: rgba(230,230,230,230);"
                  "}"
                  "QToolButton:pressed{ background-color: rgba(240,240,240,240); }");

    toolbar = new QToolBar();
    toolbarLayout = new QVBoxLayout();

    toolbarButton = new QToolButton(this);
    toolbarButton->setCheckable(true);
    toolbarButton->setStyleSheet("QToolButton{ background-color: rgba(200,200,200,225); border-radius: 5px; }"
                                 "QToolButton:checked{ background-color: rgba(180,180,180,225); }"
                                 "QToolButton:hover{ background-color: rgba(210,210,210,235); }");

    expandIcon = QIcon(":/Resources/Icons/menu_down_arrow.png");
    contractIcon = QIcon(":/Resources/Icons/menu_up_arrow.png");

    toggleGridButton = new QToolButton(this);
    toggleGridButton->setCheckable(true);
    toggleGridButton->setStyleSheet("QToolButton:checked{ background-color: rgba(180,180,180,225); }");

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
    alignVerticalButton = new QToolButton(this);
    alignHorizontalButton = new QToolButton(this);
    popupButton = new QToolButton(this);
    backButton = new QToolButton(this);
    forwardButton = new QToolButton(this);
    contextToolbarButton = new QToolButton(this);

    toolbarButton->setIcon(expandIcon);
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
    toggleGridButton->setIcon(QIcon(":/Resources/Icons/grid.png"));
    alignVerticalButton->setIcon(QIcon(":/Resources/Icons/alignVertically.png"));
    alignHorizontalButton->setIcon(QIcon(":/Resources/Icons/alignHorizontally.png"));
    popupButton->setIcon(QIcon(":/Resources/Icons/popup.png"));
    backButton->setIcon(QIcon(":/Resources/Icons/back.png"));
    forwardButton->setIcon(QIcon(":/Resources/Icons/forward.png"));
    contextToolbarButton->setIcon(QIcon(":/Resources/Icons/toolbar.png"));

    QSize buttonSize = QSize(46,40);
    toolbarButton->setFixedSize(buttonSize.width(), buttonSize.height()/2);
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
    toggleGridButton->setFixedSize(buttonSize);
    alignVerticalButton->setFixedSize(buttonSize);
    alignHorizontalButton->setFixedSize(buttonSize);
    popupButton->setFixedSize(buttonSize);
    backButton->setFixedSize(buttonSize);
    forwardButton->setFixedSize(buttonSize);
    contextToolbarButton->setFixedSize(buttonSize);

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
    toggleGridButton->setToolTip("Turn On Grid Lines");
    alignVerticalButton->setToolTip("Align Selection Vertically");
    alignHorizontalButton->setToolTip("Align Selection Horizontally");
    popupButton->setToolTip("View Selection In New Window");
    backButton->setToolTip("Go Back");
    forwardButton->setToolTip("Go Forward");
    contextToolbarButton->setToolTip("Show Context Toolbar");

    QWidget* spacerWidgetLeft = new QWidget();
    QWidget* spacerWidgetRight = new QWidget();
    QWidget* spacerLeftMost = new QWidget();
    QWidget* spacerLeftMid = new QWidget();
    QWidget* spacerMidLeft = new QWidget();
    QWidget* spacerMidRight = new QWidget();
    QWidget* spacerRightMid = new QWidget();
    QWidget* spacerRightMost = new QWidget();
    int spacerWidth = 8;

    spacerWidgetLeft->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    spacerWidgetRight->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    spacerLeftMost->setFixedWidth(spacerWidth);
    spacerLeftMid->setFixedWidth(spacerWidth);
    spacerMidLeft->setFixedWidth(spacerWidth);
    spacerMidRight->setFixedWidth(spacerWidth);
    spacerRightMid->setFixedWidth(spacerWidth);
    spacerRightMost->setFixedWidth(spacerWidth);

    leftSpacerAction = toolbar->addWidget(spacerWidgetLeft);
    QAction* contextToolbarAction = toolbar->addWidget(contextToolbarButton);
    leftMostSpacer = toolbar->addWidget(spacerLeftMost);
    QAction* backAction = toolbar->addWidget(backButton);
    QAction* forwardAction = toolbar->addWidget(forwardButton);
    leftMidSpacer = toolbar->addWidget(spacerLeftMid);
    QAction* toggleGridAction = toolbar->addWidget(toggleGridButton);
    QAction* popupAction = toolbar->addWidget(popupButton);
    midLeftSpacer = toolbar->addWidget(spacerMidLeft);
    QAction* cutAction = toolbar->addWidget(cutButton);
    QAction* copyAction = toolbar->addWidget(copyButton);
    QAction* pasteAction = toolbar->addWidget(pasteButton);
    QAction* duplicateAction = toolbar->addWidget(duplicateButton);
    midRightSpacer = toolbar->addWidget(spacerMidRight);
    QAction* fitToScreenAction = toolbar->addWidget(fitToScreenButton);
    QAction* centerAction = toolbar->addWidget(centerButton);
    QAction* zoomToFitAction = toolbar->addWidget(zoomToFitButton);
    QAction* sortAction = toolbar->addWidget(sortButton);
    rightMidSpacer = toolbar->addWidget(spacerRightMid);
    QAction* alignVerticalAction = toolbar->addWidget(alignVerticalButton);
    QAction* alignHorizontalAction = toolbar->addWidget(alignHorizontalButton);
    rightMostSpacer = toolbar->addWidget(spacerRightMost);
    QAction* undoAction = toolbar->addWidget(undoButton);
    QAction* redoAction = toolbar->addWidget(redoButton);
    rightSpacerAction = toolbar->addWidget(spacerWidgetRight);

    toolbar->setIconSize(buttonSize*0.6);
    toolbar->setMinimumSize(toolbar->contentsRect().width(), buttonSize.height()+spacerWidth);
    toolbar->setStyle(QStyleFactory::create("windows"));

    toolbarLayout->setMargin(0);
    toolbarLayout->setSpacing(0);
    toolbarLayout->addWidget(toolbar);

    QHBoxLayout* toolbarHLayout = new QHBoxLayout();
    toolbarHLayout->addStretch();
    toolbarHLayout->addWidget(toolbarButton);
    toolbarHLayout->addStretch();
    layout->addLayout(toolbarHLayout);
    layout->addLayout(toolbarLayout);

    // DEMO CHANGE
    /*
    toolbarAreaLayout->setMargin(0);
    toolbarAreaLayout->setSpacing(0);
    toolbarAreaLayout->addLayout(toolbarLayout);

    toolbarDialogLayout->setMargin(2);
    toolbarDialogLayout->setSpacing(0);

    toolbarArea->setStyleSheet("QGroupBox{ border: none; padding: 0px; margin: 0px; background-color: rgba(0,0,0,0); }");
    toolbarArea->setMinimumSize(toolbar->contentsRect().width(), buttonSize.height()+spacerWidth);
    toolbarArea->setLayout(toolbarAreaLayout);

    toolbarStandAloneDialog->setVisible(false);
    toolbarStandAloneDialog->setFixedHeight(buttonSize.height()+spacerWidth*1.5);
    toolbarStandAloneDialog->setMinimumWidth(toolbar->contentsRect().width());
    toolbarStandAloneDialog->setWindowTitle("MEDEA - Toolbar");
    toolbarStandAloneDialog->setStyleSheet("padding: 0px; margin: 0px; background-color: rgba(175,175,175,255);");
    toolbarStandAloneDialog->setLayout(toolbarDialogLayout);
    */

    // keep a list of all the tool buttons and create a checkbox for each one
    toolbarActions[contextToolbarAction] = new QCheckBox("Context Toolbar", this);
    toolbarActions[backAction] = new QCheckBox("Back", this);
    toolbarActions[forwardAction] = new QCheckBox("Forward", this);
    toolbarActions[toggleGridAction] = new QCheckBox("Toggle Grid Lines", this);
    toolbarActions[popupAction] = new QCheckBox("Popup New Window", this);
    toolbarActions[cutAction] = new QCheckBox("Cut", this);
    toolbarActions[copyAction] = new QCheckBox("Copy", this);
    toolbarActions[pasteAction] = new QCheckBox("Paste", this);
    toolbarActions[duplicateAction] = new QCheckBox("Replicate", this);
    toolbarActions[fitToScreenAction] = new QCheckBox("Fit To Screen", this);
    toolbarActions[centerAction] = new QCheckBox("Center On", this);
    toolbarActions[zoomToFitAction] = new QCheckBox("Zoom To Fit", this);
    toolbarActions[sortAction] = new QCheckBox("Sort", this);
    toolbarActions[alignVerticalAction] = new QCheckBox("Align Vertically", this);
    toolbarActions[alignHorizontalAction] = new QCheckBox("Align Horizontally", this);
    toolbarActions[undoAction] = new QCheckBox("Undo", this);
    toolbarActions[redoAction] = new QCheckBox("Redo", this);

    // group actions into sections
    leftMostActions[backAction] = 1;
    leftMostActions[forwardAction] = 1;
    leftMidActions[toggleGridAction] = 1;
    leftMidActions[popupAction] = 1;
    midLeftActions[cutAction] = 1;
    midLeftActions[copyAction] = 1;
    midLeftActions[pasteAction] = 1;
    midLeftActions[duplicateAction] = 1;
    midRightActions[fitToScreenAction] = 1;
    midRightActions[centerAction] = 1;
    midRightActions[zoomToFitAction] = 1;
    midRightActions[sortAction] = 1;
    rightMidActions[alignVerticalAction] = 1;
    rightMidActions[alignHorizontalAction] = 1;
    rightMostActions[undoAction] = 1;
    rightMostActions[redoAction] = 1;

    // store spacer actions in a list too
    checkedToolbarSpacers.append(leftMostSpacer);
    checkedToolbarSpacers.append(leftMidSpacer);
    checkedToolbarSpacers.append(midLeftSpacer);
    checkedToolbarSpacers.append(midRightSpacer);
    checkedToolbarSpacers.append(rightMidSpacer);
    checkedToolbarSpacers.append(rightMostSpacer);

    // add checkboxes to the toolbar settings popup dialog
    QVBoxLayout* checkboxLayout = new QVBoxLayout();
    QStringList initiallyHidden(QStringList() << "Grid" << "Popup" << "Align" << "Back" << "Forward");

    foreach (QCheckBox* cb, toolbarActions.values() ) {

        checkboxLayout->addWidget(cb);
        connect(cb, SIGNAL(clicked(bool)), this, SLOT(updateCheckedToolbarActions(bool)));

        // initially hide some of the tool buttons
        bool hidden = false;
        foreach (QString s, initiallyHidden) {
            if (cb->text().contains(s)) {
                cb->clicked(false);
                cb->setChecked(false);
                hidden = true;
                if (cb->text() == "Back" || cb->text() == "Forward") {
                    cb->setEnabled(false);
                }
                break;
            }
        }
        if (!hidden) {
            cb->setChecked(true);
            checkedToolbarActions.append(toolbarActions.key(cb));
        }
    }

    toolbarSettingsDialog = new QDialog(this);
    toolbarSettingsDialog->setLayout(checkboxLayout);
    toolbarSettingsDialog->setWindowTitle("Available Tool Buttons");
    toolbarSettingsDialog->setStyleSheet("QCheckBox::indicator{ width: 25px; height: 25px; }");
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

    connect(minimap, SIGNAL(minimap_Pressed(QMouseEvent*)), nodeView, SLOT(minimapPressed(QMouseEvent*)));
    connect(minimap, SIGNAL(minimap_Moved(QMouseEvent*)), nodeView, SLOT(minimapMoved(QMouseEvent*)));
    connect(minimap, SIGNAL(minimap_Released(QMouseEvent*)), nodeView, SLOT(minimapReleased(QMouseEvent*)));
    //connect(minimap, SIGNAL(minimap_Panned(QPointF)), nodeView, SLOT(minimapPan(QPointF)));

    connect(minimap, SIGNAL(minimap_Scrolled(int)), nodeView, SLOT(scrollEvent(int)));

    connect(notificationTimer, SIGNAL(timeout()), notificationsBar, SLOT(hide()));
    connect(notificationTimer, SIGNAL(timeout()), this, SLOT(checkNotificationsQueue()));
    connect(nodeView, SIGNAL(view_displayNotification(QString)), this, SLOT(displayNotification(QString)));

    connect(projectName, SIGNAL(clicked()), nodeView, SLOT(selectModel()));

    connect(file_newProject, SIGNAL(triggered()), this, SLOT(on_actionNew_Project_triggered()));
    connect(file_importGraphML, SIGNAL(triggered()), this, SLOT(on_actionImport_GraphML_triggered()));
    connect(file_exportGraphML, SIGNAL(triggered()), this, SLOT(on_actionExport_GraphML_triggered()));
    connect(file_importJenkinsNodes, SIGNAL(triggered()), this, SLOT(on_actionImportJenkinsNode()));
    connect(this, SIGNAL(window_ExportProject()), nodeView, SIGNAL(view_ExportProject()));
    connect(this, SIGNAL(window_ImportProjects(QStringList)), nodeView, SIGNAL(view_ImportProjects(QStringList)));

    connect(edit_undo, SIGNAL(triggered()), this, SLOT(menuActionTriggered()));
    connect(edit_redo, SIGNAL(triggered()), this, SLOT(menuActionTriggered()));

    connect(edit_undo, SIGNAL(triggered()), nodeView, SLOT(undo()));
    connect(edit_redo, SIGNAL(triggered()), nodeView, SLOT(redo()));
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

    connect(settings_displayDocks, SIGNAL(triggered(bool)), this, SLOT(showDocks(bool)));
    connect(settings_detachDocks, SIGNAL(triggered(bool)), this, SLOT(detachDocks(bool)));
    connect(settings_displayWindowToolbar, SIGNAL(triggered(bool)), this, SLOT(showWindowToolbar(bool)));
    // DEMO CHANGE
    //connect(settings_detachWindowToolbar, SIGNAL(triggered(bool)), this, SLOT(detachWindowToolbar(bool)));
    connect(settings_editWindowToolbar, SIGNAL(triggered()), toolbarSettingsDialog, SLOT(show()));
    connect(settings_selectOnConstruction, SIGNAL(triggered(bool)), nodeView, SLOT(selectNodeOnConstruction(bool)));
    connect(settings_autoCenterView, SIGNAL(triggered(bool)), nodeView, SLOT(autoCenterAspects(bool)));
    connect(settings_viewZoomAnchor, SIGNAL(triggered(bool)), nodeView, SLOT(toggleZoomAnchor(bool)));
    connect(settings_ChangeSettings, SIGNAL(triggered()), appSettings, SLOT(launchSettingsUI()));

    connect(settings_showGridLines, SIGNAL(triggered()), this, SLOT(menuActionTriggered()));
    connect(settings_showGridLines, SIGNAL(triggered(bool)), toggleGridButton, SLOT(setChecked(bool)));
    connect(settings_showGridLines, SIGNAL(triggered(bool)), nodeView, SLOT(toggleGridLines(bool)));

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
    connect(alignVerticalButton, SIGNAL(clicked()), nodeView, SLOT(alignSelectionVertically()));
    connect(alignHorizontalButton, SIGNAL(clicked()), nodeView, SLOT(alignSelectionHorizontally()));
    connect(popupButton, SIGNAL(clicked()), nodeView, SLOT(constructNewView()));
    connect(backButton, SIGNAL(clicked()), nodeView, SLOT(moveViewBack()));
    connect(forwardButton, SIGNAL(clicked()), nodeView, SLOT(moveViewForward()));
    connect(contextToolbarButton, SIGNAL(clicked()), nodeView, SLOT(showToolbar()));

    connect(toggleGridButton, SIGNAL(clicked(bool)), settings_showGridLines, SLOT(setChecked(bool)));
    connect(toggleGridButton, SIGNAL(clicked(bool)), settings_showGridLines, SIGNAL(triggered(bool)));

    connect(nodeView, SIGNAL(view_ExportedProject(QString)), this, SLOT(writeExportedProject(QString)));
    connect(nodeView, SIGNAL(view_UndoListChanged(QStringList)), this, SLOT(updateUndoStates(QStringList)));
    connect(nodeView, SIGNAL(view_RedoListChanged(QStringList)), this, SLOT(updateRedoStates(QStringList)));
    connect(nodeView, SIGNAL(view_SetClipboardBuffer(QString)), this, SLOT(setClipboard(QString)));
    connect(nodeView, SIGNAL(view_ProjectNameChanged(QString)), this, SLOT(changeWindowTitle(QString)));

    connect(assemblyButton, SIGNAL(clicked()), this, SLOT(updateViewAspects()));
    connect(hardwareButton, SIGNAL(clicked()), this, SLOT(updateViewAspects()));
    connect(definitionsButton, SIGNAL(clicked()), this, SLOT(updateViewAspects()));
    connect(workloadButton, SIGNAL(clicked()), this, SLOT(updateViewAspects()));

    connect(dockStandAloneDialog, SIGNAL(finished(int)), this, SLOT(detachedDockClosed()));
    // DEMO CHANGE
    //connect(toolbarStandAloneDialog, SIGNAL(finished(int)), this, SLOT(detachedToolbarClosed()));

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

    connect(dataTable, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(dataTableDoubleClicked(QModelIndex)));

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
 * This is called when the main window is resized.
 * It doesn't however, always pick up on maximise/un-maximise events.
 * @param event
 */
void MedeaWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateWidgetsOnWindowChanged();
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
    if (event->type() == QEvent::WindowStateChange) {
        updateWidgetsOnWindowChanged();
    }
}


/**
 * @brief MedeaWindow::updateWidgetsOnWindowChanged
 * This is called when the GUI widgets size/pos need to be
 * updated after the window has been changed.
 */
void MedeaWindow::updateWidgetsOnWindowChanged()
{
    double windowRatio = 0.8;
    double containerRatio = 0.9;

    // check where the dock is currently being shown and base its height on that
    if (settings_detachDocks->isChecked()) {
        boxHeight = dockStandAloneDialog->height() * windowRatio;
    } else {
        boxHeight = this->height() * windowRatio;
    }

    // update the dock's and their containers' sizes
    partsDock->setMinimumHeight(boxHeight * containerRatio);
    definitionsDock->setMinimumHeight(boxHeight * containerRatio);
    hardwareDock->setMinimumHeight(boxHeight * containerRatio);

    docksArea->setMinimumHeight(boxHeight);
    //dockStandAloneDialog->setMinimumHeight(boxHeight + 30);
    dockStandAloneDialog->setFixedHeight(boxHeight + 30);

    // update the masks' regions
    updateWidgetMask(docksArea, dockButtonsBox, true);

    // update the stored view center point and re-center the view
    if (nodeView && controller) {
        nodeView->updateViewCenterPoint();
        nodeView->recenterView();
    }

    // update dataTable size
    updateDataTable();
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
    settings_displayDocks->setChecked(true);
    settings_detachDocks->setChecked(false);
    settings_displayWindowToolbar->setChecked(true);
    // DEMO CHANGE
    //settings_detachWindowToolbar->setChecked(false);
    settings_autoCenterView->setChecked(true);
    settings_showGridLines->setChecked(true);
    settings_selectOnConstruction->setChecked(false);
    view_showManagementComponents->setChecked(false);

    settings_displayDocks->triggered(true);
    settings_detachDocks->triggered(false);
    settings_displayWindowToolbar->triggered(true);
    // DEMO CHANGE
    //settings_detachWindowToolbar->triggered(false);
    settings_autoCenterView->triggered(true);
    settings_showGridLines->triggered(true);
    settings_selectOnConstruction->triggered(false);
    view_showManagementComponents->triggered(false);

    // initially show, but contract the toolbar
    toolbarButton->setChecked(false);
    toolbarButton->clicked(false);

    if (nodeView) {
        nodeView->fitToScreen();
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

    // hide initial notifications
    notificationsBar->hide();
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
    }else if(saveProject != QMessageBox::No){
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

    nodeView->triggerAction("Medea: Sorting Node");

    if (nodeView->getSelectedNode()){
        nodeView->sortNode(nodeView->getSelectedNode());
    } else {
        nodeView->sortEntireModel();
        displayNotification("Sorted Whole Model");
    }
}


/**
 * @brief MedeaWindow::on_actionValidate_triggered
 * This is called to validate a model and will display a list of errors.
 */
void MedeaWindow::on_actionValidate_triggered()
{
    progressAction = "Validating Model";

    nodeView->triggerAction("Medea: Validate");

    QDir dir;
    QString scriptPath = applicationDirectory + "/Resources/Scripts";
    //qDebug() << absPath;

    // First export Model to temporary file in scripts directory
    exportFileName = scriptPath + "/tmp.graphml";
    emit window_ExportProject();

    // transform .graphml to report.xml
    // The MEDEA.xsl transform is produced by Schematron/iso_svrl_for_xslt1.xsl
    QString program = "java";
    QStringList arguments;
    QString inputFile = "tmp.graphml";
    QString outputFile = "report.xml";
    arguments << "-jar" << "xalan.jar"
              << "-in" << inputFile
              << "-xsl" << "MEDEA.xsl"
              << "-out" << outputFile;

    // alternative if using xalan-c
    //  QString program =  "Xalan";
    //  arguments << "-o" << outputFile
    //            << inputFile
    //            << "MEDEA.xsl";

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
    } else {
        displayNotification("Select Entity To Center On");
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
    qDebug() << "clipboard text: " << clipboard->text();
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
            /*
            if (scrollableSearchResults->isVisible()) {
                scrollableSearchResults->setVisible(false);
            }*/
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
        //scrollableSearchResults->show();
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
    projectName->setFixedSize(projectName->fontMetrics().width(label) + 20,
                              projectName->height());
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
 * @param checked
 */
void MedeaWindow::showWindowToolbar(bool checked)
{
    QAction* senderAction = qobject_cast<QAction*>(QObject::sender());
    if (senderAction) {
        // DEMO CHANGE
        toolbar->setVisible(checked);
        toolbarButton->setVisible(checked);
        //toolbarArea->setVisible(!settings_detachWindowToolbar->isChecked() && checked);
        //toolbarStandAloneDialog->setVisible(settings_detachWindowToolbar->isChecked() && checked);
        //settings_detachWindowToolbar->setEnabled(checked);
        return;
    }

    QToolButton* senderButton = qobject_cast<QToolButton*>(QObject::sender());
    if (senderButton) {
        // show/hide all tool buttons
        foreach (QAction* action, checkedToolbarActions) {
            action->setVisible(checked);
        }
        foreach (QAction* spacer, checkedToolbarSpacers) {
            spacer->setVisible(checked);
        }
        if (checked) {
            toolbarButton->setToolTip("Contract Toolbar");
            //toolbarButton->icon().swap(contractIcon);
            toolbar->clearMask();
        } else {
            // NOTE: hover/focus doesn't leave the button until you move the mouse
            toolbarButton->setToolTip("Expand Toolbar");
            //toolbarButton->icon().swap(expandIcon);
            toolbar->setMask(QRegion(0,0,1,1, QRegion::Ellipse));
        }
    }
}


/**
 * @brief MedeaWindow::detachWindowToolbar
 * @param checked
 */
void MedeaWindow::detachWindowToolbar(bool checked)
{
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
    settings_displayWindowToolbar->setChecked(false);
    settings_displayWindowToolbar->triggered(false);
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
    QAction* spacerAction;

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
        if (visible) {
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
                            maskWidget->height() + border.width(),
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
        if (settings_showGridLines->isChecked()) {
            toggleGridButton->setToolTip("Turn Off Grid Lines");
        } else {
            toggleGridButton->setToolTip("Turn On Grid Lines");
        }

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
    if (nodeView) {
        nodeView->view_ClearHistory();
        //nodeView->setDefaultAspects();
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

    // this allows mouse events to pass through the dock's hidden
    // groupbox when none of the docks are currently opened
    if (!partsButton->getSelected() &&
            !compDefinitionsButton->getSelected() &&
            !hardwareNodesButton->getSelected()) {
        updateWidgetMask(docksArea, dockButtonsBox);
    } else {
        docksArea->clearMask();
    }
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
 * @brief MedeaWindow::showDocks
 * @param checked
 */
void MedeaWindow::showDocks(bool checked)
{
    dockStandAloneDialog->setVisible(settings_detachDocks->isChecked() && checked);
    docksArea->setVisible(!settings_detachDocks->isChecked() && checked);
    settings_detachDocks->setEnabled(checked);
}


/**
 * @brief MedeaWindow::updateDockView
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

    dockStandAloneDialog->setVisible(settings_displayDocks->isChecked() && checked);
    docksArea->setVisible(settings_displayDocks->isChecked() && !checked);
}


/**
 * @brief MedeaWindow::detachedDockClosed
 */
void MedeaWindow::detachedDockClosed()
{
    settings_displayDocks->setChecked(false);
    settings_displayDocks->triggered(false);
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
    if (tableModel) {
        dataTableBox->setVisible(true);
    } else {
        dataTableBox->setVisible(false);
        return; // this is the freaking reason why it wasn't re-allowing mouse events!!!
    }

    int vOffset = dataTable->horizontalHeader()->size().height();
    vOffset += 2; // this check is only added to get rid of the initial extra gap in height

    // calculate the required height
    int height = 0;
    for (int i = 0; i < tableModel->rowCount(); i++) {
        height += dataTable->rowHeight(i);
    }

    //int maxHeight = dataTableBox->height();
    int newHeight = height + vOffset;

    dataTable->resize(dataTable->width(), newHeight);

    // check if the datatable should be hidden or if its height needs restricting
    /*
    if (maxHeight == 0) {
        dataTableBox->setVisible(false);
    } else if (newHeight > maxHeight) {
        dataTable->resize(dataTable->width(), maxHeight);
    } else {
        dataTable->resize(dataTable->width(), newHeight);
    }
    */

    int w = dataTable->width();
    int h = dataTable->height();

    // update the visible region of the groupbox to fit the dataTable
    if (w == 0 || h == 0) {
        dataTableBox->setVisible(false);
    } else {
        //dataTableBox->setMask(QRegion(0, 0, w, h, QRegion::Rectangle));
        updateWidgetMask(dataTableBox, dataTable);
    }

    // align the contents of the datatable
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
        nodeView->fitToScreen();

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
        nodeView->fitToScreen();
    }
}


/**
 * @brief MedeaWindow::closeEvent
 * @param e
 */
void MedeaWindow::closeEvent(QCloseEvent * e)
{
    return;
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
    txtMultiLine = new QPlainTextEdit();
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
