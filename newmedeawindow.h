#ifndef NEWMEDEAWINDOW_H
#define NEWMEDEAWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGroupBox>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableView>
#include <QTextEdit>
#include <QMenu>
#include <QAction>
#include <QProcess>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>

#include <QDebug>
#include <QClipboard>
#include <QItemSelectionModel>

#include "GUI/projectwindow.h"
#include "GUI/attributetablemodel.h"
#include "GUI/filterbutton.h"
#include "GUI/docktogglebutton.h"
#include "GUI/dockscrollarea.h"



class NewMedeaWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit NewMedeaWindow(QString graphMLFile=0, QWidget* parent = 0);
    ~NewMedeaWindow();

protected:
    void resizeEvent(QResizeEvent* event);

signals:
    void view_PasteData(QString value);
    void view_ActionTriggered(QString name);
    void view_ImportGraphML(QStringList files);
    void view_ExportGraphML(QString fileName);
    void setViewAspects(QStringList aspects);

    void clearDock();
    void checkDockScrollBar();

private slots:
    void loadJenkinsData(int code);

    void on_actionImportJenkinsNode();
    void on_actionNew_Project_triggered();
    void on_actionImport_GraphML_triggered();
    void on_actionExport_GraphML_triggered();
    void on_actionClearModel_triggered();
    void on_actionPaste_triggered();
    void on_actionExit_triggered();

    void writeExportedGraphMLData(QString filename, QString data);
    void updateUndoStates(QStringList list);
    void updateRedoStates(QStringList list);
    void setClipboard(QString value);

    void updateViewMargin();
    void updateProjectName(QString label);

    void on_dockButtonPressed(QString buttonName);

    void setAttributeModel(AttributeTableModel* model);

    void updateDataTable();
    void updateViewAspects();
    void updateDockButtons(QString dockButton);
    void updateDockContainer(QString container);

    void addNewNodeToDock(QString type, NodeItem* nodeItem);
    void setAdoptableNodeList(Node* node);
    void nodeSelected(Node* node);

    // this is just for testing
    void addNodeToDock();

private:
    void importGraphMLFiles(QStringList files);
    void initialiseGUI();
    void makeConnections();
    void connectToController();
    void setupJenkinsSettings();
    void setupMenu(QPushButton* button);
    void setupDock(QHBoxLayout* layout);
    void setupController();
    void resetGUI();

    QVector<ProjectWindow*> projectWindows;
    bool isEnabled;

    QGraphicsScene* scene;
    QGraphicsScene* newScene;

    QMenu* menu;
    QMenu* file_menu;
    QMenu* edit_menu;
    QMenu* view_menu;
    QAction* exit;
    QAction* file_newProject;
    QAction* file_importGraphML;
    QAction* file_exportGraphML;
    QAction* file_importJenkinsNodes;
    QAction* file_validateModel;
    QAction* file_clearModel;
    QAction* edit_undo;
    QAction* edit_redo;
    QAction* edit_cut;
    QAction* edit_copy;
    QAction* edit_paste;
    QAction* view_fitToScreen;

    DockToggleButton* partsButton;
    DockToggleButton* hardwareNodesButton;
    DockToggleButton* compDefinitionsButton;
    DockToggleButton* prevPressedButton;
    DockScrollArea* partsContainer;
    DockScrollArea* hardwareContainer;
    DockScrollArea* definitionsContainer;
    QTableView* dataTable;
    QGroupBox* dataTableBox;

    QPushButton *projectName;
    QPushButton* assemblyButton;
    QPushButton* workloadButton;
    QPushButton* definitionsButton;

    NodeView* nodeView;
    NewController* controller;
    Node* prevSelectedNode;
    Node* selectedNode;

    int boxWidth;
    int boxHeight;
    QStringList checkedViewAspects;

    QProcess *myProcess;
    QString DEPGEN_ROOT;
    QString JENKINS_ADDRESS;
    QString JENKINS_USERNAME;
    QString JENKINS_PASSWORD;
};

#endif // NEWMEDEAWINDOW_H
