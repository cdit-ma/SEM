#include "actioncontroller.h"
#include "../ViewController/viewcontroller.h"
#include "../../theme.h"
#include "../../Utils/filehandler.h"
#include "../../Utils/rootaction.h"
#include <QDebug>
#include "../../ModelController/nodekinds.h"
#include "../../ModelController/modelcontroller.h"
ActionController::ActionController(ViewController* vc) : QObject(vc)
{

    viewController = vc;
    shortcutDialog = 0;
    recentProjectMapper = 0;

    selectionController = viewController->getSelectionController();

    _controllerReady = false;
    _modelReady = false;
    _jenkinsValidated = false;
    _gotJava = false;
    setupActions();

    setupMainMenu();
    setupApplicationToolbar();
    setupContextToolbar();

    setupRecentProjects();


    connect(SettingsController::settings(), &SettingsController::settingChanged, this, &ActionController::settingChanged);
    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    connect(Theme::theme(), SIGNAL(refresh_Icons()), this, SLOT(themeChanged()));

    themeChanged();
    connectViewController(vc);
    connectSelectionController();

    updateActions();
}

void ActionController::connectViewController(ViewController *controller)
{

    if(viewController){
        connect(controller, &ViewController::vc_actionFinished, this, &ActionController::actionFinished);
        connect(controller, &ViewController::vc_controllerReady, this, &ActionController::controllerReady);
        connect(controller, &ViewController::mc_modelReady, this, &ActionController::modelReady);
        connect(controller, &ViewController::vc_JenkinsReady, this, &ActionController::jenkinsValidated);
        connect(controller, &ViewController::vc_JavaReady, this, &ActionController::gotJava);

        connect(controller, &ViewController::mc_undoRedoUpdated, this, &ActionController::updateUndoRedo);
        connect(controller, &ViewController::vc_addProjectToRecentProjects, this, &ActionController::updateRecentProjects);


        connect(file_newProject, &QAction::triggered, viewController, &ViewController::newProject);
        connect(file_openProject, &QAction::triggered, viewController, &ViewController::openProject);
        connect(file_closeProject, &QAction::triggered, viewController, &ViewController::closeProject);
        connect(file_saveProject, &QAction::triggered, viewController, &ViewController::saveProject);
        connect(file_saveAsProject, &QAction::triggered, viewController, &ViewController::saveAsProject);
        connect(file_importGraphML, &QAction::triggered, viewController, &ViewController::importProjects);
        connect(file_exit, &QAction::triggered, viewController, &ViewController::closeMEDEA);

        connect(edit_undo, &QAction::triggered, viewController, &ViewController::vc_undo);
        connect(edit_redo, &QAction::triggered, viewController, &ViewController::vc_redo);
        connect(edit_cut, &QAction::triggered, viewController, &ViewController::cut);
        connect(edit_copy, &QAction::triggered, viewController, &ViewController::copy);
        connect(edit_paste, &QAction::triggered, viewController, &ViewController::paste);
        connect(edit_replicate, &QAction::triggered, viewController, &ViewController::replicate);
        connect(view_fitView, &QAction::triggered, viewController, &ViewController::fitView);
        connect(view_fitAllViews, &QAction::triggered, viewController, &ViewController::fitAllViews);

        connect(edit_alignHorizontal, &QAction::triggered, viewController, &ViewController::alignSelectionHorizontal);
        connect(edit_alignVertical, &QAction::triggered, viewController, &ViewController::alignSelectionVertical);


        connect(view_centerOn, &QAction::triggered, viewController, &ViewController::centerSelection);
        connect(edit_delete, &QAction::triggered, viewController, &ViewController::deleteSelection);
        connect(edit_renameActiveSelection, &QAction::triggered, viewController, &ViewController::editLabel);
        connect(toolbar_replicateCount, &QAction::triggered, viewController, &ViewController::editReplicationCount);

        connect(view_centerOnImpl, &QAction::triggered, viewController, &ViewController::centerImpl);
        connect(view_centerOnDefn, &QAction::triggered, viewController, &ViewController::centerDefinition);
        connect(view_viewDefnInNewWindow, &QAction::triggered, viewController, &ViewController::popupDefinition);
        connect(view_viewImplInNewWindow, &QAction::triggered, viewController, &ViewController::popupImpl);
        connect(view_viewInNewWindow, &QAction::triggered, viewController, &ViewController::popupSelection);
        connect(view_viewConnections, &QAction::triggered, viewController, &ViewController::selectAndCenterConnectedEntities);


        connect(toolbar_wiki, &QAction::triggered, viewController, &ViewController::showWikiForSelectedItem);
        connect(help_wiki, &QAction::triggered, viewController, &ViewController::showWiki);
        connect(help_reportBug, &QAction::triggered, viewController, &ViewController::reportBug);
        connect(help_aboutQt, &QAction::triggered, viewController, &ViewController::aboutQt);
        connect(help_aboutMedea, &QAction::triggered, viewController, &ViewController::aboutMEDEA);


        connect(edit_expand, &QAction::triggered, viewController, &ViewController::expandSelection);
        connect(edit_contract, &QAction::triggered, viewController, &ViewController::contractSelection);


        connect(jenkins_executeJob, &QAction::triggered, viewController, &ViewController::executeJenkinsJob);





        connect(options_settings, &QAction::triggered, SettingsController::settings(), &SettingsController::showSettingsWidget);

        connect(toolbar_addDDSQOSProfile, &QAction::triggered, viewController, &ViewController::constructDDSQOSProfile);

        connect(model_getCodeForComponent, &QAction::triggered, viewController, &ViewController::getCodeForComponent);
        connect(model_validateModel, &QAction::triggered, viewController, &ViewController::validateModel);
        connect(model_selectModel, &QAction::triggered, viewController, &ViewController::selectModel);
        connect(model_generateModelWorkspace, &QAction::triggered, viewController, &ViewController::generateWorkspace);

        //connect(model_executeLocalJob, &QAction::triggered, viewController, &ViewController::launchLocalDeployment);
        //connect(file_importXME, &QAction::triggered, viewController, &ViewController::importXMEProject);
        //connect(file_importXMI, &QAction::triggered, viewController, &ViewController::importXMIProject);
        connect(file_recentProjects_clearHistory, &QAction::triggered, this, &ActionController::clearRecentProjects);

        connect(help_shortcuts, &QAction::triggered, this, &ActionController::showShortcutDialog);



    }
}

