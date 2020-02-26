#ifndef ACTIONCONTROLLER_H
#define ACTIONCONTROLLER_H

#include <QObject>
#include <QAction>
#include <QMenu>
#include <QSignalMapper>

#include "../SelectionController/selectioncontroller.h"
#include "../../Utils/rootaction.h"
#include "../../Utils/actiongroup.h"
#include "../../Widgets/Dialogs/shortcutdialog.h"
#include "../../theme.h"


class ViewController;
class ActionController : public QObject
{
    Q_OBJECT

private:
    bool gotRegenAndJava();
    RootAction* createRootAction(const QString& category, const QString& name, const QString& actionHash, const QString& iconPath = "", const QString& aliasPath = "");
    
    void createRecentProjectAction(QString fileName);
    void recentProjectsChanged();
    void setupActions();
    void setupMainMenu();
    void setupApplicationToolbar();
    void setupContextToolbar();
    void setupRecentProjects();

signals:
    void recentProjectsUpdated();

private slots:
    void showShortcutDialog();
    void clearRecentProjects();
    void addRecentProject(QString file_path);
    void removeRecentProject(QString file_path);
    
    void settingChanged(SETTINGS key, const QVariant& value);
    void jenkinsValidated(bool success);
    void gotJava(bool java);
    void gotRe(bool re);
    void gotRegen(bool regen);

    void selectionChanged(int selectionSize);
    void actionFinished();
    
    void ModelControllerReady(bool ready);
    void themeChanged();

    void updateJenkinsActions();
    void updateReActions();
    void updateUndoRedo();
    void updateActions();

    QAction* getSettingAction(SETTINGS key);

public:
    // TODO - Remove enum???
    enum ACTION{};
    explicit ActionController(ViewController* vc);
    
    void connectSelectionController();
    void connectViewController(ViewController* controller);
    
    void updateIcon(RootAction* action, Theme* theme = Theme::theme());
    
    QList<RootAction*> getRecentProjectActions();
    QList<QAction*> getAllActions();
    QList<QAction*> getNodeViewActions();

    SelectionController* selectionController = nullptr;
    ViewController* viewController = nullptr;
    
    QList<RootAction*> allActions;
    QHash<QString, RootAction*> actionHash;
    QMultiMap<QString, RootAction*> actionCategoryMap;

    QHash<ACTION, RootAction*> rootActionHash;

    ActionGroup* applicationToolbar = nullptr;
    ActionGroup* contextToolbar = nullptr;
    ActionGroup* recentProjects = nullptr;

    QAction* toggleDock = nullptr;

    QAction* toolbar_context = nullptr;
    QAction* toolbar_undo = nullptr;
    QAction* toolbar_redo = nullptr;
    QAction* toolbar_cut = nullptr;
    QAction* toolbar_copy = nullptr;
    QAction* toolbar_paste = nullptr;
    QAction* toolbar_replicate = nullptr;
    QAction* toolbar_fitToScreen = nullptr;
    QAction* toolbar_centerOn = nullptr;
    QAction* toolbar_viewInNewWindow = nullptr;
    QAction* toolbar_sort = nullptr;
    QAction* toolbar_delete = nullptr;
    QAction* toolbar_alignVertical = nullptr;
    QAction* toolbar_alignHorizontal = nullptr;
    QAction* toolbar_search = nullptr;
    QAction* toolbar_contract = nullptr;
    QAction* toolbar_expand = nullptr;
    QAction* toolbar_validate = nullptr;

    RootAction* dock_addPart = nullptr;
    RootAction* dock_deploy = nullptr;

    RootAction* file_recentProjects_clearHistory = nullptr;
    RootAction* file_newProject = nullptr;
    RootAction* file_openProject = nullptr;
    RootAction* file_saveProject = nullptr;
    RootAction* file_saveAsProject = nullptr;
    RootAction* file_closeProject = nullptr;
    RootAction* file_importGraphML = nullptr;
    RootAction* file_exit = nullptr;

