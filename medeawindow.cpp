#include "medeawindow.h"
#include "ui_medeawindow.h"

#include <QFileDialog>
#include <QMdiSubWindow>
#include <QDebug>
#include <QClipboard>
#include <QStatusBar>
#include <QProgressBar>
#include <QPushButton>
#include <QItemSelectionModel>
#include <QSettings>
#include <QHeaderView>

#include <QProcess>

MedeaWindow::MedeaWindow(QString graphMLFile, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MedeaWindow)
{


    ui->setupUi(this);
    myProcess = 0;



    setupJenkinsSettings();

    currentOperationBar = new QProgressBar(this);
    currentOperationBar->setMaximum(100);

    statusBar()->addPermanentWidget(currentOperationBar);

    QHeaderView *headerView = ui->nodeTreeView->header();
    headerView->setSectionResizeMode(QHeaderView::Stretch);

    ui->nodeTreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->nodeTreeView->setSelectionMode(QTableView::SingleSelection);



    //ui->nodeTreeView->header()->setStretchLastSection(true);
    ui->attributeTable->horizontalHeader()->setStretchLastSection(true);

    //ui->nodeTreeView->header()->resizeSections(QHeaderView::ResizeToContents);


    selectedProject = 0;

    connect(ui->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this , SLOT(projectWindowSelected(QMdiSubWindow*)));

    connect(ui->actionTile, SIGNAL(triggered()), ui->mdiArea, SLOT(tileSubWindows()));
    connect(ui->actionCascade, SIGNAL(triggered()), ui->mdiArea, SLOT(cascadeSubWindows()));
    connect(ui->actionClose_All, SIGNAL(triggered()), ui->mdiArea, SLOT(closeAllSubWindows()));

    connect(ui->clearFilters, SIGNAL(clicked()), this, SLOT(clearFilters()));
    connect(ui->addFilter, SIGNAL(clicked()), this, SLOT(appendFilter()));
    connect(ui->addAspect, SIGNAL(clicked()), this, SLOT(appendAspect()));

    ui->actionNew_Project->trigger();
   this->setSelectedProject(projectWindows[0]);

    /*
    QFile file("C:/test.graphml");

    if(!file.open(QFile::ReadOnly | QFile::Text)){
        qDebug() << "could not open file for read";
    }

    QTextStream in(&file);
    QString xmlText = in.readAll();
    file.close();
    this->setSelectedProject(projectWindows[0]);

    emit view_TriggerAction("Loading GraphML");
    //emit view_ActionTriggered("Loading XME");
    emit view_PasteData(xmlText);

    updateProgressBar(100,"");


    ui->nodeIDCombo->addItems(selectedProject->getController()->getNodeIDS());


    selectedProject->getController()->view_SelectFromID("49");
    //on_nodeIDCombo_activated("etst");
    //ui->actionImport_GraphML->trigger();
    */

    if(graphMLFile.length() != 0){
        QStringList files;
        files.append(graphMLFile);
        importGraphMLFiles(files);
    }
}

MedeaWindow::~MedeaWindow()
{
    delete ui;
}

void MedeaWindow::clearSelectedTreeViewItem()
{
    this->ui->nodeTreeView->clearSelection();
}

void MedeaWindow::setupJenkinsSettings()
{
    DEPGEN_ROOT = QString(qgetenv("DEPGEN_ROOT"));

    QSettings Settings(DEPGEN_ROOT+"/release/config.ini", QSettings::IniFormat);

    Settings.beginGroup("Jenkins-Settings");
    JENKINS_ADDRESS = Settings.value("JENKINS_ADDRESS").toString();
    JENKINS_USERNAME = Settings.value("JENKINS_USERNAME").toString();
    JENKINS_PASSWORD = Settings.value("JENKINS_PASSWORD").toString();
    Settings.endGroup();

    if(JENKINS_ADDRESS  != ""){

        QAction *getJenkinsNodes = new QAction(0);
        getJenkinsNodes->setText("Get Jenkins Nodes from: " + JENKINS_ADDRESS);
        getJenkinsNodes->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_J));
        connect(getJenkinsNodes, SIGNAL(triggered()), this, SLOT(getJenkinsNodes()));

        ui->menuFile->addAction(getJenkinsNodes);

    }

}

