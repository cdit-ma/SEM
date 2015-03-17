#include "medeawindow.h"
#include "Controller/newcontroller.h"

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

/**
 * @brief MedeaWindow::MedeaWindow
 * @param parent
 */
MedeaWindow::MedeaWindow(QString graphMLFile, QWidget *parent) :
    QMainWindow(parent)
{
    thread = 0;
    nodeView = 0;
    controller = 0;
    myProcess = 0;
    minimap = 0;
    checkedViewAspects.clear();;

    setupJenkinsSettings();

    // initialise gui and connect signals and slots
    initialiseGUI();
    makeConnections();
    newProject();

    // this is used for when a file is dragged and
    // dropped on top of this tool's icon
    /*
    if(graphMLFile.length() != 0){
        QStringList files;
        files.append(graphMLFile);
        importGraphMLFiles(files);
    }
    */

    //this->view_SetGUIEnabled(false);
}


/**
 * @brief MedeaWindow::~MedeaWindow
 */
MedeaWindow::~MedeaWindow()
{
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
    prevPressedButton = 0;
    prevSelectedNode = 0;
    selectedNode = 0;

    nodeView = new NodeView();
    toolbar = new QToolBar();
    dataTable = new QTableView();


    //dataTable->set
    dataTableBox = new QGroupBox();
    projectName = new QPushButton("Model");
    assemblyButton = new QPushButton("Assembly");
    hardwareButton = new QPushButton("Hardware");
    workloadButton = new QPushButton("Behaviour");
    definitionsButton = new QPushButton("Interface");

    QPushButton *menuButton = new QPushButton(QIcon(":/Resources/Icons/menuIcon.png"), "");
    QLineEdit *searchBar = new QLineEdit();
    QPushButton *searchButton = new QPushButton(QIcon(":/Resources/Icons/search_icon.png"), "");
    QVBoxLayout *tableLayout = new QVBoxLayout();

    // set window size; used for graphicsview and main widget
    int windowWidth = 1300;
    int windowHeight = 800;
    int rightPanelWidth = 210;

    // set central widget and window size
    this->setCentralWidget(nodeView);
    this->setMinimumSize(windowWidth, windowHeight);
    nodeView->setMinimumSize(windowWidth, windowHeight);


    // setup widgets
    menuButton->setFixedSize(50,45);
    menuButton->setIconSize(menuButton->size());
    projectName->setFlat(true);
    projectName->setFixedWidth(rightPanelWidth-menuButton->width()-10);
    searchButton->setFixedSize(45, 28);
    searchButton->setIconSize(searchButton->size()*0.65);
    searchBar->setFixedSize(rightPanelWidth - searchButton->width() - 5, 25);
    assemblyButton->setFixedSize(rightPanelWidth/2.05, rightPanelWidth/2.5);
    hardwareButton->setFixedSize(rightPanelWidth/2.05, rightPanelWidth/2.5);
    definitionsButton->setFixedSize(rightPanelWidth/2.05, rightPanelWidth/2.5);
    workloadButton->setFixedSize(rightPanelWidth/2.05, rightPanelWidth/2.5);
    assemblyButton->setStyleSheet("background-color: rgb(230,130,130);");
    hardwareButton->setStyleSheet("background-color: rgb(80,140,190);");
    definitionsButton->setStyleSheet("background-color: rgb(80,180,180);");
    workloadButton->setStyleSheet("background-color: rgb(224,154,96);");
    searchBar->setStyleSheet("background-color: rgba(230,230,230,1);");
    projectName->setStyleSheet("font-size: 16px; text-align: left;");
    menuButton->setStyleSheet("QPushButton{ background-color: rgba(220,220,220,0.5); }"
                              "QPushButton::menu-indicator{ image: none; }");

    // setup and add dataTable/dataTableBox widget/layout
    dataTable->setFixedWidth(rightPanelWidth);

    dataTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

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
    minimap->setFixedHeight(rightPanelWidth/16 * 10);
    minimap->setStyleSheet("background-color: rgba(125,125,125,225);");

    rightVlayout->addWidget(minimap);

    // other settings
    assemblyButton->setCheckable(true);
    hardwareButton->setCheckable(true);
    definitionsButton->setCheckable(true);
    workloadButton->setCheckable(true);

    // setup the menu and dock
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

    view_fitToScreen = view_menu->addAction(QIcon(":/Resources/Icons/zoomToFit.png"), "Fit To Sreen");
    view_fitToScreen->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Space));
    view_autoCenterView = view_menu->addAction(QIcon(":/Resources/Icons/autoCenter.png"), "Manually Center Views");
    view_menu->addSeparator();
    view_goToDefinition = view_menu->addAction(QIcon(":/Resources/Icons/definition.png"), "Go to Definition");
    view_goToDefinition->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_D));
    view_goToImplementation = view_menu->addAction(QIcon(":/Resources/Icons/implementation.png"), "Go to Implementation");
    view_goToImplementation->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_I));

    view_gridLines = view_menu->addAction("Toggle Gridlines");
    view_gridLines->setCheckable(true);
    view_gridLines->setChecked(false);
    view_gridLines->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_G));

    model_clearModel = model_menu->addAction(QIcon(":/Resources/Icons/clear.png"), "Clear Model");
    model_clearModel->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    model_sortModel = model_menu->addAction(QIcon(":/Resources/Icons/sort.png"), "Sort Model");
    model_sortModel->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
    model_menu->addSeparator();
    model_validateModel = model_menu->addAction(QIcon(":/Resources/Icons/validate.png"), "Validate Model");

    exit->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_1));

    button->setMenu(menu);

    // initially disable model & goto menu actions
    model_validateModel->setEnabled(false);
    view_goToDefinition->setEnabled(false);
    view_goToImplementation->setEnabled(false);

    // set deafult view aspects centering to automatic
    autoCenterOn = true;


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

    // width of the containers is fixed
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
 */
