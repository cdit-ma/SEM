#include "medeawindow.h"
#include "ui_medeawindow.h"

#include "GUI/nodeitem.h"
#include "GUI/nodeconnection.h"
#include "GUI/attributetablemodel.h"

#include <QFileDialog>
#include <QMdiSubWindow>
#include <QDebug>
#include <QClipboard>




MedeaWindow::MedeaWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MedeaWindow)
{
    ui->setupUi(this);

    selectedProject = 0;

    connect(ui->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this , SLOT(projectWindowSelected(QMdiSubWindow*)));

    connect(ui->actionTile, SIGNAL(triggered()), ui->mdiArea, SLOT(tileSubWindows()));
    connect(ui->actionCascade, SIGNAL(triggered()), ui->mdiArea, SLOT(cascadeSubWindows()));
    connect(ui->actionClose_All, SIGNAL(triggered()), ui->mdiArea, SLOT(closeAllSubWindows()));

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

    //ui->actionImport_GraphML->trigger();

}

MedeaWindow::~MedeaWindow()
{
    delete ui;
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
    connect(controller, SIGNAL(view_SetSelectedAttributeModel(AttributeTableModel*)), this, SLOT(setAttributeModel(AttributeTableModel*)));
}

void MedeaWindow::windowClosed(QObject *window)
{
    int position = projectWindows.indexOf((ProjectWindow*)window);
    projectWindows.removeAt(position);
    qCritical() << "Window: " << position << " Closed!";
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


    }

    selectedProject = newSelected;

    if(selectedProject){
        connect(ui->actionMaximize, SIGNAL(triggered()), selectedProject, SLOT(showMaximized()));

        NewController* controller = selectedProject->getController();


        connect(controller, SIGNAL(view_UpdateUndoList(QStringList)), this, SLOT(updateUndoStates(QStringList)));
        connect(controller, SIGNAL(view_UpdateRedoList(QStringList)), this, SLOT(updateRedoStates(QStringList)));
        connect(controller, SIGNAL(view_updateCopyBuffer(QString)), this, SLOT(setClipboard(QString)));

        connect(ui->actionUndo, SIGNAL(triggered()), controller, SLOT(view_Undo()));
        connect(ui->actionRedo, SIGNAL(triggered()), controller, SLOT(view_Redo()));
        connect(ui->actionCut, SIGNAL(triggered()), controller, SLOT(view_Cut()));
        connect(ui->actionCopy, SIGNAL(triggered()), controller, SLOT(view_Copy()));

        connect(this, SIGNAL(view_ActionTriggered(QString)), controller, SLOT(view_ActionTriggered(QString)));
        connect(this, SIGNAL(view_PasteData(QString)), controller, SLOT(view_Paste(QString)));
        connect(ui->actionExport_GraphML, SIGNAL(triggered()), controller, SLOT(view_ExportGraphML()));

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
