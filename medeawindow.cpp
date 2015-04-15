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


/**
 * @brief MedeaWindow::MedeaWindow
 * @param parent
 */
MedeaWindow::MedeaWindow(QString graphMLFile, QWidget *parent) :
    QMainWindow(parent)
{
    // this needs to happen before the menu is set up and connected
    setupJenkinsSettings();

    // initialise gui and connect signals and slots
    initialiseGUI();
    makeConnections();
    newProject();

    on_SearchTextChanged("");

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
    firstTableUpdate = true;

    nodeView = new NodeView();
    toolbar = new QToolBar();
    dataTable = new QTableView();
    delegate = new ComboBoxTableDelegate(0);
    appSettings = new AppSettings(this);

    dataTableBox = new QGroupBox();
    projectName = new QPushButton("Model");
    assemblyButton = new QPushButton("Assembly");
    hardwareButton = new QPushButton("Hardware");
    workloadButton = new QPushButton("Behaviour");
    definitionsButton = new QPushButton("Interface");

    QPushButton *menuButton = new QPushButton(QIcon(":/Resources/Icons/menuIcon.png"), "");
    searchBar = new QLineEdit();
    searchButton = new QPushButton(QIcon(":/Resources/Icons/search_icon.png"), "");
    QVBoxLayout *tableLayout = new QVBoxLayout();

    // set the size for the right panel where the view buttons and data table are located
    int rightPanelWidth = 210;

    // set central widget and window size
    this->setCentralWidget(nodeView);
    this->setMinimumSize(MIN_WIDTH, MIN_HEIGHT);
    nodeView->setMinimumSize(MIN_WIDTH, MIN_HEIGHT);
    nodeView->viewport()->setMinimumSize(MIN_WIDTH, MIN_HEIGHT);

    // setup widgets
    menuButton->setFixedSize(50,45);
    menuButton->setIconSize(menuButton->size());
    projectName->setFlat(true);
    projectName->setFixedWidth(rightPanelWidth-menuButton->width()-10);
    searchButton->setFixedSize(45, 28);
    searchButton->setIconSize(searchButton->size()*0.65);
    searchBar->setFixedSize(rightPanelWidth - searchButton->width() - 5, 25);
    searchBar->setStyleSheet("background-color: rgba(230,230,230,1);");
    projectName->setStyleSheet("font-size: 16px; text-align: left;");
    menuButton->setStyleSheet("QPushButton{ background-color: rgba(220,220,220,0.5); }"
                              "QPushButton::menu-indicator{ image: none; }");
    assemblyButton->setFixedSize(rightPanelWidth/2.05, rightPanelWidth/2.5);
    hardwareButton->setFixedSize(rightPanelWidth/2.05, rightPanelWidth/2.5);
    definitionsButton->setFixedSize(rightPanelWidth/2.05, rightPanelWidth/2.5);
    workloadButton->setFixedSize(rightPanelWidth/2.05, rightPanelWidth/2.5);
    assemblyButton->setStyleSheet("background-color: rgb(230,130,130);");
    hardwareButton->setStyleSheet("background-color: rgb(80,140,190);");
    definitionsButton->setStyleSheet("background-color: rgb(80,180,180);");
    workloadButton->setStyleSheet("background-color: rgb(224,154,96);");


    // setup and add dataTable/dataTableBox widget/layout
    dataTable->setItemDelegateForColumn(2, delegate);
    dataTable->setFixedWidth(rightPanelWidth);
    dataTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    tableLayout->setMargin(0);
    tableLayout->setContentsMargins(0,0,0,0);
    tableLayout->addWidget(dataTable);

    dataTableBox->setFixedWidth(rightPanelWidth);
    dataTableBox->setLayout(tableLayout);
    dataTableBox->setStyleSheet("QGroupBox {"
                                "background-color: rgba(0,0,0,0);"
                                "border: 0px;"
                                "}");
    // layouts
    QHBoxLayout *mainHLayout = new QHBoxLayout();
    QHBoxLayout *topHLayout = new QHBoxLayout();
    QVBoxLayout *leftVlayout = new QVBoxLayout();
    QVBoxLayout *rightVlayout =  new QVBoxLayout();
    QHBoxLayout *titleLayout = new QHBoxLayout();
    QHBoxLayout *searchLayout = new QHBoxLayout();
    QHBoxLayout *bodyLayout = new QHBoxLayout();
    QGridLayout *viewButtonsGrid = new QGridLayout();

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

    searchLayout->addWidget(searchBar, 3);
    searchLayout->addWidget(searchButton, 1);

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

    mainHLayout->setMargin(0);
    mainHLayout->setSpacing(0);
    mainHLayout->addLayout(leftVlayout, 4);
    mainHLayout->addLayout(rightVlayout, 1);
    mainHLayout->setContentsMargins(25, 25, 25, 25);
    nodeView->setLayout(mainHLayout);

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

    rightVlayout->addWidget(minimap);

    // other settings
    assemblyButton->setCheckable(true);
    hardwareButton->setCheckable(true);
    definitionsButton->setCheckable(true);
    workloadButton->setCheckable(true);

    // setup the menu, dock and toolbar
    setupMenu(menuButton);
    setupDock(bodyLayout);
    setupToolbar();
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
    view_menu = menu->addMenu(QIcon(":/Resources/Icons/view.png"), "View");
    menu->addSeparator();
    model_menu = menu->addMenu(QIcon(":/Resources/Icons/model.png"), "Model");
    menu->addSeparator();
    settings_ChangeSettings = menu->addAction(QIcon(":/Resources/Icons/settings.png"), "Settings");
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

    view_fitToScreen = view_menu->addAction(QIcon(":/Resources/Icons/fitToScreen.png"), "Fit to Screen");
    view_fitToScreen->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Space));
    view_menu->addSeparator();
    view_goToDefinition = view_menu->addAction(QIcon(":/Resources/Icons/definition.png"), "Go to Definition");
    view_goToDefinition->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_D));
    view_goToImplementation = view_menu->addAction(QIcon(":/Resources/Icons/implementation.png"), "Go to Implementation");
    view_goToImplementation->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_I));
    view_menu->addSeparator();
    view_showManagementComponents = view_menu->addAction("Show Management Components");
    view_menu->addSeparator();
    view_showGridLines = view_menu->addAction("Show Grid Lines");
    view_showGridLines->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_G));
    view_autoCenterView = view_menu->addAction("Automatically Center Views");
    view_selectOnConstruction = view_menu->addAction("Select Node on Construction");

    model_clearModel = model_menu->addAction(QIcon(":/Resources/Icons/clear.png"), "Clear Model");
    model_clearModel->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    model_sortModel = model_menu->addAction(QIcon(":/Resources/Icons/sort.png"), "Sort Model");
    model_sortModel->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
    model_menu->addSeparator();
    model_validateModel = model_menu->addAction(QIcon(":/Resources/Icons/validate.png"), "Validate Model");

    button->setMenu(menu);

    // setup toggle actions
    view_autoCenterView->setCheckable(true);
    view_showGridLines->setCheckable(true);
    view_selectOnConstruction->setCheckable(true);
    view_showManagementComponents->setCheckable(true);

    // initially disable model & goto menu actions
    model_validateModel->setEnabled(false);
    view_goToDefinition->setEnabled(false);
    view_goToImplementation->setEnabled(false);
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
    layout->addStretch(3);
}


