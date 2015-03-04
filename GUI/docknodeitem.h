#ifndef DOCKNODEITEM_H
#define DOCKNODEITEM_H

#include <QPushButton>
#include <QImage>
#include <QColor>
#include <QLabel>

#include "docknodeitem.h"
#include "dockscrollarea.h"
#include "../Model/graphml.h"
#include "./nodeitem.h"


class DockNodeItem : public QPushButton
{
    Q_OBJECT
public:
    explicit DockNodeItem(NodeItem *node_item, QWidget* parent = 0);

    void setContainer(DockScrollArea* container);

    NodeItem* getNodeItem();

protected:
    void paintEvent(QPaintEvent* e);

signals:
    void itemPressed(QString kind);

    void getSelectedNode();

    void dockNode_addComponentInstance(Node* assm, Node* defn);
    void dockNode_connectComponentInstance(Node* inst, Node* defn);
    void dockNode_connectHardwareNode(Node* src, Node* hardwareNode);

    void removeFromDockNodeList(QWidget* widget);

public slots:
    void buttonPressed();

    void updateData();
    void deleteLater();
    void setOpacity(double opacity);

    void setSelectedNode(Node* node);

private:
    void setupLayout();
    void makeConnections();

    DockScrollArea* parentContainer;

    NodeItem* nodeItem;
    Node* selectedNode;

    QString kind;
    QString label;

    QLabel* textLabel;
    QLabel* imageLabel;

};

#endif // DOCKNODEITEM_H
