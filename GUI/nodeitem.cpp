#include "nodeitem.h"
#include <QGraphicsTextItem>
#include "nodeconnection.h"
#include <QDebug>
#include <QFont>
#include <QFontMetrics>
NodeItem::NodeItem(Node *node, NodeItem *parent):QObject(parent)
{

    drawDetail = true;
    drawObject = true;

    this->isPressed = false;
    this->node = node;
    this->setParentItem(parent);

     label  = new QGraphicsTextItem("NULL",this);

    if(parent == 0){
        depth = 1;
        this->width = 1000;
        this->height = 1000;
    }else{
        depth = parent->depth +1;
        this->width = parent->width/4;
        this->height =  parent->height/4;
    }

    bRec = QRect(0,0,this->width, this->height);




    GraphMLData* xData = node->getData("x");
    GraphMLData* yData = node->getData("y");
    GraphMLData* kindData = node->getData("kind");
    GraphMLData* labelData = node->getData("label");



    connect(xData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(updatedData(GraphMLData*)));
    connect(yData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(updatedData(GraphMLData*)));
    connect(labelData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(updatedData(GraphMLData*)));
    connect(kindData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(updatedData(GraphMLData*)));

    connect(this, SIGNAL(updateData(QString,QString)),node,SLOT(updateData(QString,QString)));
    connect(node, SIGNAL(deleteGUI(GraphMLContainer*)), this, SLOT(deleteD(GraphMLContainer*)));





    emit updatedData(xData);
    emit updatedData(yData);
    emit updatedData(kindData);
    emit updatedData(labelData);



    //recieveData();

}

NodeItem::~NodeItem()
{
    disconnect(this, SIGNAL(updateData(QString,QString)),node,SLOT(updateData(QString,QString)));
}

QRectF NodeItem::boundingRect() const
{
    return bRec;
}


void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    label->setVisible(drawDetail);

    if(drawObject){
        QRectF rectangle = boundingRect();

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
        //if(drawDetail){
        painter->fillRect(rectangle, Brush);
        painter->drawRect(rectangle);
    }
}

void NodeItem::notifyEdges()
{
    for(int i =0;i< connections.size();i++){
        connections[i]->updateLine();
    }

    for(int i=0;i<this->childItems().size();i++){
        NodeItem * childNode = dynamic_cast<NodeItem*>(childItems()[i]);
        if(childNode != 0){
            childNode->notifyEdges();
        }
    }
}

void NodeItem::addConnection(NodeConnection *line)
{
    connections.append(line);
}

void NodeItem::deleteConnnection(NodeConnection *line)
{
    int position = connections.indexOf(line);
    connections.remove(position);
}

void NodeItem::toggleDetailDepth(int level)
{

    if(level>=depth){
        drawDetail = true;
    }else{
        if(level + 1 < depth){
            drawObject = false;

        }else{
            drawObject = true;
        }
        drawDetail = false;
    }
    notifyEdges();
    update();
}


void NodeItem::updatedData(GraphMLData* data)
{
    QString dataKey = data->getKey()->getName();
    QString dataValue = data->getValue();

    if(dataKey == "x"){
        updatePosition(dataValue,0);
    }else if(dataKey == "y"){
        updatePosition(0,dataValue);
    }else if(dataKey == "label"){

        QFont font("Arial");
        font.setPointSize(5);
        QFontMetrics fm(font);

        float factor = width / fm.width(dataValue);

        if ((factor < 1) || (factor > 1.25))
        {
            font.setPointSizeF(font.pointSizeF()*factor);
            label->setFont(font);
        }


        label->setPlainText(dataValue);
    }else if(dataKey == "kind"){
        kind = dataValue;
        update();
    }
}

void NodeItem::recieveData()
{
    name = node->getDataValue("label");

    QString x = node->getDataValue("x");
    QString y = node->getDataValue("y");

    this->setPos(QPointF(x.toDouble(),y.toDouble()));

}

void NodeItem::deleteD(GraphMLContainer *)
{
    delete this;
}


void NodeItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(drawObject){
        this->isPressed = true;

        previousPosition = event->scenePos();

        if ( event->button() == Qt::RightButton ) {
            emit exportSelected(node);
        }else if( event->button() == Qt::MiddleButton ) {
            emit makeChildNode(node);
        }
        else{

            emit setSelected(this);
        }
    }
}

void NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(drawObject){
        this->isPressed = false;
    }
}

void NodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(this->isPressed && drawObject){

        QPointF newPosition = pos() + (event->scenePos() - previousPosition);

        this->setPos(newPosition);

        emit updateData("x", QString::number(newPosition.x()));
        emit updateData("y", QString::number(newPosition.y()));
        notifyEdges();

        previousPosition = event->scenePos();
    }
}

void NodeItem::updatePosition(QString x, QString y)
{
    qreal xR = pos().x();
    qreal yR = pos().y();
    if(x !=0){
        xR = x.toDouble();
    }
    if(y != 0){
        yR = y.toDouble();
    }
    this->setPos(xR,yR);
}
