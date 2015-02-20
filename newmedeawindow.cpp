#include "newmedeawindow.h"
#include "GUI/toolbarwidgetaction.h"

#include <QDebug>
#include <QImage>
#include <QFileDialog>
#include <QClipboard>
#include <QMessageBox>
#include <QApplication>
#include <QHeaderView>
#include <QScrollBar>
#include <QSettings>
#include <QPicture>


/**
 * @brief NewMedeaWindow::NewMedeaWindow
 * @param parent
 */
NewMedeaWindow::NewMedeaWindow(QString graphMLFile, QWidget *parent) :
    QMainWindow(parent)
{
    nodeView = 0;
    controller = 0;
    myProcess = 0;
    minimap = 0;

    // initialise gui and connect signals and slots
    initialiseGUI();
    makeConnections();

    // this is used for when a file is dragged and
    // dropped on top of this tool's icon
    if(graphMLFile.length() != 0){
        QStringList files;
        files.append(graphMLFile);
        importGraphMLFiles(files);
    }

    /*
    QFile file("C:/ArrowTest.graphml");

    if(!file.open(QFile::ReadOnly | QFile::Text)){
        qDebug() << "could not open file for read";
    }

    QTextStream in(&file);
    QString xmlText = in.readAll();
    file.close();

    emit view_ActionTriggered("Loading GraphML");
    //emit view_ActionTriggered("Loading XME");
    emit view_PasteData(xmlText);
    */
}


/**
 * @brief NewMedeaWindow::~NewMedeaWindow
 */
NewMedeaWindow::~NewMedeaWindow()
{
    if (controller) {
        delete controller;
    }
    if (nodeView) {
        delete nodeView;
    }
}


/**
 * @brief NewMedeaWindow::initialiseGUI
 * Initialise variables, setup widget sizes, organise layout
 * and setup the view, scene and menu.
 */
