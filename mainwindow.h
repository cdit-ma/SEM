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
#include "GUI/nodeitem.h"
#include "GUI/nodeconnection.h"
#include "Controller/graphmlcontroller.h"
#include <Vector>

#include <stdio.h>

#include <QStringList>

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

    void enableGUI(bool enabled);

    void updateProgressBar(int percentage, QString label);

    void writeExportedGraphMLData(QString filename, QString data);
    void updateZoom(qreal zoom);
signals:
    void init_enableGUI(bool enabled);

    void removeComponent(GraphMLContainer* v);
    void init_ImportGraphML(QStringList inputGraphML);
    void init_ExportGraphML(QString file);
    void Controller_Paste(QString data);

private slots:
    void on_actionImport_GraphML_triggered();

    void on_actionExport_GraphML_triggered();

    void on_actionExit_triggered();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void copyText(QString value);

    void on_pushButton_4_clicked();


private:
    bool CONTROL_DOWN;
    bool SHIFT_DOWN;
    void createNewModel();
    GraphMLController* controller;
    Model* model;
    NodeItem *previousParent;
    QThread *modelThread;
    Ui::MainWindow *ui;
    QProgressDialog *progressDialog;
    //Model *model;
    QGraphicsScene *scene;
    QVector<NodeItem*> currentSelectedItems;
    NodeItem* currentSelected;
    QHash<Node *, NodeItem*> hash;

    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent *);
};

#endif // MAINWINDOW_H
