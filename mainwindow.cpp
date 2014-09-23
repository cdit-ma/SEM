#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QThread>
#include <QObject>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    //Make a thread for the Model.
    modelThread = new QThread();
    controllerThread = new QThread();

    controller = 0;
    model = 0;

    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    CONTROL_DOWN = false;
    SHIFT_DOWN = false;

    QStringList kindsToMake;
    kindsToMake << "ComponentAssembly" << "ComponentInstance" << "InEventPort" << "OutEventPort" << "Attribute" << "HardwareNode" << "HardwareCluster";

    ui->constructBox->addItems(kindsToMake);

    progressDialog = new QProgressDialog(this);
    progressDialog->setAutoClose(false);

    //Connect Enable
    connect(this, SIGNAL(init_enableGUI(bool)), this->ui->DebugOuputText,SLOT(setEnabled(bool)));
    connect(this, SIGNAL(init_enableGUI(bool)), this->ui->menubar,SLOT(setEnabled(bool)));
    connect(this, SIGNAL(init_enableGUI(bool)), this->ui->centralwidget,SLOT(setEnabled(bool)));
    connect(this, SIGNAL(init_enableGUI(bool)), this->ui->pushButton,SLOT(setEnabled(bool)));
    connect(this, SIGNAL(init_enableGUI(bool)), this->ui->graphicsView,SLOT(setEnabled(bool)));

   // connect(ui->lineEdit,SIGNAL(textChanged(QString)),this, SLOT(updateText(QString)));





    createNewModel();

    QFile file("C:/asd12.graphml");
    if(!file.open(QFile::ReadOnly | QFile::Text)){
        qDebug() << "could not open file for read";
    }
    QTextStream in(&file);
    QString xmlText = in.readAll();
    file.close();


    QStringList file2;
    file2 << xmlText;


    emit init_ImportGraphML(file2);
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::enableGUI(bool enabled)
{
    emit init_enableGUI(enabled);
}

void MainWindow::updateProgressBar(int percentage, QString label)
{
    if(label != 0){
        progressDialog->setLabelText(label);
    }
    progressDialog->setValue(percentage);

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
        ui->pushButton_6->setEnabled(false);
    }else{
        ui->pushButton_6->setEnabled(true);
    }
    ui->pushButton_6->setText(QString("Undo [%1]").arg(list.size()));

    QStandardItemModel*  undoModel = new QStandardItemModel(this);

    foreach(QString item, list){
        QStandardItem* undoItem = new QStandardItem(item);
        undoItem->setFlags(undoItem->flags() & ~Qt::ItemIsEditable);
        undoModel->insertRow(0,undoItem);

    }

    ui->UndoListView->setModel(undoModel);

}


void MainWindow::updateRedoCount(QStringList list)
{
    if(list.size() == 0){
        ui->pushButton_7->setEnabled(false);
    }else{
        ui->pushButton_7->setEnabled(true);
    }

    ui->pushButton_7->setText(QString("Redo [%1]").arg(list.size()));

    QStandardItemModel*  redoModel = new QStandardItemModel(this);

    foreach(QString item, list){
        QStandardItem* redoItem = new QStandardItem(item);
        redoItem->setFlags(redoItem->flags() & ~Qt::ItemIsEditable);
        redoModel->insertRow(0,redoItem);

    }

    ui->RedoListView->setModel(redoModel);


}

void MainWindow::setModel(AttributeTableModel *model)
{
    this->ui->tableView->setModel(model);
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

        qCritical() << "Reading in file: " << files.at(i);
        QFile file(files.at(i));
        if(!file.open(QFile::ReadOnly | QFile::Text)){
            qDebug() << "could not open file for read";
        }

        try{
            QTextStream in(&file);
            QString xmlText = in.readAll();
            file.close();

            qDebug() << "Importing text from file: " << files.at(i);

            fileData << xmlText;
            qDebug() << "Loaded: " << files.at(i) << "Successfully!";
        }catch(...){
            qCritical() << "Error Loading: " << files.at(i);
        }
    }

    emit init_ImportGraphML(fileData);
}

