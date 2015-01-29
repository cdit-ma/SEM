#ifndef TOOLBARWIDGET_H
#define TOOLBARWIDGET_H

#include "nodeitem.h"

#include <QWidget>
#include <QToolButton>
#include <QMenu>


class ToolbarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ToolbarWidget(QWidget *parent = 0);

    void setNodeItem(NodeItem* item);

signals:
    void goToDefinition(Node* node);
    void goToImplementation(Node* node);

    void updateMenuList(Node* node);

    void constructNode(QString nodeKind);

public slots:
    void goToDefinition();
    void goToImplementation();

    void getAdoptableNodeList();
    void updateMenuList(QString action, QStringList nodeList);

    void addChildNode();

private:
    void setupToolBar();
    void setupButtonMenus();
    void makeConnections();
    void connectToView();

    NodeItem* nodeItem;

    QToolButton* addChildButton;
    QToolButton* deleteButton;
    QToolButton* connectButton;

    QToolButton* definitionButton;
    QToolButton* implementationButton;

    QMenu* addMenu;
    QMenu* connectMenu;
};

#endif // TOOLBARWIDGET_H
