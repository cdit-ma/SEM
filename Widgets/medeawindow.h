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
#include <QProgressDialog>
#include <QTemporaryFile>

#include <QGuiApplication>
#include <QScreen>
#include <QDebug>

#include <QXmlQuery>
#include <QXmlResultItems>

#include "Plugins/Jenkins/jenkinsmanager.h"
#include "Plugins/Jenkins/GUI/jenkinsstartjobwidget.h"
#include "GUI/actionbutton.h"

#include "New/medeawindownew.h"
#include "Controller/controller.h"

#include "View/nodeviewminimap.h"

#include "View/Table/comboboxtabledelegate.h"
#include "View/Table/attributetablemodel.h"

#include "View/Dock/docktogglebutton.h"
#include "View/Dock/docknodeitem.h"
#include "View/Dock/partsdockscrollarea.h"
#include "View/Dock/definitionsdockscrollarea.h"
#include "View/Dock/hardwaredockscrollarea.h"
#include "View/Dock/functionsdockscrollarea.h"

#include "View/Validate/validatedialog.h"

#include "Plugins/CUTS/cutsmanager.h"
#include "Plugins/XMI/xmiimporter.h"

#include "GUI/searchsuggestcompletion.h"
#include "GUI/searchdialog.h"
#include "GUI/searchitem.h"

#include "GUI/aspecttogglewidget.h"
#include "GUI/appsettings.h"
#include "GUI/codeeditor.h"
#include "Controller/viewcontroller.h"
#include "../View/nodeviewnew.h"
#include <QAbstractMessageHandler>

#include "../View/docktitlebarwidget.h"


struct NotificationStruct{
    QString text;
    QString actionImage;
};

class SearchDialog;

class MedeaWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MedeaWindow(ViewController* vc, QString graphMLFile=0, QWidget* parent = 0);
    ~MedeaWindow();

signals:
    void window_SetViewVisible(bool visible);
    void window_ConnectViewAndSetupModel(NodeView* view);


    void jenkins_RunGroovyScript(QString groovyScriptPath, QString parameters);

    void window_Paste(QString value);

    void window_ImportSnippet(QString fileName, QString fileData);
    void window_ImportProjects(QStringList file);
    void window_ImportJenkinsNodes(QString fileData);

    void window_OpenProject(QString fileName, QString fileData);
    void window_ProjectSaved(bool success, QString filePath);


    //Calls to CUTSManager
    void window_GetCPPForComponent(QString graphmlPath, QString component);
    void window_ExecuteXSLValidation(QString graphmlPath, QString reportPath);
    void window_ExecuteXMETransform(QString xmePath, QString graphmlPath);


    void window_searchItemClicked(SearchItem* item);
    void window_aspectMiddleClicked(VIEW_ASPECT aspect);
    void window_toggleAspect(VIEW_ASPECT aspect, bool on);
    void window_centerAspect(VIEW_ASPECT aspect);

    void window_dockButtonPressed(DOCK_TYPE);
    void window_clearDocks();
    void window_clearDocksSelection();

    void window_changeHardwareDockFunction(bool clictToCenter);

    void window_updateActionsEnabled();

    void checkDockScrollBar();

    void window_refreshActions();
    void window_ImportXMIProject(QString xmi_path);
public slots:
    void newSubView();
    void projectRequiresSaving(bool requiresSave);
    void modelReady();
    void modelDisconnected();

    void projectCleared();
    void settingChanged(QString groupName, QString keyName, QVariant value);
    void settingsApplied();

    void loadSettingsFromINI();
    void setupInitialSettings();



    void importXMIProject(QString xmiPath="");
    void generateCPPForComponent(QString componentName);
    void executeProjectValidation();
    void executeLocalNodeDeployment();
    void executeJenkinsDeployment();

