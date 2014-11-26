#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QThread>
#include <QObject>
#include <QClipboard>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //Setup the GUI
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);



    //Make a thread for the Model.
    modelThread = new QThread();

    //Set the Controller and Model to 0
    newController = 0;

    view_resetModel();


    //Add the Components which can be constructed in this canvas to the drop down box.
    ui->constructBox->addItems(newController->getNodeKinds());

    ui->aspectBox->addItems(newController->getViewAspects());


    //Connect Enable GUI flags.
    connect(ui->actionClear, SIGNAL(triggered()), this, SLOT(view_resetModel()));
    connect(this, SIGNAL(view_EnableGUI(bool)), this->ui->DebugOuputText,SLOT(setEnabled(bool)));
    connect(this, SIGNAL(view_EnableGUI(bool)), this->ui->menubar,SLOT(setEnabled(bool)));
    connect(this, SIGNAL(view_EnableGUI(bool)), this->ui->centralwidget,SLOT(setEnabled(bool)));
    connect(this, SIGNAL(view_EnableGUI(bool)), this->ui->pushButton,SLOT(setEnabled(bool)));
    connect(this, SIGNAL(view_EnableGUI(bool)), this->ui->graphicsView,SLOT(setEnabled(bool)));




    //Load a File.

    QFile file("C:/asd12.graphml");

    if(!file.open(QFile::ReadOnly | QFile::Text)){
        qDebug() << "could not open file for read";
    }

    QTextStream in(&file);
    QString xmlText = in.readAll();
    file.close();

    newController->view_TriggerAction("Loading XME");
    //emit view_ImportGraphML(xmlText);
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::enableGUI(bool enabled)
{
    emit view_EnableGUI(enabled);
}

void MainWindow::updateProgressBar(int percentage, QString label)
{

    /*
    if(label != 0){
        progressDialog->setLabelText(label);
    }
    progressDialog->setValue(percentage);
*/
}

void MainWindow::writeExportedGraphMLData(QString filename, QString data)
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

void MainWindow::updateZoom(qreal zoom)
{
    int value=0;
    if(zoom < .5){
        value = 1;
    }else if(zoom < 1){
        value = 2;
    }else if(zoom <1.5){
        value = 3;
    }else if(zoom < 2){
        value = 4;
    }else{
        value = 5;
    }

    ui->verticalSlider->setValue(value);
}

void MainWindow::updateUndoCount(QStringList list)
{
    if(list.size() == 0){
        ui->actionUndo->setEnabled(false);
    }else{
        ui->actionUndo->setEnabled(true);
    }
    ui->actionUndo->setText(QString("Undo [%1]").arg(list.size()));

    QStandardItemModel*  undoModel = new QStandardItemModel(this);

    foreach(QString item, list){
        QStandardItem* undoItem = new QStandardItem(item);
        undoItem->setFlags(undoItem->flags() & ~Qt::ItemIsEditable);
        undoModel->insertRow(0,undoItem);

    }

    //ui->UndoListView->setModel(undoModel);

}


void MainWindow::updateRedoCount(QStringList list)
{
    if(list.size() == 0){
        ui->actionRedo->setEnabled(false);
    }else{
        ui->actionRedo->setEnabled(true);
    }

    ui->actionRedo->setText(QString("Redo [%1]").arg(list.size()));

    QStandardItemModel*  redoModel = new QStandardItemModel(this);

    foreach(QString item, list){
        QStandardItem* redoItem = new QStandardItem(item);
        redoItem->setFlags(redoItem->flags() & ~Qt::ItemIsEditable);
        redoModel->insertRow(0,redoItem);

    }

    //ui->RedoListView->setModel(redoModel);


}

void MainWindow::setModel(AttributeTableModel *model)
{
   // this->ui->tableView->setModel(model);
}


void MainWindow::on_actionImport_GraphML_triggered()
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

    emit view_ImportGraphML(fileData);
}

void MainWindow::on_actionExport_GraphML_triggered()
{

    QString filename = QFileDialog::getSaveFileName(
                this,
                "Export .graphML",
                "c:\\",
                "GraphML Documents (*.graphml *.xml)");




    emit view_ExportGraphML(filename);
}

void MainWindow::on_actionExit_triggered()
{
    this->~MainWindow();
}


void MainWindow::view_CopyData(QString value)
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(value);
}



void MainWindow::on_actionPaste_triggered()
{
    QClipboard *clipboard = QApplication::clipboard();
    if(clipboard->ownsClipboard()){
        emit view_PasteData(clipboard->text());
    }

}

