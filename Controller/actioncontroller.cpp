#include "actioncontroller.h"
#include "../View/theme.h"
#include <QDebug>
ActionController::ActionController(QObject *parent) : QObject(parent)
{
    selectionController = 0;





    _modelReady = true;
    _jenkinsValidated = false;
    setupActions();
    setupMainMenu();
    setupApplicationToolbar();
    setupContextToolbar();

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));

}

void ActionController::connectSelectionController(SelectionController *controller)
{
    selectionController = controller;
    connect(selectionController, SIGNAL(selectionChanged(int)), this, SLOT(selectionChanged(int)));
    connect(edit_CycleActiveSelectionForward, SIGNAL(triggered(bool)), controller, SLOT(cycleActiveSelectionForward()));
    connect(edit_CycleActiveSelectionBackward, SIGNAL(triggered(bool)), controller, SLOT(cycleActiveSelectionBackward()));
    connect(edit_selectAll, SIGNAL(triggered(bool)), controller, SIGNAL(selectAll()));
    connect(edit_clearSelection, SIGNAL(triggered(bool)), controller, SIGNAL(clearSelection()));
}

RootAction *ActionController::createRootAction(QString name, QString actionHash, QString iconPath, QString aliasPath)
{
    RootAction* action = new RootAction(name, this);
    action->setIconPath(iconPath, aliasPath);
    allActions.append(action);
    return action;
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
        QVector<ViewItem*> selection = selectionController->getSelection();

        bool noModel = selectionSize == -1;
        bool emptySelection = selectionSize == 0;

        if(emptySelection || !_modelReady){
            edit_cut->setEnabled(false);
            edit_copy->setEnabled(false);
            edit_paste->setEnabled(false);
            edit_replicate->setEnabled(false);
            edit_delete->setEnabled(false);
            edit_sort->setEnabled(false);
            edit_alignHorizontal->setEnabled(false);
            edit_alignVertical->setEnabled(false);
            edit_CycleActiveSelectionForward->setEnabled(false);
            edit_CycleActiveSelectionBackward->setEnabled(false);

            view_centerOn->setEnabled(false);
            view_centerOnDefn->setEnabled(false);
            view_centerOnImpl->setEnabled(false);
            view_viewInNewWindow->setEnabled(false);
            view_viewConnections->setEnabled(false);

            file_importSnippet->setEnabled(false);
            file_exportSnippet->setEnabled(false);
            edit_clearSelection->setEnabled(false);
            if(_modelReady){
                edit_selectAll->setEnabled(true);
            }else{
                edit_selectAll->setEnabled(false);
            }
        }else if(!emptySelection){
            if(selectionSize > 1){
                edit_selectAll->setEnabled(false);
                edit_CycleActiveSelectionForward->setEnabled(true);
                edit_CycleActiveSelectionBackward->setEnabled(true);
            }else if(selectionSize == 1){
                edit_selectAll->setEnabled(true);
                edit_clearSelection->setEnabled(true);
                view_viewInNewWindow->setEnabled(true);
            }
            edit_clearSelection->setEnabled(true);
            view_centerOn->setEnabled(true);
            edit_cut->setEnabled(true);
            edit_copy->setEnabled(true);
            edit_paste->setEnabled(true);
            edit_replicate->setEnabled(true);
            edit_delete->setEnabled(true);
            edit_sort->setEnabled(true);
            edit_alignHorizontal->setEnabled(true);
            edit_alignVertical->setEnabled(true);

        }


        applicationToolbar->updateSpacers();
    }
}