void MainWindow::on_actionExport_GraphML_triggered()
{

    QString filename = QFileDialog::getSaveFileName(
                this,
                "Export .graphML",
                "c:\\",
                "GraphML Documents (*.graphml *.xml)");




    emit init_ExportGraphML(filename);
}

void MainWindow::on_actionExit_triggered()
{
    this->~MainWindow();
}

void MainWindow::on_pushButton_clicked()
{
    this->ui->DebugOuputText->clear();

}

void MainWindow::on_pushButton_2_clicked()
{
    createNewModel();
}

void MainWindow::copyText(QString value)
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(value);
}

void MainWindow::createNewModel()
{
    if(controller != 0){
        delete controller;
        ui->graphicsView->clearView();
    }
    controller = new GraphMLController(ui->graphicsView);

    //this->ui->treeView->setModel(controller->getTreeModel());
    this->ui->treeView->setModel(  controller->getTreeModel());
    //Connect to Models Signals

    connect(ui->constructBox, SIGNAL(currentTextChanged(QString)), controller, SLOT(view_SetChildNodeType(QString)));

    connect(ui->graphicsView, SIGNAL(paste()), this, SLOT(on_pushButton_4_clicked()));

    connect(controller, SIGNAL(view_EnableGUI(bool)), this, SLOT(enableGUI(bool)));

    connect(this,SIGNAL(init_ExportGraphML(QString)),controller, SLOT(view_ExportGraphML(QString)));
    connect(this,SIGNAL(init_ImportGraphML(QStringList)),controller, SLOT(view_ImportGraphML(QStringList)));

    connect(controller, SIGNAL(view_UndoCommandList(QStringList)), this, SLOT(updateUndoCount(QStringList)));
    connect(controller, SIGNAL(view_RedoCommandList(QStringList)), this, SLOT(updateRedoCount(QStringList)));

    //connect(ui->treeView, SIGNAL(doubleClicked(QModelIndex)), controller, SLOT(view_SetCentered(QModelIndex)));
    //connect(ui->treeView, SIGNAL(pressed (QModelIndex)), controller, SLOT(view_SetSelected(QModelIndex)));


    //connect(ui->lineEdit,SIGNAL(textChanged(QString)), controller, SLOT(view_UpdateLabel(QString)));

    //Progress Dialog Signals
    //connect(model, SIGNAL(currentAction_ShowProgress(bool)), progressDialog, SLOT(setVisible(bool)));
   //connect(model,SIGNAL(currentAction_UpdateProgress(int,QString)), this, SLOT(updateProgressBar(int,QString)));
    connect(ui->pushButton_3, SIGNAL(clicked()), controller, SLOT(view_Copy()));
    connect(ui->pushButton_5, SIGNAL(clicked()), controller, SLOT(view_Cut()));

    connect(controller, SIGNAL(view_CopyText(QString)), this, SLOT(copyText(QString)));
    connect(this, SIGNAL(Controller_Paste(QString)), controller, SLOT(view_Paste(QString)));

    connect(ui->pushButton_6, SIGNAL(clicked()), controller, SLOT(view_Undo()));
    connect(ui->pushButton_7, SIGNAL(clicked()), controller, SLOT(view_Redo()));

    connect(this, SIGNAL(actionTriggered(QString)), controller, SLOT(view_ActionTriggered(QString)));


    connect(ui->verticalSlider, SIGNAL(valueChanged(int)), ui->graphicsView, SLOT(depthChanged(int)));

    connect(controller, SIGNAL(view_SetAttributeModel(AttributeTableModel*)), this, SLOT(setModel(AttributeTableModel*)));


    controller->getModel()->moveToThread(this->modelThread);
    modelThread->start();



}






void MainWindow::on_pushButton_4_clicked()
{
    QClipboard *clipboard = QApplication::clipboard();
    if(clipboard->ownsClipboard()){
        emit Controller_Paste(clipboard->text());
    }
}



void MainWindow::on_constructBox_highlighted(const QString &arg1)
{
    currentSet = arg1;
}

void MainWindow::on_verticalSlider_actionTriggered(int action)
{

}
