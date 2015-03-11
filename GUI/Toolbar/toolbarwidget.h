#ifndef TOOLBARWIDGET_H
#define TOOLBARWIDGET_H

#include "nodeview.h"
#include "nodeitem.h"
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

    //void setCurrentNodeItem(NodeItem* item);
    void setNodeItem(NodeItem* item);

    void showDefinitionButton(Node *definition);
    void showImplementationButton(Node* implementation);


protected:
    virtual void enterEvent(QEvent*);
    virtual void leaveEvent(QEvent*);


signals:
    void toolbar_goToDefinition(Node* node);
    void toolbar_goToImplementation(Node* node);
    void toolbar_goToInstance(Node* node);

    void toolbar_constructNode(QString nodeKind, int sender);
    void toolbar_constructEdge(Node* src, Node* dst);
    void toolbar_constructNewView(Node* node);

    void toolbar_constructComponentInstance(Node* assm, Node* defn, int sender);
    void toolbar_constructEventPortDelegate(Node* assm, Node* eventPortInstance);

    void toolbar_closed();


public slots:
    void goToDefinition();
    void goToImplementation();
    void goToInstance();

    void addChildNode();
    void connectNodes();
    void makeNewView();

    void addComponentInstance();
    void addEventPorDelegate();

    void attachOptionMenu();

    void hideToolbar(bool actionTriggered);

    void testSlot();

private:
    void setupToolBar();
    void setupMenus();
    void makeConnections();
    void connectToView();
    void updateToolButtons();
    void updateMenuLists();

    void setupInstancesList(QList<Node*> instances);

    void setupAdoptableNodesList(QStringList nodeKinds);
    void setupLegalNodesList(QList<Node*> nodeList);

    void setupFilesList(QList<Node*> files);
    void setupComponentInstanceList(QList<Node*> components);

    void setupChildrenComponentInstanceList(QList<Node*> componentInstances);
    void setupInEventPortInstanceList();
    void setupOutEventPortInstanceList();

    void clearMenus();


    NodeView* parentNodeView;
    NodeItem* currentSelectedItem;

    NodeItem* nodeItem;
    NodeItem* prevNodeItem;

    Node* definitionNode;
    Node* implementationNode;

    QToolButton* addChildButton;
    QToolButton* deleteButton;
    QToolButton* connectButton;
    QToolButton* showNewViewButton;
    QToolButton* definitionButton;
    QToolButton* implementationButton;
    QToolButton* instancesButton;

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
    ToolbarWidgetAction* componentInstanceDefaultAction;

    ToolbarWidgetAction* eventPort_componentInstanceDefaultAction;
    ToolbarWidgetAction* inEventPortDefaultAction;
    ToolbarWidgetAction* outEventPortDefaultAction;

    QFrame* frame;
    bool eventFromToolbar;

    //QList<Node*>* componentInstances;

};

#endif // TOOLBARWIDGET_H