void NewMedeaWindow::initialiseGUI()
{
    // initial values
    prevPressedButton = 0;
    prevSelectedNode = 0;
    selectedNode = 0;

    nodeView = new NodeView();


    // set window size; used for graphicsview and main widget
    int windowWidth = 1300;
    int windowHeight = 800;
    int rightPanelWidth = 210;

    // set central widget and window size
    this->setCentralWidget(nodeView);
    this->setMinimumSize(windowWidth, windowHeight);
    nodeView->setMinimumSize(windowWidth, windowHeight);

    projectName = new QPushButton("Model");
    dataTable = new QTableView();
    dataTableBox = new QGroupBox();
    assemblyButton = new QPushButton("Assembly");
    hardwareButton = new QPushButton("Hardware");
    workloadButton = new QPushButton("Behaviour");
    definitionsButton = new QPushButton("Interface");

    // initialise other private variables
    QPushButton *menuButton = new QPushButton(QIcon(":/Resources/Icons/menuIcon.png"), "");
    QLineEdit *searchBar = new QLineEdit();
    QPushButton *searchButton = new QPushButton(QIcon(":/Resources/Icons/search_icon.png"), "");
    QTextEdit *notificationArea = new QTextEdit("Notifications can be displayed here.");
    QVBoxLayout *tableLayout = new QVBoxLayout();

    // setup widgets
    projectName->setFlat(true);
    projectName->setFixedWidth(rightPanelWidth);
    menuButton->setFixedSize(50,25);
    menuButton->setIconSize(menuButton->size()*0.6);
    menuButton->setStyleSheet("QPushButton::menu-indicator{ image: none; }");
    searchButton->setFixedSize(45, 25);
    searchButton->setIconSize(searchButton->size()*0.8);
    searchBar->setFixedSize(rightPanelWidth - searchButton->width() - 5, 25);
    notificationArea->setFixedSize(rightPanelWidth, 60);
    assemblyButton->setFixedSize(rightPanelWidth/2.05, rightPanelWidth/2.5);
    hardwareButton->setFixedSize(rightPanelWidth/2.05, rightPanelWidth/2.5);
    definitionsButton->setFixedSize(rightPanelWidth/2.05, rightPanelWidth/2.5);
    workloadButton->setFixedSize(rightPanelWidth/2.05, rightPanelWidth/2.5);
    assemblyButton->setStyleSheet("background-color: rgba(230,130,130,0.9);");
    hardwareButton->setStyleSheet("background-color: rgba(80,140,190,0.9);");
    definitionsButton->setStyleSheet("background-color: rgba(80,180,180,0.9);");
    workloadButton->setStyleSheet("background-color: rgba(224,154,96,0.9);");
    projectName->setStyleSheet("font-size: 16px; text-align: left;");
    searchBar->setStyleSheet("background-color: rgba(230,230,230,1);");

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

    // layouts and spacer items
    QHBoxLayout *mainHLayout = new QHBoxLayout();
    QVBoxLayout *leftVlayout = new QVBoxLayout();
    QVBoxLayout *rightVlayout =  new QVBoxLayout();
    QHBoxLayout *titleLayout = new QHBoxLayout();
    QHBoxLayout *searchLayout = new QHBoxLayout();
    QHBoxLayout *bodyLayout = new QHBoxLayout();
    QGridLayout *viewButtonsGrid = new QGridLayout();

    // add widgets to/and layouts
    titleLayout->addWidget(menuButton, 1);
    titleLayout->addSpacerItem(new QSpacerItem(10, 0));
    titleLayout->addWidget(projectName, 1);
    titleLayout->addStretch(7);

    searchLayout->addWidget(searchBar, 3);
    searchLayout->addWidget(searchButton, 1);

    leftVlayout->addLayout(titleLayout);
    leftVlayout->addStretch(1);
    leftVlayout->addLayout(bodyLayout, 20);

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
    //rightVlayout->addWidget(notificationArea, 1);
    //rightVlayout->addStretch();
    rightVlayout->addSpacerItem(new QSpacerItem(20, 30));

    mainHLayout->addLayout(leftVlayout, 4);
    mainHLayout->addLayout(rightVlayout, 1);
    mainHLayout->setContentsMargins(25, 25, 25, 25);
    nodeView->setLayout(mainHLayout);

    // setup mini map
    minimap = new NodeViewMinimap();
    minimap->setScene(nodeView->scene());
    connect(nodeView, SIGNAL(updateViewPort(QRectF)), minimap, SLOT(updateViewPort(QRectF)));

    minimap->scale(.002,.002);
    minimap->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    minimap->setVerticalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
    minimap->setInteractive(false);

    minimap->setFixedWidth(rightPanelWidth);
    minimap->setFixedHeight(rightPanelWidth/16 * 10);
    minimap->setStyleSheet("background-color: rgba(125,125,125,225);");

    rightVlayout->addWidget(minimap);

    // other settings
    notificationArea->setEnabled(false);
    assemblyButton->setCheckable(true);
    hardwareButton->setCheckable(true);
    definitionsButton->setCheckable(true);
    workloadButton->setCheckable(true);

    // intially only turn the assembly view aspect on
    assemblyButton->setChecked(true);
    //hardwareButton->setChecked(true);
    //definitionsButton->setChecked(true);
    //workloadButton->setChecked(true);

    checkedViewAspects.append(assemblyButton->text());
    //checkedViewAspects.append(hardwareButton->text());
    //checkedViewAspects.append(definitionsButton->text());
    //checkedViewAspects.append(workloadButton->text());

    // setup controller and jenkins settings
    setupJenkinsSettings();
    setupController();

    // setup the menu and dock
    setupMenu(menuButton);
    setupDock(bodyLayout);
}


/**
 * @brief NewMedeaWindow::setupMenu
 * Initialise and setup menus and their actions.
 * @param button
 */
void NewMedeaWindow::setupMenu(QPushButton *button)
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
    view_goToDefinition = view_menu->addAction(QIcon(":/Resources/Icons/definition.png"), "Go to Definition");
    view_goToDefinition->setShortcut(QKeySequence(Qt::Key_D));
    view_goToImplementation = view_menu->addAction(QIcon(":/Resources/Icons/implementation.png"), "Go to Implementation");
    view_goToImplementation->setShortcut(QKeySequence(Qt::Key_I));

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
}


