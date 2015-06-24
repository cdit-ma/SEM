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
#include <QListView>
#include <QTextEdit>
#include <QMenu>
#include <QAction>
#include <QProcess>
#include <QThread>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QClipboard>
#include <QProgressBar>
#include <QCheckBox>
#include <QItemSelectionModel>
#include <QDebug>

#include <QTemporaryFile>

#include <QXmlQuery>
#include <QXmlResultItems>

#include "Jenkins/jenkinsmanager.h"
#include "Jenkins/GUI/jenkinsstartjobwidget.h"

#include "Controller/controller.h"

#include "View/nodeviewminimap.h"

#include "View/Table/comboboxtabledelegate.h"
#include "View/Table/attributetablemodel.h"

#include "View/Dock/docktogglebutton.h"
#include "View/Dock/docknodeitem.h"
#include "View/Dock/partsdockscrollarea.h"
#include "View/Dock/definitionsdockscrollarea.h"
#include "View/Dock/hardwaredockscrollarea.h"

#include "View/Toolbar/toolbarwidgetaction.h"

#include "View/Validate/validatedialog.h"

#include "GUI/searchitembutton.h"
#include "GUI/aspecttogglewidget.h"
#include "GUI/appsettings.h"
#include "GUI/codeeditor.h"


class MedeaWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MedeaWindow(QString graphMLFile=0, QWidget* parent = 0);
    ~MedeaWindow();

signals:
    void jenkins_RunGroovyScript(QString groovyScriptPath);

    void window_PasteData(QString value);
    void window_ExportProject();
    void window_ImportProjects(QStringList file);
    void window_ImportSnippet(QString fileName, QString fileData);
    void window_LoadJenkinsNodes(QString fileData);

    void window_AspectsChanged(QStringList aspects);
    void window_SortModel();

    void window_searchItemClicked(SearchItemButton* item);
    void window_aspectToggleDoubleClicked(AspectToggleWidget* aspect);

    void window_highlightDeployment(Node* selectedNode = 0);

    void checkDockScrollBar();
    void clearDocks();

public slots:
    void projectCleared();
    void settingChanged(QString groupName, QString keyName, QString value);
    void setupInitialSettings();
    void aspectToggleClicked(bool checked, int state);

    void exportTempFile();
    void jenkins_InvokeJob(QString filePath);

private slots:
    void jenkinsNodesLoaded();
    void saveSettings();

    void gotJenkinsNodeGraphML(QString graphML);
    void on_actionImportJenkinsNode();


    void on_actionNew_Project_triggered();
    void on_actionImport_GraphML_triggered();
    void on_actionExport_GraphML_triggered();

    void on_actionClearModel_triggered();
    void on_actionSortNode_triggered();
    void on_actionFitCenterNode_triggered();
    void on_actionValidate_triggered();

    void on_actionPopupNewWindow();

    void on_actionPaste_triggered();
    void on_actionExit_triggered();
    void on_actionSearch_triggered();

    void on_searchResultItem_clicked(GraphMLItem* clickedItem);
    void on_validationItem_clicked(QString ID);

    void writeExportedProject(QString data);
    void writeExportedSnippet(QString parentName, QString snippetXMLData);
    void importSnippet(QString parentName);

    void updateUndoStates(QStringList list);
    void updateRedoStates(QStringList list);

    void setClipboard(QString value);
    void setAttributeModel(AttributeTableModel* model);
    void setViewAspects(QStringList aspects);
    void setMenuActionEnabled(QString action, bool enable);

    void menuActionTriggered();

    void graphicsItemSelected();
    void graphicsItemDeleted();

    void dockButtonPressed(QString buttonName);

    void updateWindowTitle(QString newProjectName);
    void updateProgressStatus(int value, QString status);
    void updateWidgetMask(QWidget* widget, QWidget* maskWidget, bool check = false, QSize border = QSize());

    void searchItemClicked();
    void searchMenuButtonClicked(bool checked);
    void searchMenuClosed();
    void updateSearchLineEdits();

    void displayNotification(QString notification = "",  int seqNum = 0, int totalNum = 1);
    void checkNotificationsQueue();

    void showDocks(bool checked);
    void detachDocks(bool checked);
    void detachedDockClosed();

    void showWindowToolbar(bool checked);
    void detachWindowToolbar(bool checked);
    void detachedToolbarClosed();
    void updateCheckedToolbarActions(bool checked = true);

    //multi-line popup for QTableView (SLOTS)
    void dataTableDoubleClicked(QModelIndex);
    void dialogAccepted();

    void dialogRejected();

protected:
    void closeEvent(QCloseEvent*);
    void resizeEvent(QResizeEvent* event);
    void changeEvent(QEvent * event);