void MedeaWindow::setupToolbar()
{
    QSize buttonSize = QSize(46,40);

    toolbar->setFixedSize(buttonSize.width()*7, buttonSize.height()+10);
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

    cutButton->setIcon(QIcon(":/Resources/Icons/cut.png"));
    copyButton->setIcon(QIcon(":/Resources/Icons/copy.png"));
    pasteButton->setIcon(QIcon(":/Resources/Icons/paste.png"));
    sortButton->setIcon(QIcon(":/Resources/Icons/sort.png"));
    centerButton->setIcon(QIcon(":/Resources/Icons/autoCenter.png"));
    popupButton->setIcon(QIcon(":/Resources/Icons/popup.png"));

    cutButton->setFixedSize(buttonSize);
    copyButton->setFixedSize(buttonSize);
    pasteButton->setFixedSize(buttonSize);
    sortButton->setFixedSize(buttonSize);
    centerButton->setFixedSize(buttonSize);
    popupButton->setFixedSize(buttonSize);

    cutButton->setIconSize(buttonSize*0.6);
    copyButton->setIconSize(buttonSize*0.65);
    pasteButton->setIconSize(buttonSize*0.65);
    sortButton->setIconSize(buttonSize*0.65);
    centerButton->setIconSize(buttonSize*0.65);
    popupButton->setIconSize(buttonSize*0.65);

    cutButton->setToolTip("Cut Node");
    copyButton->setToolTip("Copy Node");
    pasteButton->setToolTip("Paste Node");
    sortButton->setToolTip("Sort Node");
    centerButton->setToolTip("Center Node");
    popupButton->setToolTip("Show Node In New Window");

    QWidget* spacerWidget1 = new QWidget();
    QWidget* spacerWidget2 = new QWidget();
    spacerWidget1->setFixedWidth(5);
    spacerWidget2->setFixedWidth(5);

    toolbar->addWidget(cutButton);
    toolbar->addWidget(copyButton);
    toolbar->addWidget(pasteButton);
    toolbar->addWidget(spacerWidget1);
    toolbar->addSeparator();
    toolbar->addWidget(spacerWidget2);
    toolbar->addWidget(sortButton);
    toolbar->addWidget(centerButton);
    toolbar->addWidget(popupButton);
}