/**
 * @brief NewMedeaWindow::setupDock
 * Initialise and setup dock widgets.
 * @param layout
 */
void NewMedeaWindow::setupDock(QHBoxLayout *layout)
{
    QVBoxLayout *dockLayout = new QVBoxLayout();
    QGroupBox* buttonsBox = new QGroupBox();
    QHBoxLayout *dockButtonsHlayout = new QHBoxLayout();

    partsButton = new DockToggleButton("P", this);
    hardwareNodesButton = new DockToggleButton("H", this);
    compDefinitionsButton = new DockToggleButton("D", this);

    partsContainer = new DockScrollArea("Parts", partsButton);
    hardwareContainer = new DockScrollArea("Hardware Nodes", hardwareNodesButton);
    definitionsContainer = new DockScrollArea("Definitions", compDefinitionsButton);

    // width of the containers is fixed
    boxWidth = (partsButton->getWidth()*3) + 30;

    // set buttonBox's size and get rid of its border
    buttonsBox->setStyleSheet("border: 0px;");
    buttonsBox->setFixedSize(boxWidth, 60);

    // set dockScrollAreas sizes
    partsContainer->setMaximumWidth(boxWidth);
    hardwareContainer->setMaximumWidth(boxWidth);
    definitionsContainer->setMaximumWidth(boxWidth);

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
    dockButtonsHlayout->addWidget(hardwareNodesButton);
    dockButtonsHlayout->addWidget(compDefinitionsButton);
    buttonsBox->setLayout(dockButtonsHlayout);

    dockLayout->addWidget(buttonsBox);
    dockLayout->addWidget(partsContainer);
    dockLayout->addWidget(hardwareContainer);
    dockLayout->addWidget(definitionsContainer);
    dockLayout->addStretch();

    layout->addLayout(dockLayout, 1);
    layout->addStretch(3);

    // initially disable dock buttons
    updateDockButtons("N");
}


/**
 * @brief NewMedeaWindow::setupController
 */
void NewMedeaWindow::setupController()
{
    if (controller) {
        delete controller;
    }
    controller = 0;
    controller = new NewController();
    controller->connectView(nodeView);
    controller->initializeModel();
}


/**
 * @brief NewMedeaWindow::resetGUI
 */
void NewMedeaWindow::resetGUI()
{
    prevPressedButton = 0;
    prevSelectedNode = 0;
    selectedNode = 0;

    setupController();
    connectToController();

    // force projectName to be the same as the model label
    updateProjectName("Model");
}


/**
 * @brief NewMedeaWindow::makeConnections
 * Connect signals and slots.
 */
