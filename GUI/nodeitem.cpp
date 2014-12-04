#include "nodeitem.h"
#include "nodeedge.h"

#include <QGraphicsTextItem>
#include <QDebug>
#include <QFont>
#include <QFontMetrics>
#include <QRubberBand>

NodeItem::NodeItem(Node *node, NodeItem *parent):  GraphMLItem(node)
{
    isSelected = false;
    drawDetail = true;
    drawObject = true;
    USING_RUBBERBAND_SELECTION = false;

    ID = node->getID();

    CONTROL_DOWN = false;
    hasMoved = false;

    rubberBand = new QRubberBand(QRubberBand::Rectangle,0);
    QPalette palette;
    palette.setBrush(QPalette::Foreground, QBrush(Qt::green));
    palette.setBrush(QPalette::Base, QBrush(Qt::red));

    rubberBand->setPalette(palette);
    rubberBand->resize(500, 500);



    this->isPressed = false;
    this->node = node;

    setParentItem(parent);

    label  = new QGraphicsTextItem("NULL",this);

    if(!parent){
        depth = 1;
        this->width = 19200;
        this->height = 10800;
    }else{
        depth = parent->depth +1;
        this->width = parent->width/2;
        this->height =  parent->height/2;
    }


    GraphMLData* xData = node->getData("x");
    GraphMLData* yData = node->getData("y");
    GraphMLData* kindData = node->getData("kind");
    GraphMLData* labelData = node->getData("label");

    GraphMLData* hData = node->getData("height");
    GraphMLData* wData = node->getData("width");


    if(xData)
    connect(xData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataUpdated(GraphMLData*)));

    if(yData)
    connect(yData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataUpdated(GraphMLData*)));

    if(hData)
    connect(hData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataUpdated(GraphMLData*)));

    if(wData)
    connect(wData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataUpdated(GraphMLData*)));

    if(labelData)
    connect(labelData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataUpdated(GraphMLData*)));

    if(kindData)
    connect(kindData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataUpdated(GraphMLData*)));




    if(wData && wData->getValue() == ""){
        wData->setValue(QString::number(width));
    }else{
        if(wData)
            width = wData->getValue().toFloat();
    }

    if(hData && hData->getValue() == ""){
        hData->setValue(QString::number(height));
    }else{
        if(hData)
            height = hData->getValue().toFloat();
    }



    bRec = QRect(0,0,this->width, this->height);



    setFlag(ItemDoesntPropagateOpacityToChildren);
    setFlag(ItemIgnoresParentOpacity);
    setFlag(ItemIsSelectable);

    color = QColor(255,255,255,255);
    selectedColor = QColor(0,0,255,180);


    brush.setColor(color);
    selectedBrush.setColor(selectedColor);




    if(xData)
        graphMLDataUpdated(xData);
    if(yData)
        graphMLDataUpdated(yData);
    if(kindData)
        graphMLDataUpdated(kindData);
    if(labelData)
        graphMLDataUpdated(labelData);

    updateBrushes();
    if(kindData->getValue() == "Model"){
        drawObject = false;
        drawDetail = false;
    }
}

NodeItem::~NodeItem()
{
    delete rubberBand;
    delete label;
}


QRectF NodeItem::boundingRect() const
{
    return bRec;
}


void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);


    label->setVisible(drawDetail);

    if(drawObject){
        QRectF rectangle = boundingRect();
        QPen Pen;
        QBrush Brush;

        if(isSelected){
            Brush = selectedBrush;
            Pen = selectedPen;
        }else{
            Brush = brush;
            Pen = pen;
        }

        if(node->isInstance() || node->isImpl()){
            if(!node->getDefinition()){
                Brush.setStyle(Qt::DiagCrossPattern);
            }
        }

        painter->fillRect(rectangle, Brush);
        painter->setPen(Pen);
        painter->drawRect(rectangle);
    }
}

