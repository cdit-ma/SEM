#include "nodeedge.h"
#include <QDebug>
#include "graphmlitem.h"

NodeEdge::NodeEdge(Edge* edge, NodeItem* s, NodeItem* d): GraphMLItem(edge)
{
    //Only show the Parent Instance
    IS_VISIBLE = true;
    IS_SELECTED = false;

    IS_INSTANCE_LINK = edge->isInstanceLink();
    IS_IMPL_LINK = edge->isImplLink();
    IS_AGG_LINK = edge->isAggregateLink();

    if(IS_INSTANCE_LINK || IS_IMPL_LINK){
        Node* sNode = s->node;
        Node* dNode = d->node;

        if(sNode->getParentNode()->isDefinition() && (dNode->getParentNode()->isInstance() || dNode->getParentNode()->isImpl())){
            //Don't show Non-Top Most Instance Links
            IS_VISIBLE = false;
        }
    }

    //Setup Instance Variables
    QGline = 0;
    label = 0;
    inScene = false;

    //Set Circle Width/Height
    width = 50;
    height = 50;

    //Construct the Bounding Rectangle
    bRec = QRect(0, 0, width, height);


    //Set Flags
    setFlag(ItemDoesntPropagateOpacityToChildren);
    setFlag(ItemIgnoresParentOpacity);
    setFlag(ItemIsSelectable);

    //Add this Node Edge to the Node Items
    source = s;
    destination = d;
    source->addConnection(this);
    destination->addConnection(this);


    updateBrushes();

    updateLabel();
    updateLine();
}

NodeEdge::~NodeEdge()
{
    source->deleteConnnection(this);
    destination->deleteConnnection(this);

    delete QGline;
}

QRectF NodeEdge::boundingRect() const
{
    return bRec;
}

void NodeEdge::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if(IS_VISIBLE){
        QRectF rectangle = boundingRect();

        QPen Pen;
        QBrush Brush;

        if(IS_SELECTED){
            Brush = selectedBrush;
            Pen = selectedPen;
        }else{
            Brush = brush;
            Pen = pen;
        }

        painter->setBrush(Brush);
        painter->setPen(Pen);

        QPainterPath circle_path;
        circle_path.addEllipse(rectangle);
        painter->drawPath(circle_path);
    }
}

NodeItem *NodeEdge::getSource()
{
    return source;
}

NodeItem *NodeEdge::getDestination()
{
    return destination;
}

void NodeEdge::addToScene(QGraphicsScene *scene)
{
    if(scene){
        QGline = scene->addLine(line, pen);
        inScene = true;
        scene->addItem(this);
        setVisible(IS_VISIBLE);
    }
}


void NodeEdge::destructNodeEdge()
{
    delete this;
}

void NodeEdge::setOpacity(qreal opacity)
{
    //TODO
    Q_UNUSED(opacity);
}


void NodeEdge::setSelected(bool selected)
{
    IS_SELECTED = selected;

    if(selected){
        QGline->setPen(selectedPen);
    }else{
        QGline->setPen(pen);
    }

}

void NodeEdge::setVisible(bool visible)
{

    if(IS_VISIBLE){

        if(source->isVisible() && destination->isVisible()){
            if(QGline){
                QGline->setVisible(visible);
            }
            QGraphicsItem::setVisible(visible);
        }else{
            if(QGline){
                QGline->setVisible(false);
            }
            QGraphicsItem::setVisible(false);
        }


    }else{
        if(QGline){
            QGline->setVisible(false);
        }
        QGraphicsItem::setVisible(false);
    }
}

void NodeEdge::graphMLDataUpdated(GraphMLData *data)
{
    //TODO
    Q_UNUSED(data);
}

void NodeEdge::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(!IS_VISIBLE){
        event->setAccepted(false);
        return;
    }
    switch (event->button()) {

    case Qt::MiddleButton:{
        emit triggerCentered(getGraphML());
        break;
    }
    case Qt::LeftButton:{
        emit triggerSelected(getGraphML());
        break;
    }
    };

}

void NodeEdge::updateLabel()
{
    QString text = getGraphML()->getID();
    if(label){
        label->setDefaultTextColor(QColor(Qt::white));
        label->setPlainText(text);
    }else{
        label = new QGraphicsTextItem(text, this);
    }
}

void NodeEdge::updateBrushes()
{
    if(IS_INSTANCE_LINK){
        color = QColor(0, 0, 180);
    }else if(IS_IMPL_LINK){
        color = QColor(0, 180, 0);
    }else if(IS_AGG_LINK){
        color = QColor(180, 0, 0);
    }else{
        color = QColor(50, 50, 50);
    }

    selectedColor = color;
    color.setAlpha(150);
    selectedColor.setAlpha(250);

    brush = QBrush(color);
    selectedBrush = QBrush(selectedColor);

    pen.setColor(color);
    pen.setWidth(4);
    selectedPen.setColor(selectedColor);
    selectedPen.setWidth(8);
}


void NodeEdge::updateLine()
{
    float sx = source->scenePos().x() + source->width/2;
    float sy = source->scenePos().y() + source->height/2;

    float dx = destination->scenePos().x() + destination->width/2;;
    float dy = destination->scenePos().y() + destination->height/2;;

    line.setP1(QPoint(sx,sy));
    line.setP2(QPoint(dx,dy));

    if(QGline){
        QGline->setPen(pen);
        QGline->setLine(line);
    }

    setPos(((sx+dx)/2) - (width / 2), ((sy+dy) /2) - (height / 2));
}