void ActionController::connectSelectionController()
{
    if(selectionController){
        connect(selectionController, &SelectionController::selectionChanged, this, &ActionController::selectionChanged);
        connect(edit_CycleActiveSelectionForward, SIGNAL(triggered(bool)), selectionController, SLOT(cycleActiveSelectionForward()));
        connect(edit_CycleActiveSelectionBackward, SIGNAL(triggered(bool)), selectionController, SLOT(cycleActiveSelectionBackward()));
        connect(edit_selectAll, SIGNAL(triggered(bool)), selectionController, SIGNAL(selectAll()));
        connect(edit_clearSelection, SIGNAL(triggered(bool)), selectionController, SIGNAL(clearSelection()));
    }
}


/**
 * @brief ActionController::getRootAction
 * This will return a copy of the root action specified by kind if it exists.
 * If it doesn't, it will construct the root action and then return a copy of it.
 * @param action
 * @param stealth
 * @return
 */
RootAction* ActionController::getRootAction(QString actionKey)
{

    RootAction* action = 0;
    if(actionHash.contains(actionKey)){
        action = actionHash[actionKey];
    }
    return action;
}

RootAction *ActionController::createRootAction(QString category, QString name, QString hashKey, QString iconPath, QString aliasPath)
{
    RootAction* action = new RootAction(category, name, this);
    action->setIconPath(iconPath, aliasPath);
    if(hashKey != ""){
        actionHash[hashKey] = action;
    }
    if(!actionCategoryMap.contains(category, action)){
        actionCategoryMap.insertMulti(category, action);
    }
    allActions.append(action);
    return action;
}

void ActionController::showShortcutDialog()
{
    if(!shortcutDialog){
        shortcutDialog = new ShortcutDialog(0);

        QStringList keyOrders;
        keyOrders << "Project" << "Edit" << "Selection" << "View" << "Model" << "Help";

        foreach(QString title, keyOrders){
            shortcutDialog->addTitle(title, "Icons", title);

            //Go Through backwards.
            QList<RootAction*> actions = actionCategoryMap.values(title);
            for(int i = actions.size() - 1; i >= 0; i--){
                RootAction* action = actions.at(i);
                if(action && !action->shortcut().isEmpty()){
                    shortcutDialog->addShortcut(action->shortcut().toString(), action->text(), action->getIconPath(), action->getIconAlias());
                }
            }
        }
        shortcutDialog->resizeTable();
    }
    shortcutDialog->show();
}

void ActionController::clearRecentProjects()
{
    SettingsController::settings()->setSetting(SK_GENERAL_RECENT_PROJECTS, QStringList());
}

void ActionController::updateRecentProjects(QString filePath)
{
    //Sanitize!
    filePath = FileHandler::sanitizeFilePath(filePath);

    QStringList files = recentProjectKeys;

    //Get the index of the filename opened (if it exists)
    files.removeAll(filePath);
    files.insert(0, filePath);

    while(files.size() > 8){
        files.removeLast();
    }
    SettingsController::settings()->setSetting(SK_GENERAL_RECENT_PROJECTS, files);
}

void ActionController::settingChanged(SETTING_KEY key, QVariant value)
{
    bool boolVal = value.toBool();

    QAction* action = getSettingAction(key);

    if(action){
        action->setVisible(boolVal);
    }

    if(key == SK_GENERAL_RECENT_PROJECTS){
        recentProjectsChanged();
    }
}

void ActionController::jenkinsValidated(bool success)
{
    if(_jenkinsValidated != success){
        _jenkinsValidated = success;
        updateJenkinsActions();
    }
}

void ActionController::gotJava(bool java)
{
    if(_gotJava != java){
        _gotJava = java;
        updateJenkinsActions();
    }
}

