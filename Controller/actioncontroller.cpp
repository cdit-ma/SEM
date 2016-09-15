#include "actioncontroller.h"
#include "viewcontroller.h"
#include "../View/theme.h"
#include <QDebug>
#include "filehandler.h"
ActionController::ActionController(ViewController* vc) : QObject(vc)
{
    viewController = vc;
    selectionController = viewController->getSelectionController();



    _controllerReady = false;
    _modelReady = false;
    _jenkinsValidated = false;
    setupActions();

    setupMainMenu();
    setupApplicationToolbar();
    setupContextToolbar();

    setupRecentProjects();


    connect(SettingsController::settings(), &SettingsController::settingChanged, this, &ActionController::settingChanged);
    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));

    themeChanged();
    connectViewController(vc);

    updateActions();
}

void ActionController::connectViewController(ViewController *controller)
{

    if(viewController){
        connect(controller, &ViewController::vc_actionFinished, this, &ActionController::actionFinished);
        connect(controller, &ViewController::vc_controllerReady, this, &ActionController::controllerReady);
        connect(controller, &ViewController::mc_modelReady, this, &ActionController::modelReady);
        connect(controller, &ViewController::vc_JenkinsReady, this, &ActionController::jenkinsValidated);

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
        connect(edit_renameActiveSelection, &QAction::triggered, viewController, &ViewController::renameActiveSelection);

        connect(view_centerOnImpl, &QAction::triggered, viewController, &ViewController::centerImpl);
        connect(view_centerOnDefn, &QAction::triggered, viewController, &ViewController::centerDefinition);
        connect(view_viewDefnInNewWindow, &QAction::triggered, viewController, &ViewController::popupDefinition);
        connect(view_viewImplInNewWindow, &QAction::triggered, viewController, &ViewController::popupImpl);
        connect(view_viewInNewWindow, &QAction::triggered, viewController, &ViewController::popupSelection);

        connect(jenkins_executeJob, &QAction::triggered, viewController, &ViewController::executeJenkinsJob);


        connect(options_settings, &QAction::triggered, SettingsController::settings(), &SettingsController::showSettingsWidget);

        connect(toolbar_addDDSQOSProfile, &QAction::triggered, viewController, &ViewController::constructDDSQOSProfile);

        connect(model_getCodeForComponent, &QAction::triggered, viewController, &ViewController::getCodeForComponent);
        connect(model_validateModel, &QAction::triggered, viewController, &ViewController::validateModel);

        connect(model_executeLocalJob, &QAction::triggered, viewController, &ViewController::launchLocalDeployment);
        connect(file_importXME, &QAction::triggered, viewController, &ViewController::importXMEProject);
        connect(file_importXMI, &QAction::triggered, viewController, &ViewController::importXMIProject);
        connect(file_importSnippet, &QAction::triggered, viewController, &ViewController::importSnippet);
        connect(file_exportSnippet, &QAction::triggered, viewController, &ViewController::exportSnippet);

        connect(file_recentProjects_clearHistory, &QAction::triggered, this, &ActionController::clearRecentProjects);
        connectSelectionController(controller->getSelectionController());
    }
}