void ActionController::modelReady(bool ready)
{
    _modelReady = ready;
    file_importGraphML->setEnabled(ready);
    file_importXME->setEnabled(ready);
    file_importXMI->setEnabled(ready);
    file_saveProject->setEnabled(ready);
    file_saveAsProject->setEnabled(ready);
    file_closeProject->setEnabled(ready);

    edit_undo->setEnabled(ready);
    edit_redo->setEnabled(ready);

    model_validateModel->setEnabled(ready);
    model_clearModel->setEnabled(ready);
    model_executeLocalJob->setEnabled(ready);

    edit_search->setEnabled(ready);
    view_fitToScreen->setEnabled(ready);
    window_printScreen->setEnabled(ready);
    jenkins_importNodes->setEnabled(ready);
    jenkins_executeJob->setEnabled(ready);
    toolbar_contextToolbar->setEnabled(ready);

    if(!ready){
        //Update the selection changed with a special thing.
        selectionChanged(-1);
    }
    updateJenkinsActions();
}

void ActionController::themeChanged()
{
    Theme* theme = Theme::theme();

    foreach(RootAction* action, allActions){
        updateIcon(action, theme);
    }

    menu_file_recentProjects->setIcon(theme->getIcon("Actions", "Timer"));
}

void ActionController::updateJenkinsActions()
{
    jenkins_importNodes->setEnabled(_modelReady && _jenkinsValidated);
    jenkins_executeJob->setEnabled(_modelReady && _jenkinsValidated);
}

void ActionController::updateIcon(RootAction *action, Theme *theme)
{
    if(theme && action){
        action->setIcon(theme->getIcon(action->getIconPath(), action->getIconAlias()));
    }
}