void MedeaWindow::updateStatusText(QString statusText)
{
    this->ui->errorCode->setText(statusText);
}

void MedeaWindow::view_SetTableModel(QAbstractTableModel *model)
{
    this->ui->nodeTreeView->setModel(model);
}


void MedeaWindow::updateFilterButtons(QVector<FilterButton *> buttons)
{
    int count = ui->AppliedFilters->count();
    for (int i = 0; i < count; i++) {

        QWidget *w = ui->AppliedFilters->itemAt(i)->widget();
        w->setVisible(false);
        //ui->AppliedFilters->removeWidget(w);
    }

    update();

    foreach(FilterButton* filter, buttons){
        ui->AppliedFilters->addWidget(filter);
        filter->setVisible(true);
    }
    update();

}

void MedeaWindow::updateAspectButtons(QVector<FilterButton *> buttons)
{
    int count = ui->VisibleAspects->count();
    for (int i = 0; i < count; i++) {
        QWidget *w = ui->VisibleAspects->itemAt(i)->widget();
        w->setVisible(false);
        //ui->AppliedFilters->removeWidget(w);
    }

    update();

    foreach(FilterButton* filter, buttons){
        ui->VisibleAspects->addWidget(filter);
        filter->setVisible(true);
    }
    update();

}

void MedeaWindow::updateProgressBar(int percentage, QString label)
{
    if(percentage >= 100){
        this->currentOperationBar->setVisible(false);
        this->currentOperationBar->setValue(0);
    }else{
        this->currentOperationBar->setVisible(true);
         this->currentOperationBar->setValue(percentage);

    }
}

void MedeaWindow::on_actionImport_GraphML_triggered()
{

         QStringList files = QFileDialog::getOpenFileNames(
                this,
                "Select one or more files to open",
                "c:\\",
                "GraphML Documents (*.graphml *.xml)");
         importGraphMLFiles(files);

   // emit view_ImportGraphML(fileData);

}

void MedeaWindow::on_actionNew_Project_triggered()
{
    //Add a new Window.
    ProjectWindow* newWindow = new ProjectWindow(ui->mdiArea);
    projectWindows.append(newWindow);

    connect(newWindow, SIGNAL(destroyed(QObject*)), this, SLOT(windowClosed(QObject*)));

}

void MedeaWindow::windowClosed(QObject *window)
{
    int position = projectWindows.indexOf((ProjectWindow*)window);
    if(window == selectedProject){
        ui->nodeTreeView->setModel(0);
        ui->attributeTable->setModel(0);
        selectedProject = 0;
    }
    projectWindows.removeAt(position);
    projectWindowSelected(0);
}

void MedeaWindow::appendFilter()
{
    QString data = ui->searchBox->text();

    if(data != ""){
        qCritical() << "Emitting Append Filter: " << data;
        ui->searchBox->clear();
        emit view_AddFilter(data);
    }
}

void MedeaWindow::appendAspect()
{

    QString data = ui->aspectComboBox->currentText();

    if(data != ""){
        qCritical() << "Emitting Append Aspect: " << data;
        emit view_AddAspect(data);
    }

}


void MedeaWindow::clearFilters()
{
    emit view_ClearFilters();
    ui->filterBox->update();
}

void MedeaWindow::projectWindowSelected(QMdiSubWindow *window)
{
    ProjectWindow* newlySelected  = dynamic_cast<ProjectWindow*>(window);
    setSelectedProject(newlySelected);
}

void MedeaWindow::setAttributeModel(AttributeTableModel* model)
{
    ui->attributeTable->setModel(model);
    //ui->nodeTable->setModel(0);
}

