#ifndef ACTIONCONTROLLER_H
#define ACTIONCONTROLLER_H

#include <QObject>
#include <QAction>
#include <QMenu>
#include "../Widgets/New/actiongroup.h"
#include "../Widgets/New/selectioncontroller.h"
#include "rootaction.h"

class ActionController : public QObject
{
    Q_OBJECT
public:
    explicit ActionController(QObject *parent = 0);

    void connectSelectionController(SelectionController* controller);

    QMenu* getMainMenu();
private slots:
    void selectionChanged(int selectionSize);
    void themeChanged();
public:
    SelectionController* selectionController;

    ActionGroup* applicationToolbar;

    //EDIT
    RootAction* edit_undo;
    RootAction* edit_redo;
    RootAction* edit_cut;
    RootAction* edit_copy;
    RootAction* edit_paste;
    RootAction* edit_replicate;
    RootAction* edit_delete;
    RootAction* edit_search;
    RootAction* edit_sort;
    RootAction* edit_alignHorizontal;
    RootAction* edit_alignVertical;

    RootAction* menu_exit;
    RootAction* menu_settings;

    RootAction* view_fitToScreen;
    RootAction* view_centerOn;
    RootAction* view_centerOnDefn;
    RootAction* view_centerOnImpl;
    RootAction* view_viewConnections;
    RootAction* view_viewInNewWindow;


    RootAction* toolbar_contextToolbar;


    QAction* view_CenterOn;
    QAction* view_CycleActiveSelectionForward;
    QAction* view_CycleActiveSelectionBackward;

    QMenu* mainMenu;
    QMenu* mainMenu_file;
    QMenu* mainMenu_edit;
    QMenu* mainMenu_view;
    QMenu* mainMenu_model;
    QMenu* mainMenu_jenkins;
    QMenu* mainMenu_help;
    QMenu* mainMenu_window;

private:
    void setupActions();
    void setupMainMenu();
    void setupApplicationToolbar();
};

#endif // ACTIONCONTROLLER_H
