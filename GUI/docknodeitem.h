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

    NodeItem* getNodeItem();
    void connectToNodeItem();

    void setContainer(DockScrollArea* container);

protected:
    void paintEvent(QPaintEvent* e);

signals:
    void itemPressed(QString kind);

    void dockNode_addComponentInstance(Node* assm, Node* defn);
    void dockNode_connectComponentInstance(Node* inst, Node* defn);

    void removeFromDockNodeList(QWidget* widget);

    void getSelectedNode();

public slots:
    void buttonPressed();
    void setSelected(bool selected);

    void updateData();
    void deleteLater();
    void setOpacity(double opacity);

    void setSelectedNode(Node* node);

private:
    DockScrollArea* parentContainer;
    NodeItem* nodeItem;
    Node* selectedNode;

    QString kind;
    QString label;

    QImage* image;
    QLabel* textLabel;

    QString color;
    QString selectedColor;
    bool isSelected;

};

#endif // DOCKNODEITEM_H