void MedeaWindow::updateUndoStates(QStringList list)
{

    if(list.size() == 0){
        ui->actionUndo->setEnabled(false);
    }else{
        ui->actionUndo->setEnabled(true);
    }

    ui->actionUndo->setText(QString("Undo [%1]").arg(list.size()));

    QMenu* menu = ui->actionUndo->menu();
    if(menu){
        menu->clear();
    }else{
        menu = new QMenu();
        ui->actionUndo->setMenu(menu);
    }
    foreach(QString item, list){
        QAction* newUndo = new QAction(item, menu);
        newUndo->setEnabled(false);
        menu->addAction(newUndo);
    }

}

void MedeaWindow::writeExportedGraphMLData(QString filename, QString data)
{
    try{
        QFile file(filename);
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&file);
        out << data;
        file.close();

        qDebug() << "Successfully written file: " << filename;
    }catch(...){
        qCritical() << "Export Failed!" ;
    }

}


void MedeaWindow::updateRedoStates(QStringList list)
{
    if(list.size() == 0){
        ui->actionRedo->setEnabled(false);
    }else{
        ui->actionRedo->setEnabled(true);
    }

    ui->actionRedo->setText(QString("Redo [%1]").arg(list.size()));

    QMenu* menu = ui->actionRedo->menu();
    if(menu){
        menu->clear();
    }else{
        menu = new QMenu();
        ui->actionRedo->setMenu(menu);
    }
    foreach(QString item, list){
        QAction* newRedo = new QAction(item, menu);
        newRedo->setEnabled(false);
        menu->addAction(newRedo);
    }

}

void MedeaWindow::setClipboard(QString value)
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(value);
}

void MedeaWindow::getJenkinsNodes()
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

