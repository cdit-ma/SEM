#include "nodeitem.h"
#include <QGraphicsTextItem>

NodeItem::NodeItem(Node *node, NodeItem *parent):QObject(parent)
{
    QString x = node->getData("x");
    QString y = node->getData("y");

    name = node->getData("label");
    kind = node->getData("kind");

    QGraphicsTextItem* label  = new QGraphicsTextItem(name,this);

    this->isPressed = false;

    this->node = node;
    this->previousMousePosition = QPointF(x.toInt(),y.toInt());

    this->setAcceptTouchEvents(true);

    if(parent != 0){
        this->width = parent->width - (parent->width /3);
        this->height =  parent->height - (parent->height /3);
        this->setParentItem(parent);
        //this->setParent(parent);
    }else{
        this->width = 250;
        this->height = 250;
    }

    setPos(previousMousePosition);

    bRec = QRect(0,0,this->width,this->height);
}

QRectF NodeItem::boundingRect() const
{
    return bRec;
}


void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QRectF rectangle = boundingRect();
   // update(boundingRect());


    QBrush Brush(Qt::white);

    if(kind == "OutEventPort"){
        Brush.setColor(Qt::red);
    }else if(kind == "InEventPort"){
        Brush.setColor(Qt::green);
    }else if(kind == "ComponentInstance"){
        Brush.setColor(Qt::gray);
    }else if(kind == "Attribute"){
        Brush.setColor(Qt::blue);
    }


    /*
    if(!scene()->collidingItems(this).isEmpty()){
        Brush.setColor(Qt::red);

        foreach(QGraphicsItem * item, scene()->collidingItems(this)){
            item->update(item->boundingRect());
        }
    }
    */

   // if(this->isPressed){
//
  //      Brush.setColor(Qt::green);
  //  }

    painter->fillRect(rectangle, Brush);
    painter->drawRect(rectangle);


}
void NodeItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{

    this->isPressed = true;
    this->previousMousePosition = event->scenePos();
   // update(boundingRect());

    if ( event->button() == Qt::RightButton ) {
        emit exportSelected(node);
    }else{
        emit setSelected(this);
    }



}

void NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    this->isPressed = false;
    //update(boundingRect());
}

void NodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(this->isPressed){
        QPointF delta = this->pos() + (event->scenePos() - this->previousMousePosition);
        this->setPos(delta);

       /* for(int i=0; i < this->childItems().size(); i++){
            this->childItems().at(i)->setPos(delta);
        }
        */


        this->previousMousePosition = event->scenePos();
    }
}
