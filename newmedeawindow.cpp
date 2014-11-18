#include "newmedeawindow.h"
#include "ui_newmedeawindow.h"


NewMedeaWindow::NewMedeaWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NewMedeaWindow)
{
    ui->setupUi(this);

    // Initialise graphicsview and scene
    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene, this);
    box = new QGroupBox(this);
    scene->addWidget(box);

    // set window size; used for graphicsview and groupbox
    int windowWidth = 1400;
    int windowHeight = 800;

    this->setCentralWidget(view);
    this->setMinimumSize(windowWidth, windowHeight);
    view->setMinimumSize(windowWidth, windowHeight);
    box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    box->setMinimumSize(view->width(), view->height());

    // layouts and spacer items
    QHBoxLayout *mainHlayout = new QHBoxLayout(this);
    QVBoxLayout *leftVlayout = new QVBoxLayout(this);
    QVBoxLayout *rightVlayout = new QVBoxLayout(this);
    QHBoxLayout *titleLayout = new QHBoxLayout(this);
    QHBoxLayout *searchLayout = new QHBoxLayout(this);
    QSpacerItem *vSpacer = new QSpacerItem(20, 30, QSizePolicy::Expanding, QSizePolicy::Expanding);

    // initialise other private variables
    projectName = new QLabel(" Project Name");
    projectName->setStyleSheet("font-size: 16px;");
    menuButton = new QPushButton("Menu", this);
    menu = new QMenu("+", this);
    searchBar = new QLineEdit(this);
    searchButton = new QPushButton("Search", this);
    assemblyButton = new QPushButton("Assembly", this);
    workloadButton = new QPushButton("Workload", this);
    definitionsButton = new QPushButton("Definitions", this);
    dataTable = new QTableView(this);
    notificationArea = new QTextEdit("Notifications can be displayed here.", this);

    // menu button/actions
    menu_newProject = menu->addAction("New Project");
    menu_importGraphML = menu->addAction("Import");
    menuButton->setMenu(menu);

    // set buttons' sizes
    menuButton->setFixedWidth(50);
    assemblyButton->setMinimumHeight(40);
    workloadButton->setMinimumHeight(40);
    definitionsButton->setMinimumHeight(40);

    notificationArea->setEnabled(false);

    // add widgets and layouts
    titleLayout->addWidget(menuButton, 1);
    titleLayout->addWidget(projectName, 8);
    searchLayout->addWidget(searchBar, 3);
    searchLayout->addWidget(searchButton, 1);
    leftVlayout->addLayout(titleLayout);
    leftVlayout->addStretch(5);
    rightVlayout->addLayout(searchLayout, 1);
    rightVlayout->addSpacerItem(vSpacer);
    rightVlayout->addWidget(assemblyButton, 1);
    rightVlayout->addWidget(workloadButton, 1);
    rightVlayout->addWidget(definitionsButton, 1);
    rightVlayout->addSpacerItem(vSpacer);
    rightVlayout->addWidget(dataTable, 4);
    rightVlayout->addSpacerItem(vSpacer);
    rightVlayout->addWidget(notificationArea, 1);
    rightVlayout->addSpacerItem(vSpacer);

    mainHlayout->addLayout(leftVlayout, 4);
    mainHlayout->addLayout(rightVlayout, 1);
    box->setLayout(mainHlayout);

    //-----------------------------------------------------------------------

    selectedProject = 0;
}



void NewMedeaWindow::setClipboard(QString value)
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(value);
}

ProjectWindow* NewMedeaWindow::getProjectWindow(QObject *object)
{
    int position = projectWindows.indexOf((ProjectWindow*)object);
    return projectWindows.at(position);
}

void NewMedeaWindow::on_actionPaste_triggered()
{
    QClipboard *clipboard = QApplication::clipboard();
    if (clipboard->ownsClipboard()) {
        emit view_PasteData(clipboard->text());
    }
}

NewMedeaWindow::~NewMedeaWindow()
{
    delete ui;
}
