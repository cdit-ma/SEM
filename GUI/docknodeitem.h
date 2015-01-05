#ifndef DOCKNODEITEM_H
#define DOCKNODEITEM_H

#include <QPushButton>
#include <QImage>
#include <QColor>
#include <QLabel>

#include "docknodeitem.h"
#include "../Model/graphml.h"

class NodeItem;

class DockNodeItem : public QPushButton
{
    Q_OBJECT
public:
    explicit DockNodeItem(NodeItem *node_item, QWidget* parent = 0);

    NodeItem* getNodeItem();

    void mousePressEvent(QMouseEvent *event);

protected:
    void paintEvent(QPaintEvent* e);

private:    
    NodeItem* nodeItem;
    QString kind;
    QString label;

    QImage* image;
    QLabel* textLabel;

    QString color;
    QString selectedColor;
    bool isSelected;

signals:
    void itemPressed(QString kind);
    void clearDockSelection();
    void dockNode_addComponentInstance(NodeItem* itm);

public slots:
    void buttonPressed();
    void updateData();
    void setSelected(bool selected);
    void setOpacity(double opacity);
};

#endif // DOCKNODEITEM_H
