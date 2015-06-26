#ifndef TOOLBARWIDGET_H
#define TOOLBARWIDGET_H

#include "../nodeview.h"
#include "../GraphicsItems/nodeitem.h"
#include "toolbarwidgetaction.h"

#include <QWidget>
#include <QToolButton>
#include <QMenu>
#include <QFrame>


class ToolbarWidgetAction;
class ToolbarWidgetMenu;

class ToolbarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ToolbarWidget(NodeView *parent = 0);

    void updateSelectedNodeItem(QList<NodeItem *> items);

    void showDefinitionButton(QString definitionID);
    void showImplementationButton(QString implementationID);

    void showSnippetButton(QString button, bool show);

    void updateSeparators(bool snippet = true, bool goTo = true);

protected:
    virtual void enterEvent(QEvent*);
    virtual void leaveEvent(QEvent*);

public slots:
    void updateActionEnabled(QString actionName, bool enabled);

    void goToDefinition();
    void goToImplementation();
    void goToInstance();

    void addChildNode();
    void connectNodes();
    void addConnectedNode();

    void makeNewView();

    void exportGraphMLSnippet();
    void importGraphMLSnippet();

    void attachOptionMenu();

    void hideToolbar(bool actionTriggered);

private:
    void setupToolBar();
    void setupMenus();
    void makeConnections();
    void updateToolButtons();
    void updateMenuLists();

    void multipleSelection(QList<NodeItem *> items);

    void setupInstancesList(QList<NodeItem*> instances);

    void setupAdoptableNodesList(QStringList nodeKinds);
    void setupLegalNodesList(QList<NodeItem *> nodeList);

    void setupFilesList(QList<NodeItem*> files, QString kind);
    void setupComponentList(QList<NodeItem*> components, QString kind);
    void setupBlackBoxList(QList<NodeItem*> blackBoxes);

    void setupChildrenComponentInstanceList(QList<NodeItem *> componentInstances);
    void setupInEventPortInstanceList();
    void setupOutEventPortInstanceList();

    void clearMenus();

    NodeView* nodeView;
    NodeItem* currentSelectedItem;

    NodeItem* nodeItem;
    NodeItem* prevNodeItem;

    QString definitionNodeID;
    QString implementationNodeID;

    QToolButton* addChildButton;
    QToolButton* deleteButton;
    QToolButton* connectButton;

    QToolButton* showNewViewButton;
    QToolButton* showConnectionsButton;

    QToolButton* definitionButton;
    QToolButton* implementationButton;
    QToolButton* instancesButton;

    QToolButton* alignVerticallyButton;
    QToolButton* alignHorizontallyButton;

    QToolButton* exportSnippetButton;
    QToolButton* importSnippetButton;

    ToolbarWidgetAction* componentImplAction;
    ToolbarWidgetAction* componentInstanceAction;
    ToolbarWidgetAction* inEventPortDelegateAction;
    ToolbarWidgetAction* outEventPortDelegateAction;

    ToolbarWidgetMenu* addMenu;
    ToolbarWidgetMenu* connectMenu;
    ToolbarWidgetMenu* definitionMenu;
    ToolbarWidgetMenu* implementationMenu;

    ToolbarWidgetMenu* instancesMenu;
    ToolbarWidgetMenu* instanceOptionMenu;

    ToolbarWidgetMenu* fileMenu;
    ToolbarWidgetMenu* inEventPort_componentInstanceMenu;
    ToolbarWidgetMenu* outEventPort_componentInstanceMenu;

    ToolbarWidgetAction* fileDefaultAction;
    ToolbarWidgetAction* inEventPort_componentInstanceDefaultAction;
    ToolbarWidgetAction* outEventPort_componentInstanceDefaultAction;

    ToolbarWidgetAction* blackBoxInstanceDefaultAction;
    ToolbarWidgetAction* blackBoxInstanceAction;
    ToolbarWidgetMenu* blackBoxInstanceMenu;

    QList<QToolButton*> singleSelectionToolButtons;
    QList<QToolButton*> multipleSelectionToolButtons;
    QList<QToolButton*> alterModelToolButtons;

    QFrame* mainFrame;
    QFrame* shadowFrame;
    QFrame* snippetFrame;
    QFrame* alterViewFrame;
    QFrame* goToFrame;

    bool eventFromToolbar;
    bool showToolbar;
    bool showAlterViewFrame;
    bool showSnippetFrame;
    bool showGoToFrame;

};

#endif // TOOLBARWIDGET_H
