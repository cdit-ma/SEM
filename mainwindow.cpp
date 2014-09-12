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


    progressDialog = new QProgressDialog(this);
    progressDialog->setAutoClose(false);

    //Connect Enable
    connect(this, SIGNAL(init_enableGUI(bool)), this->ui->DebugOuputText,SLOT(setEnabled(bool)));
    connect(this, SIGNAL(init_enableGUI(bool)), this->ui->menubar,SLOT(setEnabled(bool)));
    connect(this, SIGNAL(init_enableGUI(bool)), this->ui->centralwidget,SLOT(setEnabled(bool)));
    connect(this, SIGNAL(init_enableGUI(bool)), this->ui->pushButton,SLOT(setEnabled(bool)));
    connect(this, SIGNAL(init_enableGUI(bool)), this->ui->pushButton_2,SLOT(setEnabled(bool)));
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
/*
void MainWindow::setNodeSelected(NodeItem *node)
{
    if(SHIFT_DOWN){
        //Start Connecting.
        if(currentSelectedItems.size() == 1){
            if(currentSelectedItems[0]->node->isEdgeLegal(node->node)){
                ui->DebugOuputText->append("Edge Legal!");
                Edge * edge = new Edge(currentSelectedItems[0]->node, node->node);
                makeEdge(edge);

              }else{
                ui->DebugOuputText->append("Edge IlLegal!");

            }
        }


    }

    //append current Item to the list.
    if(CONTROL_DOWN){
        //If we already have this node selected, we should remove it.
        if(currentSelectedItems.contains(node)){
            deselectNode(node);
            return;
        }else{
            //Else we should append it to the list and select it.
        }
    }else{
        deselectNode(0);
    }



    //Append it and Select it.
    currentSelectedItems.append(node);
    node->setSelected();
    //ui->DebugOuputText->append(node->node->getDataValue("label"));
    //ui->DebugOuputText->append("Depth: "+QString::number(node->depth));
    ui->lineEdit->setText(node->node->getDataValue("label"));
}
 

void MainWindow::deselectNode(QObject *obj)
{
    //Deselect from the group.
    if(CONTROL_DOWN){
        NodeItem* item = dynamic_cast<NodeItem*>(obj);

        if(item != 0){
            int position = currentSelectedItems.indexOf(item);

            if(position != -1){
                currentSelectedItems[position]->setDeselected();
                currentSelectedItems.removeAt(position);
            }
        }

    }else{
     //Clear all the items
        for (int i = 0 ; i < currentSelectedItems.size(); i++){
            currentSelectedItems[i]->setDeselected();
        }
        currentSelectedItems.clear();

    }
}

void MainWindow::centreNode(NodeItem *)
{

}

void MainWindow::makeNode(Node *node){
    if(node->getDataValue("kind") == "ComponentAssembly" ||node->getDataValue("kind") == "ComponentInstance" || node->getDataValue("kind") == "InEventPort" || node->getDataValue("kind") == "OutEventPort"){


        NodeItem* parent = 0;
        if(previousParent->node->isAncestorOf(node)){
            parent = previousParent;
        }

        Node* parentNode = node->getParentNode();
        NodeItem* parentNodeItem=0;
        if(parentNode != 0){
            parentNodeItem =  hash[parentNode];
        }

        NodeItem* nodeMade = new NodeItem(node, parentNodeItem);

        hash[node] = nodeMade;

        connect(nodeMade, SIGNAL(centreNode(NodeItem*)), (NodeView*)ui->graphicsView, SLOT(centreItem(NodeItem*)));
        connect(nodeMade, SIGNAL(exportSelected(Node*)), this, SLOT(exportNodeSelected(Node*)));

        connect(nodeMade, SIGNAL(destroyed(QObject*)), this,SLOT(deselectNode(QObject*)));

        connect(nodeMade, SIGNAL(triggerSelected(NodeItem*)), this, SLOT(setNodeSelected(NodeItem*)));

        //connect(nodeMade, SIGNAL(makeChildNode(NodeItem*)),this,SLOT(makeChildNode(NodeItem*)));

        connect(nodeMade,SIGNAL(makeChildNode(Node*)),model,SLOT(constructIENode(Node*)));
        //connect(qi, SIGNAL(qi->

        connect(ui->verticalSlider, SIGNAL(valueChanged(int)),nodeMade, SLOT(toggleDetailDepth(int)));

        nodeMade->toggleDetailDepth(ui->verticalSlider->value());

        if(!scene->items().contains((QGraphicsItem*)nodeMade)){
            scene->addItem((QGraphicsItem *)nodeMade);
        }

        //previousParent = nodeMade;
    }
}

void MainWindow::makeEdge(Edge *edge)
{
    GraphMLContainer* source = edge->getSource();
    GraphMLContainer* destination = edge->getDestination();

    NodeItem* sourceItem = hash[(Node*)source];
    NodeItem* destinationItem = hash[(Node*)destination];

    if(sourceItem != 0 && destinationItem != 0){
        qCritical() << "Valid Edge!";
        NodeEdge* nC = new NodeEdge(edge,sourceItem,destinationItem);
        nC->addToScene(scene);

    }else{
        qCritical() << "Non Valid Edge!";
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

void MainWindow::shiftPressed()
{
    SHIFT_DOWN= !SHIFT_DOWN;
    ui->DebugOuputText->append(QString("Shift: %1").arg(SHIFT_DOWN));
}

void MainWindow::controlPressed()
{
    CONTROL_DOWN = !CONTROL_DOWN;
    ui->DebugOuputText->append(QString("Control: %1").arg(CONTROL_DOWN));
}

void MainWindow::deleteSelected()
{
    ui->DebugOuputText->append(QString("Current Size: %1").arg(currentSelectedItems.size()));

    for (int i = 0 ; i < currentSelectedItems.size(); i++){
        delete currentSelectedItems[i]->node;
    }
    currentSelectedItems.clear();

}

void MainWindow::makeChildNode(NodeItem *nodeItme)
{
    Node* parentNode = nodeItme->node;

    InputEventPort* ie = new InputEventPort("asd");




    parentNode->adopt(ie);


    NodeItem* nodeMade = new NodeItem((Node *)ie, nodeItme);

    hash[(Node *)ie] = nodeMade;

    connect(nodeMade, SIGNAL(triggerSelected(NodeItem*)), this, SLOT(setNodeSelected(NodeItem*)));
    connect(nodeMade, SIGNAL(exportSelected(Node*)), this, SLOT(exportNodeSelected(Node*)));

    connect(nodeMade, SIGNAL(centreNode(NodeItem*)), this, SLOT(centreNode(NodeItem*)));

    connect(nodeMade, SIGNAL(destroyed(QObject*)), this,SLOT(deselectNode(QObject*)));

    connect(nodeMade, SIGNAL(makeChildNode(NodeItem*)),this,SLOT(makeChildNode(NodeItem*)));

    //connect(qi, SIGNAL(qi->

    //scene->addItem((QGraphicsItem *)nodeMade);






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

        if(currentSelectedItems.size() == 1){
            currentSelectedItems[0]->updateData("label",data);
        }
    }catch(...){
        currentSelected = 0 ;
    }

}
*/

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

    this->ui->treeView->setModel(controller->getTreeModel());
    //Connect to Models Signals

    connect(ui->graphicsView, SIGNAL(paste()), this, SLOT(on_pushButton_4_clicked()));

    connect(controller, SIGNAL(view_EnableGUI(bool)), this, SLOT(enableGUI(bool)));

    connect(this,SIGNAL(init_ExportGraphML(QString)),controller, SLOT(view_ExportGraphML(QString)));
    connect(this,SIGNAL(init_ImportGraphML(QStringList)),controller, SLOT(view_ImportGraphML(QStringList)));


    connect(ui->treeView, SIGNAL(doubleClicked(QModelIndex)), controller, SLOT(view_SetCentered(QModelIndex)));
    connect(ui->treeView, SIGNAL(pressed (QModelIndex)), controller, SLOT(view_SetSelected(QModelIndex)));


    //connect(ui->lineEdit,SIGNAL(textChanged(QString)), controller, SLOT(view_UpdateLabel(QString)));
    connect(ui->lineEdit, SIGNAL(returnPressed()),this, SLOT(labelPressed()));
    connect(controller, SIGNAL(view_LabelChanged(QString)),  this->ui->lineEdit, SLOT(setText(QString)));

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
   // connect(ui->verticalSlider, SIGNAL(valueChanged(int)), nodeMade, SLOT(toggleDetailDepth(int)));

    //nodeMade->toggleDetailDepth(ui->verticalSlider->value());



    //ui->graphicsView->moveToThread(modelThread);

    controller->getModel()->moveToThread(this->modelThread);
   modelThread->start();


   // controller->moveToThread(controllerThread);
  // controllerThread->start();

}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Delete){
        controller->view_DeleteTriggered(true);
    }

}





void MainWindow::on_pushButton_4_clicked()
{
    QClipboard *clipboard = QApplication::clipboard();
    //if(clipboard->ownsClipboard()){
        emit Controller_Paste(clipboard->text());
    //}

}

void MainWindow::labelPressed()
{
    controller->view_UpdateLabel(ui->lineEdit->text());
}