void MedeaWindow::setSelectedProject(ProjectWindow *newSelected)
{
    if(selectedProject){
        NewController* controller = selectedProject->getController();
        NodeView* view = selectedProject->getView();

        disconnect(controller, SIGNAL(view_UpdateUndoList(QStringList)), this, SLOT(updateUndoStates(QStringList)));
        disconnect(controller, SIGNAL(view_UpdateRedoList(QStringList)), this, SLOT(updateRedoStates(QStringList)));

        disconnect(controller, SIGNAL(view_UpdateCopyBuffer(QString)), this, SLOT(setClipboard(QString)));

        disconnect(this, SIGNAL(view_PasteData(QString)), controller, SLOT(view_Paste(QString)));
        disconnect(this, SIGNAL(view_TriggerAction(QString)), controller, SLOT(view_TriggerAction(QString)));

        disconnect(ui->actionMaximize, SIGNAL(triggered()), selectedProject, SLOT(showMaximized()));
        disconnect(ui->actionUndo, SIGNAL(triggered()), controller, SLOT(view_Undo()));
        disconnect(ui->actionRedo, SIGNAL(triggered()), controller, SLOT(view_Redo()));
        disconnect(ui->actionCut, SIGNAL(triggered()), controller, SLOT(view_Cut()));
        disconnect(ui->actionCopy, SIGNAL(triggered()), controller, SLOT(view_Copy()));


        disconnect(selectedProject, SIGNAL(updateFilterButtons(QVector<FilterButton*>)), this, SLOT(updateFilterButtons(QVector<FilterButton*>)));
        disconnect(selectedProject, SIGNAL(updateAspectButtons(QVector<FilterButton*>)), this, SLOT(updateAspectButtons(QVector<FilterButton*>)));
        disconnect(this, SIGNAL(view_AddFilter(QString)), selectedProject, SLOT(appendFilterString(QString)));
        disconnect(this, SIGNAL(view_AddAspect(QString)), selectedProject, SLOT(appendAspectString(QString)));
        disconnect(this, SIGNAL(view_ClearFilters()), selectedProject, SLOT(clearFilters()));

        disconnect(this, SIGNAL(view_AspectsVisible(QStringList)), selectedProject, SLOT(setVisibleAspects(QStringList)));

        disconnect(controller, SIGNAL(view_SetGUIEnabled(bool)), this, SLOT(setEnableGUI(bool)));

        disconnect(view, SIGNAL(view_SetSelectedAttributeModel(AttributeTableModel*)), this, SLOT(setAttributeModel(AttributeTableModel*)));

        disconnect(this, SIGNAL(view_ExportGraphML(QString)), controller, SLOT(view_ExportGraphML(QString)));
        disconnect(controller, SIGNAL(view_WriteGraphML(QString,QString)), this, SLOT(writeExportedGraphMLData(QString,QString)));

        disconnect(this, SIGNAL(view_ImportGraphML(QStringList)), controller, SLOT(view_ImportGraphML(QStringList)));

        disconnect(ui->actionValidate_Model, SIGNAL(triggered()), controller, SLOT(view_ValidateModel()));

        disconnect(controller, SIGNAL(view_UpdateStatusText(QString)), this, SLOT(updateStatusText(QString)));

        disconnect(selectedProject, SIGNAL(setTableView(QAbstractTableModel*)), this, SLOT(view_SetTableModel(QAbstractTableModel*)));

    }

    selectedProject = newSelected;

    if(selectedProject){
        connect(ui->actionMaximize, SIGNAL(triggered()), selectedProject, SLOT(showMaximized()));
        NewController* controller = selectedProject->getController();
        NodeView* view = selectedProject->getView();

        this->ui->aspectComboBox->clear();;
        this->ui->aspectComboBox->addItems(controller->getViewAspects());


        connect(ui->nodeTreeView, SIGNAL(clicked(QModelIndex)), selectedProject, SLOT(treeViewItemSelected(QModelIndex)));


        connect(selectedProject, SIGNAL(setTableView(QAbstractTableModel*)), this, SLOT(view_SetTableModel(QAbstractTableModel*)));
        connect(selectedProject, SIGNAL(updateFilterButtons(QVector<FilterButton*>)), this, SLOT(updateFilterButtons(QVector<FilterButton*>)));
        connect(selectedProject, SIGNAL(updateAspectButtons(QVector<FilterButton*>)), this, SLOT(updateAspectButtons(QVector<FilterButton*>)));
        connect(this, SIGNAL(view_AddFilter(QString)), selectedProject, SLOT(appendFilterString(QString)));
        connect(this, SIGNAL(view_AddAspect(QString)), selectedProject, SLOT(appendAspectString(QString)));
        connect(this, SIGNAL(view_ClearFilters()), selectedProject, SLOT(clearFilters()));

        connect(this, SIGNAL(view_AspectsVisible(QStringList)), selectedProject, SLOT(setVisibleAspects(QStringList)));

        connect(this, SIGNAL(view_ExportGraphML(QString)), controller, SLOT(view_ExportGraphML(QString)));
        connect(controller, SIGNAL(view_WriteGraphML(QString,QString)), this, SLOT(writeExportedGraphMLData(QString,QString)));
        connect(this, SIGNAL(view_ImportGraphML(QStringList)), controller, SLOT(view_ImportGraphML(QStringList)));



        connect(ui->horizontalSlider, SIGNAL(valueChanged(int)), selectedProject->getView(), SLOT(depthChanged(int)));
        connect(controller, SIGNAL(view_UpdateUndoList(QStringList)), this, SLOT(updateUndoStates(QStringList)));
        connect(controller, SIGNAL(view_UpdateRedoList(QStringList)), this, SLOT(updateRedoStates(QStringList)));
        connect(controller, SIGNAL(view_UpdateCopyBuffer(QString)), this, SLOT(setClipboard(QString)));

        connect(controller, SIGNAL(view_UpdateProgressBar(int,QString)), this, SLOT(updateProgressBar(int,QString)));
        connect(ui->actionUndo, SIGNAL(triggered()), controller, SLOT(view_Undo()));
        connect(ui->actionRedo, SIGNAL(triggered()), controller, SLOT(view_Redo()));
        connect(ui->actionCut, SIGNAL(triggered()), controller, SLOT(view_Cut()));
        connect(ui->actionCopy, SIGNAL(triggered()), controller, SLOT(view_Copy()));

        connect(this, SIGNAL(view_TriggerAction(QString)), controller, SLOT(view_TriggerAction(QString)));
        connect(this, SIGNAL(view_PasteData(QString)), controller, SLOT(view_Paste(QString)));



        connect(view, SIGNAL(view_SetSelectedAttributeModel(AttributeTableModel*)), this, SLOT(setAttributeModel(AttributeTableModel*)));
        connect(controller, SIGNAL(view_SetGUIEnabled(bool)), this, SLOT(setEnableGUI(bool)));

        connect(controller, SIGNAL(view_UpdateStatusText(QString)), this, SLOT(updateStatusText(QString)));

        connect(ui->actionValidate_Model, SIGNAL(triggered()), controller, SLOT(view_ValidateModel()));

        //Force a refresh
        selectedProject->selectedProject();

    }
}


