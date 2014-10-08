#include "medeawindow.h"
#include "ui_medeawindow.h"

#include "GUI/nodeitem.h"
#include "GUI/nodeconnection.h"
#include "GUI/attributetablemodel.h"

#include <QFileDialog>
#include <QMdiSubWindow>
#include <QDebug>
#include <QClipboard>
#include <QStatusBar>
#include <QProgressBar>
#include <QPushButton>




MedeaWindow::MedeaWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MedeaWindow)
{
    ui->setupUi(this);

    currentOperationBar = new QProgressBar(this);
    currentOperationBar->setMaximum(100);

    statusBar()->addPermanentWidget(currentOperationBar);



    selectedProject = 0;

    connect(ui->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this , SLOT(projectWindowSelected(QMdiSubWindow*)));

    connect(ui->actionTile, SIGNAL(triggered()), ui->mdiArea, SLOT(tileSubWindows()));
    connect(ui->actionCascade, SIGNAL(triggered()), ui->mdiArea, SLOT(cascadeSubWindows()));
    connect(ui->actionClose_All, SIGNAL(triggered()), ui->mdiArea, SLOT(closeAllSubWindows()));

    connect(ui->clearFilters, SIGNAL(clicked()), this, SLOT(clearFilters()));
    connect(ui->addFilter, SIGNAL(clicked()), this, SLOT(appendFilter()));

    ui->actionNew_Project->trigger();


    QFile file("C:/asd12.graphml");

    if(!file.open(QFile::ReadOnly | QFile::Text)){
        qDebug() << "could not open file for read";
    }

    QTextStream in(&file);
    QString xmlText = in.readAll();
    file.close();

    this->setSelectedProject(projectWindows[0]);

    emit view_ActionTriggered("Loading XME");
    emit view_PasteData(xmlText);

    updateProgressBar(100,"");
    //ui->actionImport_GraphML->trigger();
}

MedeaWindow::~MedeaWindow()
{
    delete ui;
}

void MedeaWindow::updateFilterButtons(QStringList activeFilters)
{

    for (int i = 0; i < ui->AppliedFilters->count(); i++) {
        QWidget *w = ui->AppliedFilters->itemAt(i)->widget();
        w->setVisible(false);
        ui->AppliedFilters->removeWidget(w);
        delete w;
    }

    foreach(QString filter, activeFilters){
        QPushButton* newFilter = new QPushButton(this);
        newFilter->setText(filter);
        ui->AppliedFilters->addWidget(newFilter);
    }

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
        NewController* controller = selectedProject->getController();
        if(controller){
            controller->view_ImportGraphML(fileData, 0);
        }else{
            qCritical() << "No Controller";
        }
    }else{
        qCritical() << "No Active Window";
    }

   // emit view_ImportGraphML(fileData);

}

void MedeaWindow::on_actionNew_Project_triggered()
{
    //Add a new Window.
    ProjectWindow* newWindow = new ProjectWindow(ui->mdiArea);
    projectWindows.append(newWindow);

    connect(newWindow, SIGNAL(destroyed(QObject*)), this, SLOT(windowClosed(QObject*)));

    NewController* controller = newWindow->getController();
    NodeView* view = newWindow->getView();
    connect(view, SIGNAL(view_SetSelectedAttributeModel(AttributeTableModel*)), this, SLOT(setAttributeModel(AttributeTableModel*)));
}

void MedeaWindow::windowClosed(QObject *window)
{
    int position = projectWindows.indexOf((ProjectWindow*)window);
    projectWindows.removeAt(position);
    qCritical() << "Window: " << position << " Closed!";
}

void MedeaWindow::appendFilter()
{
    QString data = ui->searchBox->text();

    if(data != ""){
        emit view_AddFilter(data);
    }


}

void MedeaWindow::clearFilters()
{
    emit view_ClearFilters();
    ui->filterBox->update();
}

void MedeaWindow::projectWindowSelected(QMdiSubWindow *window)
{
    selectedProject  = dynamic_cast<ProjectWindow*>(window);
    setSelectedProject(selectedProject);
}