void ActionController::selectionChanged(int selectionSize)
{
    if(selectionController){
        if(selectionSize == -1){
            selectionSize = selectionController->getSelectionCount();
        }
        auto selection = selectionController->getSelectionIDs();

        bool controllerReady = viewController->isControllerReady();
        bool modelReady = viewController->isModelReady();

        bool modelActions = controllerReady && modelReady;
        auto model_controller = viewController->getModelController();

        bool gotSingleSelection = modelActions && (selectionSize == 1);
        bool gotSelection = modelActions && (selectionSize > 0);
        bool gotMultipleSelection = modelActions && (selectionSize > 1);


        ViewItem* singleItem = selectionController->getActiveSelectedItem();

        bool hasDefn = false;
        bool hasImpl = false;
        bool hasCode = false;
        bool hasComponentAssembly = false;
        bool canLock = false;

        if(gotSingleSelection && singleItem && singleItem->isNode()){
            NodeViewItem* node = (NodeViewItem*) singleItem;
            NODE_KIND kind = node->getNodeKind();

            hasDefn = node->isNodeOfType(NODE_TYPE::INSTANCE) || node->isNodeOfType(NODE_TYPE::IMPLEMENTATION);
            hasImpl = hasDefn || node->isNodeOfType(NODE_TYPE::DEFINITION);
            canLock = !(node->isNodeOfType(NODE_TYPE::ASPECT) || kind == NODE_KIND::MODEL);

            hasCode = kind == NODE_KIND::COMPONENT || kind == NODE_KIND::COMPONENT_INSTANCE || kind == NODE_KIND::COMPONENT_IMPL;
            hasComponentAssembly = kind == NODE_KIND::COMPONENT_ASSEMBLY;
        }

        

        toolbar_wiki->setEnabled(gotSelection);

        edit_expand->setEnabled(gotSelection);
        edit_contract->setEnabled(gotSelection);


        model_getCodeForComponent->setEnabled(_gotJava && hasCode);

        view_centerOnDefn->setEnabled(hasDefn);
        view_viewDefnInNewWindow->setEnabled(hasDefn);

        view_centerOnImpl->setEnabled(hasImpl);
        view_viewImplInNewWindow->setEnabled(hasImpl);

        auto mc = model_controller;

        edit_cut->setEnabled(mc && mc->canCut(selection));
        edit_copy->setEnabled(mc && mc->canCopy(selection));
        edit_paste->setEnabled(mc && mc->canPaste(selection));
        edit_delete->setEnabled(mc && mc->canRemove(selection));

        edit_replicate->setEnabled(gotSelection);
        
        //edit_sort->setEnabled(gotSelection);
        edit_renameActiveSelection->setEnabled(gotSelection);
        edit_clearSelection->setEnabled(gotMultipleSelection);
        edit_selectAll->setEnabled(gotSingleSelection);


        edit_alignHorizontal->setEnabled(gotMultipleSelection);
        edit_alignVertical->setEnabled(gotMultipleSelection);

        edit_CycleActiveSelectionForward->setEnabled(gotMultipleSelection);
        edit_CycleActiveSelectionBackward->setEnabled(gotMultipleSelection);

        toolbar_replicateCount->setEnabled(hasComponentAssembly);

        view_fitView->setEnabled(modelActions);
        view_fitAllViews->setEnabled(modelActions);


        view_centerOn->setEnabled(gotSelection);

        view_viewInNewWindow->setEnabled(gotSingleSelection);

        view_viewConnections->setEnabled(gotSelection);



        


        applicationToolbar->updateSpacers();
    }
}

void ActionController::actionFinished()
{
    selectionChanged(-1);
}

void ActionController::controllerReady(bool)
{
    //Do we need to do this?
    //updateActions();
}

void ActionController::modelReady(bool)
{
    updateActions();
}

void ActionController::themeChanged()
{
    Theme* theme = Theme::theme();

    foreach(RootAction* action, allActions){
        updateIcon(action, theme);
    }

    foreach(RootAction* action, recentProjectActions.values()){
        updateIcon(action, theme);
    }


    menu_file_recentProjects->setIcon(theme->getIcon("Icons", "clock"));
}

void ActionController::updateJenkinsActions()
{
    bool modelReady = viewController->isModelReady();

    jenkins_importNodes->setEnabled(modelReady && _jenkinsValidated);
    jenkins_executeJob->setEnabled(modelReady && _jenkinsValidated);

    model_generateModelWorkspace->setEnabled(modelReady && _gotJava);
    model_validateModel->setEnabled(modelReady && _gotJava);
}

void ActionController::updateUndoRedo()
{
    if(viewController){
        bool controllerReady = viewController->isControllerReady();
        bool modelReady = viewController->isModelReady();

        bool modelActions = controllerReady && modelReady;
        edit_undo->setEnabled(modelActions && viewController->canUndo());
        edit_redo->setEnabled(modelActions && viewController->canRedo());
    }
}

