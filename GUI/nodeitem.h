#ifndef NODEITEM_H
#define NODEITEM_H

#include <QAbstractItemModel>
#include <QAbstractTableModel>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsColorizeEffect>
#include <QGraphicsSceneMouseEvent>
#include <QObject>
#include <QPainter>
#include <QTouchEvent>
#include <iostream>
#include <QRubberBand>
#include <QGraphicsProxyWidget>
#include <QPushButton>

#include "../Model/node.h"
#include "../Model/graphmldata.h"
#include "attributetablemodel.h"
#include "graphmlitem.h"

class NodeEdge;
class NodeItem : public GraphMLItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    NodeItem(Node *node, NodeItem *parent);
    ~NodeItem();
    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);


    double width;
    double height;

    void addNodeEdge(NodeEdge* line);
    void removeNodeEdge(NodeEdge* line);

    float getChildSize();
    int getDepth();

    //void setHidden(bool h);
    void resetSize();

    bool isExpanded();
    bool hasExpandButton();
    void removeExpandButton();


signals:
    //Node Edge Signals
    void setEdgeVisibility(bool visible);
    void updateEdgePosition();
    void setEdgeSelected(bool selected);

    //View Signals.
    void moveSelection(QPointF delta);
    void clearSelection();
    void centerModel();

    //DockNodeItem Signals
    void updateDockNodeItem();
    void updateDockNodeItem(bool selected);
    void updateOpacity(qreal opacity);

    void sortModel();

    void addExpandButtonToParent();
public slots:
    //Model Signals
    void graphMLDataUpdated(GraphMLData *data);


public slots:
    void setOpacity(qreal opacity);
    void setRubberbandMode(bool On);
    void setSelected(bool selected);
    void setVisible(bool visible);

    void sort();

    void setHidden(bool hidden);
    void setHideChildren(bool hideChildren);

    //Depth/Aspect Slots
    void toggleDetailDepth(int depth);
    void updateViewAspects(QStringList aspects);

    void addExpandButton();
    void expandItem(bool show);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private:
    void updateBrushes();

    void setPos(qreal x, qreal y);
    void setPos(const QPointF &pos);

    //Visual Components
    QGraphicsTextItem* label;
    QGraphicsPixmapItem* icon;
    QGraphicsProxyWidget *proxyWidget;
    QPushButton *expandButton;

    QRubberBand* rubberBand;
    QVector<NodeEdge*> connections;

    bool USING_RUBBERBAND_SELECTION;

    bool isSelected;
    bool isNodePressed;

    float defaultChildSize;

    QString nodeKind;
    double initialWidth;
    double initialHeight;

    double prevWidth;
    double prevHeight;


    QStringList currentViewAspects;
    //QRectF bRec;

    //Used to store the initial position of the Rubber band selection.
    QPointF rubberBand_ScreenOrigin;
    QPointF rubberBand_SceneOrigin;

    QPointF previousScenePosition;
    bool hasSelectionMoved;

    //Used to store the Color/Brush/Pen for the selected Style.
    QColor selectedColor;
    QColor color;
    QBrush selectedBrush;
    QBrush brush;
    QPen pen;
    QPen selectedPen;



    bool DRAW_DETAIL;

    bool DRAW_OBJECT;



    QString parentNodeKind;

    bool hidden;
    int depth;



    void setLabelFont();
    void setupIcon();
     bool expanded;



    int getNumberOfChildren();
    QStringList getChildrenKind();

    double getCurvedCornerWidth();
    double getMaxLabelWidth();

    Node* getNode();
};

#endif // NODEITEM_H