void MainWindow::view_resetModel()
{
    if(newController){
        delete newController;
    }

    newController = new NewController(ui->graphicsView);

    connect(ui->actionUndo, SIGNAL(triggered()), newController, SLOT(view_Undo()));
    connect(ui->actionRedo, SIGNAL(triggered()), newController, SLOT(view_Redo()));
    connect(ui->actionCopy, SIGNAL(triggered()), newController, SLOT(view_Copy()));
    connect(ui->actionCut, SIGNAL(triggered()), newController, SLOT(view_Cut()));
    connect(this, SIGNAL(view_PasteData(QString)), newController, SLOT(view_Paste(QString)));


    connect(ui->actionClearHistory, SIGNAL(triggered()), newController, SLOT(view_ClearHistory()));

    connect(ui->constructBox, SIGNAL(currentTextChanged(QString)), newController, SLOT(view_SetChildNodeKind(QString)));
    connect(ui->actionExport_GraphML, SIGNAL(triggered()), newController, SLOT(view_ExportGraphML()));
    connect(newController, SIGNAL(view_ExportGraphML(QString)), this, SLOT(view_CopyData(QString)));
    connect(this, SIGNAL(view_ImportGraphML(QString)), newController, SLOT(view_ImportGraphML(QString)));
    connect(this, SIGNAL(view_ImportGraphML(QStringList)), newController, SLOT(view_ImportGraphML(QStringList)));

    connect(newController, SIGNAL(view_SetSelectedAttributeModel(AttributeTableModel*)), this, SLOT(setModel(AttributeTableModel*)));

    connect(newController, SIGNAL(view_UpdateUndoList(QStringList)), this, SLOT(updateUndoCount(QStringList)));
    connect(newController, SIGNAL(view_UpdateRedoList(QStringList)), this, SLOT(updateRedoCount(QStringList)));

    connect(newController, SIGNAL(view_updateCopyBuffer(QString)), this, SLOT(view_CopyData(QString)));

    //this->ui->treeView->setModel(controller->getTreeModel());

    /*

    //Connect to Models Signals

    connect(ui->constructBox, SIGNAL(currentTextChanged(QString)), controller, SLOT(view_SetChildNodeType(QString)));
    connect(ui->aspectBox, SIGNAL(currentTextChanged(QString)), controller, SLOT(view_SetViewAspect(QString)));


    connect(controller, SIGNAL(view_EnableGUI(bool)), this, SLOT(enableGUI(bool)));

    connect(this,SIGNAL(view_ExportGraphML(QString)),controller, SLOT(view_ExportGraphML(QString)));
    //connect(this,SIGNAL(view_ImportGraphML(QStringList)),controller, SLOT(view_ImportGraphML(QStringList)));
   // connect(this,SIGNAL(view_ImportGraphML(QStringList)),controller, SLOT(view_ImportGraphML(QStringList)));
    //connect(this,SIGNAL(view_ImportGraphML(QString)),controller, SLOT(view_ImportGraphML(QString)));



    connect(controller, SIGNAL(view_UndoCommandList(QStringList)), this, SLOT(updateUndoCount(QStringList)));
    connect(controller, SIGNAL(view_RedoCommandList(QStringList)), this, SLOT(updateRedoCount(QStringList)));

    connect(ui->treeView, SIGNAL(doubleClicked(QModelIndex)), controller, SLOT(view_SetCentered(QModelIndex)));
    connect(ui->treeView, SIGNAL(pressed (QModelIndex)), controller, SLOT(view_SetSelected(QModelIndex)));



    //Progress Dialog Signals
    //connect(model, SIGNAL(currentAction_ShowProgress(bool)), progressDialog, SLOT(setVisible(bool)));
    //connect(model,SIGNAL(currentAction_UpdateProgress(int,QString)), this, SLOT(updateProgressBar(int,QString)));

    connect(ui->actionCopy, SIGNAL(triggered()), controller, SLOT(view_Copy()));
    connect(ui->actionCut, SIGNAL(triggered()), controller, SLOT(view_Cut()));

    connect(ui->actionUndo, SIGNAL(triggered()), controller, SLOT(view_Undo()));
    connect(ui->actionRedo, SIGNAL(triggered()), controller, SLOT(view_Redo()));

    connect(ui->actionMake_Instance,SIGNAL(triggered()), controller, SLOT(view_MakeInstance()));

    connect(controller, SIGNAL(view_CopyData(QString)), this, SLOT(view_CopyData(QString)));
    connect(this, SIGNAL(view_PasteData(QString)), controller, SLOT(view_Paste(QString)));

    connect(ui->pushButton_6, SIGNAL(clicked()), controller, SLOT(view_Undo()));
    connect(ui->pushButton_7, SIGNAL(clicked()), controller, SLOT(view_Redo()));

    connect(this, SIGNAL(actionTriggered(QString)), controller, SLOT(view_ActionTriggered(QString)));



    connect(controller, SIGNAL(view_SetAttributeModel(AttributeTableModel*)), this, SLOT(setModel(AttributeTableModel*)));


    connect(ui->verticalSlider, SIGNAL(valueChanged(int)), ui->graphicsView, SLOT(depthChanged(int)));


    controller->getModel()->moveToThread(this->modelThread);
    modelThread->start();

    */
    emit ui->aspectBox->currentTextChanged("Assembly");

}

void MainWindow::on_actionMake_Instance_triggered()
{


}