private:
    void initialiseJenkinsManager();
    void resetGUI();
    void resetView();
    void newProject();
    void initialiseGUI();
    void makeConnections();

    void setupController();
    void setupMenu(QPushButton* button);
    void setupDock(QHBoxLayout* layout);
    void setupSearchTools();
    void setupToolbar(QVBoxLayout* layout);
    void setupMultiLineBox();

    void updateWidgetsOnWindowChanged();
    void updateDataTable();

    bool exportProject();
    void importProjects(QStringList files);


    void toggleAndTriggerAction(QAction* action, bool value);

    QStringList getCheckedItems(int menu);

    QTemporaryFile* writeTemporaryFile(QString data);

    QString applicationDirectory;

    QPushButton *projectName;
    QGroupBox* menuTitleBox;

    QMenu* menu;
    QMenu* file_menu;
    QMenu* edit_menu;
    QMenu* view_menu;
    QMenu* model_menu;
    QMenu* jenkins_menu;
    QMenu* settings_Menu;

    QAction* exit;
    QAction* file_newProject;
    QAction* file_importGraphML;
    QAction* file_importSnippet;
    QAction* file_exportGraphML;
    QAction* file_exportSnippet;

    QAction* edit_undo;
    QAction* edit_redo;
    QAction* edit_cut;
    QAction* edit_copy;
    QAction* edit_paste;
    QAction* view_fitToScreen;
    QAction* view_snapToGrid;
    QAction* view_snapChildrenToGrid;
    QAction* view_goToDefinition;
    QAction* view_goToImplementation;
    QAction* view_showConnectedNodes;
    QAction* view_showManagementComponents;
    QAction* model_validateModel;
    QAction* model_clearModel;
    QAction* model_sortModel;
    QAction* settings_useGridLines;
    QAction* settings_editToolbarButtons;
    QAction* settings_changeAppSettings;

    QAction* jenkins_ImportNodes;
    QAction* jenkins_ExecuteJob;

    DockToggleButton* partsButton;
    DockToggleButton* hardwareNodesButton;
    DockToggleButton* definitionsButton;
    DockToggleButton* prevPressedButton;
    PartsDockScrollArea* partsDock;
    DefinitionsDockScrollArea* definitionsDock;
    HardwareDockScrollArea* hardwareDock;

    QDialog* dockStandAloneDialog;
    QGroupBox* docksArea;
    QGroupBox* dockButtonsBox;
    QVBoxLayout* dockLayout;

    // DEMO CHANGE
    //QDialog* toolbarStandAloneDialog;
    //QGroupBox* toolbarArea;
    QVBoxLayout* toolbarLayout;

    QLabel* toolbarButtonLabel;
    QPixmap expandPixmap;
    QPixmap contractPixmap;

    QToolBar* toolbar;
    QAction* toolbarAction;
    QAction* leftSpacerAction;
    QAction* rightSpacerAction;
    QToolButton* toolbarButton;
    QToolButton* cutButton;
    QToolButton* copyButton;
    QToolButton* pasteButton;
    QToolButton* sortButton;
    QToolButton* centerButton;
    QToolButton* zoomToFitButton;
    QToolButton* fitToScreenButton;
    QToolButton* duplicateButton;
    QToolButton* undoButton;
    QToolButton* redoButton;
    QToolButton* toggleGridButton;
    QToolButton* alignVerticalButton;
    QToolButton* alignHorizontalButton;
    QToolButton* popupButton;
    QToolButton* backButton;
    QToolButton* forwardButton;
    QToolButton* deleteButton;
    QToolButton* contextToolbarButton;
    QAction* leftMostSpacer;
    QAction* leftMidSpacer;
    QAction* midLeftSpacer;
    QAction* midRightSpacer;
    QAction* rightMidSpacer;
    QAction* rightMostSpacer;

    AspectToggleWidget* definitionsToggle;
    AspectToggleWidget* workloadToggle;
    AspectToggleWidget* assemblyToggle;
    AspectToggleWidget* hardwareToggle;

    QProgressBar* progressBar;
    QLabel* progressLabel;
    QString progressAction;

    QLabel* notificationsBar;
    QTimer* notificationTimer;
    QQueue<QString> notificationsQueue;
    QHash<int, QString> multipleNotification;
    int leftOverTime;

    QTableView* dataTable;
    QGroupBox* dataTableBox;
    ComboBoxTableDelegate* delegate;

    QHBoxLayout* searchLayout;
    QPushButton* searchButton;
    QListView* searchSuggestions;
    QDialog* searchResults;
    QScrollArea* scrollableSearchResults;
    QVBoxLayout* resultsLayout;

    QPushButton* searchOptionButton;
    QLineEdit* searchBar;
    QMenu* searchOptionMenu;
    QPushButton* viewAspectsButton;
    QLineEdit* viewAspectsBar;
    QMenu* viewAspectsMenu;
    QPushButton* nodeKindsButton;
    QLineEdit* nodeKindsBar;
    QMenu* nodeKindsMenu;

    QString searchBarDefaultText;
    QString viewAspectsBarDefaultText;
    QString nodeKindsDefaultText;

    AppSettings* appSettings;

    ValidateDialog validateResults;
    QDialog* toolbarSettingsDialog;

    NewController* controller;
    NodeView* nodeView;
    NodeViewMinimap* minimap;

    QThread* controllerThread;


    QHash<QPushButton*, GraphMLItem*> searchItems;
    QMap<QAction*, QCheckBox*> toolbarActions;
    QList<QAction*> checkedToolbarActions;
    QList<QAction*> checkedToolbarSpacers;
    QStringList checkedViewAspects;

    QHash<QAction*, int> leftMostActions;
    QHash<QAction*, int> leftMidActions;
    QHash<QAction*, int> midLeftActions;
    QHash<QAction*, int> midRightActions;
    QHash<QAction*, int> rightMidActions;
    QHash<QAction*, int> rightMostActions;


    bool modelCleared;
    QFont guiFont;
    int boxWidth, boxHeight;
    int minLeftPanelHeight;

    //QString DEPGEN_ROOT;
    QString exportFileName;
    QString tempFileName;
    bool tempExport;

    //multi-line popup for QTableView (VARIABLES)
    QDialog *popupMultiLine;
    CodeEditor *txtMultiLine;
    QModelIndex clickedModelIndex;

    JenkinsManager* jenkinsManager;
    QAction* jenkins_getJobParameters;

};


#endif // MEDEAWINDOW_H
