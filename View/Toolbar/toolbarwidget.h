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

    void showDefinitionButton(Node *definition);
    void showImplementationButton(Node* implementation);

protected:
    virtual void enterEvent(QEvent*);
    virtual void leaveEvent(QEvent*);

public slots:
    void goToDefinition();
    void goToImplementation();
    void goToInstance();

    void addChildNode();
    void connectNodes();
    void addConnectedNode();
    void makeNewView();

    void attachOptionMenu();

    void hideToolbar(bool actionTriggered);

private:
    void setupToolBar();
    void setupMenus();
    void makeConnections();
    void updateToolButtons();
    void updateMenuLists();

    void multipleSelection(QList<NodeItem *> items);

    void setupInstancesList(QList<Node*> instances);

    void setupAdoptableNodesList(QStringList nodeKinds);
    void setupLegalNodesList(QList<Node*> nodeList);

    void setupFilesList(QList<Node*> files, QString kind);
    void setupComponentList(QList<Node*> components, QString kind);

    void setupChildrenComponentInstanceList(QList<Node*> componentInstances);
    void setupInEventPortInstanceList();
    void setupOutEventPortInstanceList();

    void clearMenus();

    NodeView* nodeView;
    NodeItem* currentSelectedItem;

    NodeItem* nodeItem;
    NodeItem* prevNodeItem;

    Node* definitionNode;
    Node* implementationNode;

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

    ToolbarWidgetAction* componentImplAction;
    ToolbarWidgetAction* componentInstanceAction;
    ToolbarWidgetAction* inEventPortDelegateAction;
    ToolbarWidgetAction* outEventPortDelegateAction;

    ToolbarWidgetMenu* addMenu;
    ToolbarWidgetMenu* connectMenu;
    ToolbarWidgetMenu* definitionMenu;
    ToolbarWidgetMenu* implementationMenu;
    //ToolbarWidgetMenu* showConnectionsMenu;

    ToolbarWidgetMenu* instancesMenu;
    ToolbarWidgetMenu* instanceOptionMenu;

    ToolbarWidgetMenu* fileMenu;
    ToolbarWidgetMenu* inEventPort_componentInstanceMenu;
    ToolbarWidgetMenu* outEventPort_componentInstanceMenu;

    ToolbarWidgetAction* fileDefaultAction;
    ToolbarWidgetAction* eventPort_componentInstanceDefaultAction;

    QFrame* frame;
    int frameVisibilityCount;
    bool eventFromToolbar;
    QList<QToolButton*> singleSelectionToolButtons;
    QList<QToolButton*> multipleSelectionToolButtons;

};

#endif // TOOLBARWIDGET_H
