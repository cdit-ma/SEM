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

    hasMoved = false;
    graphicsEffect = new QGraphicsColorizeEffect(this);



    QColor blue(70,130,180);
    graphicsEffect->setColor(blue);
    graphicsEffect->setStrength(0);



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

    emit updatedData(xData);
    emit updatedData(yData);
    emit updatedData(kindData);
    emit updatedData(labelData);


    this->setGraphicsEffect(graphicsEffect);

   //setFlag(ItemDoesntPropagateOpacityToChildren);
    setFlag(ItemIgnoresParentOpacity);
    setFlag(ItemIsSelectable);

    attributeModel = new AttributeTableModel(this);

    if(parent == 0){
        //PARENT MODEL!
        treeModelItem = new NodeItemTreeItem(this, 0);
    }else{
        treeModelItem = new NodeItemTreeItem(this, parent->getTreeModelItem());
    }

}

NodeItem::~NodeItem()
{
    delete treeModelItem;
}

void NodeItem::setTreeModelItem(NodeItemTreeItem *newItem)
{
    this->treeModelItem = newItem;
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
        }else if(kind == "HardwareNode"){
            Brush.setColor(Qt::yellow);
        }else if(kind == "HardwareCluster"){
            Brush.setColor(QColor(255,0,255));
        }
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

void NodeItem::addConnection(NodeEdge *line)
{
    connections.append(line);
}

void NodeItem::deleteConnnection(NodeEdge *line)
{
    int position = connections.indexOf(line);
    connections.remove(position);
}

AttributeTableModel *NodeItem::getTable()
{
    return attributeModel;
}

QVariant NodeItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemSelectedChange)
       {
           if (value == true)
           {
               qCritical() << "Set Selected";
               emit triggerSelected(this);
               //emit setSelected();

               // do stuff if selected
           }
           else
           {
               emit setDeselected();
               // do stuff if not selected
           }
       }

    return QGraphicsItem::itemChange(change, value);
}

NodeItemTreeItem *NodeItem::getTreeModelItem()
{
    return this->treeModelItem;
}



void NodeItem::setOpacity(qreal opacity)
{
   QGraphicsItem::setOpacity(opacity);
}


void NodeItem::setSelected()
{
    if(graphicsEffect != 0){
        graphicsEffect->setStrength(1);
    }

    for(int i =0;i< connections.size();i++){
        if(connections[i] != 0){
            connections[i]->setSelected();
        }
    }
}

void NodeItem::setDeselected()
{
    if(graphicsEffect != 0){
        graphicsEffect->setStrength(0);
    }

    for(int i =0;i< connections.size();i++){
        if(connections[i] != 0){
        connections[i]->setDeselected();
        }
    }
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

        dataValue = dataValue + " [" + node->getID()+"]";
        QFont font("Arial");
        font.setPointSize(1);
        QFontMetrics fm(font);

        if(dataValue !=""){
            float factor = width / fm.width(dataValue);

            font.setPointSizeF(font.pointSizeF()*factor);
            label->setFont(font);
        }


        label->setPlainText(dataValue);
    }else if(dataKey == "kind"){
        kind = dataValue;
        update();
    }

    if(dataKey == "x" || dataKey == "y"){
        notifyEdges();
    }
}

void NodeItem::recieveData()
{
    name = node->getDataValue("label");

    QString x = node->getDataValue("x");
    QString y = node->getDataValue("y");

    this->setPos(QPointF(x.toDouble(),y.toDouble()));
    notifyEdges();

}

void NodeItem::destructNodeItem()
{
    delete this;

}

void NodeItem::updateChildNodeType(QString type)
{
    toBuildType = type;
}

void NodeItem::sortChildren()
{
    int currentX  = width/10;
    int currentY = height/5;

    foreach(QGraphicsItem* children, this->childItems()){

        NodeItem* nodeItem = dynamic_cast<NodeItem*>(children);
        if(nodeItem != 0){

        if((currentX + nodeItem->width * 1.1) > width){
            currentY += nodeItem->height * 1.1;
            currentX = width/10;
        }

        nodeItem->setPos(currentX, currentY);
        currentX += nodeItem->width * 1.1;
        }
    }
}



void NodeItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(drawObject){
        if( event->button() == Qt::MiddleButton ) {
            sortChildren();
            emit centreNode(this);
        }else{
            if ( event->button() == Qt::LeftButton ) {
                hasMoved = false;
                emit triggerSelected(this);
            }else if(event->button() == Qt::RightButton){
                QPointF position = this->mapToItem(this,event->pos());
                emit makeChildNode(position, node);
            }
            this->isPressed = true;
            previousPosition = event->scenePos();
        }
    }
}

void NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(drawObject){
        if ( event->button() != Qt::MiddleButton ) {

        }
    }

    if ( event->button() == Qt::LeftButton ){

        if(hasMoved){
            QPointF newPosition = pos() + (event->scenePos() - previousPosition);
            this->setPos(newPosition);


            emit actionTriggered("Updated Node Position");
            emit updateGraphMLData(node,"x",QString::number(newPosition.x()));
            emit updateGraphMLData(node,"y",QString::number(newPosition.y()));


            hasMoved = false;
        }
        notifyEdges();

        foreach(NodeEdge* edge, connections ){
            edge->setVisible(true);
        }
         this->isPressed = false;
    }

}

void NodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(this->isPressed && drawObject){
        //Rended the Node Item in the new location, but don't update coordinates
        QPointF newPosition = pos() + (event->scenePos() - previousPosition);
        this->setPos(newPosition);
        previousPosition = event->scenePos();
        hasMoved = true;

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
