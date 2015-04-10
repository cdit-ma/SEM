#ifndef MEDEAWINDOW_H
#define MEDEAWINDOW_H

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
#include <QClipboard>
#include <QItemSelectionModel>
#include <QDebug>

#include "Controller/controller.h"

#include "View/Table/comboboxtabledelegate.h"
#include "View/Table/attributetablemodel.h"
#include "View/nodeviewminimap.h"

#include "View/Dock/docktogglebutton.h"
#include "View/Dock/docknodeitem.h"
#include "View/Dock/partsdockscrollarea.h"
#include "View/Dock/definitionsdockscrollarea.h"
#include "View/Dock/hardwaredockscrollarea.h"

#include "View/Toolbar/toolbarwidgetaction.h"
#include "GUI/appsettings.h"

class MedeaWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MedeaWindow(QString graphMLFile=0, QWidget* parent = 0);
    ~MedeaWindow();
    void resizeEvent(QResizeEvent* event);

signals:
    void window_PasteData(QString value);
    void window_ExportProject();
    void window_ImportProjects(QStringList file);
    void window_AspectsChanged(QStringList aspects);
    void window_SortModel();

    void checkDockScrollBar();
    void clearDocks();

public slots:
    void setupDefaultSettings();

private slots:
    void loadSettings();
    void saveSettings();

    void loadJenkinsData(int code);

    void on_actionImportJenkinsNode();
    void on_actionNew_Project_triggered();
    void on_actionImport_GraphML_triggered();
    void on_actionExport_GraphML_triggered();

    void on_actionClearModel_triggered();
    void on_actionSortNode_triggered();
    void on_actionCenterNode_triggered();

    void on_actionPopupNewWindow();

    void on_actionPaste_triggered();
    void on_SearchTextChanged(QString text);
    void on_actionSearch();
    void on_actionExit_triggered();

    void writeExportedProject(QString data);
    void updateUndoStates(QStringList list);
    void updateRedoStates(QStringList list);
    void setClipboard(QString value);

    void changeWindowTitle(QString label);

    void setAttributeModel(AttributeTableModel* model);

    void editMultiLineData(GraphMLData* data);

    void dockButtonPressed(QString buttonName);

    void setViewAspects(QStringList aspects);
    void updateViewAspects();

    void setGoToMenuActions(QString action, bool enable);

    void showWindowToolbar();

private:

    void resetGUI();
    void resetView();
    void newProject();
    void initialiseGUI();
    void makeConnections();
    void setupController();
    void setupJenkinsSettings();
    void setupMenu(QPushButton* button);
    void setupDock(QHBoxLayout* layout);
    void setupToolbar();
    void updateDataTable();
    bool exportProject();
    void importProjects(QStringList files);
    void enableDeploymentViewAspect();

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
    QAction* view_goToDefinition;
    QAction* view_goToImplementation;
    QAction* view_autoCenterView;
    QAction* view_showGridLines;
    QAction* view_selectOnConstruction;


    QAction* model_validateModel;
    QAction* model_clearModel;
    QAction* model_sortModel;

    QAction* settings_ChangeSettings;

    DockToggleButton* partsButton;
    DockToggleButton* hardwareNodesButton;
    DockToggleButton* compDefinitionsButton;
    DockToggleButton* prevPressedButton;

    PartsDockScrollArea* partsDock;
    DefinitionsDockScrollArea* definitionsDock;
    HardwareDockScrollArea* hardwareDock;

    QToolBar* toolbar;
    QToolButton* cutButton;
    QToolButton* copyButton;
    QToolButton* pasteButton;
    QToolButton* sortButton;
    QToolButton* centerButton;
    QToolButton* popupButton;
    QToolButton* snapToGridButton;
    QToolButton* snapChildrenToGridButton;
    QToolButton* zoomToFitButton;
    QToolButton* fitToScreenButton;
    QToolButton* duplicateButton;

    QToolButton* alignSelectionVertical;
    QToolButton* alignSelectionHorizontal;

    ComboBoxTableDelegate* delegate;

    QTableView* dataTable;
    QGroupBox* dataTableBox;

    QPushButton *projectName;
    QPushButton* assemblyButton;
    QPushButton* hardwareButton;
    QPushButton* workloadButton;
    QPushButton* definitionsButton;
    QPushButton* searchButton;
    QLineEdit* searchBar;

    AppSettings* appSettings;

    NewController* controller;
    NodeView* nodeView;
    NodeViewMinimap* minimap;

    int boxWidth;
    int boxHeight;
    bool firstTableUpdate;

    QStringList checkedViewAspects;

    QThread* thread;
    QProcess *myProcess;
    QString DEPGEN_ROOT;
    QString JENKINS_ADDRESS;
    QString JENKINS_USERNAME;
    QString JENKINS_PASSWORD;


    QString exportFileName;

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *);
};

#endif // MEDEAWINDOW_H