private slots:
    void viewFocussed(NodeViewNew* view, bool focussed);
    void closeCodeTab(int tabID);
    void CUTSOutputPathChanged(QString path);
    void themeChanged();

    void updateRightMask();

    void updateMenuIcons();

    void recentProjectItemClicked(QListWidgetItem* item);
    void recentProjectMenuActionClicked();
    void clearRecentProjectsList();

    void screenshot();
    void XSLValidationCompleted(bool success, QString reportPath);
    void projectFileChanged(QString name="");
    void projectNameChanged(QString name="");

    void gotSaveData(QString filePath, QString fileData);
    void setFullscreenMode(bool fullscreen);
    void gotXMETransform(bool success, QString errorString, QString path);
    void gotXMIGraphml(bool success, QString errorString, QString graphmlData);
    void gotCPPForComponent(bool success, QString errorString, QString componentName, QString code);
    void localDeploymentOkay();
    void toggleGridLines();
    void aboutMedea();
    void aboutQt();
    void reportBug();
    void showWiki(QString componentName="");
    void showShortcutList();
    void jenkinsSettingsValidated(bool success, QString message);
    void jenkinsNodesLoaded();
    void saveSettings();
    void search();

    void gotJenkinsNodeGraphML(QString graphML = "");
    void on_actionImportJenkinsNode();

    void on_actionNew_Project_triggered();
    void on_actionCloseProject_triggered();
    void on_actionOpenProject_triggered();
    void on_actionSaveProject_triggered();
    void on_actionSaveProjectAs_triggered();

    void on_actionImport_GraphML_triggered();
    void on_actionImport_XME_triggered();
    void on_actionImport_XMI_triggered();

    void on_actionPaste_triggered();
    void on_actionExit_triggered();
    void on_actionSearch_triggered();

    void on_searchResultItem_clicked(int ID);
    void on_validationItem_clicked(int ID);

    void importSnippet(QString snippetType = "");
    void exportSnippet(QString snippetType = "");

    void setClipboard(QString value);
    void setAttributeModel(AttributeTableModel* model);

    void forceToggleAspect(VIEW_ASPECT aspect, bool on);
    void setActionEnabled(QString action, bool enable);
    QIcon getIcon(QString alias, QString image);
    void menuActionTriggered();

    void dockButtonPressed();
    void dockToggled(bool opened, QString kindToConstruct = "");
    void hardwareDockFunctionChanged(bool checked);
    void enableHardwareDockDeployButton(bool enable);

    void displayLoadingStatus(bool show, QString displayText = "");
    void updateProgressStatus(int value, QString status);
    void closeProgressDialog();

    void updateWidgetMask(QWidget* widget, QWidget* maskWidget, bool check = false, QSize border = QSize());

    void updateSearchLineEdits();

    void updateSearchSuggestions();
    void searchItemClicked();
    void searchMenuButtonClicked(bool checked);
    void searchMenuClosed();

    void displayNotification(QString notification = "", QString actionImage="");
    void checkNotificationsQueue();

    void showDocks(bool checked);
    void detachDocks(bool checked);
    void detachedDockClosed();

    void showWindowToolbar(bool checked);
    void setToolbarVisibility(bool visible);
    void detachWindowToolbar(bool checked);
    void detachedToolbarClosed();
    void updateCheckedToolbarActions(bool checked = true);

    //multi-line popup for QTableView (SLOTS)
    void dataTableDoubleClicked(QModelIndex);
    void dialogAccepted();
    void dialogRejected();

    QStringList fileSelector(QString title, QString fileString, QString defaultSuffix, bool open, bool allowMultiple=true, QString fileName = "");

    void updateStyleSheets();
    void toggleMinimap(bool on);

    void closeDockWidget();
    void maximizeDockWidget();
    void minimizeDockWidget();
    void popOutDockWidget();
    void popInDockWidget();

protected:
    void closeEvent(QCloseEvent*);
    void resizeEvent(QResizeEvent* event);
    void changeEvent(QEvent * event);


