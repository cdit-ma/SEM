#include "nodeconnection.h"
#include <QDebug>

NodeConnection::NodeConnection(Edge *edge, NodeItem* s, NodeItem* d):QObject()
{

    connect(edge, SIGNAL(deleteGUI(Edge*)), this, SLOT(deleteD(Edge*)));


    graphicsEffect = new QGraphicsColorizeEffect();

    QColor blue(70,130,180);
    graphicsEffect->setColor(blue);
    graphicsEffect->setStrength(0);



    this->edge=edge;
    this->source = s;
    this->destination = d;

    source->addConnection(this);
    destination->addConnection(this);
    QGline=0;


    QString text = edge->getName();
    label  = new QGraphicsTextItem(text,this);

    width = 50;
    height = 50;

    bRec = QRect(0, 0, width, height);

    linePen.setWidth(((d->height + s->height) /2)/20);
    linePen.setColor(Qt::gray);

    inScene = false;

    this->setGraphicsEffect(graphicsEffect);


    updateLine();
}

NodeConnection::~NodeConnection()
{
    source->deleteConnnection(this);
    destination->deleteConnnection(this);
    delete QGline;
}

QRectF NodeConnection::boundingRect() const
{
    return bRec;
}

void NodeConnection::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
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

void NodeConnection::addToScene(QGraphicsScene *scene)
{
    QGline = scene->addLine(line,linePen);
    QGline->setGraphicsEffect(graphicsEffect);
    scene->addItem(this);
    inScene = true;
}



void NodeConnection::deleteD(Edge *)
{
    qCritical() << "SHOULD DELETE";
    delete this;
}

void NodeConnection::setSelected()
{
    graphicsEffect->setStrength(1);
}

void NodeConnection::setDeselected()
{
    graphicsEffect->setStrength(0);
}

void NodeConnection::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if ( event->button() == Qt::LeftButton ) {
        emit setSelected(this);
    }else if ( event->button() == Qt::RightButton ) {
        delete edge;
    }

}

void NodeConnection::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{

}

void NodeConnection::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{

}


void NodeConnection::updateLine()
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
        if(source->drawObject && destination->drawObject){

            QGline->setVisible(true);
        }else{
            QGline->setVisible(false);
        }
    }

    this->setPos(((sx+dx)/2) - (width / 2), ((sy+dy) /2) - (height / 2));
}