/**
 * @brief MedeaWindow::setupToolbar
 * Initialise and setup toolbar widgets.
 */
void MedeaWindow::setupToolbar()
{
    QSize buttonSize = QSize(46,40);

    toolbar->setIconSize(buttonSize*0.6);
    toolbar->setStyleSheet("QToolButton{"
                           "border: 1px solid grey;"
                           "border-radius: 10px;"
                           "background-color: rgba(200,200,200,225);"
                           "margin: 0px 3px 0px 3px;"
                           "}");

    cutButton = new QToolButton(this);
    copyButton = new QToolButton(this);
    pasteButton = new QToolButton(this);
    sortButton = new QToolButton(this);
    centerButton = new QToolButton(this);
    popupButton = new QToolButton(this);
    snapToGridButton = new QToolButton(this);
    snapChildrenToGridButton = new QToolButton(this);
    zoomToFitButton = new QToolButton(this);
    fitToScreenButton = new QToolButton(this);
    duplicateButton = new QToolButton(this);
    alignSelectionVertical = new QToolButton(this);
    alignSelectionHorizontal = new QToolButton(this);

    cutButton->setIcon(QIcon(":/Resources/Icons/cut.png"));
    copyButton->setIcon(QIcon(":/Resources/Icons/copy.png"));
    pasteButton->setIcon(QIcon(":/Resources/Icons/paste.png"));
    sortButton->setIcon(QIcon(":/Resources/Icons/sort.png"));
    centerButton->setIcon(QIcon(":/Resources/Icons/center.png"));
    popupButton->setIcon(QIcon(":/Resources/Icons/popup.png"));
    snapToGridButton->setIcon(QIcon(":/Resources/Icons/snapToGrid.png"));
    snapChildrenToGridButton->setIcon(QIcon(":/Resources/Icons/snapChildrenToGrid.png"));
    zoomToFitButton->setIcon(QIcon(":/Resources/Icons/zoomToFit.png"));
    fitToScreenButton->setIcon(QIcon(":/Resources/Icons/fitToScreen.png"));
    duplicateButton->setIcon(QIcon(":/Resources/Icons/duplicate.png"));

    alignSelectionVertical->setText("|");
    alignSelectionHorizontal->setText("_");


    cutButton->setFixedSize(buttonSize);
    copyButton->setFixedSize(buttonSize);
    pasteButton->setFixedSize(buttonSize);
    sortButton->setFixedSize(buttonSize);
    centerButton->setFixedSize(buttonSize);
    popupButton->setFixedSize(buttonSize);
    snapToGridButton->setFixedSize(buttonSize);
    snapChildrenToGridButton->setFixedSize(buttonSize);
    zoomToFitButton->setFixedSize(buttonSize);
    fitToScreenButton->setFixedSize(buttonSize);
    duplicateButton->setFixedSize(buttonSize);
    alignSelectionVertical->setFixedSize(buttonSize);
    alignSelectionHorizontal->setFixedSize(buttonSize);

    cutButton->setToolTip("Cut Node");
    copyButton->setToolTip("Copy Node");
    pasteButton->setToolTip("Paste Node");
    sortButton->setToolTip("Sort Node");
    centerButton->setToolTip("Center on Node");
    popupButton->setToolTip("Show Node In New Window");
    snapToGridButton->setToolTip("Snap Selected Nodes to Grid");
    snapChildrenToGridButton->setToolTip("Snap Children Nodes to Grid");
    zoomToFitButton->setToolTip("Zoom to fit Node");
    fitToScreenButton->setToolTip("Fit View to Screen");
    duplicateButton->setToolTip("Replicate Node");
    alignSelectionVertical->setToolTip("Align Selected Nodes Vertically");
    alignSelectionHorizontal->setToolTip("Align Selected Nodes Horizontally");

    QWidget* spacerWidgetLeft = new QWidget();
    QWidget* spacerWidgetRight = new QWidget();
    QWidget* spacerWidget1 = new QWidget();
    QWidget* spacerWidget2 = new QWidget();
    QWidget* spacerWidget3 = new QWidget();
    QWidget* spacerWidget4 = new QWidget();
    int spacerWidth = 3;

    spacerWidgetLeft->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    spacerWidgetRight->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    spacerWidget1->setFixedWidth(spacerWidth);
    spacerWidget2->setFixedWidth(spacerWidth);
    spacerWidget3->setFixedWidth(spacerWidth);
    spacerWidget4->setFixedWidth(spacerWidth);

    toolbar->addWidget(spacerWidgetLeft);
    toolbar->addWidget(cutButton);
    toolbar->addWidget(copyButton);
    toolbar->addWidget(pasteButton);
    toolbar->addWidget(duplicateButton);
    toolbar->addWidget(spacerWidget1);
    toolbar->addSeparator();
    toolbar->addWidget(spacerWidget2);
    toolbar->addWidget(fitToScreenButton);
    toolbar->addWidget(centerButton);
    toolbar->addWidget(zoomToFitButton);
    toolbar->addWidget(spacerWidget3);
    toolbar->addSeparator();
    toolbar->addWidget(spacerWidget4);
    toolbar->addWidget(sortButton);
    toolbar->addWidget(snapChildrenToGridButton);
    toolbar->addWidget(snapToGridButton);
    toolbar->addWidget(alignSelectionVertical);
    toolbar->addWidget(alignSelectionHorizontal);
    toolbar->addWidget(spacerWidgetRight);

    //toolbar->addWidget(popupButton);
    popupButton->hide();

    toolbar->setFixedSize(toolbar->contentsRect().width(), buttonSize.height()+spacerWidth);
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
    connect(this, SIGNAL(window_AspectsChanged(QStringList)), nodeView, SLOT(setAspects(QStringList)));
    connect(nodeView, SIGNAL(view_GUIAspectChanged(QStringList)), this, SLOT(setViewAspects(QStringList)));
    connect(nodeView, SIGNAL(view_updateGoToMenuActions(QString,bool)), this, SLOT(setGoToMenuActions(QString,bool)));
    connect(nodeView, SIGNAL(view_SetAttributeModel(AttributeTableModel*)), this, SLOT(setAttributeModel(AttributeTableModel*)));
    connect(nodeView, SIGNAL(view_updateProgressStatus(int,QString)), this, SLOT(updateProgressStatus(int,QString)));
    connect(nodeView, SIGNAL(view_showWindowToolbar()), this, SLOT(showWindowToolbar()));

    connect(projectName, SIGNAL(clicked()), nodeView, SLOT(view_SelectModel()));

    connect(settings_ChangeSettings, SIGNAL(triggered()), appSettings, SLOT(launchSettingsUI()));

    connect(file_newProject, SIGNAL(triggered()), this, SLOT(on_actionNew_Project_triggered()));
    connect(file_importGraphML, SIGNAL(triggered()), this, SLOT(on_actionImport_GraphML_triggered()));
    connect(file_exportGraphML, SIGNAL(triggered()), this, SLOT(on_actionExport_GraphML_triggered()));
    connect(file_importJenkinsNodes, SIGNAL(triggered()), this, SLOT(on_actionImportJenkinsNode()));
    connect(this, SIGNAL(window_ExportProject()), nodeView, SIGNAL(view_ExportProject()));
    connect(this, SIGNAL(window_ImportProjects(QStringList)), nodeView, SIGNAL(view_ImportProjects(QStringList)));

    connect(edit_undo, SIGNAL(triggered()), nodeView, SIGNAL(view_Undo()));
    connect(edit_redo, SIGNAL(triggered()), nodeView, SIGNAL(view_Redo()));
    connect(edit_cut, SIGNAL(triggered()), nodeView, SLOT(cut()));
    connect(edit_copy, SIGNAL(triggered()), nodeView, SLOT(copy()));
    connect(edit_paste, SIGNAL(triggered()), this, SLOT(on_actionPaste_triggered()));
    connect(this, SIGNAL(window_PasteData(QString)), nodeView, SLOT(paste(QString)));

    connect(searchButton, SIGNAL(pressed()), this, SLOT(on_actionSearch()));
    connect(searchBar, SIGNAL(textChanged(QString)), this, SLOT(on_SearchTextChanged(QString)));
    connect(searchBar, SIGNAL(returnPressed()), this, SLOT(on_actionSearch()));

    connect(view_fitToScreen, SIGNAL(triggered()), nodeView, SLOT(fitToScreen()));
    connect(view_autoCenterView, SIGNAL(triggered(bool)), nodeView, SLOT(autoCenterAspects(bool)));
    connect(view_showGridLines, SIGNAL(triggered(bool)), nodeView, SLOT(toggleGridLines(bool)));
    connect(view_selectOnConstruction, SIGNAL(triggered(bool)), nodeView, SLOT(selectNodeOnConstruction(bool)));
    connect(view_showManagementComponents, SIGNAL(triggered(bool)), nodeView, SLOT(showManagementComponents(bool)));

    connect(view_goToDefinition, SIGNAL(triggered()), nodeView, SLOT(goToDefinition()));
    connect(view_goToImplementation, SIGNAL(triggered()), nodeView, SLOT(goToImplementation()));

    connect(model_clearModel, SIGNAL(triggered()), nodeView, SLOT(clearModel()));
    connect(model_sortModel, SIGNAL(triggered()), this, SLOT(on_actionSortNode_triggered()));

    connect(exit, SIGNAL(triggered()), this, SLOT(on_actionExit_triggered()));

    connect(cutButton, SIGNAL(clicked()), nodeView, SLOT(cut()));
    connect(copyButton, SIGNAL(clicked()), nodeView, SLOT(copy()));
    connect(pasteButton, SIGNAL(clicked()), this, SLOT(on_actionPaste_triggered()));
    connect(sortButton, SIGNAL(clicked()), this, SLOT(on_actionSortNode_triggered()));
    connect(popupButton, SIGNAL(clicked()), this, SLOT(on_actionPopupNewWindow()));

    connect(snapToGridButton, SIGNAL(clicked()), nodeView, SLOT(snapSelectionToGrid()));
    connect(snapChildrenToGridButton, SIGNAL(clicked()), nodeView, SLOT(snapChildrenToGrid()));

    connect(duplicateButton, SIGNAL(clicked()), nodeView, SLOT(duplicate()));
    connect(alignSelectionHorizontal, SIGNAL(clicked()), nodeView, SLOT(alignSelectionHorizontally()));
    connect(alignSelectionVertical, SIGNAL(clicked()), nodeView, SLOT(alignSelectionVertically()));
    connect(fitToScreenButton, SIGNAL(clicked()), nodeView, SLOT(fitToScreen()));
    connect(centerButton, SIGNAL(clicked()), nodeView, SLOT(centerOnItem()));
    connect(zoomToFitButton, SIGNAL(clicked()), this, SLOT(on_actionCenterNode_triggered()));

    connect(nodeView, SIGNAL(view_ExportedProject(QString)), this, SLOT(writeExportedProject(QString)));
    connect(nodeView, SIGNAL(view_UndoListChanged(QStringList)), this, SLOT(updateUndoStates(QStringList)));
    connect(nodeView, SIGNAL(view_RedoListChanged(QStringList)), this, SLOT(updateRedoStates(QStringList)));
    connect(nodeView, SIGNAL(view_SetClipboardBuffer(QString)), this, SLOT(setClipboard(QString)));
    connect(nodeView, SIGNAL(view_ProjectNameChanged(QString)), this, SLOT(changeWindowTitle(QString)));

    connect(assemblyButton, SIGNAL(clicked()), this, SLOT(updateViewAspects()));
    connect(hardwareButton, SIGNAL(clicked()), this, SLOT(updateViewAspects()));
    connect(definitionsButton, SIGNAL(clicked()), this, SLOT(updateViewAspects()));
    connect(workloadButton, SIGNAL(clicked()), this, SLOT(updateViewAspects()));

    connect(nodeView, SIGNAL(view_enableDocks(bool)), partsButton, SLOT(enableDock(bool)));
    connect(nodeView, SIGNAL(view_enableDocks(bool)), compDefinitionsButton, SLOT(enableDock(bool)));
    connect(nodeView, SIGNAL(view_enableDocks(bool)), hardwareNodesButton, SLOT(enableDock(bool)));

    connect(this, SIGNAL(clearDocks()), definitionsDock, SLOT(clear()));
    connect(this, SIGNAL(clearDocks()), hardwareDock, SLOT(clear()));

    connect(nodeView, SIGNAL(view_nodeConstructed(NodeItem*)), hardwareDock, SLOT(nodeConstructed(NodeItem*)));
    connect(nodeView, SIGNAL(view_nodeConstructed(NodeItem*)), definitionsDock, SLOT(nodeConstructed(NodeItem*)));
    connect(nodeView, SIGNAL(view_nodeConstructed(NodeItem*)), partsDock, SLOT(updateDock()));

    connect(nodeView, SIGNAL(view_nodeDestructed(NodeItem*)), definitionsDock, SLOT(nodeDestructed(NodeItem*)));
    connect(nodeView, SIGNAL(view_nodeDestructed(NodeItem*)), partsDock, SLOT(updateDock()));

    connect(nodeView, SIGNAL(view_nodeSelected(Node*)), partsDock, SLOT(updateCurrentNodeItem(Node*)));
    connect(nodeView, SIGNAL(view_nodeSelected(Node*)), definitionsDock, SLOT(updateCurrentNodeItem(Node*)));
    connect(nodeView, SIGNAL(view_nodeSelected(Node*)), hardwareDock, SLOT(updateCurrentNodeItem(Node*)));

    connect(nodeView, SIGNAL(customContextMenuRequested(QPoint)), nodeView, SLOT(showToolbar(QPoint)));
    connect(nodeView, SIGNAL(view_ViewportRectChanged(QRectF)), minimap, SLOT(viewportRectChanged(QRectF)));

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
    DEPGEN_ROOT = QString(qgetenv("DEPGEN_ROOT"));

    QSettings Settings(DEPGEN_ROOT+"/release/config.ini", QSettings::IniFormat);

    Settings.beginGroup("Jenkins-Settings");
    JENKINS_ADDRESS = Settings.value("JENKINS_ADDRESS").toString();
    JENKINS_USERNAME = Settings.value("JENKINS_USERNAME").toString();
    JENKINS_PASSWORD = Settings.value("JENKINS_PASSWORD").toString();
    Settings.endGroup();
}


