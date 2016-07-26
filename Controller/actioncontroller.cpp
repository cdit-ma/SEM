#include "actioncontroller.h"
#include "../View/theme.h"
#include <QDebug>
ActionController::ActionController(QObject *parent) : QObject(parent)
{
    selectionController = 0;


    view_CenterOn = new QAction("Center View on Selection", this);

    view_CycleActiveSelectionForward = new QAction("Cycle the current active selected item forward", this);
    view_CycleActiveSelectionForward->setShortcut(QKeySequence::NextChild);
    view_CycleActiveSelectionForward->setShortcutContext(Qt::ApplicationShortcut);


    view_CycleActiveSelectionBackward = new QAction("Cycle the current active selected item backward", this);
    view_CycleActiveSelectionBackward->setShortcut(QKeySequence::PreviousChild);
    view_CycleActiveSelectionBackward->setShortcutContext(Qt::ApplicationShortcut);



    setupActions();
    setupMainMenu();
    setupApplicationToolbar();
    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
}

void ActionController::connectSelectionController(SelectionController *controller)
{
    selectionController = controller;
    connect(selectionController, SIGNAL(selectionChanged(int)), this, SLOT(selectionChanged(int)));
    connect(view_CycleActiveSelectionForward, SIGNAL(triggered(bool)), controller, SLOT(cycleActiveSelectionForward()));
    connect(view_CycleActiveSelectionBackward, SIGNAL(triggered(bool)), controller, SLOT(cycleActiveSelectionBackward()));
}

void ActionController::selectionChanged(int selectionSize)
{
    if(selectionController){
        QVector<ViewItem*> selection = selectionController->getSelection();
        if(selectionSize == 1){
            view_CenterOn->setEnabled(true);
        }else{
            view_CenterOn->setEnabled(false);
        }

        if(selectionSize > 1){
            view_CycleActiveSelectionForward->setEnabled(true);
            view_CycleActiveSelectionBackward->setEnabled(true);
        }else{
            view_CycleActiveSelectionForward->setEnabled(false);
            view_CycleActiveSelectionBackward->setEnabled(false);
        }
        applicationToolbar->updateSpacers();
    }
}

void ActionController::themeChanged()
{
    Theme* theme = Theme::theme();
    view_CenterOn->setIcon(theme->getIcon("Actions", "Crosshair"));
    view_CycleActiveSelectionBackward->setIcon(theme->getIcon("Actions", "Arrow_Left"));
    view_CycleActiveSelectionForward->setIcon(theme->getIcon("Actions", "Arrow_Right"));


    edit_undo->setIcon(theme->getIcon("Actions", "Undo"));
    edit_redo->setIcon(theme->getIcon("Actions", "Redo"));
    edit_cut->setIcon(theme->getIcon("Actions", "Cut"));
    edit_copy->setIcon(theme->getIcon("Actions", "Copy"));
    edit_paste->setIcon(theme->getIcon("Actions", "Paste"));
    edit_replicate->setIcon(theme->getIcon("Actions", "Replicate"));
    edit_delete->setIcon(theme->getIcon("Actions", "Delete"));
    edit_search->setIcon(theme->getIcon("Actions", "Search"));
    edit_sort->setIcon(theme->getIcon("Actions", "Sort"));
    edit_alignHorizontal->setIcon(theme->getIcon("Actions", "Align_Horizontal"));
    edit_alignVertical->setIcon(theme->getIcon("Actions", "Align_Vertical"));


    view_fitToScreen->setIcon(theme->getIcon("Actions", "FitToScreen"));
    view_centerOn->setIcon(theme->getIcon("Actions", "Crosshair"));
    view_centerOnDefn->setIcon(theme->getIcon("Actions", "Definition"));
    view_centerOnImpl->setIcon(theme->getIcon("Actions", "Implementation"));
    view_viewConnections->setIcon(theme->getIcon("Actions", "Connections"));
    view_viewInNewWindow->setIcon(theme->getIcon("Actions", "Popup"));


    toolbar_contextToolbar->setIcon(theme->getIcon("Actions", "Toolbar"));
}

QMenu *ActionController::getMainMenu()
{
    return mainMenu;
}

void ActionController::setupActions()
{
    edit_undo = new RootAction("Undo");
    edit_redo = new RootAction("Redo");
    edit_cut = new RootAction("Cut");
    edit_copy = new RootAction("Copy");
    edit_paste = new RootAction("Paste");
    edit_replicate = new RootAction("Replicate");
    edit_delete = new RootAction("Delete");
    edit_search = new RootAction("Search");
    edit_sort = new RootAction("Sort");
    edit_alignVertical = new RootAction("Align Vertically");
    edit_alignHorizontal = new RootAction("Align Horizontally");

    view_fitToScreen = new RootAction("Fit To Screen");
    view_centerOn = new RootAction("Center On Selection");
    view_centerOnDefn = new RootAction("Center On Definition");
    view_centerOnImpl = new RootAction("Center On Implementation");
    view_viewConnections = new RootAction("View Connections");
    view_viewInNewWindow = new RootAction("View In New Window");

    menu_settings = new RootAction("Settings");
    menu_exit = new RootAction("Exit");


    toolbar_contextToolbar = new RootAction("Show Context Toolbar");
}

void ActionController::setupMainMenu()
{
    mainMenu = new QMenu();
    mainMenu_file = mainMenu->addMenu("File");
    mainMenu_edit = mainMenu->addMenu("Edit");
    mainMenu->addSeparator();
    mainMenu_view = mainMenu->addMenu("View");
    mainMenu_model = mainMenu->addMenu("Model");
    mainMenu_jenkins = mainMenu->addMenu("Jenkins");
    mainMenu->addSeparator();
    mainMenu_window = mainMenu->addMenu("Window");
    mainMenu->addAction(menu_settings);
    mainMenu->addSeparator();
    mainMenu_help = mainMenu->addMenu("Help");
    mainMenu->addAction(menu_exit);
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

