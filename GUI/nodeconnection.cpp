#include "nodeconnection.h"
#include <QDebug>
#include "graphmlitem.h";

NodeEdge::NodeEdge(Edge *edge, NodeItem* s, NodeItem* d): GraphMLItem(edge)
{

    itemGraphicsEffect = new QGraphicsColorizeEffect();
    lineGraphicsEffect = new QGraphicsColorizeEffect();


    QColor blue(70,130,180);
    itemGraphicsEffect->setColor(blue);
    itemGraphicsEffect->setStrength(0);

    lineGraphicsEffect->setColor(blue);
    lineGraphicsEffect->setStrength(0);



    this->edge=edge;
    this->source = s;
    this->destination = d;

    source->addConnection(this);
    destination->addConnection(this);
    QGline=0;

    this->setParentItem(0);

    QString text = edge->getName();
    label  = new QGraphicsTextItem(text,this);

    width = 20;
    height = 20;

    bRec = QRect(0, 0, width, height);

    linePen.setWidth(((d->height + s->height) /2)/20);
    linePen.setColor(Qt::red);

    inScene = false;


    this->setGraphicsEffect(itemGraphicsEffect);

    setFlag(ItemDoesntPropagateOpacityToChildren);
    setFlag(ItemIgnoresParentOpacity);
    setFlag(ItemIsSelectable);



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
    if(inScene){
        if(source->drawObject && destination->drawObject){
            QRectF rectangle = boundingRect();

            painter->setBrush(QBrush(Qt::yellow));
            QPainterPath circle_path;
            circle_path.addEllipse(rectangle);
            painter->drawPath(circle_path);
        }
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
    if(scene != 0){
        QGline = scene->addLine(line, linePen);
        QGline->setGraphicsEffect(lineGraphicsEffect);
        scene->addItem(this);
        inScene = true;
    }
}

QVariant NodeEdge::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemSelectedChange)
    {
        if (value == true)
        {
            emit this->setItemSelected(edge, true);
        }else{
            emit this->setItemSelected(edge, false);
        }
        //return 0;
    }

    return QGraphicsItem::itemChange(change, value);

}

void NodeEdge::destructNodeEdge()
{
    delete this;
}



void NodeEdge::deleteD(Edge *)
{
    delete this;
}

void NodeEdge::setSelected(bool selected)
{
    if(selected){
         itemGraphicsEffect->setStrength(1);
         lineGraphicsEffect->setStrength(1);
    }else{
         itemGraphicsEffect->setStrength(0);
         lineGraphicsEffect->setStrength(0);
    }

}

void NodeEdge::setOpacity(qreal opacity)
{
    QGraphicsItem::setOpacity(opacity);
    QGline->setOpacity(opacity);

}

void NodeEdge::setVisible(bool visible)
{
    QGraphicsItem::setVisible(visible);
    if(QGline != 0){
        QGline->setVisible(visible);
    }
}

void NodeEdge::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if ( event->button() == Qt::LeftButton ) {
        emit this->setItemSelected(edge, true);
    }

}



void NodeEdge::updateLine()
{
    sx = source->scenePos().x() + source->width/2;

    sy = source->scenePos().y() + source->height/2;

    dx = destination->scenePos().x() + destination->width/2;;

    dy = destination->scenePos().y() + destination->height/2;;

    line = QLine(sx,sy,dx,dy);

    if(QGline != 0){
        QGline->setLine(line);
    }
    if(inScene){
        if(source->isVisible() && destination->isVisible()){

            QGline->setVisible(true);
        }else{
            QGline->setVisible(false);
        }
    }

    this->setPos(((sx+dx)/2) - (width / 2), ((sy+dy) /2) - (height / 2));
}
