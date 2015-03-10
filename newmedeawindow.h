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

#include <QThread>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>

#include <QDebug>
#include <QClipboard>
#include <QItemSelectionModel>

#include "Controller/newcontroller.h"
#include "GUI/attributetablemodel.h"

#include "GUI/docktogglebutton.h"
#include "GUI/docknodeitem.h"

#include "GUI/partsdockscrollarea.h"
#include "GUI/definitionsdockscrollarea.h"
#include "GUI/hardwaredockscrollarea.h"

#include "GUI/toolbarwidgetaction.h"

#include "GUI/nodeviewminimap.h"



class NewMedeaWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit NewMedeaWindow(QString graphMLFile=0, QWidget* parent = 0);
    ~NewMedeaWindow();
    void resizeEvent(QResizeEvent* event);

signals:
    void window_PasteData(QString value);
    void window_ExportProject();
    void window_ImportProjects(QStringList file);
    void window_AspectsChanged(QStringList aspects);
    void window_SortModel();


    //Leave
    void updateToolbarAdoptableNodesList(QStringList nodeList);
    void checkDockScrollBar();
    void setupViewLayout();
    void sendComponentDefinitions(QList<Node*>* definitions);
    void clearDocks();

private slots:
    void setAspects(QStringList aspects);

    void sortAndCenterViewAspects();

    void loadJenkinsData(int code);

    void on_actionImportJenkinsNode();
    void on_actionNew_Project_triggered();
    void on_actionImport_GraphML_triggered();
    void on_actionExport_GraphML_triggered();

    void autoCenterViews();

    void on_actionClearModel_triggered();
    void on_actionSortModel_triggered();
    //void on_actionSortNode_triggered();

    void on_actionPaste_triggered();
    void on_actionExit_triggered();

    void writeExportedProject(QString data);
    void updateUndoStates(QStringList list);
    void updateRedoStates(QStringList list);
    void setClipboard(QString value);

    void changeWindowTitle(QString label);

    void dockButtonPressed(QString buttonName);

    void setAttributeModel(AttributeTableModel* model);


    void updateViewAspects();



    void goToDefinition();
    void goToImplementation();

    void setGoToMenuActions(QString action, bool enabled);

private:
    void resetView();
    void newProject();
    bool exportProject();
    void importProjects(QStringList files);
    void initialiseGUI();
    void makeConnections();
    void connectToController();
    void setupJenkinsSettings();
    void setupMenu(QPushButton* button);
    void setupDock(QHBoxLayout* layout);
    void setupController();
    void resetGUI();
    void updateDataTable();

    bool isEnabled;

    QMenu* menu;
    QMenu* file_menu;
    QMenu* edit_menu;
    QMenu* view_menu;
    QMenu* model_menu;

    QAction* exit;
    QAction* file_newProject;
    QAction* file_importGraphML;
    QAction* file_exportGraphML;
    QAction* file_importJenkinsNodes;
    QAction* edit_undo;
    QAction* edit_redo;
    QAction* edit_cut;
    QAction* edit_copy;
    QAction* edit_paste;
    QAction* view_fitToScreen;
    QAction* view_autoCenterView;
    QAction* view_goToDefinition;
    QAction* view_goToImplementation;
    QAction* model_validateModel;
    QAction* model_clearModel;
    QAction* model_sortModel;

    DockToggleButton* partsButton;
    DockToggleButton* hardwareNodesButton;
    DockToggleButton* compDefinitionsButton;
    DockToggleButton* prevPressedButton;
	PartsDockScrollArea* partsDock;
    DefinitionsDockScrollArea* definitionsDock;
    HardwareDockScrollArea* hardwareDock;

    QTableView* dataTable;
    QGroupBox* dataTableBox;

    QPushButton *projectName;
    QPushButton* assemblyButton;
    QPushButton* hardwareButton;
    QPushButton* workloadButton;
    QPushButton* definitionsButton;

    NodeView* nodeView;
    NewController* controller;
    Node* prevSelectedNode;
    Node* selectedNode;

    NodeViewMinimap* minimap;

    QThread* thread;
    int boxWidth;
    int boxHeight;
    bool autoCenterOn;
    QStringList checkedViewAspects;

    QProcess *myProcess;
    QString DEPGEN_ROOT;
    QString JENKINS_ADDRESS;
    QString JENKINS_USERNAME;
    QString JENKINS_PASSWORD;


    QString exportFileName;
};

#endif // NEWMEDEAWINDOW_H