void NewMedeaWindow::makeConnections()
{
    connect(file_newProject, SIGNAL(triggered()), this, SLOT(on_actionNew_Project_triggered()));
    connect(file_importGraphML, SIGNAL(triggered()), this, SLOT(on_actionImport_GraphML_triggered()));
    connect(file_exportGraphML, SIGNAL(triggered()), this, SLOT(on_actionExport_GraphML_triggered()));
    connect(file_importJenkinsNodes, SIGNAL(triggered()), this, SLOT(on_actionImportJenkinsNode()));

    connect(edit_paste, SIGNAL(triggered()), this, SLOT(on_actionPaste_triggered()));

    connect(exit, SIGNAL(triggered()), this, SLOT(on_actionExit_triggered()));

    connect(view_fitToScreen, SIGNAL(triggered()), nodeView, SLOT(fitToScreen()));
    connect(view_goToDefinition, SIGNAL(triggered()), this, SLOT(goToDefinition()));
    connect(view_goToImplementation, SIGNAL(triggered()), this, SLOT(goToImplementation()));

    connect(model_clearModel, SIGNAL(triggered()), this, SLOT(on_actionClearModel_triggered()));
    connect(model_sortModel, SIGNAL(triggered()), this, SLOT(on_actionSortModel_triggered()));
    //connect(model_sortModel, SIGNAL(triggered()), this, SLOT(on_actionSortNode_triggered()));

    connect(projectName, SIGNAL(clicked()), nodeView, SLOT(clearSelection()));

    connect(assemblyButton, SIGNAL(clicked()), this, SLOT(updateViewAspects()));
    connect(hardwareButton, SIGNAL(clicked()), this, SLOT(updateViewAspects()));
    connect(definitionsButton, SIGNAL(clicked()), this, SLOT(updateViewAspects()));
    connect(workloadButton, SIGNAL(clicked()), this, SLOT(updateViewAspects()));

    connect(this, SIGNAL(setViewAspects(QStringList)), nodeView, SLOT(setViewAspects(QStringList)));

    connect(this, SIGNAL(clearDock()), partsContainer, SLOT(clear()));
    connect(this, SIGNAL(clearDock()), hardwareContainer, SLOT(clear()));
    connect(this, SIGNAL(clearDock()), definitionsContainer, SLOT(clear()));

    connect(nodeView, SIGNAL(hardwareNodeMade(QString, NodeItem*)), this, SLOT(addNewNodeToDock(QString, NodeItem*)));
    connect(nodeView, SIGNAL(componentNodeMade(QString, NodeItem*)), this, SLOT(addNewNodeToDock(QString, NodeItem*)));
    connect(nodeView, SIGNAL(updateDockAdoptableNodesList(Node*)), this, SLOT(nodeSelected(Node*)));

    connect(partsContainer, SIGNAL(constructDockNode(Node*, QString)), nodeView, SLOT(view_DockConstructNode(Node*, QString)));

    connect(definitionsContainer, SIGNAL(getSelectedNode()), this, SLOT(getSelectedNode()));
    connect(definitionsContainer, SIGNAL(trigger_addComponentInstance(Node*,Node*,int)), nodeView, SLOT(view_ConstructComponentInstance(Node*,Node*,int)));
    connect(definitionsContainer, SIGNAL(trigger_connectComponentInstance(Node*,Node*)), nodeView, SLOT(view_ConstructEdge(Node*,Node*)));

    connect(nodeView, SIGNAL(view_SetSelectedAttributeModel(AttributeTableModel*)), this, SLOT(setAttributeModel(AttributeTableModel*)));
    connect(nodeView, SIGNAL(customContextMenuRequested(QPoint)), nodeView, SLOT(showContextMenu(QPoint)));

    connect(nodeView, SIGNAL(updateDataTable()), this, SLOT(updateDataTable()));
    connect(nodeView, SIGNAL(updateDockButtons(QString)), this, SLOT(updateDockButtons(QString)));
    connect(nodeView, SIGNAL(updateDockContainer(QString)), this, SLOT(updateDockContainer(QString)));

    connect(nodeView, SIGNAL(getAdoptableNodesList(Node*)), this, SLOT(getAdoptableNodesList(Node*)));
    connect(this, SIGNAL(updateToolbarAdoptableNodesList(QStringList)), nodeView, SLOT(updateToolbarAdoptableNodesList(QStringList)));

    connect(nodeView, SIGNAL(getComponentDefinitions(Node*)), this, SLOT(getComponentDefinitions(Node*)));
    connect(this, SIGNAL(sendComponentDefinitions(QList<Node*>*)), nodeView, SLOT(updateToolbarDefinitionsList(QList<Node*>*)));

    connect(nodeView, SIGNAL(turnOnViewAspect(QString)), this, SLOT(turnOnViewAspect(QString)));
    connect(nodeView, SIGNAL(setGoToMenuActions(QString,bool)), this, SLOT(setGoToMenuActions(QString,bool)));

    // this needs fixing
    connect(this, SIGNAL(checkDockScrollBar()), partsContainer, SLOT(checkScrollBar()));

    connect(this, SIGNAL(setupViewLayout()), this, SLOT(sortAndCenterViewAspects()));

    connectToController();
}


/**
 * @brief NewMedeaWindow::connectToController
 * Connect signals and slots to the controller.
 */