/**
 * @brief MedeaWindow::setupDefaultSettings
 * This force sorts and centers the definitions containers before they are hidden.
 * It also sets the default values for toggle menu actions and populates the parts dock.
 */
void MedeaWindow::setupDefaultSettings()
{
    // need to set initial toggle action values before triggering them
    view_autoCenterView->setChecked(true);
    view_showGridLines->setChecked(true);
    view_selectOnConstruction->setChecked(false);
    view_showManagementComponents->setChecked(false);

    view_autoCenterView->triggered(true);
    view_showGridLines->triggered(true);
    view_selectOnConstruction->triggered(false);
    view_showManagementComponents->triggered(false);

    if (nodeView) {
        nodeView->centerAspects();
    }

    // this only needs to happen once, the whole time the application is open
    partsDock->addDockNodeItems(nodeView->getConstructableNodeKinds());
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
    QString program = "python Jenkins-Groovy-Runner.py";
    program +=" -s " + JENKINS_ADDRESS;
    program +=" -u " + JENKINS_USERNAME;
    program +=" -p " + JENKINS_PASSWORD;
    program +=" -g  Jenkins_Construct_GraphMLNodesList.groovy";

    myProcess = new QProcess(this);
    myProcess->setWorkingDirectory(DEPGEN_ROOT + "/scripts");
    connect(myProcess, SIGNAL(finished(int)), this, SLOT(loadJenkinsData(int)));
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
    }

    newProject();
}


