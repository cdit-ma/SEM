#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QtDebug>
#include <QMainWindow>
#include <QFileDialog>
#include <QApplication>
#include <QProgressDialog>
#include <QDebug>
#include "Model/Model.h"
#include "Model/edge.h"
#include "Model/node.h"
#include "Model/graph.h"
#include "Model/hardwarenode.h"
#include "Model/componentinstance.h"
#include "Model/componentassembly.h"
#include "Model/inputeventport.h"
#include "Model/outputeventport.h"




namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);


    ~MainWindow();

public slots:
    void debugPrint(QString value);

private slots:
    void on_actionImport_GraphML_triggered();

    void on_actionExport_GraphML_triggered();

    void on_actionExit_triggered();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:


    Ui::MainWindow *ui;
    Model *model;
};

#endif // MAINWINDOW_H
