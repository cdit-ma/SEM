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
    void recieveMessage(QString value);

    void enableGUI(bool enabled);

    void writeExportedGraphMLData(QString filename, QString data);

    void setNodeSelected(NodeItem* node);
    void deselectNode(QObject* obj);
    void makeNode(Node* node);
    void makeEdge(Edge* edge);
    void exportNodeSelected(Node * node);

    void makeChildNode(NodeItem * nodeItme);

    void deleteComponent(GraphMLContainer* graph);

    void updateText(QString data);
    void updateZoom(qreal zoom);
signals:
    void init_enableGUI(bool enabled);

    void removeComponent(GraphMLContainer* v);
    void init_ImportGraphML(QStringList inputGraphML, GraphMLContainer *currentParent=0);
    void init_ExportGraphML(QString file);

private slots:
    void on_actionImport_GraphML_triggered();

    void on_actionExport_GraphML_triggered();

    void on_actionExit_triggered();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();


protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent* event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
private:
    bool CONTROL_DOWN;
    void createNewModel();
    NodeItem *previousParent;
    QThread *modelThread;
    Ui::MainWindow *ui;
    QProgressDialog *progressDialog;
    Model *model;
    QGraphicsScene *scene;
    NodeItem* currentSelected;
    QHash<Node *, NodeItem*> hash;
};

#endif // MAINWINDOW_H