/**
 * @brief MedeaWindow::setupController
 */
void MedeaWindow::setupController()
{
    if (controller) {
        delete controller;
    }
    if(thread){
        delete thread;
    }
    controller = 0;
    thread = 0;

    controller = new NewController();

    if(THREADING){
        //IMPLEMENT THREADING!
        thread = new QThread();
        thread->start();
        controller->moveToThread(thread);
    }

    controller->connectView(nodeView);
    connectToController();
    controller->initializeModel();

}


/**
 * @brief MedeaWindow::resetGUI
 */
void MedeaWindow::resetGUI()
{
    prevPressedButton = 0;
    prevSelectedNode = 0;
    selectedNode = 0;

    setupController();

}


/**
 * @brief MedeaWindow::makeConnections
 * Connect signals and slots.
 */
void MedeaWindow::makeConnections()
{
    connect(this, SIGNAL(setupViewLayout()), this, SLOT(sortAndCenterViewAspects()));
    connect(this, SIGNAL(window_AspectsChanged(QStringList)), nodeView, SLOT(setAspects(QStringList)));
    connect(nodeView, SIGNAL(view_GUIAspectChanged(QStringList)), this, SLOT(setAspects(QStringList)));
    connect(nodeView, SIGNAL(setGoToMenuActions(QString,bool)), this, SLOT(setGoToMenuActions(QString,bool)));

    connect(projectName, SIGNAL(clicked()), nodeView, SLOT(view_SelectModel()));
    connect(projectName, SIGNAL(clicked()), nodeView, SLOT(clearSelection()));

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

    connect(view_fitToScreen, SIGNAL(triggered()), nodeView, SLOT(fitToScreen()));
    connect(view_autoCenterView, SIGNAL(triggered()), this, SLOT(autoCenterViews()));
    connect(view_goToDefinition, SIGNAL(triggered()), this, SLOT(goToDefinition()));
    connect(view_goToImplementation, SIGNAL(triggered()), this, SLOT(goToImplementation()));
    connect(model_clearModel, SIGNAL(triggered()), this, SLOT(on_actionClearModel_triggered()));
    connect(model_sortModel, SIGNAL(triggered()), this, SLOT(on_actionSortNode_triggered()));
    connect(view_gridLines, SIGNAL(triggered(bool)), nodeView, SLOT(toggleGridLines(bool)));
    connect(exit, SIGNAL(triggered()), this, SLOT(on_actionExit_triggered()));


    connect(cutButton, SIGNAL(clicked()), nodeView, SLOT(cut()));
    connect(copyButton, SIGNAL(clicked()), nodeView, SLOT(copy()));
    connect(pasteButton, SIGNAL(clicked()), this, SLOT(on_actionPaste_triggered()));
    connect(sortButton, SIGNAL(clicked()), this, SLOT(on_actionSortNode_triggered()));
    connect(centerButton, SIGNAL(clicked()), this, SLOT(on_actionCenterNode_triggered()));
    connect(popupButton, SIGNAL(clicked()), this, SLOT(on_actionPopupNewWindow()));

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

    connect(this, SIGNAL(clearDocks()), partsDock, SLOT(clear()));
    connect(this, SIGNAL(clearDocks()), definitionsDock, SLOT(clear()));
    connect(this, SIGNAL(clearDocks()), hardwareDock, SLOT(clear()));

    connect(nodeView, SIGNAL(view_nodeConstructed(NodeItem*)), definitionsDock, SLOT(nodeConstructed(NodeItem*)));
    connect(nodeView, SIGNAL(view_nodeConstructed(NodeItem*)), hardwareDock, SLOT(nodeConstructed(NodeItem*)));

    connect(nodeView, SIGNAL(view_nodeConstructed(NodeItem*)), partsDock, SLOT(updateDock()));
    connect(nodeView, SIGNAL(view_nodeDestructed()), partsDock, SLOT(updateDock()));

    connect(nodeView, SIGNAL(view_nodeSelected(Node*)), partsDock, SLOT(updateCurrentNodeItem(Node*)));
    connect(nodeView, SIGNAL(view_nodeSelected(Node*)), definitionsDock, SLOT(updateCurrentNodeItem(Node*)));
    connect(nodeView, SIGNAL(view_nodeSelected(Node*)), hardwareDock, SLOT(updateCurrentNodeItem(Node*)));

    connect(nodeView, SIGNAL(view_SetAttributeModel(AttributeTableModel*)), this, SLOT(setAttributeModel(AttributeTableModel*)));
    connect(nodeView, SIGNAL(customContextMenuRequested(QPoint)), nodeView, SLOT(showToolbar(QPoint)));

    // this needs fixing
    //connect(this, SIGNAL(checkDockScrollBar()), partsContainer, SLOT(checkScrollBar()));

    connect(nodeView, SIGNAL(view_ViewportRectChanged(QRectF)), minimap, SLOT(viewportRectChanged(QRectF)));
}


