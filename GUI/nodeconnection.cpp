#include "nodeconnection.h"
#include <QDebug>

NodeConnection::NodeConnection(Edge *edge, NodeItem* s, NodeItem* d)
{
    connect(edge, SIGNAL(deleteGUI(Edge*)), this, SLOT(deleteD(Edge*)));

    this->source = s;
    this->destination = d;

    source->addConnection(this);
    destination->addConnection(this);
    QGline=0;


    linePen.setWidth(((d->height + s->height) /2)/20);
    linePen.setColor(Qt::gray);

    inScene = false;
    updateLine();
}

NodeConnection::~NodeConnection()
{
    source->deleteConnnection(this);
    destination->deleteConnnection(this);
}

void NodeConnection::addToScene(QGraphicsScene *scene)
{
    QGline = scene->addLine(line,linePen);
    inScene = true;
}



void NodeConnection::deleteD(Edge *)
{
    qCritical() << "SHOULD DELETE";
    delete QGline;

    delete this;
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
}