void ActionController::connectSelectionController(SelectionController *controller)
{
    if(selectionController){

        connect(selectionController, SIGNAL(selectionChanged(int)), this, SLOT(selectionChanged(int)));
        connect(edit_CycleActiveSelectionForward, SIGNAL(triggered(bool)), controller, SLOT(cycleActiveSelectionForward()));
        connect(edit_CycleActiveSelectionBackward, SIGNAL(triggered(bool)), controller, SLOT(cycleActiveSelectionBackward()));
        connect(edit_selectAll, SIGNAL(triggered(bool)), controller, SIGNAL(selectAll()));
        connect(edit_clearSelection, SIGNAL(triggered(bool)), controller, SIGNAL(clearSelection()));
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

RootAction *ActionController::createRootAction(QString name, QString hashKey, QString iconPath, QString aliasPath)
{
    RootAction* action = new RootAction(name, this);
    action->setIconPath(iconPath, aliasPath);
    if(hashKey != ""){
        actionHash[hashKey] = action;
    }
    allActions.append(action);
    return action;
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

void ActionController::selectionChanged(int selectionSize)
{
    if(selectionController){
        if(selectionSize == -1){
            selectionSize = selectionController->getSelectionCount();
        }

        bool controllerReady = viewController->isControllerReady();
        bool modelReady = viewController->isModelReady();

        bool modelActions = controllerReady && modelReady;

        bool gotSingleSelection = modelActions && selectionSize == 1;
        bool gotSelection = modelActions && selectionSize > 0;
        bool gotMultipleSelection = modelActions && selectionSize > 1;


        ViewItem* singleItem = selectionController->getActiveSelectedItem();

        bool hasDefn = false;
        bool hasImpl = false;
        bool hasCode = false;

        if(gotSingleSelection && singleItem && singleItem->isNode()){
            NodeViewItem* node = (NodeViewItem*) singleItem;
            hasDefn = node->isNodeOfType(Node::NT_INSTANCE) || node->isNodeOfType(Node::NT_IMPLEMENTATION);
            hasImpl = hasDefn || node->isNodeOfType(Node::NT_DEFINITION);
            hasCode = node->getNodeKind() == Node::NK_COMPONENT || node->getNodeKind() == Node::NK_COMPONENT_INSTANCE || node->getNodeKind() == Node::NK_COMPONENT_IMPL;
        }

        file_importSnippet->setEnabled(viewController->canImportSnippet());
        file_exportSnippet->setEnabled(viewController->canExportSnippet());


        model_getCodeForComponent->setEnabled(hasCode);

        view_centerOnDefn->setEnabled(hasDefn);
        view_viewDefnInNewWindow->setEnabled(hasDefn);

        view_centerOnImpl->setEnabled(hasImpl);
        view_viewImplInNewWindow->setEnabled(hasImpl);


        edit_cut->setEnabled(gotSelection);
        edit_copy->setEnabled(gotSelection);
        edit_paste->setEnabled(gotSingleSelection);


        edit_replicate->setEnabled(gotSelection);
        edit_delete->setEnabled(gotSelection);
        edit_sort->setEnabled(gotSelection);
        edit_renameActiveSelection->setEnabled(gotSelection);


        edit_alignHorizontal->setEnabled(gotMultipleSelection);
        edit_alignVertical->setEnabled(gotMultipleSelection);

        edit_CycleActiveSelectionForward->setEnabled(gotMultipleSelection);
        edit_CycleActiveSelectionBackward->setEnabled(gotMultipleSelection);

        view_fitView->setEnabled(modelActions);
        view_fitAllViews->setEnabled(modelActions);


        view_centerOn->setEnabled(gotSelection);

        view_viewInNewWindow->setEnabled(gotSingleSelection);

        view_viewConnections->setEnabled(false);

        edit_clearSelection->setEnabled(gotMultipleSelection);
        edit_selectAll->setEnabled(gotSingleSelection);


        applicationToolbar->updateSpacers();
    }
}

void ActionController::actionFinished()
{
    emit selectionController->selectionChanged(-1);
    //selectionChanged(-1);
}

void ActionController::controllerReady(bool ready)
{
    updateActions();
}

void ActionController::modelReady(bool ready)
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


    menu_file_recentProjects->setIcon(theme->getIcon("Actions", "Timer"));
}

void ActionController::updateJenkinsActions()
{
    bool modelReady = viewController->isModelReady();

    jenkins_importNodes->setEnabled(modelReady && _jenkinsValidated);
    jenkins_executeJob->setEnabled(modelReady && _jenkinsValidated);
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
    case SK_TOOLBAR_SORT:
        return toolbar_sort;
    case SK_TOOLBAR_DELETE:
        return toolbar_delete;
    case SK_TOOLBAR_ALIGN_HORIZONTAL:
        return toolbar_alignVertical;
    case SK_TOOLBAR_ALIGN_VERTICAL:
        return toolbar_alignHorizontal;
    case SK_TOOLBAR_SEARCH:
        return toolbar_search;
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
    file_importXME->setEnabled(modelActions);
    file_importXMI->setEnabled(modelActions);
    file_saveProject->setEnabled(modelActions);
    file_saveAsProject->setEnabled(modelActions);
    file_closeProject->setEnabled(modelActions);

    edit_undo->setEnabled(modelActions);
    edit_redo->setEnabled(modelActions);

    model_validateModel->setEnabled(modelActions);
    model_executeLocalJob->setEnabled(modelActions);

    edit_search->setEnabled(modelActions);
    view_fitView->setEnabled(modelActions);
    view_fitAllViews->setEnabled(modelActions);


    window_printScreen->setEnabled(modelActions);
    jenkins_importNodes->setEnabled(modelActions);
    jenkins_executeJob->setEnabled(modelActions);
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
        RootAction* action = new RootAction(fileName, this);
        action->setIconPath("Actions", "New");
        updateIcon(action);
        recentProjectActions.insert(fileName, action);
        connect(action, &QAction::triggered, recentProjectMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
        recentProjectMapper->setMapping(action, fileName);
    }
}

void ActionController::recentProjectsChanged()
{
    recentProjectMapper = new QSignalMapper(this);

    connect(recentProjectMapper, static_cast<void(QSignalMapper::*)(const QString &)>(&QSignalMapper::mapped),viewController, &ViewController::openExistingProject);


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
        //
        menu_file_recentProjects->addSeparator();
        menu_file_recentProjects->addAction(file_recentProjects_clearHistory);
        recentProjectKeys = orderedKeys;
        emit recentProjectsUpdated();
    }

}

void ActionController::updateIcon(RootAction *action, Theme *theme)
{
    if(theme && action){
        action->setIcon(theme->getIcon(action->getIconPair()));
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

void ActionController::setupActions()
{
    file_newProject = createRootAction("New Project", "", "Actions", "New");
    file_newProject->setShortcutContext(Qt::ApplicationShortcut);
    file_newProject->setShortcut(QKeySequence::New);

    file_openProject = createRootAction("Open Project", "", "Actions", "Open");
    file_openProject->setShortcutContext(Qt::ApplicationShortcut);
    file_openProject->setShortcut(QKeySequence::Open);

    file_recentProjects_clearHistory = createRootAction("Clear History", "", "Actions", "Clear");

    file_saveProject = createRootAction("Save Project", "", "Actions", "Save");
    file_saveProject->setShortcutContext(Qt::ApplicationShortcut);
    file_saveProject->setShortcut(QKeySequence::Save);


    file_saveAsProject = createRootAction("Save Project As", "", "Actions", "Save");
    file_saveAsProject->setShortcutContext(Qt::ApplicationShortcut);
    file_saveAsProject->setShortcut(QKeySequence::SaveAs);

    file_closeProject = createRootAction("Close Project", "", "Actions", "Close");
    file_closeProject->setShortcutContext(Qt::ApplicationShortcut);
    file_closeProject->setShortcut(QKeySequence::Close);


    file_importGraphML = createRootAction("Import Project", "", "Actions", "Import");
    file_importGraphML->setShortcutContext(Qt::ApplicationShortcut);
    file_importGraphML->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));


    file_importXME = createRootAction("Import XME Project", "", "Actions", "ImportXME");
    file_importXMI = createRootAction("Import UML XMI Project", "", "Actions", "ImportXMI");
    file_importSnippet = createRootAction("Import Snippet", "", "Actions", "ImportSnippet");
    file_exportSnippet = createRootAction("Export Snippet", "", "Actions", "ExportSnippet");

    edit_undo = createRootAction("Undo", "", "Actions", "Undo");
    edit_undo->setShortcutContext(Qt::ApplicationShortcut);
    edit_undo->setShortcut(QKeySequence::Undo);
    edit_redo = createRootAction("Redo", "", "Actions", "Redo");
    edit_redo->setShortcutContext(Qt::ApplicationShortcut);
    edit_redo->setShortcut(QKeySequence::Redo);

    edit_cut = createRootAction("Cut", "", "Actions", "Cut");
    edit_cut->setShortcutContext(Qt::ApplicationShortcut);
    edit_cut->setShortcut(QKeySequence::Cut);

    edit_copy = createRootAction("Copy", "", "Actions", "Copy");
    edit_copy->setShortcutContext(Qt::ApplicationShortcut);
    edit_copy->setShortcut(QKeySequence::Copy);

    edit_paste = createRootAction("Paste", "", "Actions", "Paste");
    edit_paste->setShortcutContext(Qt::ApplicationShortcut);
    edit_paste->setShortcut(QKeySequence::Paste);

    edit_replicate = createRootAction("Replicate", "", "Actions", "Replicate");
    edit_replicate->setShortcutContext(Qt::ApplicationShortcut);
    edit_replicate->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));

    edit_delete = createRootAction("Delete", "Delete", "Actions", "Delete");
    edit_delete->setShortcut(QKeySequence::Delete);
    edit_delete->setShortcutContext(Qt::ApplicationShortcut);

    edit_renameActiveSelection = createRootAction("Rename", "Rename", "Actions", "Rename");
    edit_renameActiveSelection->setShortcut(QKeySequence(Qt::Key_F2));
    edit_renameActiveSelection->setShortcutContext(Qt::ApplicationShortcut);



    edit_search = createRootAction("Search", "Root_Search", "Actions", "Search");
    edit_search->setShortcutContext(Qt::ApplicationShortcut);
    edit_search->setShortcut(QKeySequence::Find);


    edit_sort = createRootAction("Sort", "", "Actions", "Sort");
    edit_alignVertical = createRootAction("Align Vertically", "", "Actions", "Align_Vertical");
    edit_alignHorizontal = createRootAction("Align Horizontally", "", "Actions", "Align_Horizontal");
    edit_CycleActiveSelectionForward = createRootAction("Cycle Next Selected Item", "", "Actions", "Arrow_Right");
    edit_CycleActiveSelectionForward->setShortcutContext(Qt::ApplicationShortcut);
    edit_CycleActiveSelectionForward->setShortcut(QKeySequence::NextChild);
    edit_CycleActiveSelectionBackward = createRootAction("Cycle Prev Selected Item", "", "Actions", "Arrow_Left");
    edit_CycleActiveSelectionBackward->setShortcut(QKeySequence::PreviousChild);
    edit_CycleActiveSelectionBackward->setShortcutContext(Qt::ApplicationShortcut);
    edit_selectAll = createRootAction("Select All", "", "Actions", "SelectAll");
    edit_selectAll->setShortcut(QKeySequence::SelectAll);
    edit_selectAll->setShortcutContext(Qt::ApplicationShortcut);

    edit_clearSelection = createRootAction("Clear Selection", "", "Actions", "Clear");
    edit_clearSelection->setShortcut(QKeySequence(Qt::Key_Escape));
    edit_clearSelection->setShortcutContext(Qt::ApplicationShortcut);

    view_fitView = createRootAction("Fit View", "", "Actions", "FitToScreen");
    view_fitView->setShortcutContext(Qt::ApplicationShortcut);
    view_fitView->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Space));

    view_fitAllViews = createRootAction("Fit All Views", "", "Actions", "FitToScreen");
    view_fitAllViews->setShortcutContext(Qt::ApplicationShortcut);
    view_fitAllViews->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Space));

    view_centerOn = createRootAction("Center On Selection", "", "Actions", "Crosshair");
    view_centerOnDefn = createRootAction("Center On Definition", "", "Actions", "Definition");
    view_centerOnImpl = createRootAction("Center On Implementation", "", "Actions", "Implementation");

    view_viewDefnInNewWindow = createRootAction("Show Definition in New Window", "", "Actions", "Definition");
    view_viewImplInNewWindow = createRootAction("Show Implementation in New Window", "", "Actions", "Implementation");

    view_centerOnDefn->setShortcutContext(Qt::ApplicationShortcut);
    view_centerOnDefn->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_D));

    view_centerOnImpl->setShortcutContext(Qt::ApplicationShortcut);
    view_centerOnImpl->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_I));

    view_viewImplInNewWindow->setShortcutContext(Qt::ApplicationShortcut);
    view_viewImplInNewWindow->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_I));

    view_viewDefnInNewWindow->setShortcutContext(Qt::ApplicationShortcut);
    view_viewDefnInNewWindow->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_D));


    view_viewConnections = createRootAction("View Connections", "", "Actions", "Connections");
    view_viewInNewWindow = createRootAction("View In New Window", "", "Actions", "Popup");

    view_viewInNewWindow->setData("Test ID");
    view_viewInNewWindow->setProperty("ID", 12);
    view_viewInNewWindow->setProperty("ID2", "HELPP");
    view_viewInNewWindow->setProperty("ID3",  view_viewInNewWindow->icon());

    window_printScreen = createRootAction("Print Screen", "", "Actions", "PrintScreen");
    window_displayMinimap = createRootAction("Display Minimap", "", "Actions", "Minimap");

    model_validateModel = createRootAction("Validate Model", "", "Actions", "Validate");
    model_validateModel->setShortcutContext(Qt::ApplicationShortcut);
    model_validateModel->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_V));

    model_getCodeForComponent = createRootAction("Get Code for Component", "", "Actions", "getCPP");
    model_executeLocalJob = createRootAction("Launch: Local Deployment", "", "Actions", "Job_Build");
    model_executeLocalJob->setToolTip("Requires Valid CUTS and Windows");

    jenkins_importNodes = createRootAction("Import Jenkins Nodes", "", "Actions", "Computer");
    jenkins_executeJob = createRootAction("Launch: ", "", "Actions", "Job_Build");

    help_shortcuts = createRootAction("App Shortcuts", "", "Actions", "Keyboard");
    help_reportBug = createRootAction("Report Bug", "", "Actions", "BugReport");
    help_wiki = createRootAction("Wiki", "", "Actions", "Wiki");
    help_aboutMedea = createRootAction("About MEDEA", "", "Actions", "Info");
    help_aboutQt = createRootAction("About Qt", "", "Actions", "Qt");

    options_settings = createRootAction("Settings", "", "Actions", "Settings");
    options_settings->setShortcutContext(Qt::ApplicationShortcut);
    options_settings->setShortcut(QKeySequence(Qt::Key_F10));




    file_exit = createRootAction("Exit", "", "Actions", "Power");

    toolbar_contextToolbar = createRootAction("Show Context Toolbar", "", "Actions", "Toolbar");

    toolbar_addChild = createRootAction("Add Child Entity", "", "Actions", "Plus");
    toolbar_connect = createRootAction("Connect Selection", "", "Actions", "Connect");
    toolbar_hardware = createRootAction("Deploy Selection", "", "Actions", "Computer");
    toolbar_disconnectHardware = createRootAction("Disconnect Selection From Its Current Deployment", "", "Actions", "Computer_Cross");
    toolbar_popOutDefn = createRootAction("Popout Definition", "", "Actions", "Popup");
    toolbar_popOutImpl = createRootAction("Popout Implementation", "", "Actions", "Popup");
    //toolbar_popOutInst = createRootAction("View Selection's Instance", "", "Actions", "Popup");
    toolbar_setReadOnly = createRootAction("Set Selection To Read Only", "", "Actions", "Lock_Closed");
    toolbar_unsetReadOnly = createRootAction("Unset Selection From Read Only", "", "Actions", "Lock_Open");
    toolbar_expand = createRootAction("Expand Selection", "", "Actions", "Expand");
    toolbar_contract = createRootAction("Contract Selection", "", "Actions", "Contract");
    toolbar_wiki = createRootAction("View Wiki Page For Selected Entity", "", "Actions", "Wiki");
    toolbar_replicateCount = createRootAction("Change Replicate Count", "", "Actions", "Replicate_Count");
    toolbar_displayedChildrenOption = createRootAction("Change Displayed Nodes Settings", "", "Actions", "Menu_Vertical");

    toolbar_addDDSQOSProfile = createRootAction("Construct new DDS QOS Profile", "", "Actions", "Plus");

    toggleDock = createRootAction("Show/Hide Dock", "", "Actions", "Menu_Vertical");
}