/**
 * @brief MedeaWindow::connectToController
 * Connect signals and slots to the controller.
 */
void MedeaWindow::connectToController()
{
    connect(model_clearModel, SIGNAL(triggered()), controller, SLOT(clearModel()));
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

    boxHeight = this->height()*0.73;
    partsDock->setMinimumHeight(boxHeight);
    definitionsDock->setMinimumHeight(boxHeight);
    hardwareDock->setMinimumHeight(boxHeight);

    // update dataTable size
    updateDataTable();
}

void MedeaWindow::editMultiLineData(GraphMLData *data)
{

}

/**
 * @brief MedeaWindow::sortAndCenterModel
 * This force sorts the main definitions containers before they are hidden.
 * The visible view aspect(s) is then centered.
 */
void MedeaWindow::sortAndCenterViewAspects()
{
    if (nodeView) {
        //Change to Emits!
        nodeView->centerAspects();
        nodeView->fitToScreen();
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
 * @brief MedeaWindow::on_actionAutoCenterViews_triggered
 * This tells the nodeView to set the automatic centering of view aspects on/off.
 */
void MedeaWindow::autoCenterViews()
{
    if (autoCenterOn) {
        autoCenterOn = false;
        view_autoCenterView->setText("Automatically Center Views");
    } else {
        autoCenterOn = true;
        view_autoCenterView->setText("Manually Center Views");
    }
    nodeView->setAutoCenterViewAspects(autoCenterOn);
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
        // TODO: create method to set the initial values for objects
        partsDock->addDockNodeItems(nodeView->getConstructableNodeKinds());
    }
}


/**
 * @brief MedeaWindow::on_actionSortModel_triggered
 */
void MedeaWindow::on_actionSortNode_triggered()
{
    if (nodeView->getSelectedNode()){
        nodeView->sortNode(nodeView->getSelectedNode());
    } else {
        nodeView->sortEntireModel();
        nodeView->fitToScreen();
    }
}


/**
 * @brief MedeaWindow::on_actionCenterNode_triggered
 */
void MedeaWindow::on_actionCenterNode_triggered()
{
    if (nodeView->getSelectedNodeItem()) {
        nodeView->centerItem(nodeView->getSelectedNodeItem());
    }
}


/**
 * @brief MedeaWindow::on_actionPopupNewWindow
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


/**
 * @brief MedeaWindow::on_actionExit_triggered
 */
void MedeaWindow::on_actionExit_triggered()
{
    close();
}

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
 * @brief MedeaWindow::updateAspects
 * Add view aspect to checkedViewAspects when the corresponding button
 * is clicked and remove it when it's unclick then update the view.
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
    if(newAspects != checkedViewAspects){
        window_AspectsChanged(newAspects);
    }
}


