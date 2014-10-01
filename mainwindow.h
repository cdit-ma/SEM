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
#include "Model/Assembly/hardwarenode.h"
#include "Model/Assembly/componentinstance.h"
#include "Model/Assembly/componentassembly.h"
#include "Model/Assembly/ineventportidl.h"
#include "Model/Assembly/outeventportidl.h"
#include "GUI/nodeitem.h"
#include "GUI/nodeconnection.h"
#include "Controller/graphmlcontroller.h"
#include "Controller/newcontroller.h"
#include <Vector>
#include <QStack>

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

    void updateUndoCount(QStringList list);
    void updateRedoCount(QStringList list);

    void setModel(AttributeTableModel* model);
signals:
    void view_EnableGUI(bool enabled);

    void view_ImportGraphML(QStringList inputGraphML);
    void view_ImportGraphML(QString inputGraphML);
    void view_ExportGraphML(QString file);


    void view_PasteData(QString data);

    void actionTriggered(QString action);

private slots:
    void on_actionImport_GraphML_triggered();

    void on_actionExport_GraphML_triggered();

    void on_actionExit_triggered();

    void view_CopyData(QString value);

    void on_actionPaste_triggered();

    void view_resetModel();
    void on_actionMake_Instance_triggered();

private:
    //GraphMLController* controller;

    NewController* newController;

    QThread *modelThread;

    Ui::MainWindow *ui;
    QProgressDialog *progressDialog;

    QGraphicsScene *scene;
    QVector<NodeItem*> currentSelectedItems;
    NodeItem* currentSelected;
    QHash<Node *, NodeItem*> hash;

    // QWidget interface
};

#endif // MAINWINDOW_H