void ActionController::setupMainMenu()
{
    menu_file = new QMenu("File");
    menu_edit = new QMenu("Edit");
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
    menu_file->addAction(file_importXME);
    menu_file->addAction(file_importXMI);
    menu_file->addSeparator();
    menu_file->addAction(file_importSnippet);
    menu_file->addAction(file_exportSnippet);
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
    menu_edit->addAction(edit_sort);
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
    menu_view->addAction(view_viewDefnInNewWindow);
    menu_view->addAction(view_viewImplInNewWindow);



    menu_view->addAction(view_viewConnections);
    menu_view->addAction(view_viewInNewWindow);

    // Model Menu
    menu_model->addAction(model_validateModel);
    menu_model->addAction(model_getCodeForComponent);

    menu_model->addAction(model_executeLocalJob);

    // Jenkins Menu
    menu_jenkins->addAction(jenkins_executeJob);

    // Window Menu
    menu_window->addAction(window_printScreen);
    menu_window->addAction(window_displayMinimap);

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
    //toggleDock->icon().addPixmap(Theme::theme()->getImage("Actions", "Menu_Vertical", QSize(), Qt::red), QIcon::Normal, QIcon::On);

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
    toolbar_sort = applicationToolbar->addAction(edit_sort->constructSubAction(false));
    toolbar_alignVertical = applicationToolbar->addAction(edit_alignVertical->constructSubAction(false));
    toolbar_alignHorizontal = applicationToolbar->addAction(edit_alignHorizontal->constructSubAction(false));
    toolbar_delete = applicationToolbar->addAction(edit_delete->constructSubAction(false));
    applicationToolbar->addSeperator();
    toolbar_context = applicationToolbar->addAction(toolbar_contextToolbar->constructSubAction(false));
    toolbar_search = applicationToolbar->addAction(edit_search->constructSubAction(false));
    applicationToolbar->addSeperator();

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
    contextToolbar->addAction(toolbar_hardware);
    contextToolbar->addAction(toolbar_disconnectHardware);
    contextToolbar->addSeperator();
    contextToolbar->addAction(edit_alignVertical->constructSubAction());
    contextToolbar->addAction(edit_alignHorizontal->constructSubAction());
    contextToolbar->addSeperator();
    contextToolbar->addAction(toolbar_expand);
    contextToolbar->addAction(toolbar_contract);
    contextToolbar->addSeperator();
    contextToolbar->addAction(file_importSnippet->constructSubAction());
    contextToolbar->addAction(file_exportSnippet->constructSubAction());
    contextToolbar->addSeperator();
    contextToolbar->addAction(view_centerOnDefn->constructSubAction());
    contextToolbar->addAction(view_centerOnImpl->constructSubAction());
    //contextToolbar->addAction(toolbar_popOutDefn);
    //contextToolbar->addAction(toolbar_popOutImpl);
    //contextToolbar->addAction(toolbar_popOutInst);
    contextToolbar->addSeperator();
    contextToolbar->addAction(toolbar_displayedChildrenOption);
    contextToolbar->addAction(toolbar_replicateCount);
    contextToolbar->addAction(toolbar_setReadOnly);
    contextToolbar->addAction(toolbar_unsetReadOnly);
    contextToolbar->addSeperator();
    contextToolbar->addAction(view_viewConnections->constructSubAction());
    contextToolbar->addAction(model_getCodeForComponent->constructSubAction());
    contextToolbar->addAction(view_viewInNewWindow->constructSubAction());
    contextToolbar->addAction(toolbar_wiki);

    /*
    contextToolbar->addAction(edit_cut->getStealthAction());
    contextToolbar->addAction(edit_copy->getStealthAction());
    contextToolbar->addAction(edit_paste->getStealthAction());
    contextToolbar->addAction(edit_replicate->getStealthAction());
    contextToolbar->addSeperator();
    contextToolbar->addAction(view_viewInNewWindow->getStealthAction());
    contextToolbar->addSeperator();
    contextToolbar->addAction(edit_sort->getStealthAction());
    contextToolbar->addAction(edit_delete->getStealthAction());
    contextToolbar->addSeperator();
    contextToolbar->addAction(edit_alignVertical->getStealthAction());
    contextToolbar->addAction(edit_alignHorizontal->getStealthAction());
    */
}

void ActionController::setupRecentProjects()
{
    recentProjects = new ActionGroup(this);
    recentProjectsChanged();
}