void NodeItem::notifyEdges()
{
    foreach(NodeEdge* edge, connections){
        edge->updateLine();
    }
    foreach(QGraphicsItem* child, childItems()){
        NodeItem * childNode = dynamic_cast<NodeItem*>(child);
        if(childNode){
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



void NodeItem::setOpacity(qreal opacity)
{
    QGraphicsItem::setOpacity(opacity);

    foreach(NodeEdge* edge, connections){
        if(edge->getSource()->opacity() != 0 && edge->getDestination()->opacity() != 0){
            edge->setOpacity(opacity);
        }else{
            edge->setOpacity(0);
        }
    }

}


void NodeItem::setSelected2()
{
    if(graphicsEffect != 0){
        graphicsEffect->setStrength(1);
    }

    for(int i =0;i< connections.size();i++){
        if(connections[i] != 0){
            connections[i]->setSelected(true);
        }
    }
    this->update(this->boundingRect());
}

void NodeItem::setSelected(bool selected)
{
    isSelected = selected;
    for(int i =0;i< connections.size();i++){
        if(connections[i] != 0){
            connections[i]->setSelected(selected);
        }
    }

}

void NodeItem::setDeselected2()
{
    if(graphicsEffect != 0){
        graphicsEffect->setStrength(0);
    }

    for(int i =0;i< connections.size();i++){
        if(connections[i] != 0){
        //connections[i]->s();
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


void NodeItem::graphMLDataUpdated(GraphMLData* data)
{
    if(data){
        QString dataKey = data->getKey()->getName();
        QString dataValue = data->getValue();

        if(dataKey == "x"){
            updatePosition(dataValue,0);
        }else if(dataKey == "y"){
            updatePosition(0,dataValue);
        }else if(dataKey == "label"){
            Node* node = (Node*) this->getGraphML();

            dataValue = dataValue + " [" + node->getID()+"]";
            QFont font("Arial");
            font.setPointSize(2);
            QFontMetrics fm(font);

            if(dataValue != ""){
                float factor = width / fm.width(dataValue);

                font.setPointSizeF(font.pointSizeF()*factor);
                label->setFont(font);
            }


            label->setPlainText(dataValue);
        }else if(dataKey == "kind"){
            kind = dataValue;
            update();
        }
        else if(dataKey == "width"){
            updateSize(dataValue,0);
            graphMLDataUpdated(node->getData("label"));
        }
        else if(dataKey == "height"){
            updateSize(0,dataValue);
            graphMLDataUpdated(node->getData("label"));
        }

        if(dataKey == "x" || dataKey == "y"){
            notifyEdges();
        }
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
    this->~NodeItem();
}

void NodeItem::updateChildNodeType(QString type)
{
    toBuildType = type;
}

void NodeItem::updateViewAspects(QStringList aspects)
{
    viewAspect = aspects;

    bool isVisible = false;
    foreach(QString aspect, aspects){
        if(node->isInAspect(aspect)){
            isVisible = true;
            break;
        }
    }

    this->setVisible(isVisible);

    foreach(NodeEdge* edge, connections){
        edge->setVisible(isVisible);
    }

    update();
}

void NodeItem::sortChildren()
{
    emit triggerAction("Sorting Children");
    int currentX  = width/10;
    int currentY = height/5;

    foreach(QGraphicsItem* children, this->childItems()){

        NodeItem* nodeItem = dynamic_cast<NodeItem*>(children);
        if(nodeItem != 0){

        if((currentX + nodeItem->width * 1.1) > width){
            currentY += nodeItem->height * 1.1;
            currentX = width/10;
        }


        //nodeItem->node->updateDataValue("x",QString::number(currentX));
        //nodeItem->node->updateDataValue("y",QString::number(currentY));

        emit updateGraphMLData(nodeItem->getGraphML(),"x",QString::number(currentX));
        emit updateGraphMLData(nodeItem->getGraphML(),"y",QString::number(currentY));

        currentX += nodeItem->width * 1.1;
        }
    }
}


void NodeItem::setRubberbandMode(bool On)
{
    USING_RUBBERBAND_SELECTION = On;
}

void NodeItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(!drawObject){
        event->setAccepted(false);
        return;
    }


    switch (event->button()) {

    case Qt::MiddleButton:{
        if(event->modifiers().testFlag(Qt::ControlModifier)){
            sortChildren();
        }else{
            emit triggerCentered(getGraphML());
        }
        //emit centreNode(this);
        break;
    }
    case Qt::LeftButton:{
        if(USING_RUBBERBAND_SELECTION){
            origin = event->screenPos();
            sceneOrigin = mapToScene(event->pos());
            rubberBand->setGeometry(QRect(origin.toPoint(), QSize()));
            rubberBand->show();
        }else{
            //Left and Right buttons should target this line.
            previousPosition = event->scenePos();
            hasMoved = false;
            isPressed = true;
            emit triggerSelected(getGraphML());
            //emit triggerSelected(this);
            //emit setItemSelected(node);
        }
        break;
    }
    case Qt::RightButton:{
        //Select this node, and construct a child node.
        emit triggerSelected(getGraphML());
        break;

    }

    default:{

        break;
    }
    }
}

void NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(!drawObject){
        return;
    }

    switch (event->button()) {
    case Qt::LeftButton:{
        if(USING_RUBBERBAND_SELECTION){
            QPointF scenePos = mapToScene(event->pos());
            QRect selectionRectangle(sceneOrigin.toPoint(), scenePos.toPoint());
            QPainterPath pp;
            pp.addRect(selectionRectangle);

            scene()->setSelectionArea(pp, Qt::ContainsItemBoundingRect);
            rubberBand->hide();
        }else{
            hasMoved = false;
            isPressed = false;
        }
        break;
    }
    default:
        break;

    }
}

void NodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(!drawObject){
        return;
    }

    if(USING_RUBBERBAND_SELECTION){
        if(boundingRect().contains(event->pos())){
            rubberBand->setGeometry(QRect(origin.toPoint(), event->screenPos()).normalized());
        }
    }else if(isPressed){
        if(hasMoved == false){
            emit triggerAction("Moving Selection");
        }

        QPointF delta = (event->scenePos() - previousPosition);
        this->setPos(pos() + delta);

        emit moveSelection(delta);
        hasMoved = true;
        previousPosition = event->scenePos();
    }
}

void NodeItem::updateBrushes()
{
    QString nodeKind = getGraphML()->getDataValue("kind");

    if(nodeKind == "OutEventPort"){
        color = QColor(250,0,0);
    }
    else if(kind == "OutEventPortInstance"){
        color = QColor(200,0,0);
    }
    else if(kind == "OutEventPortImpl"){
        color = QColor(150,0,0);
    }
    else if(nodeKind == "InEventPort"){
        color = QColor(0,250,0);
    }
    else if(kind == "InEventPortInstance"){
        color = QColor(0,200,0);
    }
    else if(kind == "InEventPortImpl"){
        color = QColor(0,150,0);
    }
    else if(nodeKind == "Component"){
        color = QColor(200,200,200);
    }
    else if(kind == "ComponentInstance"){
        color = QColor(150,150,150);
    }
    else if(kind == "ComponentImpl"){
        color = QColor(100,100,100);
    }
    else if(nodeKind == "Attribute"){
        color = QColor(0,0,250);
    }
    else if(kind == "AttributeInstance"){
        color = QColor(0,0,200);
    }
    else if(kind == "AttributeImpl"){
        color = QColor(0,0,150);
    }
    else if(nodeKind == "HardwareNode"){
        color = QColor(0,250,250);
    }
    else if(kind == "HardwareCluster"){
        color = QColor(0,200,200);
    }

    else if(nodeKind == "BehaviourDefinitions"){
        color = QColor(250,250,250);
    }
    else if(kind == "InterfaceDefinitions"){
        color = QColor(250,250,250);
    }
    else if(nodeKind == "DeploymentDefinitions"){
        color = QColor(250,250,250);
    }

    else if(kind == "File"){
        color = QColor(150,150,150);
    }
    else if(nodeKind == "ComponentAssembly"){
        color = QColor(200,200,200);
    }

    else if(kind == "Aggregate"){
        color = QColor(200,200,200);
    }
    else if(nodeKind == "AggregateMember"){
        color = QColor(150,150,150);
    }
    else if(nodeKind == "Member"){
        color = QColor(100,100,100);
    }





    if(nodeKind.endsWith("Definitions")){
        selectedColor = color;
        color.setAlpha(50);
        selectedColor.setAlpha(150);
    }else{
        selectedColor = color;
        color.setAlpha(200);
        selectedColor.setAlpha(250);

    }




    brush = QBrush(color);
    selectedBrush = QBrush(selectedColor);

    pen.setColor(Qt::gray);
    pen.setWidth(4);
    selectedPen.setColor(Qt::blue);
    selectedPen.setWidth(4);

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

void NodeItem::updateSize(QString w, QString h)
{
    if(w != 0){
        width = w.toDouble();
    }
    if(h != 0){
        height = h.toDouble();
    }
     bRec = QRect(-4,-4,width+4, height+4);
     update();

}


