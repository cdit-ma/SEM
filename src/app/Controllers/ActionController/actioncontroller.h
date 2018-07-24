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
public:
    enum ACTION{};
    explicit ActionController(ViewController* vc);

    void connectSelectionController();

    void connectViewController(ViewController* controller);

    void updateIcon(RootAction* action, Theme* theme = Theme::theme());
    QList<RootAction*> getRecentProjectActions();

    QList<QAction*> getAllActions();
    QList<QAction*> getNodeViewActions();
private:
    RootAction* createRootAction(QString category, QString name, QString actionHash, QString iconPath="", QString aliasPath="");
signals:
    void recentProjectsUpdated();
private slots:
    void showShortcutDialog();
    void clearRecentProjects();
    void addRecentProject(QString file_path);
    void removeRecentProject(QString file_path);
    
    void settingChanged(SETTINGS key, QVariant value);
    void jenkinsValidated(bool success);
    void gotJava(bool java);
    void gotRe(bool re);

    void selectionChanged(int selectionSize);
    void actionFinished();
    
    void ModelControllerReady(bool ready);
    void themeChanged();

    void updateJenkinsActions();
    void updateReActions();
    void updateUndoRedo();

    QAction* getSettingAction(SETTINGS key);

    void updateActions();
public:
    SelectionController* selectionController;
    ViewController* viewController;
    QList<RootAction*> allActions;
    QHash<QString, RootAction*> actionHash;
    QMultiMap<QString, RootAction*> actionCategoryMap;

    QHash<ACTION, RootAction*> rootActionHash;

    ActionGroup* applicationToolbar;
    ActionGroup* contextToolbar;

    ActionGroup* recentProjects;

    QAction* toggleDock;

    QAction* toolbar_context;
    QAction* toolbar_undo;
    QAction* toolbar_redo;
    QAction* toolbar_cut;
    QAction* toolbar_copy;
    QAction* toolbar_paste;
    QAction* toolbar_replicate;
    QAction* toolbar_fitToScreen;
    QAction* toolbar_centerOn;
    QAction* toolbar_viewInNewWindow;
    QAction* toolbar_sort;
    QAction* toolbar_delete;
    QAction* toolbar_alignVertical;
    QAction* toolbar_alignHorizontal;
    QAction* toolbar_search; 
    QAction* toolbar_contract;
    QAction* toolbar_expand;
    QAction* toolbar_validate;

    RootAction* file_recentProjects_clearHistory;
    RootAction* file_newProject;
    RootAction* file_importGraphML;
    
    
    RootAction* file_openProject;
    RootAction* file_saveProject;
    RootAction* file_saveAsProject;
    RootAction* file_closeProject;
    RootAction* file_exit;

    RootAction* edit_undo;
    RootAction* edit_redo;
    RootAction* edit_cut;
    RootAction* edit_copy;
    RootAction* edit_paste;
    RootAction* edit_replicate;
    RootAction* edit_delete;
    RootAction* edit_search;
    RootAction* edit_goto;
    

    RootAction* dock_addPart;
    RootAction* dock_deploy;

    //RootAction* edit_sort;
    RootAction* edit_clearSelection;
    RootAction* edit_selectAll;
    RootAction* edit_alignHorizontal;
    RootAction* edit_alignVertical;
    RootAction* edit_CycleActiveSelectionForward;
    RootAction* edit_CycleActiveSelectionBackward;
    RootAction* edit_renameActiveSelection;
    RootAction* edit_expand;
    RootAction* edit_contract;
    
    RootAction* edit_incrementIndex;
    RootAction* edit_decrementIndex;

    RootAction* edit_incrementRow;
    RootAction* edit_decrementRow;
    
    

    RootAction* view_fitView;
    RootAction* view_fitAllViews;

    RootAction* view_centerOn;
    RootAction* view_centerOnDefn;
    RootAction* view_viewDefnInNewWindow;
    RootAction* view_centerOnImpl;
    RootAction* view_viewImplInNewWindow;
    RootAction* view_viewConnections;
    RootAction* view_viewInNewWindow;
    RootAction* view_zoomIn;
    RootAction* view_zoomOut;



    RootAction* model_validateModel;
    RootAction* model_selectModel;
    RootAction* model_getCodeForComponent;
    RootAction* model_generateModelWorkspace;
    RootAction* model_executeLocalJob;

    RootAction* options_settings;

    RootAction* help_aboutMedea;
    RootAction* help_wiki;
    RootAction* help_aboutQt;
    RootAction* help_shortcuts;
    RootAction* help_reportBug;

    RootAction* jenkins_importNodes;
    RootAction* jenkins_executeJob;
    RootAction* jenkins_listJobs;
    RootAction* jenkins_showBrowser;
    RootAction* toolbar_contextToolbar;

    RootAction* toolbar_addChild;
    RootAction* toolbar_connect;
    RootAction* toolbar_popOutDefn;
    RootAction* toolbar_popOutImpl;
    
    RootAction* toolbar_wiki;
    RootAction* toolbar_replicateCount;
    RootAction* toolbar_displayedChildrenOption;


    RootAction* toolbar_addDDSQOSProfile;
    RootAction* toolbar_removeDDSQOSProfile;

    bool got_valid_jenkins = false;
    bool got_java = false;
    bool got_re = false;

    QMenu* menu_file;
    QMenu* menu_file_recentProjects;
    QMenu* menu_edit;
    QMenu* menu_view;
    QMenu* menu_model;
    QMenu* menu_jenkins;
    QMenu* menu_help;
    QMenu* menu_options;

    ShortcutDialog* shortcutDialog;




    QSignalMapper* recentProjectMapper;
    QSignalMapper* readOnlyMapper;
    QHash<QString, RootAction*> recentProjectActions;
    QStringList recentProjectKeys;

    QList<QAction*> view_actions;

private:

    void createRecentProjectAction(QString fileName);
    void recentProjectsChanged();
    void setupActions();
    void setupMainMenu();
    void setupApplicationToolbar();
    void setupContextToolbar();
    void setupRecentProjects();
};

#endif // ACTIONCONTROLLER_H
