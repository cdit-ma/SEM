#include "nodeconnection.h"
#include <QDebug>
#include "graphmlitem.h";

NodeEdge::NodeEdge(Edge* edge, NodeItem* s, NodeItem* d): GraphMLItem(edge)
{
    //Only show the Parent Instance
    IS_VISIBLE = true;

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


    if(IS_VISIBLE){
        if(IS_INSTANCE_LINK){
            linePen.setColor(QColor(0,0,180));
            linePen.setWidth(2);

            selectedLinePen.setColor(QColor(0,0,255));
            selectedLinePen.setWidth(4);
        }else if(IS_IMPL_LINK){
            linePen.setColor(QColor(0,180,0));
            linePen.setWidth(2);

            selectedLinePen.setColor(QColor(0,255,0));
            selectedLinePen.setWidth(4);
        }else if(IS_AGG_LINK){
            linePen.setColor(QColor(180,0,180));
            linePen.setWidth(2);

            selectedLinePen.setColor(QColor(255,0,255));
            selectedLinePen.setWidth(4);
        }else{
            linePen.setColor(QColor(180,0,0));
            linePen.setWidth(((d->height + s->height) /2)/20);

            selectedLinePen.setColor(QColor(255,0,0));
            selectedLinePen.setWidth(linePen.width() + 2);
        }
    }

    //Set Flags
    setFlag(ItemDoesntPropagateOpacityToChildren);
    setFlag(ItemIgnoresParentOpacity);
    setFlag(ItemIsSelectable);

    //Add this Node Edge to the Node Items
    source = s;
    destination = d;
    source->addConnection(this);
    destination->addConnection(this);



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

        if(IS_INSTANCE_LINK){
            painter->setBrush(QBrush(Qt::yellow));
        }else{
            painter->setBrush(QBrush(Qt::white));
        }
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
        QGline = scene->addLine(line, linePen);
        inScene = true;
        scene->addItem(this);
        setVisible(IS_VISIBLE);
    }
}

QVariant NodeEdge::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemSelectedChange)
    {
        if (value == true)
        {
            emit setItemSelected(getGraphML(), true);
        }else{
            emit setItemSelected(getGraphML(), false);
        }
    }

    return QGraphicsItem::itemChange(change, value);

}

void NodeEdge::destructNodeEdge()
{
    delete this;
}

void NodeEdge::setOpacity(qreal opacity)
{

}


void NodeEdge::setSelected(bool selected)
{
    if(selected){
        QGline->setPen(selectedLinePen);
    }else{
        QGline->setPen(linePen);
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

void NodeEdge::updateLabel()
{
    QString text = getGraphML()->getID();
    if(label){
        label->setPlainText(text);
    }else{
        label = new QGraphicsTextItem(text, this);
    }
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
        QGline->setPen(linePen);
        QGline->setLine(line);
    }

    setPos(((sx+dx)/2) - (width / 2), ((sy+dy) /2) - (height / 2));
}