QAction *ActionController::getSettingAction(SETTING_KEY key)
{
    switch(key){
    case SK_TOOLBAR_CONTEXT:
        return toolbar_context;
    case SK_TOOLBAR_UNDO:
        return toolbar_undo;
    case SK_TOOLBAR_REDO:
        return toolbar_redo;
    case SK_TOOLBAR_CUT:
        return toolbar_cut;
    case SK_TOOLBAR_COPY:
        return toolbar_copy;
    case SK_TOOLBAR_PASTE:
        return toolbar_paste;
    case SK_TOOLBAR_REPLICATE:
        return toolbar_replicate;
    case SK_TOOLBAR_FIT_TO_SCREEN:
        return toolbar_fitToScreen;
    case SK_TOOLBAR_CENTER_SELECTION:
        return toolbar_centerOn;
    case SK_TOOLBAR_VIEW_IN_NEWWINDOW:
        return toolbar_viewInNewWindow;
    case SK_TOOLBAR_DELETE:
        return toolbar_delete;
    case SK_TOOLBAR_ALIGN_HORIZONTAL:
        return toolbar_alignVertical;
    case SK_TOOLBAR_ALIGN_VERTICAL:
        return toolbar_alignHorizontal;
    case SK_TOOLBAR_SEARCH:
        return toolbar_search;
    case SK_TOOLBAR_CONTRACT:
        return toolbar_contract;
    case SK_TOOLBAR_EXPAND:
        return toolbar_expand;
    default:
        return 0;
    }
}

void ActionController::updateActions()
{
    bool controllerReady = viewController->isControllerReady();
    bool modelReady = viewController->isModelReady();

    bool modelActions = controllerReady && modelReady;

    file_newProject->setEnabled(controllerReady);
    file_openProject->setEnabled(controllerReady);
    file_closeProject->setEnabled(controllerReady);



    file_importGraphML->setEnabled(modelActions);
    //file_importXME->setEnabled(modelActions);
    //file_importXMI->setEnabled(modelActions);
    file_saveProject->setEnabled(modelActions);
    file_saveAsProject->setEnabled(modelActions);
    file_closeProject->setEnabled(modelActions);

    edit_undo->setEnabled(modelActions);
    edit_redo->setEnabled(modelActions);

    model_selectModel->setEnabled(modelActions);
    model_validateModel->setEnabled(modelActions);
    model_generateModelWorkspace->setEnabled(modelActions);
    //model_executeLocalJob->setEnabled(modelActions);

    edit_search->setEnabled(modelActions);
    view_fitView->setEnabled(modelActions);
    view_fitAllViews->setEnabled(modelActions);


    //jenkins_importNodes->setEnabled(modelActions);
    //jenkins_executeJob->setEnabled(modelActions);
    toolbar_contextToolbar->setEnabled(modelActions);

    toolbar_addDDSQOSProfile->setEnabled(modelActions);

    //Update the selection changed with a special thing.
    selectionChanged(-1);

    updateUndoRedo();
    updateJenkinsActions();
}

