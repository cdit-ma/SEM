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

    void setCurrentNodeItem(NodeItem* item);
    void setNodeItem(NodeItem* item);

    void showDefinitionButton(Node *definition);
    void showImplementationButton(Node* implementation);


protected:
    virtual void enterEvent(QEvent*);
    virtual void leaveEvent(QEvent*);



signals:
    void goToDefinition(Node* node);
    void goToImplementation(Node* node);

    void updateMenuList(QString action, Node* node);

    void constructNode(QString nodeKind);
    void constructEdge(Node* src, Node* dst);
    void constructNewView(Node* node);

    void constructComponentInstance(Node* assm, Node* defn, int sender);

    void toolbarClosed();

public slots:
    void goToDefinition();
    void goToImplementation();

    void getAdoptableNodesList();
    void getLegalNodesList();
    void getFilesList();
    void getComponentInstanceList();
    void updateMenuList(QString action, QStringList* stringList, QList<Node*>* nodeList);

    void addChildNode();
    void connectNodes();
    void makeNewView();

    void addComponentInstance();
    //void addInEventPortInstance();
    //void addOutEventPortInstance();

    void hideToolbar(bool actionTriggered);


private:
    void setupToolBar();
    void setupMenus();
    void makeConnections();
    void connectToView();
    void updateToolButtons();

    void setupAdoptableNodesList(QStringList *nodeKinds);
    void setupLegalNodesList(QList<Node*> *nodeList);

    void setupFilesList(QList<Node *> *files);
    void setupComponentInstanceList(QList<Node*> *components);

    void setupChildrenComponentInstanceList(QList<Node*> *componentInstances);
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

    ToolbarWidgetAction* componentInstanceAction;
    ToolbarWidgetAction* inEventPortDelegateAction;
    ToolbarWidgetAction* outEventPortDelegateAction;

    ToolbarWidgetMenu* addMenu;
    ToolbarWidgetMenu* connectMenu;
    ToolbarWidgetMenu* definitionMenu;
    ToolbarWidgetMenu* implementationMenu;

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