void NewMedeaWindow::connectToController()
{
    connect(this, SIGNAL(view_ImportGraphML(QStringList)), controller, SLOT(view_ImportGraphML(QStringList)));
    connect(this, SIGNAL(view_ExportGraphML(QString)), controller, SLOT(view_ExportGraphML(QString)));
    connect(controller, SIGNAL(view_WriteGraphML(QString,QString)), this, SLOT(writeExportedGraphMLData(QString,QString)));

    connect(model_clearModel, SIGNAL(triggered()), controller, SLOT(view_ClearModel()));

    connect(controller, SIGNAL(view_UpdateUndoList(QStringList)), this, SLOT(updateUndoStates(QStringList)));
    connect(controller, SIGNAL(view_UpdateRedoList(QStringList)), this, SLOT(updateRedoStates(QStringList)));

    connect(edit_undo, SIGNAL(triggered()), controller, SLOT(view_Undo()));
    connect(edit_redo, SIGNAL(triggered()), controller, SLOT(view_Redo()));
    connect(edit_cut, SIGNAL(triggered()), controller, SLOT(view_Cut()));
    connect(edit_copy, SIGNAL(triggered()), controller, SLOT(view_Copy()));
    connect(this, SIGNAL(view_PasteData(QString)), controller, SLOT(view_Paste(QString)));
    connect(controller, SIGNAL(view_UpdateCopyBuffer(QString)), this, SLOT(setClipboard(QString)));

    connect(projectName, SIGNAL(clicked()), controller, SLOT(view_SelectModel()));
    connect(controller, SIGNAL(view_UpdateProjectName(QString)), this, SLOT(updateProjectName(QString)));
}


/**
 * @brief NewMedeaWindow::resizeEvent
 * The width of the scroll areas and group boxes in the dock are fixed.
 * The height changes depending on window size and content.
 * @param event
 */
void NewMedeaWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    boxHeight = this->height()*0.75;
    partsContainer->setMinimumHeight(boxHeight);
    hardwareContainer->setMinimumHeight(boxHeight);
    definitionsContainer->setMinimumHeight(boxHeight);

    // update dataTable size
    updateDataTable();
}


/**
 * @brief NewMedeaWindow::sortAndCenterModel
 * This force sorts the main definitions containers before they are hidden.
 * The visible view aspect(s) is then centered.
 */
void NewMedeaWindow::sortAndCenterViewAspects()
{
    if (nodeView) {
        nodeView->forceSortViewAspects();
        updateViewAspects();
    }
}


/**
 * @brief NewMedeaWindow::setupJenkinsSettings
 */
void NewMedeaWindow::setupJenkinsSettings()
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
 * @brief NewMedeaWindow::on_actionImportJenkinsNode
 */
void NewMedeaWindow::on_actionImportJenkinsNode()
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
 * @brief NewMedeaWindow::on_actionNew_Project_triggered
 * At the moment it olnly allows one project to be opened at a time.
 */
void NewMedeaWindow::on_actionNew_Project_triggered()
{
    // ask user if they want to save current project before closing it
    QMessageBox::StandardButton saveProject = QMessageBox::question(this,
                                                                    "Creating A New Project",
                                                                    "Current project will be closed.\nSave changes?",
                                                                    QMessageBox::Yes | QMessageBox::No);
    if (saveProject == QMessageBox::Yes) {
        // if failed to export, do nothing
        if (!exportGraphML()) {
            return;
        }
    }

    // clear item selection and reset gui
    nodeView->clearSelection();
    resetGUI();
}


/**
 * @brief NewMedeaWindow::on_actionImport_GraphML_triggered
 */
void NewMedeaWindow::on_actionImport_GraphML_triggered()
{
    QStringList files = QFileDialog::getOpenFileNames(
                this,
                "Select one or more files to open",
                "c:\\",
                "GraphML Documents (*.graphml *.xml)");

    importGraphMLFiles(files);
    QStringList fileData;

    // clear item selection
    nodeView->clearSelection();
}


/**
 * @brief NewMedeaWindow::on_actionExport_GraphML_triggered
 */
void NewMedeaWindow::on_actionExport_GraphML_triggered()
{
    exportGraphML();
}