private:
    QToolBar *constructToolbar(bool ignoreStyle = false);
    void saveTheme(bool apply = true);
    void resetTheme(bool darkTheme);
    void resetAspectTheme(bool colorBlindTheme);
    void updateRecentProjectsWidgets(QString topFileName="");

    QPixmap getDialogPixmap(QString alias, QString image, QSize size = QSize(50,50));
    bool openProject(QString fileName);
    QRect getCanvasRect();
    QString getTimestamp();
    QString getTempFileName(QString prefix="");
    QString getTempTimeName();
    bool closeProject();
    bool saveProject(bool saveAs=false);

    QString readTextFile(QString fileName);
    bool writeTextFile(QString filePath, QString fileData, bool notify=true);

    bool writeQImage(QString filePath, QImage image, bool notify=true);

    bool ensureDirectory(QString filePath);
    QString writeTempFile(QString fileData);
    QString writeProjectToTempFile();
    QTemporaryFile* writeTemporaryFile(QString data);

    void populateDocks();
    bool canFilesBeDragImported(const QList<QUrl> files);
    void setupApplication();
    void initialiseJenkinsManager();
    void initialiseSettings();
    void updateTheme();
    void initializePlugins();
    void initialiseCUTSManager();
    void initialiseTheme();
    void importXMEProject(QString fileName);
    void importProjects(QStringList files);

    void toolbarSettingChanged(QString keyName, QVariant value);
    void themeSettingChanged(QString keyName, QVariant value);
    void jenkinsSettingChanged(QString keyName, QVariant value);
    void jenkins_JobName_Changed(QString jobName);
    void enableTempExport(bool enable);

    EventAction getEventAction();    
    QStringList getCheckedItems(int menu);

    void setApplicationEnabled(bool enable);
    void setViewWidgetsEnabled(bool enable);

    void toggleWelcomeScreen(bool show);
    void toggleAndTriggerAction(QAction* action, bool value);

    void resetGUI();
    void resetView();
    void newProject();
    void initialiseGUI();
    void setupConnections();

    void setupNewNodeView();
    void teardownProject();
    void setupProject();

    void setupMenu();



    void setupSearchTools();
    void setupDocks(QHBoxLayout* layout);
    void setupInfoWidgets(QHBoxLayout* layout);

    void setupToolbar();
    bool constructToolbarButton(QToolBar* toolbar, QAction* action, QString actionName);

    void setupWelcomeScreen();
    void setupMinimap();
    void setupMultiLineBox();

    void updateWidgetsOnWindowChange();
    void updateWidgetsOnProjectChange(bool projectActive = true);

    void updateDock();
    void updateToolbar();
    void updateDataTable();


    void addCodeViewerTab(QString tabName, QIcon tabIcon, QString content, bool editable);
    void removeAllCodeViewerTabs();


    QString applicationDirectory;

    QToolBar* closeProjectToolbar;

    QPushButton *projectName;
    QToolButton *closeProjectToolButton;
    QGroupBox* menuTitleBox;

    QPushButton* menuButton;
    QMenu* menu;
    QMenu* file_menu;
    QMenu *file_recentProjectsMenu;
    QMenu* edit_menu;
    QMenu* view_menu;
    QMenu* model_menu;
    QMenu* jenkins_menu;
    QMenu* help_menu;

    QAction* exit;
    QAction* file_newProject;
    QAction* file_importGraphML;
    QAction* file_importXME;
    QAction* file_importXMI;

    QAction* file_recentProjects_clearHistory;

    QAction* file_openProject;
    QAction* file_saveProject;
    QAction* file_saveAsProject;
    QAction* file_closeProject;

    QAction* file_importSnippet;
    QAction* file_exportSnippet;

    QList<QAction*> modelActions;
    QList<QAction*> nonModelActions;
    QAction* edit_undo;
    QAction* edit_redo;
    QAction* edit_cut;
    QAction* edit_copy;
    QAction* edit_paste;
    QAction* edit_replicate;
    QAction* edit_delete;
    QAction* edit_search;

    QAction* view_fitToScreen;
    QAction* view_goToDefinition;
    QAction* view_goToImplementation;
    QAction* view_showConnectedNodes;
    QAction* view_fullScreenMode;
    QAction* view_printScreen;
    QAction* view_showMinimap;

    QAction* model_validateModel;
    QAction* model_clearModel;
    QAction* model_ExecuteLocalJob;

    QVBoxLayout* rightVlayout;

    QAction* settings_editToolbarButtons;
    QAction* settings_changeAppSettings;

    QAction* help_AboutMedea;
    QAction* help_Wiki;
    QAction* help_AboutQt;
    QAction* help_Shortcuts;
    QAction* help_ReportBug;

    QAction* jenkins_ImportNodes;
    QAction* jenkins_ExecuteJob;

    QAction* actionSort;
    QAction* actionCenter;
    QAction* actionZoomToFit;
    QAction* actionFitToScreen;
    QAction* actionAlignVertically;
    QAction* actionAlignHorizontally;
    QAction* actionPopupSubview;
    QAction* actionBack;
    QAction* actionForward;
    QAction* actionToggleGrid;
    QAction* actionContextMenu;

    QAction* actionSearch;
    //QAction* actionSearchOptions;

    DockToggleButton* partsButton;
    DockToggleButton* hardwareNodesButton;

    PartsDockScrollArea* partsDock;
    HardwareDockScrollArea* hardwareDock;
    DefinitionsDockScrollArea* definitionsDock;
    FunctionsDockScrollArea* functionsDock;

    int searchOptionMenuWidth;
    QGroupBox* dockGroupBox;
    QGroupBox* dockHeaderBox;
    QGroupBox* dockBackButtonBox;
    QLabel* openedDockLabel;
    QPushButton* dockBackButton;
    QLabel* dockActionLabel;

    QDialog* dockStandAloneDialog;
    QGroupBox* docksArea;
    QGroupBox* dockButtonsBox;
    QVBoxLayout* dockLayout;

    QGroupBox* nodesDockButtonsBox;
    QPushButton* nodesDockDeployButton;
    QPushButton* nodesDockCenterButton;

    // DEMO CHANGE
    //QDialog* toolbarStandAloneDialog;
    //QGroupBox* toolbarArea;
    QVBoxLayout* toolbarLayout;

    QLabel* toolbarButtonLabel;

    QToolBar* toolbar;
    QAction* toolbarAction;
    QAction* leftSpacerAction;
    QAction* rightSpacerAction;

    QToolBar* toolbarButtonBar;
    QToolButton* toolbarButton;
    bool SHOW_TOOLBAR;
    bool wasToolbarVisible;
    bool EXPAND_TOOLBAR;

    QHash<QString, QAction*> toolbarActionLookup;
    QHash<QString, ActionButton*> toolbarButtonLookup;

    QAction* actionToggleToolbar;
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
    QList<AspectToggleWidget*> aspectToggles;

    QHash<QDockWidget*, DockTitleBarWidget*> aspectDockWidgets;

	QDialog* progressDialog;
    QMovie* loadingMovie;

    QProgressBar* progressBar;
    QLabel* progressLabel;
    QString progressAction;
    bool progressDialogVisible;

    QGroupBox* loadingBox;
    QWidget* loadingWidget;
    QLabel* loadingLabel;
    QLabel* loadingMovieLabel;

    QGroupBox* notificationsBox;
    QLabel* notificationsIcon;
    QLabel* notificationsBar;
    QTimer* notificationTimer;
    QQueue<NotificationStruct> notificationsQueue;
    QHash<int, QString> multipleNotification;

    QWidget* rightPanelWidget;


    QTableView* dataTable;
    QTableView* dataTable2;
    QScrollArea* tableScroll;

    ComboBoxTableDelegate* delegate;

    QLineEdit* searchBar;
    QToolBar* searchToolbar;

    QDialog* searchResults;
    SearchSuggestCompletion* searchSuggestions;
    QHBoxLayout* searchLayout;
    QVBoxLayout* resultsLayout;

    SearchDialog* searchDialog;



    QToolButton* searchToolButton;
    QToolButton* searchOptionToolButton;
    QMenu* searchOptionMenu;
    QLineEdit* viewAspectsBar;
    QToolButton* viewAspectsButton;
    QMenu* viewAspectsMenu;
    QLineEdit* nodeKindsBar;
    QToolButton* nodeKindsButton;
    QMenu* nodeKindsMenu;
    QLineEdit* dataKeysBar;
    QToolButton* dataKeysButton;
    QMenu* dataKeysMenu;

    QWidget* minimapTitleBar;
    QToolButton* closeMinimapButton;
    QLabel* minimapLabel;

    QStringList dataKeys;

    QString searchBarDefaultText;
    QString viewAspectsBarDefaultText;
    QString nodeKindsDefaultText;
    QString dataKeysDefaultText;

    NodeViewMinimap* minimap;
    QWidget* minimapBox;

    QGraphicsDropShadowEffect *projectNameShadow;
    QString currentProjectFilePath;

    AppSettings* appSettings;

    ValidateDialog validateResults;
    QDialog* toolbarSettingsDialog;

    NewController* controller;
    ViewController* viewController;
    NodeView* nodeView;
    NodeViewNew* nodeViewNew;
    NodeViewNew* nodeViewNew1;
    NodeViewNew* nodeViewNew2;
    NodeViewNew* nodeViewNew3;
    NodeViewNew* nodeViewNew4;

    QFileDialog* fileDialog;

    QThread* controllerThread;

    QHash<QPushButton*, GraphMLItem*> searchItems;
    QMap<QAction*, QCheckBox*> toolbarActions;
    QList<QAction*> checkedToolbarActions;
    QList<QAction*> checkedToolbarSpacers;

    QHash<QAction*, int> leftMostActions;
    QHash<QAction*, int> leftMidActions;
    QHash<QAction*, int> midLeftActions;
    QHash<QAction*, int> midRightActions;
    QHash<QAction*, int> rightMidActions;
    QHash<QAction*, int> rightMostActions;

    QFont guiFont;
    int boxWidth, boxHeight;
    int minLeftPanelHeight;

    int dockWithLabelHeight;
    int dockWithoutLabelHeight;

    //QString DEPGEN_ROOT;

    bool IS_WINDOW_MAXIMIZED;
    bool SETTINGS_LOADING;
    bool INITIAL_SETTINGS_LOADED;
    bool maximizedSettingInitiallyChanged;

    bool SAVE_WINDOW_SETTINGS;

    bool WINDOW_MAXIMIZED;
    bool WINDOW_FULLSCREEN;

    //multi-line popup for QTableView (VARIABLES)
    QDialog *popupMultiLine;
    QTabWidget* codeViewerTabWidget;
    //CodeEditor *txtMultiLine;
    QModelIndex clickedModelIndex;

    JenkinsManager* jenkinsManager;
    CUTSManager* cutsManager;
    XMIImporter* xmiImporter;

    QString MEDEA_VERSION;
    QString DEFAULT_PATH;
    QAction* action_ContextMenu;
    ActionButton* toolbar_ContextMenu;

    QString validation_report_path;
    QString componentName_CPPExport;


    QString cutsOutputPath;
    QVBoxLayout* viewHolderLayout;
    QHBoxLayout* viewLayout;
    QVBoxLayout* holderLayout;
    QHBoxLayout* welcomeLayout;
    bool welcomeScreenOn;

    VIEW_THEME CURRENT_THEME;


    QSettings* persistantSettings;

    int NOTIFICATION_TIME;

    //MedeaWindowNew* window;


    QStack<QString> recentProjectsList;
    QListWidget* recentProjectsListWidget;

    NodeViewMinimap* minimap2;
    // QWidget interface
protected:
    void dropEvent(QDropEvent *);

    // QWidget interface
protected:
    void dragEnterEvent(QDragEnterEvent *);
};


#endif // MEDEAWINDOW_H
