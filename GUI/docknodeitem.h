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

    void mousePressEvent(QMouseEvent *event);
    void setContainer(DockScrollArea* container);

protected:
    void paintEvent(QPaintEvent* e);

signals:
    void itemPressed(QString kind);
    void dockNode_addComponentInstance(NodeItem* itm);

    void removeFromDockNodeList(QWidget* widget);

public slots:
    void buttonPressed();
    void updateData();
    void deleteLater();
    void setSelected(bool selected);
    void setOpacity(double opacity);

private:
    DockScrollArea* parentContainer;
    NodeItem* nodeItem;
    QString kind;
    QString label;

    QImage* image;
    QLabel* textLabel;

    QString color;
    QString selectedColor;
    bool isSelected;

};

#endif // DOCKNODEITEM_H