void ActionController::createRecentProjectAction(QString fileName)
{
    if(!recentProjectActions.contains(fileName)){
        RootAction* action = new RootAction("Project", fileName, this);
        action->setIconPath("Icons", "file");
        updateIcon(action);
        recentProjectActions.insert(fileName, action);
        connect(action, &QAction::triggered, recentProjectMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
        recentProjectMapper->setMapping(action, fileName);
    }
}

void ActionController::recentProjectsChanged()
{
    if(!recentProjectMapper){
        recentProjectMapper = new QSignalMapper(this);
        connect(recentProjectMapper, static_cast<void(QSignalMapper::*)(const QString &)>(&QSignalMapper::mapped),viewController, &ViewController::openExistingProject);
    }

    //Load in the defaults.
    QStringList list = SettingsController::settings()->getSetting(SK_GENERAL_RECENT_PROJECTS).toStringList();

    QStringList orderedKeys;

    foreach(QString filepath, list){
        filepath = FileHandler::sanitizeFilePath(filepath);
        createRecentProjectAction(filepath);
        orderedKeys.append(filepath);
    }
    foreach(QString oldKey, recentProjectKeys){
        if(orderedKeys.contains(oldKey)){
            continue;
        }else{
            RootAction* action = recentProjectActions.value(oldKey, 0);
            if(action){
                recentProjectActions.remove(oldKey);
                action->deleteLater();
            }
        }
    }
    if(orderedKeys != recentProjectKeys){

        //Update Menus
        menu_file_recentProjects->clear();

        foreach(QString key, orderedKeys){
            RootAction* action = recentProjectActions.value(key, 0);
            if(action){
                menu_file_recentProjects->addAction(action);
            }
        }

        menu_file_recentProjects->addSeparator();
        menu_file_recentProjects->addAction(file_recentProjects_clearHistory);
        recentProjectKeys = orderedKeys;
        emit recentProjectsUpdated();
    }

}

void ActionController::updateIcon(RootAction *action, Theme *theme)
{
    if(theme && action){
        auto pair = action->getIconPair();
        auto icon = theme->getIcon(pair);
        if(!icon.isNull()){
            action->setIcon(icon);
        }else{
            //qCritical() << pair;
        }
    }
}

QList<RootAction *> ActionController::getRecentProjectActions()
{
    QList<RootAction*> actions;

    foreach(QAction* action, menu_file_recentProjects->actions()){
        if(action->isSeparator()){
            continue;
        }
        if(action == file_recentProjects_clearHistory){
            continue;
        }
        RootAction* a = qobject_cast<RootAction*>(action);
        if(a){
            actions.append(a);
        }
    }

    return actions;
}

QList<QAction *> ActionController::getNodeViewActions()
{
    QList<QAction*> actions;
    foreach(RootAction* action, allActions){
        if(action && action->shortcutContext() == Qt::WidgetWithChildrenShortcut){
            actions.append(action);
        }
    }
    return actions;
}

void ActionController::setupActions()
{
    file_newProject = createRootAction("Project", "New Project", "", "Icons", "file");
    file_newProject->setToolTip("Construct a new project.");
    file_newProject->setShortcutContext(Qt::ApplicationShortcut);
    file_newProject->setShortcut(QKeySequence::New);

    file_openProject = createRootAction("Project", "Open Project", "", "Icons", "folder");
    file_openProject->setToolTip("Open an existing project.");
    file_openProject->setShortcutContext(Qt::ApplicationShortcut);
    file_openProject->setShortcut(QKeySequence::Open);

    file_recentProjects_clearHistory = createRootAction("Misc", "Clear History", "", "Icons", "cross");

    file_saveProject = createRootAction("Project", "Save Project", "", "Icons", "floppyDisk");
    file_saveProject->setToolTip("Save current project.");
    file_saveProject->setShortcutContext(Qt::ApplicationShortcut);
    file_saveProject->setShortcut(QKeySequence::Save);


    file_saveAsProject = createRootAction("Project", "Save Project As", "", "Icons", "floppyDisk");
    file_saveAsProject->setToolTip("Save current project into a different file.");
    file_saveAsProject->setShortcutContext(Qt::ApplicationShortcut);
    file_saveAsProject->setShortcut(QKeySequence::SaveAs);

    file_closeProject = createRootAction("Project", "Close Project", "", "Icons", "cross");
    file_closeProject->setToolTip("Close current project.");
    file_closeProject->setShortcutContext(Qt::ApplicationShortcut);
    file_closeProject->setShortcut(QKeySequence::Close);


    file_importGraphML = createRootAction("Project", "Import Project", "", "Icons", "clipboardDown");
    file_importGraphML->setToolTip("Import Project into current project.");
    file_importGraphML->setShortcutContext(Qt::ApplicationShortcut);
    file_importGraphML->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));


    //file_importXME = createRootAction("Project", "Import XME Project", "", "Icons", "gme");
    //file_importXME->setToolTip("Import XME Project into current project.");
    //file_importXMI = createRootAction("Project", "Import UML XMI Project", "", "Icons", "uml");
    //file_importXMI->setToolTip("Import XMI Project into current project.");

  
    edit_undo = createRootAction("Edit", "Undo", "", "Icons", "arrowUndo");
    edit_undo->setToolTip("Undo last model change.");
    edit_undo->setShortcutContext(Qt::ApplicationShortcut);
    edit_undo->setShortcut(QKeySequence::Undo);

    edit_redo = createRootAction("Edit", "Redo", "", "Icons", "arrowRedo");
    edit_redo->setToolTip("Redo last undo.");
    edit_redo->setShortcutContext(Qt::ApplicationShortcut);
    edit_redo->setShortcut(QKeySequence::Redo);


    edit_cut = createRootAction("Edit", "Cut", "", "Icons", "scissors");
    edit_cut->setToolTip("Cut selection.");
    edit_cut->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    edit_cut->setShortcut(QKeySequence::Cut);

    edit_copy = createRootAction("Edit", "Copy", "", "Icons", "copy");
    edit_copy->setToolTip("Copy selection.");
    edit_copy->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    edit_copy->setShortcut(QKeySequence::Copy);

    edit_paste = createRootAction("Edit", "Paste", "", "Icons", "clipboard");
    edit_paste->setToolTip("Paste clipboard into selected entity.");
    edit_paste->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    edit_paste->setShortcut(QKeySequence::Paste);

    edit_replicate = createRootAction("Edit", "Replicate", "", "Icons", "copyList");
    edit_replicate->setToolTip("Replicate the selected entities.");
    edit_replicate->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    edit_replicate->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));

    edit_delete = createRootAction("Edit", "Delete", "Delete", "Icons", "bin");
    edit_delete->setToolTip("Delete the selected entities.");
    edit_delete->setShortcut(QKeySequence::Delete);
    edit_delete->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    edit_renameActiveSelection = createRootAction("Edit", "Rename", "Rename", "Icons", "letterA");
    edit_renameActiveSelection->setToolTip("Rename the selected entity.");
    edit_renameActiveSelection->setShortcut(QKeySequence(Qt::Key_F2));
    edit_renameActiveSelection->setShortcutContext(Qt::WidgetWithChildrenShortcut);



    edit_search = createRootAction("Edit", "Search", "Root_Search", "Icons", "zoom");
    edit_search->setToolTip("Search model.");
    edit_search->setShortcutContext(Qt::ApplicationShortcut);
    edit_search->setShortcut(QKeySequence::Find);



    //edit_sort = createRootAction("Edit", "Sort", "", "Icons", "letterAZ");
    //edit_sort->setToolTip("Sort selection.");

    edit_alignVertical = createRootAction("Edit", "Align Vertically", "", "Icons", "alignVertical");
    edit_alignVertical->setToolTip("Align selection vertically.");

    edit_alignHorizontal = createRootAction("Edit", "Align Horizontally", "", "Icons", "alignHorizontal");
    edit_alignHorizontal->setToolTip("Align selection horizontally.");


    edit_CycleActiveSelectionForward = createRootAction("Selection", "Cycle Next Selected Item", "", "Icons", "arrowHeadRight");
    edit_CycleActiveSelectionForward->setToolTip("Cycle between active selected entities.");
    edit_CycleActiveSelectionForward->setShortcutContext(Qt::ApplicationShortcut);
    edit_CycleActiveSelectionForward->setShortcut(QKeySequence::NextChild);

    edit_CycleActiveSelectionBackward = createRootAction("Selection", "Cycle Prev Selected Item", "", "Icons", "arrowHeadLeft");
    edit_CycleActiveSelectionBackward->setToolTip("Cycle(Back) between active selected entities.");
    edit_CycleActiveSelectionBackward->setShortcut(QKeySequence::PreviousChild);
    edit_CycleActiveSelectionBackward->setShortcutContext(Qt::ApplicationShortcut);

    edit_selectAll = createRootAction("Selection", "Select All", "", "Icons", "gridSelect");
    edit_selectAll->setToolTip("Select all child entities of selection.");
    edit_selectAll->setShortcut(QKeySequence::SelectAll);
    edit_selectAll->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    edit_clearSelection = createRootAction("Selection", "Clear Selection", "", "Icons", "cross");
    edit_clearSelection->setToolTip("Clear selection.");
    edit_clearSelection->setShortcut(QKeySequence(Qt::Key_Escape));
    edit_clearSelection->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    view_fitView = createRootAction("View", "Fit View", "", "Icons", "screenResize");
    view_fitView->setToolTip("Center all entities in active view.");
    view_fitView->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    view_fitView->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Space));

    view_fitAllViews = createRootAction("View", "Fit All Views", "", "Icons", "screenResize");
    view_fitAllViews->setToolTip("Center all entities in all views.");
    view_fitAllViews->setShortcutContext(Qt::ApplicationShortcut);
    view_fitAllViews->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Space));

    view_centerOn = createRootAction("View", "Center On Selection", "", "Icons", "crosshair");

    edit_expand = createRootAction("Toolbar", "Expand Selection", "", "Icons", "triangleSouthEast");
    edit_contract = createRootAction("Toolbar", "Contract Selection", "", "Icons", "triangleNorthWest");

    view_centerOnDefn = createRootAction("View", "Center On Definition", "", "Icons", "bracketsCurly");
    view_centerOnDefn->setToolTip("Center selected entity's Definition.");
    view_centerOnDefn->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    view_centerOnDefn->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_D));

    view_centerOnImpl = createRootAction("View", "Center On Implementation", "", "Icons", "gears");
    view_centerOnImpl->setToolTip("Center selected entity's Implementation.");
    view_centerOnImpl->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    view_centerOnImpl->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_I));

    view_viewDefnInNewWindow = createRootAction("View", "Show Definition in New Window", "", "Icons", "bracketsCurly");
    view_viewDefnInNewWindow->setToolTip("Popout selected entity's Definition.");
    view_viewDefnInNewWindow->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    view_viewDefnInNewWindow->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_D));

    view_viewImplInNewWindow = createRootAction("View", "Show Implementation in New Window", "", "Icons", "gears");
    view_viewImplInNewWindow->setToolTip("Popout selected entity's Implementation.");
    view_viewImplInNewWindow->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    view_viewImplInNewWindow->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_I));

    window_showNotifications = createRootAction("View", "Show Notifications", "", "Icons", "popOut");
    window_showNotifications->setToolTip("Show Notifications Panel");
    window_showNotifications->setShortcutContext(Qt::ApplicationShortcut);
    window_showNotifications->setShortcut(QKeySequence(Qt::Key_F11));
    //window_showNotifications->setCheckable(true);
    //window_showNotifications->setChecked(false);


    view_viewConnections = createRootAction("View", "Select and Center Items Connections", "", "Icons", "connectFork");
    view_viewConnections->setToolTip("Center selected entity's connected entities.");
    view_viewConnections->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    view_viewConnections->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_E));


    view_viewInNewWindow = createRootAction("View", "View In New Window", "", "Icons", "popOut");
    view_viewInNewWindow->setToolTip("Popout selected entity.");

    model_selectModel= createRootAction("Selection", "Select Model", "", "Icons", "medeaLogo");
    model_selectModel->setToolTip("Select the Model entity.");
    model_selectModel->setShortcutContext(Qt::ApplicationShortcut);
    model_selectModel->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_M));

    model_validateModel = createRootAction("Model", "Validate Model", "", "Icons", "shieldTick");
    model_validateModel->setToolTip("Validate the current project.");
    model_validateModel->setShortcutContext(Qt::ApplicationShortcut);
    model_validateModel->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_V));

    model_getCodeForComponent = createRootAction("Model", "Generate Code for Component", "", "Icons", "bracketsAngled");
    model_getCodeForComponent->setToolTip("Generate the C++ Impl code for the selected Component");

    model_generateModelWorkspace = createRootAction("Model", "Generate Model Workspace", "", "Icons", "briefcase");
    model_generateModelWorkspace->setToolTip("Generate all the C++ artifacts for the model");

    //model_executeLocalJob = createRootAction("Model", "Launch: Local Deployment", "", "Icons", "jobBuild");
    //model_executeLocalJob->setToolTip("Executes the current project on the local machine.");


    jenkins_importNodes = createRootAction("Model", "Import Jenkins Nodes", "", "EntityIcons", "HardwareNode");
    jenkins_importNodes->setToolTip("Imports the nodes from the Jenkins Server.");
    jenkins_importNodes->setShortcutContext(Qt::ApplicationShortcut);
    jenkins_importNodes->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_J));


    jenkins_executeJob = createRootAction("Model", "Launch: Jenkins Job", "", "Icons", "jobBuild");
    jenkins_executeJob->setToolTip("Executes the current project on the Jenkins Server.");
    jenkins_executeJob->setShortcutContext(Qt::ApplicationShortcut);
    jenkins_executeJob->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_B));

    help_shortcuts = createRootAction("Help", "App Shortcuts", "", "Icons", "keyboard");

    help_reportBug = createRootAction("Help", "Report Bug", "", "Icons", "bug");

    help_wiki = createRootAction("Help", "Wiki", "", "Icons", "book");
    help_wiki->setToolTip("Show the help wiki.");
    help_wiki->setShortcutContext(Qt::ApplicationShortcut);
    help_wiki->setShortcut(QKeySequence::HelpContents);


    help_aboutMedea = createRootAction("Help", "About MEDEA", "", "Icons", "circleInfo");
    help_aboutQt = createRootAction("Help", "About Qt", "", "Icons", "qt");

    options_settings = createRootAction("Help", "Settings", "", "Icons", "gear");
    options_settings->setToolTip("Show application settings.");
    options_settings->setShortcutContext(Qt::ApplicationShortcut);
    options_settings->setShortcut(QKeySequence(Qt::Key_F10));




    file_exit = createRootAction("Project", "Exit", "", "Icons", "arrowIntoBox");

    toolbar_contextToolbar = createRootAction("Toolbar", "Show Context Toolbar", "", "Icons", "gearDark");

    toolbar_addChild = createRootAction("Toolbar", "Add Child Entity", "", "Icons", "plus");
    toolbar_connect = createRootAction("Toolbar", "Connect Selection", "", "Icons", "connect");
    toolbar_popOutDefn = createRootAction("Toolbar", "Popout Definition", "", "Icons", "popOut");
    toolbar_popOutImpl = createRootAction("Toolbar", "Popout Implementation", "", "Icons", "popOut");
   




    toolbar_wiki = createRootAction("Toolbar", "View Wiki Page For Selected Entity", "", "Icons", "book");
    toolbar_replicateCount = createRootAction("Toolbar", "Change Replicate Count", "", "Icons", "copyX");
    toolbar_displayedChildrenOption = createRootAction("Toolbar", "Change Displayed Nodes Settings", "", "Icons", "dotsVertical");

    toolbar_addDDSQOSProfile = createRootAction("Toolbar", "Add Profile", "", "Icons", "plus");
    toolbar_removeDDSQOSProfile = createRootAction("Toolbar", "Remove Profile", "", "Icons", "bin");

    toggleDock = createRootAction("Misc", "Show/Hide Dock", "", "Icons", "dotsVertical");
}

