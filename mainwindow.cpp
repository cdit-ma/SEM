#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QThread>
#include <QObject>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    model = 0;
    modelThread = new QThread();
previousParent = 0;
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    //setDragMode(RubberBandDrag);
    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
    //setDragMode()

     ui->graphicsView->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

      ui->graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    //Set-up the view
     ui->graphicsView->setSceneRect(0, 0, 2000, 2000);


    progressDialog = new QProgressDialog(this);
    progressDialog->setAutoClose(false);

    //Connect Enable
    connect(this, SIGNAL(init_enableGUI(bool)), this->ui->DebugOuputText,SLOT(setEnabled(bool)));
    connect(this, SIGNAL(init_enableGUI(bool)), this->ui->menubar,SLOT(setEnabled(bool)));
    connect(this, SIGNAL(init_enableGUI(bool)), this->ui->centralwidget,SLOT(setEnabled(bool)));
    connect(this, SIGNAL(init_enableGUI(bool)), this->ui->pushButton,SLOT(setEnabled(bool)));
    connect(this, SIGNAL(init_enableGUI(bool)), this->ui->pushButton_2,SLOT(setEnabled(bool)));

    connect(ui->lineEdit,SIGNAL(textChanged(QString)),this, SLOT(updateText(QString)));
    createNewModel();
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::recieveMessage(QString value)
{
    this->ui->DebugOuputText->append(value);
}

void MainWindow::enableGUI(bool enabled)
{
    emit init_enableGUI(enabled);
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

void MainWindow::setNodeSelected(NodeItem *node)
{
    ui->DebugOuputText->append(node->node->getDataValue("label"));
    ui->DebugOuputText->append(QString("Edges: %1 ").arg(node->node->getEdges().size()));

    ui->graphicsView->repaint();

    currentSelected = node;
    ui->lineEdit->setText(node->node->getDataValue("label"));

}

void MainWindow::deselectNode(QObject *obj)
{
    if((NodeItem*) obj == currentSelected){
        currentSelected = 0;
    }
}

void MainWindow::makeNode(Node *node)
{
    if(node->getDataValue("kind") == "ComponentAssembly" ||node->getDataValue("kind") == "ComponentInstance" || node->getDataValue("kind") == "InEventPort" || node->getDataValue("kind") == "OutEventPort"){

        NodeItem* parent = 0;
        /*
        if(previousParent->node->isAncestorOf(node)){
            parent = previousParent;
        }
        */

        NodeItem* nodeMade = new NodeItem(node, parent);

        hash[node] = nodeMade;

        connect(nodeMade, SIGNAL(setSelected(NodeItem*)), this, SLOT(setNodeSelected(NodeItem*)));
        connect(nodeMade, SIGNAL(exportSelected(Node*)), this, SLOT(exportNodeSelected(Node*)));

        connect(nodeMade, SIGNAL(destroyed(QObject*)), this,SLOT(deselectNode(QObject*)));
        //connect(qi, SIGNAL(qi->

        scene->addItem((QGraphicsItem *)nodeMade);

        //previousParent = nodeMade;
    }
}

void MainWindow::exportNodeSelected(Node * node)
{
    this->ui->DebugOuputText->clear();

    ui->DebugOuputText->append(node->toGraphML());

    qCritical() << "\n\n";
    qCritical() << "Starting Parsing!";
    delete node;

}

void MainWindow::deleteComponent(GraphMLContainer *graph)
{
    if(hash.contains((Node *)graph)){
        this->ui->DebugOuputText->append("Deleting GUI");
        NodeItem* i = hash[(Node *)graph];
        delete i;

        this->ui->DebugOuputText->append("Deleted...");
    }else{
        this->ui->DebugOuputText->append("=(");

    }
}

void MainWindow::updateText(QString data)
{
    try{
        if(currentSelected != 0){
            emit currentSelected->updateData("label", data);
        }
    }catch(...){
        currentSelected = 0 ;
    }

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

        qDebug() << "Reading in file: " << files.at(i);
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

    qCritical() << "TEST";
     emit init_ImportGraphML(fileData, NULL);
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

void MainWindow::createNewModel()
{
    if(model != 0){
       model->~Model();
    }
    model = new Model();

    //Connect to Models Signals
    connect(model, SIGNAL(enableGUI(bool)), this, SLOT(enableGUI(bool)));

    //Progress Dialog Signals
    connect(model, SIGNAL(progressDialog_Hide()), progressDialog, SLOT(hide()));
    connect(model, SIGNAL(progressDialog_Show()), progressDialog, SLOT(show()));
    connect(model, SIGNAL(progressDialog_SetValue(int)), progressDialog, SLOT(setValue(int)));
    connect(model, SIGNAL(progressDialog_SetText(QString)), progressDialog, SLOT(setLabelText(QString)));
    connect(model, SIGNAL(setComponentCount(int)), this->ui->componentCount, SLOT(display(int)));

    //Returned Data Signals
    connect(model, SIGNAL(returnExportedGraphMLData(QString, QString)), this, SLOT(writeExportedGraphMLData(QString,QString)));

    //connect(model, SIGNAL(removeUIComponent(GraphMLContainer*)),this, SLOT(deleteComponent(GraphMLContainer*)));


    connect(model, SIGNAL(constructNodeItem(Node*)), this, SLOT(makeNode(Node*)));
    //Connect to Models Slots
    connect(this, SIGNAL(init_ImportGraphML(QStringList , GraphMLContainer *)), model, SLOT(init_ImportGraphML(QStringList , GraphMLContainer*)));
    connect(this, SIGNAL(init_ExportGraphML(QString)), model, SLOT(init_ExportGraphML(QString)));



    model->moveToThread(this->modelThread);
    modelThread->start();

}

