#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QThread>
#include <QObject>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->model= new Model(this);


    connect(this->model, SIGNAL(updatePercentage(int)), this->ui->progressBar, SLOT(setValue(int)));
    connect(this->model, SIGNAL(componentCount(int)), this->ui->componentCount, SLOT(display(int)));
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::debugPrint(QString value)
{
    this->ui->DebugOuputText->append(value);
}

void MainWindow::on_actionImport_GraphML_triggered()
{

    QStringList files = QFileDialog::getOpenFileNames(
                            this,
                            "Select one or more files to open",
                            "",
                            "GraphML Documents (*.graphml *.xml)");



    QProgressDialog *progressDialog = new QProgressDialog(this);

    connect(this->model, SIGNAL(updatePercentage(int)), progressDialog, SLOT(setValue(int)));


    for (int i = 0; i < files.size(); i++){
        progressDialog->show();
        progressDialog->setLabelText(QString("Importing GraphML file %1 / %2").arg(QString::number(i+1), QString::number(files.size())));

        qDebug() << "Reading in file: " << files.at(i);
        QFile file(files.at(i));
        if(!file.open(QFile::ReadOnly | QFile::Text)){
            qDebug() << "could not open file for read";
        }

        QTextStream in(&file);
        QString xmlText = in.readAll();
        file.close();

         qDebug() << "Importing text from file: " << files.at(i);

         bool output = model->importGraphML(xmlText);

        if(output){
            qDebug() << "Loaded: " << files.at(i) << "Successfully!";
            this->ui->componentCount->display(model->getNodeCount());
        }else{
            qCritical() << "Error Loading: " << files.at(i);
        }
    }

    //disconnect(this->model, SIGNAL(updatePercentage(int)), progressDialog, SLOT(setValue(int)));

    //destroy(progressDialog);


}

void MainWindow::on_actionExport_GraphML_triggered()
{

    QString filename = QFileDialog::getSaveFileName(
                                this,
                                "Export .graphML",
                                "c:\\",
                                "GraphML Documents (*.graphml *.xml)");




    try{
        QFile file(filename);
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&file);
        out << model->exportGraphML();
        file.close();

        qDebug()<<"Successfully written file: " <<filename;
    }catch(...){
        qCritical() <<"Export Failed!";
    }
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
    this->model->~Model();
    this->model = new Model(this);
    this->ui->componentCount->display(model->getNodeCount());
}