/**
 * @brief NewMedeaWindow::on_clearModel_triggered
 * When the model is cleared or the new project menu is triggered,
 * this method resets the model, clears any current selection,
 * disables the dock buttons and clears the dock containers.
 */
void NewMedeaWindow::on_actionClearModel_triggered()
{
    nodeView->clearSelection();
    nodeView->resetModel();
    emit clearDock();
}


/**
 * @brief NewMedeaWindow::on_actionSortModel_triggered
 */
void NewMedeaWindow::on_actionSortModel_triggered()
{
   if (selectedNode) {
       nodeView->sortNode(selectedNode, selectedNode);
   } else {
       nodeView->sortEntireModel();
   }
}

/*
void NewMedeaWindow::on_actionSortNode_triggered()
{

}
*/

/**
 * @brief NewMedeaWindow::on_actionPaste_triggered
 */
void NewMedeaWindow::on_actionPaste_triggered()
{
    QClipboard *clipboard = QApplication::clipboard();
    if (clipboard->ownsClipboard()) {
        emit view_PasteData(clipboard->text());
    }
}


/**
 * @brief NewMedeaWindow::on_actionExit_triggered
 */
void NewMedeaWindow::on_actionExit_triggered()
{
    close();
}


/**
 * @brief NewMedeaWindow::writeExportedGraphMLData
 * @param fileName
 * @param data
 */
void NewMedeaWindow::writeExportedGraphMLData(QString filename, QString data)
{
    try {
        QFile file(filename);
        file.open(QIODevice::WriteOnly | QIODevice::Text);

        QTextStream out(&file);
        out << data;

        file.close();
        qDebug() << "Successfully written file: " << filename;

    } catch(...){
        qCritical() << "Export Failed!" ;
    }
}


/**
 * @brief NewMedeaWindow::updateUndoStates
 * @param list
 */
void NewMedeaWindow::updateUndoStates(QStringList list)
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
 * @brief NewMedeaWindow::updateRedoStates
 * @param list
 */
void NewMedeaWindow::updateRedoStates(QStringList list)
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
 * @brief NewMedeaWindow::setClipboard
 * @param value
 */
void NewMedeaWindow::setClipboard(QString value)
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(value);
}


/**
 * @brief NewMedeaWindow::updateProjectName
 * @param label
 */
void NewMedeaWindow::updateProjectName(QString label)
{
    setWindowTitle("MEDEA - " + label);
    projectName->setText(label);
}


/**
 * @brief NewMedeaWindow::newNodeMadeAddToToolbar
 * Once a node has been defined, add its icon to its corresponding container
 * in the dock. Right now buttons are being used instead of icons.
 * @param nodeName
 */
void NewMedeaWindow::addNewNodeToDock(QString type, NodeItem *nodeItem)
{
    DockScrollArea *container;

    if (type == "component") {
        container = definitionsContainer;
    } else if (type == "hardware") {
        container = hardwareContainer;
    }

    container->addDockNode(nodeItem);
}


/**
 * @brief NewMedeaWindow::updateAspects
 * Add view aspect to checkedViewAspects when the corresponding button
 * is clicked and remove it when it's unclick then update the view.
 */
void NewMedeaWindow::updateViewAspects()
{
    QPushButton *sourceButton = qobject_cast<QPushButton*>(QObject::sender());
    if (sourceButton) {

        QString view = sourceButton->text();
        if (view == "Interface") {
            view = "Definitions";
        } else if (view == "Behaviour") {
            view = "Workload";
        }

        if (sourceButton->isChecked()) {
            checkedViewAspects.append(view);
        } else {
            int index = checkedViewAspects.indexOf(view, 0);
            checkedViewAspects.removeAt(index);
        }
    }

    emit setViewAspects(checkedViewAspects);
}


/**
 * @brief NewMedeaWindow::updateDockButtons
 * Enable/disable dock toggle buttons depending on the selected node.
 * If a button is disabled, hide its corresponding container.
 * P - Parts
 * D - Definitions
 * H - Hardware Nodes
 * N - None
 * A - All
 * @param dockButton
 */