void ActionController::setupMainMenu()
{
    menu_file = new QMenu("File");
    menu_edit = new QMenu("Edit");
    //menu_edit->setPlatformMenu();
    menu_view = new QMenu("View");
    menu_model = new QMenu("Model");
    menu_jenkins = new QMenu("Jenkins");
    menu_window = new QMenu("Window");
    menu_options = new QMenu("Options");
    menu_help = new QMenu("Help");

    // File Menu
    menu_file->addAction(file_newProject);
    menu_file->addAction(file_openProject);
    menu_file_recentProjects = menu_file->addMenu("Recent Projects");
    menu_file_recentProjects->addAction(file_recentProjects_clearHistory);
    menu_file->addSeparator();
    menu_file->addAction(file_saveProject);
    menu_file->addAction(file_saveAsProject);
    menu_file->addAction(file_closeProject);
    menu_file->addSeparator();
    menu_file->addAction(file_importGraphML);
    //menu_file->addAction(file_importXME);
    //menu_file->addAction(file_importXMI);
    //menu_file->addSeparator();

    menu_file->addSeparator();
    menu_file->addAction(file_exit);

    // Edit Menu
    menu_edit->addAction(edit_undo);
    menu_edit->addAction(edit_redo);
    menu_edit->addSeparator();
    menu_edit->addAction(edit_cut);
    menu_edit->addAction(edit_copy);
    menu_edit->addAction(edit_paste);
    menu_edit->addAction(edit_replicate);
    menu_edit->addSeparator();
    menu_edit->addAction(edit_delete);
    menu_edit->addAction(edit_renameActiveSelection);

    menu_edit->addAction(edit_search);
    menu_edit->addSeparator();
    //menu_edit->addAction(edit_sort);
    menu_edit->addAction(edit_alignHorizontal);
    menu_edit->addAction(edit_alignVertical);
    menu_edit->addSeparator();
    menu_edit->addAction(edit_selectAll);
    menu_edit->addAction(edit_clearSelection);
    menu_edit->addAction(edit_CycleActiveSelectionForward);
    menu_edit->addAction(edit_CycleActiveSelectionBackward);

     // View Menu
    menu_view->addAction(view_fitView);
    menu_view->addAction(view_fitAllViews);
    menu_view->addSeparator();
    menu_view->addAction(view_centerOn);
    menu_view->addAction(view_centerOnDefn);
    menu_view->addAction(view_centerOnImpl);
    menu_view->addSeparator();
    menu_view->addAction(view_viewInNewWindow);
    menu_view->addAction(view_viewDefnInNewWindow);
    menu_view->addAction(view_viewImplInNewWindow);
    menu_view->addAction(view_viewConnections);

    // Model Menu
    menu_model->addAction(model_selectModel);

    menu_model->addAction(model_validateModel);
    menu_model->addAction(model_getCodeForComponent);
    menu_model->addAction(model_generateModelWorkspace);

    //menu_model->addAction(model_executeLocalJob);

    // Jenkins Menu

    menu_jenkins->addAction(jenkins_importNodes);
    menu_jenkins->addAction(jenkins_executeJob);


    // Window Menu
    menu_window->addAction(window_showNotifications);

    // Options Menu
    menu_options->addAction(options_settings);

    menu_help->addAction(help_shortcuts);
    menu_help->addAction(help_wiki);
    menu_help->addAction(help_reportBug);
    menu_help->addSeparator();
    menu_help->addAction(help_aboutMedea);
    menu_help->addAction(help_aboutQt);
}