ProjectWindow *MedeaWindow::getProjectWindow(QObject *object)
{
    int position = projectWindows.indexOf((ProjectWindow*)object);
    return projectWindows.at(position);
}

void MedeaWindow::on_actionPaste_triggered()
{
    QClipboard *clipboard = QApplication::clipboard();
    //if(clipboard->ownsClipboard()){
    emit view_PasteData(clipboard->text());
    //}
}

void MedeaWindow::setEnableGUI(bool enable)
{

    if(selectedProject){
        selectedProject->setEnabled(enable);
    }

    ui->attributeTable->setEnabled(enable);
    ui->menubar->setEnabled(enable);

}

void MedeaWindow::keyPressEvent(QKeyEvent *event)
{
    if(selectedProject){
        selectedProject->getView()->keyPressEvent(event);
    }

}

void MedeaWindow::keyReleaseEvent(QKeyEvent *event)
{
    if(selectedProject){
        selectedProject->getView()->keyReleaseEvent(event);
    }

}

void MedeaWindow::importGraphMLFiles(QStringList files)
{
    QStringList fileData;
    for (int i = 0; i < files.size(); i++){
        QFile file(files.at(i));

        if(!file.open(QFile::ReadOnly | QFile::Text)){
            qCritical() << "Could not open: " << files.at(i) << " for reading!";
            return;
        }

        try{
            QTextStream in(&file);
            QString xmlText = in.readAll();
            file.close();

            fileData << xmlText;
            qDebug() << "Loaded: " << files.at(i) << "Successfully!";
        }catch(...){
            qCritical() << "Error Loading: " << files.at(i);
        }
    }

    if(selectedProject){
    int count = 2;
        while(count-=1 > 0){
            emit selectedProject->getView()->controlPressed(false);
            emit selectedProject->getView()->shiftPressed(false);
            emit view_ImportGraphML(fileData);
        }

    }else{
        qCritical() << "No Active Window";
    }
}

void MedeaWindow::on_deleteData_clicked()
{
    QAbstractItemModel* model = ui->attributeTable->model();
    if(model){
        QItemSelectionModel* selectionModel = ui->attributeTable->selectionModel();

        foreach(QModelIndex index, selectionModel->selectedIndexes()){
            model->removeRow(index.row());
            //model->removeRow(index);
        }
    }
}

void MedeaWindow::on_actionExport_GraphML_triggered()
{
    QString filename = QFileDialog::getSaveFileName(
                this,
                "Export .graphML",
                "c:\\",
                "GraphML Documents (*.graphml *.xml)");


    emit selectedProject->getView()->controlPressed(false);
    emit selectedProject->getView()->shiftPressed(false);
    emit view_ExportGraphML(filename);
}

void MedeaWindow::on_nodeIDCombo_activated(const QString &arg1)
{


    selectedProject->getController()->view_SelectFromID(arg1);



}

void MedeaWindow::loadJenkinsData(int code)
{
    qCritical() << "JENKINS: " << code;

    QStringList files;
    files << myProcess->readAll();
    emit view_ImportGraphML(files);

}