/**
 * @brief MedeaWindow::on_actionImport_GraphML_triggered
 */
void MedeaWindow::on_actionImport_GraphML_triggered()
{
    QStringList files = QFileDialog::getOpenFileNames(
                this,
                "Select one or more files to open",
                "c:\\",
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
    nodeView->view_TriggerAction("Medea: Sorting Node");

    if (nodeView->getSelectedNode()){
        nodeView->sortNode(nodeView->getSelectedNode());
    } else {
        nodeView->sortEntireModel();
    }
}


/**
 * @brief MedeaWindow::on_actionCenterNode_triggered
 * This is called whne the centerNode tool button is triggered.
 * It zooms into and centers on the selected node.
 */
void MedeaWindow::on_actionCenterNode_triggered()
{
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
    if (nodeView->getSelectedNode()) {
        nodeView->constructNewView(nodeView->getSelectedNode());
    }
}


/**
 * @brief MedeaWindow::on_actionPaste_triggered
 */
void MedeaWindow::on_actionPaste_triggered()
{
    QClipboard *clipboard = QApplication::clipboard();
    if (clipboard->ownsClipboard()) {
        window_PasteData(clipboard->text());
    }
}

void MedeaWindow::on_SearchTextChanged(QString text)
{
    if(text.length() != 0){
        searchButton->setEnabled(true);
    }else{
        searchButton->setEnabled(false);
    }

}

void MedeaWindow::on_actionSearch()
{
    QString searchText = searchBar->text();
    if(nodeView && searchText != ""){
        QList<GraphMLItem*> returnedItems = nodeView->search(searchText, GraphMLItem::NODE_ITEM);
        QString messageBoxText;
        foreach(GraphMLItem* guiItem, returnedItems){
            messageBoxText += "* "+ guiItem->getGraphML()->toString() + "\n";
        }
        if(messageBoxText == ""){
            messageBoxText = "Search Yielded no results";
        }

        QMessageBox::information(this, "Search Results: '"+searchText+"'", messageBoxText, QMessageBox::Ok);
        searchBar->clear();
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
 * Show/hide the window toolbar when the user right clicks on a !PAINTED item or the view.
 */
void MedeaWindow::showWindowToolbar()
{
    if (toolbar->isVisible()) {
        toolbar->hide();
    } else {
        toolbar->show();
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
                                                    "C:\\",
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
    //qDebug() << "Status: " << value << "% " << status;
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
        enableDeploymentViewAspect();

        // center view aspects
        nodeView->centerAspects();

    }else{
        QMessageBox::critical(this, "Jenkins Error", "Unable to request Jenkins Data", QMessageBox::Ok);
    }
}


/**
 * @brief MedeaWindow::importGraphMLFiles
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
 * @brief MedeaWindow::enableDeploymentViewAspect
 * This is called after the Jenkins nodes are imported.
 * It selects the Jenkins hardware cluster, opens the hardware dock and display
 * the Assembly and Hardware view aspects if they aren't already turned on.
 */
void MedeaWindow::enableDeploymentViewAspect()
{
    // make sure that the Assembly aspect is turned on
    if (!assemblyButton->isChecked()) {
        assemblyButton->setChecked(true);
        assemblyButton->clicked();
    }

    // make sure that the Harware aspect is turned on
    if (!hardwareButton->isChecked()) {
        hardwareButton->setChecked(true);
        hardwareButton->clicked();
    }

    // select the Jenkins hardware cluster after construction
    Model* model = controller->getModel();
    if (model) {
        QList<Node*> hardwareClusters = model->getChildrenOfKind("HardwareCluster");
        if (hardwareClusters.count() > 0) {
            // at the moment, this method assumes that the only cluster is the Jenkins cluster
            nodeView->appendToSelection(hardwareClusters.at(0));
            nodeView->snapSelectionToGrid();
        }
    }

    // if the hardware dock isn't already open, open it
    if (hardwareNodesButton->isEnabled() && !hardwareNodesButton->getSelected()) {
        hardwareNodesButton->pressed();
    }
}