void NewMedeaWindow::updateDockButtons(QString dockButton)
{
    partsButton->setEnabled(false);
    hardwareNodesButton->setEnabled(false);
    compDefinitionsButton->setEnabled(false);

    if (dockButton.contains("P")) {
        partsButton->setEnabled(true);
    }else if (dockButton.contains("D")) {
        compDefinitionsButton->setEnabled(true);
    }else if (dockButton.contains("H")) {
        hardwareNodesButton->setEnabled(true);
    }else if (dockButton == "A"){
        partsButton->setEnabled(true);
        hardwareNodesButton->setEnabled(true);
        compDefinitionsButton->setEnabled(true);
    }

    // hide necessary dock containers
    partsButton->checkEnabled();
    hardwareNodesButton->checkEnabled();
    compDefinitionsButton->checkEnabled();
}


/**
 * @brief NewMedeaWindow::updateDockContainer
 * This method updates the specified dock container.
 * @param container
 */
void NewMedeaWindow::updateDockContainer(QString container)
{
    if (container == "Parts") {
        // update dock container's adoptable nodes list and then send it to the toolbar
        if(selectedNode && controller){
            partsContainer->addAdoptableDockNodes(controller->getSelectedNode(), controller->getAdoptableNodeKinds(controller->getSelectedNode()));
            emit updateToolbarAdoptableNodesList(partsContainer->getAdoptableNodesList());
        }
    } else if (container == "Hardware") {
        // update hardwareDefinitons container
    } else if (container == "Definitions") {
        // update compDefinitons container
    }
    update();
}


/**
 * @brief NewMedeaWindow::setAdoptableNodeList
 * This checks to see if the partsButton is currently selected and that the
 * partsContainer is visible before it updates the dock adoptable nodes list.
 * @param node
 */
void NewMedeaWindow::setAdoptableNodeList(Node *node)
{
    if (node) {
        if (prevSelectedNode != 0 && prevSelectedNode == node) {
            return;
        } else {
            QStringList nodeKinds = controller->getAdoptableNodeKinds(node);
            partsContainer->addAdoptableDockNodes(node, nodeKinds);

            emit checkDockScrollBar();
            emit updateToolbarAdoptableNodesList(nodeKinds);
            update();
        }
        prevSelectedNode = node;
    }
}


/**
 * @brief NewMedeaWindow::nodeSelected
 * @param node
 */
void NewMedeaWindow::nodeSelected(Node *node)
{
    selectedNode = node;

    // if partsConatiner is open, update adoptable node list
    if (partsButton->getSelected()) {
        setAdoptableNodeList(selectedNode);
    }
}


/**
 * @brief NewMedeaWindow::getSelectedNode
 * Send the selected node to the Definitions dock.
 */
void NewMedeaWindow::getSelectedNode()
{
    emit definitionsContainer->selectedNode(nodeView->getSelectedNode());
}


/**
 * @brief NewMedeaWindow::goToDefinition
 */
void NewMedeaWindow::goToDefinition()
{
    if (selectedNode) {
        nodeView->goToDefinition(selectedNode);
    }
}


/**
 * @brief NewMedeaWindow::goToImplementation
 */
void NewMedeaWindow::goToImplementation()
{
    if (selectedNode) {
        nodeView->goToImplementation(selectedNode);
    }
}


/**
 * @brief NewMedeaWindow::getAdoptableNodesList
 * If the parts dock is already visible then it already has the adoptable nodes list.
 * If not, get the list from the controller. Send the list to the toolbar widget.
 */
void NewMedeaWindow::getAdoptableNodesList(Node* node)
{
    if (!partsButton->getSelected()) {
        setAdoptableNodeList(node);
    }
}


/**
 * @brief NewMedeaWindow::getDefinitionsList
 * @param node
 */
void NewMedeaWindow::getComponentDefinitions(Node *node)
{
   if (definitionsButton->isEnabled()) {
       QList<Node*>* definitions = new QList<Node*>(definitionsContainer->getComponentDefinitions());
       emit sendComponentDefinitions(definitions);
   }
}


/**
 * @brief NewMedeaWindow::turnOnViewAspect
 * This turns on the view aspect specified.
 * @param aspect
 */