void MedeaWindow::setAttributeModel(AttributeTableModel* model)
{
    ui->attributeTable->setModel(model);
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

void MedeaWindow::setSelectedProject(ProjectWindow *newSelected)
{

    if(selectedProject){
        NewController* controller = selectedProject->getController();
        disconnect(controller, SIGNAL(view_UpdateUndoList(QStringList)), this, SLOT(updateUndoStates(QStringList)));
        disconnect(controller, SIGNAL(view_UpdateRedoList(QStringList)), this, SLOT(updateRedoStates(QStringList)));

        disconnect(controller, SIGNAL(view_updateCopyBuffer(QString)), this, SLOT(setClipboard(QString)));

        disconnect(this, SIGNAL(view_PasteData(QString)), controller, SLOT(view_Paste(QString)));
        disconnect(this, SIGNAL(view_ActionTriggered(QString)), controller, SLOT(view_ActionTriggered(QString)));

        disconnect(ui->actionMaximize, SIGNAL(triggered()), selectedProject, SLOT(showMaximized()));
        disconnect(ui->actionUndo, SIGNAL(triggered()), controller, SLOT(view_Undo()));
        disconnect(ui->actionRedo, SIGNAL(triggered()), controller, SLOT(view_Redo()));
        disconnect(ui->actionCut, SIGNAL(triggered()), controller, SLOT(view_Cut()));
        disconnect(ui->actionCopy, SIGNAL(triggered()), controller, SLOT(view_Copy()));
        disconnect(ui->actionExport_GraphML, SIGNAL(triggered()), controller, SLOT(view_ExportGraphML()));

        disconnect(this, SIGNAL(view_AddFilter(QString)), selectedProject, SLOT(appendFilterString(QString)));
        disconnect(this, SIGNAL(view_ClearFilters()), selectedProject, SLOT(clearFilters()));

        disconnect(controller, SIGNAL(view_SetGUIEnabled(bool)), this, SLOT(setEnableGUI(bool)));
        disconnect(controller, SIGNAL(view_SetSelectedAttributeModel(AttributeTableModel*)), this, SLOT(setAttributeModel(AttributeTableModel*)));
        disconnect(selectedProject, SIGNAL(updateFilters(QStringList)), this, SLOT(updateFilterButtons(QStringList)));

    }

    selectedProject = newSelected;

    if(selectedProject){
        connect(ui->actionMaximize, SIGNAL(triggered()), selectedProject, SLOT(showMaximized()));
        connect(selectedProject, SIGNAL(updateFilters(QStringList)), this, SLOT(updateFilterButtons(QStringList)));
        NewController* controller = selectedProject->getController();


        connect(this, SIGNAL(view_AddFilter(QString)), selectedProject, SLOT(appendFilterString(QString)));

        connect(this, SIGNAL(view_ClearFilters()), selectedProject, SLOT(clearFilters()));



        connect(this, SIGNAL(view_FilterNodes(QStringList)), controller, SLOT(view_FilterNodes(QStringList)));

        connect(controller, SIGNAL(view_UpdateUndoList(QStringList)), this, SLOT(updateUndoStates(QStringList)));
        connect(controller, SIGNAL(view_UpdateRedoList(QStringList)), this, SLOT(updateRedoStates(QStringList)));
        connect(controller, SIGNAL(view_updateCopyBuffer(QString)), this, SLOT(setClipboard(QString)));

        connect(controller, SIGNAL(view_UpdateProgressBar(int,QString)), this, SLOT(updateProgressBar(int,QString)));
        connect(ui->actionUndo, SIGNAL(triggered()), controller, SLOT(view_Undo()));
        connect(ui->actionRedo, SIGNAL(triggered()), controller, SLOT(view_Redo()));
        connect(ui->actionCut, SIGNAL(triggered()), controller, SLOT(view_Cut()));
        connect(ui->actionCopy, SIGNAL(triggered()), controller, SLOT(view_Copy()));

        connect(this, SIGNAL(view_ActionTriggered(QString)), controller, SLOT(view_ActionTriggered(QString)));
        connect(this, SIGNAL(view_PasteData(QString)), controller, SLOT(view_Paste(QString)));
        connect(ui->actionExport_GraphML, SIGNAL(triggered()), controller, SLOT(view_ExportGraphML()));


        connect(controller, SIGNAL(view_SetSelectedAttributeModel(AttributeTableModel*)), this, SLOT(setAttributeModel(AttributeTableModel*)));
        connect(controller, SIGNAL(view_SetGUIEnabled(bool)), this, SLOT(setEnableGUI(bool)));
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
    if(clipboard->ownsClipboard()){
        emit view_PasteData(clipboard->text());
    }
}

void MedeaWindow::setEnableGUI(bool enable)
{
    qCritical() << "SET ENABLED" << enable;

    if(selectedProject){
        selectedProject->setEnabled(enable);
    }

    ui->attributeTable->setEnabled(enable);
    ui->menubar->setEnabled(enable);




}