void ActionController::setupActions()
{
    file_newProject = createRootAction("New Project", "", "Actions", "New");
    file_openProject = createRootAction("Open Project", "", "Actions", "Open");
    file_recentProjects_clearHistory = createRootAction("Clear History", "", "Actions", "Clear");
    file_saveProject = createRootAction("Save Project", "", "Actions", "Save");
    file_saveAsProject = createRootAction("Save Project As", "", "Actions", "Save");
    file_closeProject = createRootAction("Close Project", "", "Actions", "Close");
    file_importGraphML = createRootAction("Import Project", "", "Actions", "Import");
    file_importXME = createRootAction("Import XME File", "", "Actions", "ImportXME");
    file_importXMI = createRootAction("Import UML XMI File", "", "Actions", "ImportXMI");
    file_importSnippet = createRootAction("Import Snippet", "", "Actions", "ImportSnippet");
    file_exportSnippet = createRootAction("Export Snippet", "", "Actions", "ExportSnippet");

    edit_undo = createRootAction("Undo", "", "Actions", "Undo");
    edit_redo = createRootAction("Redo", "", "Actions", "Redo");
    edit_cut = createRootAction("Cut", "", "Actions", "Cut");
    edit_copy = createRootAction("Copy", "", "Actions", "Copy");
    edit_paste = createRootAction("Paste", "", "Actions", "Paste");
    edit_replicate = createRootAction("Replicate", "", "Actions", "Replicate");
    edit_delete = createRootAction("Delete", "", "Actions", "Delete");
    edit_delete->setShortcut(QKeySequence(Qt::Key_Delete));
    edit_delete->setShortcutContext(Qt::ApplicationShortcut);

    edit_search = createRootAction("Search", "", "Actions", "Search");
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
    edit_selectAll->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_A));
    edit_selectAll->setShortcutContext(Qt::ApplicationShortcut);

    edit_clearSelection = createRootAction("Clear Selection", "", "Actions", "Clear");
    edit_clearSelection->setShortcut(QKeySequence(Qt::Key_Escape));
    edit_clearSelection->setShortcutContext(Qt::ApplicationShortcut);

    view_fitToScreen = createRootAction("Fit To Screen", "", "Actions", "FitToScreen");
    view_centerOn = createRootAction("Center On Selection", "", "Actions", "Crosshair");
    view_centerOnDefn = createRootAction("Center On Definition", "", "Actions", "Definition");
    view_centerOnImpl = createRootAction("Center On Implementation", "", "Actions", "Implementation");
    view_viewConnections = createRootAction("View Connections", "", "Actions", "Connections");
    view_viewInNewWindow = createRootAction("View In New Window", "", "Actions", "Popup");

    view_viewInNewWindow->setData("Test ID");
    view_viewInNewWindow->setProperty("ID", 12);
    view_viewInNewWindow->setProperty("ID2", "HELPP");
    view_viewInNewWindow->setProperty("ID3",  view_viewInNewWindow->icon());

    window_printScreen = createRootAction("Print Screen", "", "Actions", "PrintScreen");
    window_displayMinimap = createRootAction("Display Minimap", "", "Actions", "Minimap");

    model_clearModel = createRootAction("Clear Model", "", "Actions", "Clear");
    model_validateModel = createRootAction("Validate Model", "", "Actions", "Validate");
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
    file_exit = createRootAction("Exit", "", "Actions", "Power");

    toolbar_contextToolbar = createRootAction("Show Context Toolbar", "", "Actions", "Toolbar");

    toolbar_addChild = createRootAction("Add Child Entity", "", "Actions", "Plus");
    toolbar_hardware = createRootAction("Deploy Selection", "", "Actions", "Computer");
    toolbar_disconnectHardware = createRootAction("Disconnect Selection From Its Current Deployment", "", "Actions", "Computer_Cross");
    //toolbar_popOutDefn = createRootAction("View Selection's Definition", "", "Actions", "Popup");
    //toolbar_popOutImpl = createRootAction("View Selection's Implementation", "", "Actions", "Popup");
    //toolbar_popOutInst = createRootAction("View Selection's Instance", "", "Actions", "Popup");
    toolbar_getCPP = createRootAction("Get CPP Code", "", "Actions", "getCPP");
    toolbar_setReadOnly = createRootAction("Set Selection To Read Only", "", "Actions", "Lock_Closed");
    toolbar_unsetReadOnly = createRootAction("Unset Selection From Read Only", "", "Actions", "Lock_Open");
    toolbar_expand = createRootAction("Expand Selection", "", "Actions", "Expand");
    toolbar_contract = createRootAction("Contract Selection", "", "Actions", "Contract");
    toolbar_wiki = createRootAction("View Wiki Page For Selected Entity", "", "Actions", "Wiki");
    toolbar_replicateCount = createRootAction("Change Replicate Count", "", "Actions", "Replicate_Count");
    toolbar_displayedChildrenOption = createRootAction("Change Displayed Nodes Settings", "", "Actions", "Menu_Vertical");
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
    menu_view->addAction(view_fitToScreen);
    menu_view->addSeparator();
    menu_view->addAction(view_centerOn);
    menu_view->addAction(view_centerOnDefn);
    menu_view->addAction(view_centerOnImpl);
    menu_view->addSeparator();
    menu_view->addAction(view_viewConnections);
    menu_view->addAction(view_viewInNewWindow);

    // Model Menu
    menu_model->addAction(model_validateModel);
    menu_model->addAction(model_clearModel);
    menu_model->addAction(model_executeLocalJob);

    // Jenkins Menu
    menu_jenkins->addAction(jenkins_importNodes);
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

    applicationToolbar->addAction(toolbar_contextToolbar);
    applicationToolbar->addSeperator();
    applicationToolbar->addAction(edit_undo);
    applicationToolbar->addAction(edit_redo);
    applicationToolbar->addSeperator();
    applicationToolbar->addAction(edit_cut);
    applicationToolbar->addAction(edit_copy);
    applicationToolbar->addAction(edit_paste);
    applicationToolbar->addAction(edit_replicate);
    applicationToolbar->addSeperator();
    applicationToolbar->addAction(view_fitToScreen);
    applicationToolbar->addAction(view_centerOn);
    applicationToolbar->addAction(view_viewInNewWindow);
    applicationToolbar->addSeperator();
    applicationToolbar->addAction(edit_sort);
    applicationToolbar->addAction(edit_delete);
    applicationToolbar->addSeperator();
    applicationToolbar->addAction(edit_alignVertical);
    applicationToolbar->addAction(edit_alignHorizontal);
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
    contextToolbar->addAction(toolbar_getCPP);
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

