#ifndef TOOLBARWIDGET_H
#define TOOLBARWIDGET_H

#include "nodeview.h"
#include "nodeitem.h"

#include <QWidget>
#include <QToolButton>
#include <QMenu>
#include <QFrame>


class ToolbarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ToolbarWidget(NodeView *parent = 0);

    void setNodeItem(NodeItem* item);

    void showDefinitionButton(bool show, Node* definition = 0);
    void showImplementationButton(bool show, Node* implementation = 0);

signals:
    void goToDefinition(Node* node);
    void goToImplementation(Node* node);

    void updateMenuList(QString action, Node* node);

    void constructNode(QString nodeKind);
    void constructEdge(Node* src, Node* dst);

    void checkDefinition(Node* node, bool show);
    void checkImplementation(Node* node, bool show);


    void constructNewView(Node* node);

public slots:
    void goToDefinition();
    void goToImplementation();

    void getAdoptableNodeList();
    void getLegalNodesList();
    void updateMenuList(QString action, QStringList* nodeKinds, QList<Node*>* nodeList);

    void addChildNode();
    void connectNodes();

    void hideToolbar();
    void resetToolbarStates();

    void makeNewView();

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

    QMenu* addMenu;
    QMenu* connectMenu;
    QMenu* definitionMenu;
    QMenu* implementationMenu;

    QFrame* frame;
    int showFrame;
};

#endif // TOOLBARWIDGET_H
