#ifndef ACTIONCONTROLLER_H
#define ACTIONCONTROLLER_H

#include <QObject>
#include <QAction>
#include <QMenu>
#include "../Widgets/New/actiongroup.h"
#include "../Widgets/New/selectioncontroller.h"
#include "rootaction.h"
#include "../view/theme.h"
class ViewController;
class ActionController : public QObject
{
    Q_OBJECT
public:
    enum ACTION{};
    explicit ActionController(ViewController* vc);

    void connectSelectionController(SelectionController* controller);

    RootAction* getRootAction(QString actionKey);
    void connectViewController(ViewController* controller);

    void updateIcon(RootAction* action, Theme* theme = Theme::theme());

private:
    RootAction* createRootAction(QString name, QString actionHash, QString iconPath="", QString aliasPath="");

private slots:
    void settingChanged(SETTING_KEY key, QVariant value);
    void jenkinsValidated(bool success);
    void selectionChanged(int selectionSize);
    void actionFinished();

    void controllerReady(bool ready);
    void modelReady(bool ready);
    void themeChanged();

    void updateJenkinsActions();
    void updateUndoRedo();

    QAction* getSettingAction(SETTING_KEY key);

    void updateActions();
public:


    SelectionController* selectionController;
    ViewController* viewController;
    QList<RootAction*> allActions;
    QHash<QString, RootAction*> actionHash;

    QHash<ACTION, RootAction*> rootActionHash;

    ActionGroup* applicationToolbar;
    ActionGroup* contextToolbar;

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

    RootAction* file_recentProjects_clearHistory;
    RootAction* file_newProject;
    RootAction* file_importGraphML;
    RootAction* file_importXME;
    RootAction* file_importXMI;
    RootAction* file_openProject;
    RootAction* file_saveProject;
    RootAction* file_saveAsProject;
    RootAction* file_closeProject;
    RootAction* file_importSnippet;
    RootAction* file_exportSnippet;
    RootAction* file_exit;

    RootAction* edit_undo;
    RootAction* edit_redo;
    RootAction* edit_cut;
    RootAction* edit_copy;
    RootAction* edit_paste;
    RootAction* edit_replicate;
    RootAction* edit_delete;
    RootAction* edit_search;
    RootAction* edit_sort;
    RootAction* edit_clearSelection;
    RootAction* edit_selectAll;
    RootAction* edit_alignHorizontal;
    RootAction* edit_alignVertical;
    RootAction* edit_CycleActiveSelectionForward;
    RootAction* edit_CycleActiveSelectionBackward;
    RootAction* edit_renameActiveSelection;

    RootAction* view_fitView;
    RootAction* view_centerOn;
    RootAction* view_centerOnDefn;
    RootAction* view_centerOnImpl;
    RootAction* view_viewConnections;
    RootAction* view_viewInNewWindow;

    RootAction* window_printScreen;
    RootAction* window_displayMinimap;

    RootAction* model_validateModel;
    RootAction* model_clearModel;
    RootAction* model_executeLocalJob;

    RootAction* options_settings;

    RootAction* help_aboutMedea;
    RootAction* help_wiki;
    RootAction* help_aboutQt;
    RootAction* help_shortcuts;
    RootAction* help_reportBug;

    RootAction* jenkins_importNodes;
    RootAction* jenkins_executeJob;


    RootAction* toolbar_contextToolbar;

    RootAction* toolbar_addChild;
    RootAction* toolbar_connect;
    RootAction* toolbar_hardware;
    RootAction* toolbar_disconnectHardware;
    RootAction* toolbar_popOutDefn;
    RootAction* toolbar_popOutImpl;
    //RootAction* toolbar_popOutInst;
    RootAction* toolbar_getCPP;
    RootAction* toolbar_setReadOnly;
    RootAction* toolbar_unsetReadOnly;
    RootAction* toolbar_expand;
    RootAction* toolbar_contract;
    RootAction* toolbar_wiki;
    RootAction* toolbar_replicateCount;
    RootAction* toolbar_displayedChildrenOption;
    RootAction* toolbar_addDDSQOSProfile;

    bool _controllerReady;
    bool _modelReady;
    bool _jenkinsValidated;

    QMenu* menu_file;
    QMenu* menu_file_recentProjects;
    QMenu* menu_edit;
    QMenu* menu_view;
    QMenu* menu_model;
    QMenu* menu_jenkins;
    QMenu* menu_help;
    QMenu* menu_window;
    QMenu* menu_options;

private:
    void setupActions();
    void setupMainMenu();
    void setupApplicationToolbar();
    void setupContextToolbar();
};

#endif // ACTIONCONTROLLER_H
