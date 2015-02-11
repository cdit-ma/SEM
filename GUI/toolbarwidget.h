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

class ToolbarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ToolbarWidget(NodeView *parent = 0);

    void setNodeItem(NodeItem* item);

    void showDefinitionButton(bool show, Node* definition = 0);
    void showImplementationButton(bool show, Node* implementation = 0);

    void checkDefinition();
    void checkImplementation();


protected:
    virtual void enterEvent(QEvent* event);
    virtual void leaveEvent(QEvent* event);


signals:
    void checkDefinition(Node* node, bool show);
    void checkImplementation(Node* node, bool show);

    void goToDefinition(Node* node);
    void goToImplementation(Node* node);

    void updateMenuList(QString action, Node* node);

    void constructNode(QString nodeKind);
    void constructEdge(Node* src, Node* dst);
    void constructNewView(Node* node);

    void constructComponentInstance(Node* assm, Node* defn, int sender);


public slots:
    void goToDefinition();
    void goToImplementation();

    void getAdoptableNodesList();
    void getLegalNodesList();
    void getComponentDefinitionsList();
    void updateMenuList(QString action, QStringList* nodeKinds, QList<Node*>* nodeList);

    void addChildNode();
    void connectNodes();
    void makeNewView();

    void addComponentInstance();

    void hideToolbar(QAction* action);
    void hideToolbar();

    void showMenu();
    void checkAddInstanceAction();

private:
    void setupToolBar();
    void setupButtonMenus();
    void makeConnections();
    void connectToView();

    void updateToolButtons();

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

    ToolbarWidgetAction* addInstanceAction;

    QMenu* addMenu;
    QMenu* connectMenu;
    QMenu* definitionMenu;
    QMenu* implementationMenu;
    QMenu* addInstanceActionMenu;

    QFrame* frame;

    bool eventFromToolbar;

};

#endif // TOOLBARWIDGET_H
