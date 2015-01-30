#ifndef TOOLBARWIDGET_H
#define TOOLBARWIDGET_H

#include "nodeitem.h"

#include <QWidget>
#include <QToolButton>
#include <QMenu>
#include <QFrame>


class ToolbarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ToolbarWidget(QWidget *parent = 0);

    void setNodeItem(NodeItem* item);

    void showDefinitionButton(bool show);
    void showImplementationButton(bool show);

signals:
    void goToDefinition(Node* node);
    void goToImplementation(Node* node);

    void updateMenuList(QString action, Node* node);

    void constructNode(QString nodeKind);
    void constructEdge(Node* src, Node* dst);

    void checkDefinition(Node* node);
    void checkImplementation(Node* node);

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

private:
    void setupToolBar();
    void setupButtonMenus();
    void makeConnections();
    void connectToView();

    void updateToolButtons();

    NodeItem* nodeItem;
    NodeItem* prevNodeItem;

    QToolButton* addChildButton;
    QToolButton* deleteButton;
    QToolButton* connectButton;

    QToolButton* definitionButton;
    QToolButton* implementationButton;

    QMenu* addMenu;
    QMenu* connectMenu;

    QFrame* frame;
    int showFrame;
};

#endif // TOOLBARWIDGET_H