void ActionController::setupApplicationToolbar()
{
    applicationToolbar = new ActionGroup(this);

    /*
    toggleDock->setCheckable(true);
    toggleDock->setChecked(true);
    //toggleDock->icon().addPixmap(Theme::theme()->getImage("Icons", "Menu_Vertical", QSize(), Qt::red), QIcon::Normal, QIcon::On);

    applicationToolbar->addAction(toggleDock);
    applicationToolbar->addSeperator();
    */

    toolbar_undo = applicationToolbar->addAction(edit_undo->constructSubAction(false));
    toolbar_redo = applicationToolbar->addAction(edit_redo->constructSubAction(false));
    applicationToolbar->addSeperator();
    toolbar_cut = applicationToolbar->addAction(edit_cut->constructSubAction(false));
    toolbar_copy = applicationToolbar->addAction(edit_copy->constructSubAction(false));
    toolbar_paste = applicationToolbar->addAction(edit_paste->constructSubAction(false));
    toolbar_replicate = applicationToolbar->addAction(edit_replicate->constructSubAction(false));
    applicationToolbar->addSeperator();
    toolbar_fitToScreen = applicationToolbar->addAction(view_fitView->constructSubAction(false));
    toolbar_centerOn = applicationToolbar->addAction(view_centerOn->constructSubAction(false));
    toolbar_viewInNewWindow = applicationToolbar->addAction(view_viewInNewWindow->constructSubAction(false));
    applicationToolbar->addSeperator();
    //toolbar_sort = applicationToolbar->addAction(edit_sort->constructSubAction(false));
    toolbar_alignVertical = applicationToolbar->addAction(edit_alignVertical->constructSubAction(false));
    toolbar_alignHorizontal = applicationToolbar->addAction(edit_alignHorizontal->constructSubAction(false));
    toolbar_contract = applicationToolbar->addAction(edit_contract->constructSubAction(false));
    toolbar_expand = applicationToolbar->addAction(edit_expand->constructSubAction(false));
    applicationToolbar->addSeperator();
    toolbar_delete = applicationToolbar->addAction(edit_delete->constructSubAction(false));
    toolbar_context = applicationToolbar->addAction(toolbar_contextToolbar->constructSubAction(false));
    toolbar_search = applicationToolbar->addAction(edit_search->constructSubAction(false));

    SettingsController* s = SettingsController::settings();
    foreach(SETTING_KEY key, s->getSettingsKeys("Toolbar", "Visible Buttons")){
        settingChanged(key, s->getSetting(key));
    }
    applicationToolbar->updateSpacers();
}

void ActionController::setupContextToolbar()
{
    contextToolbar = new ActionGroup(this);

    contextToolbar->addAction(toolbar_addChild);
    contextToolbar->addAction(edit_delete->constructSubAction());
    //contextToolbar->addAction(toolbar_hardware);
    //contextToolbar->addAction(toolbar_disconnectHardware);
    contextToolbar->addSeperator();
    //contextToolbar->addSeperator();
    contextToolbar->addAction(view_centerOnDefn->constructSubAction());
    contextToolbar->addAction(view_centerOnImpl->constructSubAction());
    contextToolbar->addSeperator();
    contextToolbar->addAction(toolbar_displayedChildrenOption);
    contextToolbar->addAction(toolbar_replicateCount);
    
    contextToolbar->addSeperator();
    contextToolbar->addAction(view_viewConnections->constructSubAction());
    contextToolbar->addAction(model_getCodeForComponent->constructSubAction());
    contextToolbar->addAction(view_viewInNewWindow->constructSubAction());
    contextToolbar->addAction(toolbar_wiki);

}

void ActionController::setupRecentProjects()
{
    recentProjects = new ActionGroup(this);
    recentProjectsChanged();
}