/**
 * @brief MedeaWindow::goToDefinition
 */
void MedeaWindow::goToDefinition()
{
    if (selectedNode) {
        nodeView->goToDefinition(selectedNode);
    }
}


/**
 * @brief MedeaWindow::goToImplementation
 */
void MedeaWindow::goToImplementation()
{
    if (selectedNode) {
        nodeView->goToImplementation(selectedNode);
    }
}

void MedeaWindow::setAspects(QStringList aspects)
{
    definitionsButton->setChecked(aspects.contains("Definitions"));
    workloadButton->setChecked(aspects.contains("Workload"));
    assemblyButton->setChecked(aspects.contains("Assembly"));
    hardwareButton->setChecked(aspects.contains("Hardware"));
    checkedViewAspects = aspects;
}



/**
 * @brief MedeaWindow::setGoToMenuActions
 * @param action
 * @param node
 */
void MedeaWindow::setGoToMenuActions(QString action, bool enabled)
{
    if (action == "definition") {
        view_goToDefinition->setEnabled(enabled);
    } else if (action == "implementation") {
        view_goToImplementation->setEnabled(enabled);
    }
}

void MedeaWindow::resetView()
{
    if(nodeView){
        nodeView->setDefaultAspects();
    }
}


void MedeaWindow::newProject()
{
    // clear view and reset gui
    resetGUI();
    on_actionClearModel_triggered();
    nodeView->view_ClearHistory();

    //Set default View.
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
    if(model){
        updateDataTable();
    }
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
    //update();
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
    qreal height = 0;
    int vOffset = 0;

    dataTable->setVisible(true);

    if (tableModel) {
        int rowCount = tableModel->rowCount() + 1;
        for (int i = 0; i < rowCount; i++) {
            height += dataTable->rowHeight(i);
        }
        vOffset = dataTable->verticalHeader()->size().width() + 21;
    }

    int maxHeight = dataTableBox->height();
    int newHeight = height + vOffset;

    if (maxHeight == 0) {
        dataTable->setVisible(false);
    } else if (newHeight > maxHeight) {
        dataTable->resize(dataTable->width(), maxHeight);
    } else {
        dataTable->resize(dataTable->width(), newHeight);
    }

    dataTable->repaint();

    int w = dataTable->width();
    int h = dataTable->height();

    // update the visible region of the groupbox to fit the dataTable
    if (w == 0 || h == 0) {
        dataTableBox->setAttribute(Qt::WA_TransparentForMouseEvents);
    } else {
        dataTableBox->setAttribute(Qt::WA_TransparentForMouseEvents, false);
        dataTableBox->setMask(QRegion(0, 0, w, h, QRegion::Rectangle));
    }

    dataTable->resizeColumnToContents(0);
    //dataTableBox->repaint();
}


/**
 * @brief MedeaWindow::loadJenkinsData
 * @param code
 */
void MedeaWindow::loadJenkinsData(int code)
{
    QStringList files;
    files << myProcess->readAll();

    exportFileName = "c:/jenkins.xml";
    writeExportedProject(files[0]);
    window_ImportProjects(files);

    // sort and center view aspects
    nodeView->centerAspects();
}


/**
 * @brief MedeaWindow::importGraphMLFiles
 * @param files
 */
void MedeaWindow::importProjects(QStringList files)
{
    QStringList projects;
    foreach(QString fileName, files){
        try {
            QFile file(fileName);

            bool fileOpened = file.open(QFile::ReadOnly | QFile::Text);

            if(!fileOpened){
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
    if(projects.size() > 0){

        window_ImportProjects(projects);
        nodeView->centerAspects();
    }
}