    RootAction* edit_undo = nullptr;
    RootAction* edit_redo = nullptr;
    RootAction* edit_cut = nullptr;
    RootAction* edit_copy = nullptr;
    RootAction* edit_paste = nullptr;
    RootAction* edit_replicate = nullptr;
    RootAction* edit_delete = nullptr;
    RootAction* edit_search = nullptr;
    RootAction* edit_goto = nullptr;
    
    RootAction* edit_clearSelection = nullptr;
    RootAction* edit_selectAll = nullptr;
    RootAction* edit_alignHorizontal = nullptr;
    RootAction* edit_alignVertical = nullptr;
    RootAction* edit_CycleActiveSelectionForward = nullptr;
    RootAction* edit_CycleActiveSelectionBackward = nullptr;
    RootAction* edit_renameActiveSelection = nullptr;
    RootAction* edit_expand = nullptr;
    RootAction* edit_contract = nullptr;
    
    RootAction* edit_incrementIndex = nullptr;
    RootAction* edit_decrementIndex = nullptr;
    RootAction* edit_incrementRow = nullptr;
    RootAction* edit_decrementRow = nullptr;

    RootAction* view_fitView = nullptr;
    RootAction* view_fitAllViews = nullptr;
    RootAction* view_centerOn = nullptr;
    RootAction* view_centerOnDefn = nullptr;
    RootAction* view_viewDefnInNewWindow = nullptr;
    RootAction* view_centerOnImpl = nullptr;
    RootAction* view_viewImplInNewWindow = nullptr;
    RootAction* view_viewConnections = nullptr;
    RootAction* view_viewInstances = nullptr;
    RootAction* view_viewInNewWindow = nullptr;
    RootAction* view_zoomIn = nullptr;
    RootAction* view_zoomOut = nullptr;

    RootAction* model_validateModel = nullptr;
    RootAction* model_selectModel = nullptr;
    RootAction* model_getCodeForComponent = nullptr;
    RootAction* model_generateModelWorkspace = nullptr;
    RootAction* model_executeLocalJob = nullptr;
    RootAction* model_reloadWorkerDefinitions = nullptr;

    RootAction* model_queryRunningExperiments = nullptr;
    RootAction* model_displayExperimentDataflow = nullptr;

    RootAction* options_settings = nullptr;

    RootAction* help_aboutMedea = nullptr;
    RootAction* help_wiki = nullptr;
    RootAction* help_aboutQt = nullptr;
    RootAction* help_shortcuts = nullptr;
    RootAction* help_reportBug = nullptr;

    RootAction* jenkins_importNodes = nullptr;
    RootAction* jenkins_executeJob = nullptr;
    RootAction* jenkins_listJobs = nullptr;
    RootAction* jenkins_showBrowser = nullptr;

    RootAction* toolbar_contextToolbar = nullptr;
    RootAction* toolbar_addChild = nullptr;
    RootAction* toolbar_connect = nullptr;
    RootAction* toolbar_popOutDefn = nullptr;
    RootAction* toolbar_popOutImpl = nullptr;
    RootAction* toolbar_wiki = nullptr;
    RootAction* toolbar_replicateCount = nullptr;
    RootAction* toolbar_displayedChildrenOption = nullptr;

    RootAction* toolbar_addDDSQOSProfile = nullptr;
    RootAction* toolbar_removeDDSQOSProfile = nullptr;

    RootAction* chart_viewInChart = nullptr;

    bool got_valid_jenkins = false;
    bool got_java = false;
    bool got_re = false;
    bool got_regen = false;

    QMenu* menu_file = nullptr;
    QMenu* menu_file_recentProjects = nullptr;
    QMenu* menu_edit = nullptr;
    QMenu* menu_view = nullptr;
    QMenu* menu_model = nullptr;
    QMenu* menu_jenkins = nullptr;
    QMenu* menu_help = nullptr;
    QMenu* menu_options = nullptr;

    ShortcutDialog* shortcutDialog = nullptr;

    QSignalMapper* recentProjectMapper = nullptr;
    QHash<QString, RootAction*> recentProjectActions;
    QStringList recentProjectKeys;

    QList<QAction*> view_actions;
};

#endif // ACTIONCONTROLLER_H