void NewMedeaWindow::turnOnViewAspect(QString aspect)
{
    if (aspect == "Definitions") {
        emit definitionsButton->click();
    } else if (aspect == "Workload") {
        workloadButton->click();
    } else if (aspect == "Assembly") {
        assemblyButton->click();
    } else if (aspect == "Hardware") {
        hardwareButton->click();
    }
}


/**
 * @brief NewMedeaWindow::setGoToMenuActions
 * @param action
 * @param node
 */
void NewMedeaWindow::setGoToMenuActions(QString action, bool enabled)
{
    if (action == "definition") {
        view_goToDefinition->setEnabled(enabled);
    } else if (action == "implementation") {
        view_goToImplementation->setEnabled(enabled);
    }
}


/**
 * @brief NewMedeaWindow::exportGraphML
 * @return
 */
bool NewMedeaWindow::exportGraphML()
{
    /*
    QFileDialog *saveDialog = new QFileDialog(this,"Export .graphML", "c:\\", "GraphML Documents (*.graphml *.xml)");

    if(QDialog::Accepted == saveDialog->exec()){
        QString fileName = saveDialog->selectedFiles().at(0);
        emit view_ExportGraphML(fileName);
        //TODO: Wait for successful export then return.
        qCritical() << "got export";
        return true;
    }else{
        qCritical() << "No export";
        return false;
    }
    */

    QString filename = QFileDialog::getSaveFileName(this,
                                                    "Export .graphML",
                                                    "C:\\",
                                                    "GraphML Documents (*.graphML *.xml)");

    if (filename != "") {
        emit view_ExportGraphML(filename);
        //TODO: Wait for successful  then return.
        return true;
    } else {
        return false;
    }
}


/**
 * @brief NewMedeaWindow::setAttributeModel
 * @param model
 */
void NewMedeaWindow::setAttributeModel(AttributeTableModel *model)
{
    dataTable->setModel(model);
    updateDataTable();
}


/**
 * @brief NewMedeaWindow::on_dockButtonPressed
 * This method makes sure that the groupbox attached to the dock button
 * that was pressed is the only groupbox being currently displayed.
 * @param buttonName
 */
void NewMedeaWindow::on_dockButtonPressed(QString buttonName)
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
    update();

    // when the partsButton is selected, update the parts container
    if (b == partsButton && partsButton->getSelected()) {
        setAdoptableNodeList(selectedNode);
    }
}


/**
 * @brief NewMedeaWindow::updateDataTable
 * Update the dataTable size whenever a node is selected/deselected,
 * when a new model is loaded and when the window is resized.
 * NOTE: Once maximum size is set, it cannot be reset.
 */
void NewMedeaWindow::updateDataTable()
{
    // this squeezes the content as much as possible
    // to make it fit in the space available
    //dataTable->resizeRowsToContents();

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

    dataTableBox->repaint();
}


/**
 * @brief NewMedeaWindow::loadJenkinsData
 * @param code
 */
void NewMedeaWindow::loadJenkinsData(int code)
{
    QStringList files;
    files << myProcess->readAll();
    emit view_ImportGraphML(files);

    // sort and center view aspects
    nodeView->view_centerViewAspects();
}


/**
 * @brief NewMedeaWindow::importGraphMLFiles
 * @param files
 */
void NewMedeaWindow::importGraphMLFiles(QStringList files)
{
    QStringList fileData;
    for (int i = 0; i < files.size(); i++) {

        QFile file(files.at(i));

        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            qCritical() << "Could not open: " << files.at(i) << " for reading!";
            return;
        }

        try {
            QTextStream in(&file);
            QString xmlText = in.readAll();
            file.close();
            fileData << xmlText;
            qDebug() << "Loaded: " << files.at(i) << "Successfully!";
        } catch (...) {
            qCritical() << "Error Loading: " << files.at(i);
        }
    }

    emit nodeView->controlPressed(false);
    emit nodeView->shiftPressed(false);

    emit view_ImportGraphML(fileData);
    nodeView->view_centerViewAspects();
}
