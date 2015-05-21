#include "edgeitem.h"
#include "graphmlitem.h"
#include <QDebug>
#include <math.h>

EdgeItem::EdgeItem(Edge* edge, NodeItem* s, NodeItem* d): GraphMLItem(edge, GraphMLItem::NODE_EDGE)
{
    label = 0;
    IS_VISIBLE = true;
    IS_SELECTED = false;

    IS_INSTANCE_LINK = edge->isInstanceLink();
    IS_IMPL_LINK = edge->isImplLink();
    IS_AGG_LINK = edge->isAggregateLink();
    IS_DEPLOYMENT_LINK = edge->isDeploymentLink();
    IS_COMPONENT_LINK = edge->isComponentLink() || edge->isAssemblyLink();

    source = s;
    destination = d;

    sourceParent = 0;
    destinationParent = 0;

    if(s->parentItem()){
        QGraphicsItem* sParent = s->parentItem();
        sourceParent = dynamic_cast<NodeItem*>(sParent);
    }

    if(d->parentItem()){
        QGraphicsItem* dParent = d->parentItem();
        destinationParent = dynamic_cast<NodeItem*>(dParent);
    }

    if(IS_INSTANCE_LINK || IS_IMPL_LINK){
        Node* src = edge->getSource();
        Node* dst = edge->getDestination();

        if(dst->getParentNode()->isDefinition() && (src->getParentNode()->isImpl() || src->getParentNode()->isInstance())){
            //Don't show Non-Top Most Instance Links
            IS_VISIBLE = false;
        }
        if(dst->getDataValue("kind") == "AggregateInstance"){
            IS_VISIBLE = false;
        }
        if(src->getDataValue("kind") == "MemberInstance"){
            IS_VISIBLE = false;
        }
        if(src->getDataValue("kind") == "AggregateInstance"){
            IS_VISIBLE = false;
        }
    }

    if(IS_AGG_LINK){
        Node* src = edge->getSource();
        if(src->isInstance()){
            //Don't show Aggregate Links to Instances.
            IS_VISIBLE = false;
        }
    }


    //Construct lines.

    for(int i=0; i < 3; i++){
        lineItems.append(new QGraphicsLineItem(this));
        lineItems.append(new QGraphicsLineItem(this));
        arrowHeadLines.append(new QGraphicsLineItem(this));
        arrowTailLines.append(new QGraphicsLineItem(this));
    }


    //Setup Sizes.
    circleRadius = (s->boundingRect().width() + d->boundingRect().width()) / 30;
    width = circleRadius * 2;
    height =  width;

    source->addEdgeItem(this);
    destination->addEdgeItem(this);

    setupBrushes();
    updateLines();
    updateLabel();
    setLabelFont();

    GraphMLData* descriptionData = edge->getData("label");

    if(descriptionData)
        connect(descriptionData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataChanged(GraphMLData*)));




    //Set Flags
    setFlag(ItemDoesntPropagateOpacityToChildren);
    setFlag(ItemIgnoresParentOpacity);
    setFlag(ItemIsSelectable);

    setVisible(IS_VISIBLE);
}

EdgeItem::~EdgeItem()
{
    if(source){
        source->removeEdgeItem(this);
    }
    if(destination){
        destination->removeEdgeItem(this);
    }

    while(!lineItems.isEmpty()){
        QGraphicsLineItem *lineI = lineItems.takeFirst();
        delete lineI;
    }
    while(!arrowHeadLines.isEmpty()){
        QGraphicsLineItem *lineI = arrowHeadLines.takeFirst();
        delete lineI;
    }
    while(!arrowTailLines.isEmpty()){
        QGraphicsLineItem *lineI = arrowTailLines.takeFirst();
        delete lineI;
    }

    delete label;
}

QRectF EdgeItem::boundingRect() const
{
    int brushSize = selectedPen.width();
    return QRectF(-brushSize, -brushSize, width + (brushSize *2), height + (brushSize *2));
}

void EdgeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if(IS_VISIBLE){
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

        QRectF rectangle(QPointF(0,0), QPointF(circleRadius * 2, circleRadius * 2));



        label->setPos(circleRadius - (label->boundingRect().width()/2), circleRadius - (label->boundingRect().height()/2));
        circle_path.addEllipse(rectangle);
        painter->drawPath(circle_path);
    }
}

NodeItem *EdgeItem::getSource()
{
    return source;
}

NodeItem *EdgeItem::getDestination()
{
    return destination;
}


void EdgeItem::setOpacity(qreal opacity)
{
    Q_UNUSED(opacity);
}


void EdgeItem::setSelected(bool selected)
{
    IS_SELECTED = selected;


    foreach(QGraphicsLineItem* line, lineItems){
        if(selected){
            line->setPen(selectedPen);
        }else{
            line->setPen(pen);
        }
        line->update();
    }

    foreach(QGraphicsLineItem* line, arrowHeadLines){
        //if(selected){
        //    line->setPen(selectedArrowPen);
       // }else{
            line->setPen(arrowHeadPen);
       // }
        //line->update();
    }
    foreach(QGraphicsLineItem* line, arrowTailLines){

         line->setPen(arrowTailPen);
         /*
        if(selected){
            line->setPen(selectedArrowPen);
        }else{
            line->setPen(arrowPen);
        }
        line->update();
        */
    }

    label->setVisible(selected);

    prepareGeometryChange();
    update();
}

void EdgeItem::setVisible(bool visible)
{
    NodeItem* start = source;
    NodeItem* finish = destination;
    if(!source->isVisible()){
        if(sourceParent){
            if(sourceParent->isVisible()){
                start = sourceParent;
            }
        }
    }

    if(!destination->isVisible()){
        if(destinationParent){
            if(destinationParent->isVisible()){
                finish = destinationParent;
            }
        }
    }


    bool isVisible = false;
    if(IS_VISIBLE && start->isVisible() && finish->isVisible() && (start != finish)){
        isVisible = true;
    }

    if(isVisible){
        //UPDATE EDGE
        updateEdge();
    }
    forceVisible(isVisible);

}

void EdgeItem::updateEdge()
{
    if(source && destination){
        updateLines();
    }
}

void EdgeItem::graphMLDataChanged(GraphMLData *data)
{
    if(data){
        QString dataKey = data->getKey()->getName();
        QString dataValue = data->getValue();

        if(dataKey == "description"){
            updateLabel();
        }
    }
}

void EdgeItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(!IS_VISIBLE){
        event->setAccepted(false);
        return;
    }
    switch (event->button()) {

    case Qt::MiddleButton:{
        GraphMLItem_SetCentered(this);
        break;
    }
    case Qt::LeftButton:{

        if (!event->modifiers().testFlag(Qt::ControlModifier)){
            //Clear First if Control isn't pressed!
            GraphMLItem_ClearSelection(false);
        }
        GraphMLItem_AppendSelected(this);
        IS_MOVING = true;

        previousScenePosition = event->scenePos();
        break;
    }
    };
}

void EdgeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{

    if(IS_MOVING){

        QPointF delta = (event->scenePos() - previousScenePosition);
        this->moveBy(delta.x(),delta.y());
        previousScenePosition = event->scenePos();
        return;
    }

}

void EdgeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    IS_MOVING = false;
    updateLines();
}

void EdgeItem::setLabelFont()
{
    QFont font("Arial");
    float fontSize = qMax(.25 * 2*circleRadius, 1.0);

    font.setPointSize(fontSize);

    label->setFont(font);

}

void EdgeItem::updateLabel()
{
    QString labelText = getGraphML()->getDataValue("description");
    if(labelText == ""){
        labelText = getGraphML()->getID();
    }

    if(label){
        //label->setDefaultTextColor(QColor(Qt::white));
        label->setPlainText(labelText);
    }else{
        label = new QGraphicsTextItem(labelText, this);
        label->setVisible(false);
    }
}

void EdgeItem::setupBrushes()
{
    int penWidth = 2;

    QColor selectedColor;
    QColor color;

    pen.setStyle(Qt::DashLine);
    if(IS_INSTANCE_LINK){
        color = QColor(0, 0, 180);
    }else if(IS_IMPL_LINK){
        color = QColor(0, 180, 0);
    }else if(IS_AGG_LINK){
        color = QColor(180, 0, 0);
    }else if(IS_DEPLOYMENT_LINK){
        color = QColor(180, 0, 180);
    }else if(IS_COMPONENT_LINK){
        color = QColor(0, 180 , 180);
        //penWidth *= 2;
        pen.setStyle(Qt::SolidLine);
    }else{
        color = QColor(50, 50, 50);
    }

    selectedColor = color;
    //color.setAlpha(150);
    //selectedColor.setAlpha(250);

    brush = QBrush(color);
    selectedBrush = QBrush(selectedColor);

    pen.setColor(color);
    pen.setWidth(penWidth);

    selectedPen.setColor(selectedColor);
    selectedPen.setWidth(penWidth);

    if(source->getNodeKind() == "OutEventPortInstance"){
        arrowTailPen.setColor(Qt::red);
    }

    if(destination->getNodeKind() == "InEventPortInstance"){
        arrowHeadPen.setColor(Qt::green);
    }

    arrowPen.setColor(selectedColor);
    arrowPen.setStyle(Qt::SolidLine);
    arrowPen.setWidth(penWidth);

    selectedArrowPen.setColor(selectedColor);
    selectedArrowPen.setWidth(penWidth);
}

void EdgeItem::forceVisible(bool visible)
{
    foreach(QGraphicsLineItem* line, lineItems){
        line->setVisible(visible);
    }
    foreach(QGraphicsLineItem* line, arrowHeadLines){
        line->setVisible(visible);
    }
    foreach(QGraphicsLineItem* line, arrowTailLines){
        line->setVisible(visible);
    }

    QGraphicsItem::setVisible(visible);
}

void EdgeItem::aspectsChanged(QStringList aspects)
{
    //Do Nothing.
}


void EdgeItem::updateLines()
{
    //The Top Left of the Circle will be the center point.
    NodeItem* start = source;
    NodeItem* finish = destination;

    int startEdgeCount = start->getEdgeItemCount();
    int finishEdgeCount = finish->getEdgeItemCount();
    int startEdgeID = start->getEdgeItemIndex(this)+1;
    int finishEdgeID = finish->getEdgeItemIndex(this)+1;



    qreal SPACE = 0.8;
    qreal startYOffset = ((1-SPACE)/2) * start->boundingRect().height();
    qreal startSpace = SPACE * start->boundingRect().height();

    qreal finishYOffset = ((1-SPACE)/2) * finish->boundingRect().height();
    qreal finishSpace = SPACE * finish->boundingRect().height();

    qreal offsetSPerID = startSpace / (startEdgeCount + 1);
    qreal offsetFPerID = finishSpace / (finishEdgeCount + 1);


    qCritical() <<"startYOffset" <<  startYOffset;
    qCritical()<< "finishYOffset"<<  finishYOffset;

    startYOffset  += offsetSPerID * startEdgeID;
    finishYOffset += offsetFPerID * finishEdgeID;
    startYOffset -= 0;//start->boundingRect().height()/2;
    finishYOffset -= 0;//finish->boundingRect().height()/2;

    qCritical() <<"startYOffset" <<  startYOffset;
    qCritical()<< "finishYOffset"<<  finishYOffset;

    setZValue(qMax(start->zValue(), finish->zValue()) +1);

    //Set start to the top most visible parent.
    while(start && !(start->isVisible() && start->isPainted())){
        start = start->getParentNodeItem();
    }

    //Set finish to the top most visible parent.
    while(finish && !(finish->isVisible() && finish->isPainted())){
        finish = finish->getParentNodeItem();
    }

    //If start or finish aren't visible, don't update or draw!
    if(!(start && finish)){
        forceVisible(false);
        return;
    }

    //If this line is meant to be visible, and both the start/finish is visible, set it as visible.
    if(IS_VISIBLE && start->isVisible() && finish->isVisible()){
        forceVisible(true);
    }else{
        forceVisible(false);
        return;
    }

    NodeItem* startParent = start->getParentNodeItem();
    NodeItem* finishParent = finish->getParentNodeItem();


    //Get Start Size Modifier
    float sDX = start->boundingRect().width() / 2;
    float sDY = startYOffset;
    //float sDX = start->getWidth() / 2;
    //float sDY = start->getHeight() / 2;

    //Get Start Parent Size Modifier
    float sPDX = startParent->boundingRect().width() / 2;

    //Get Finish Size Modifier
    float fDX = finish->boundingRect().width() / 2;
    float fDY = finishYOffset;

    //float fDX = finish->getWidth() / 2;
    //float fDY = finish->getHeight() / 2;

    //Get Finish Parent Size Modifier
    float fPDX = finishParent->boundingRect().width() / 2;

    //Get Start Center
    float sX = start->scenePos().x() + sDX;
    float sY = start->scenePos().y() + sDY;

    //Get Start Parent Center
    float sPX = startParent->scenePos().x() + sPDX;

    //Get Finish Center
    float fX = finish->scenePos().x() + fDX;
    float fY = finish->scenePos().y() + fDY;

    //Get Finish Parent Center
    float fPX = finishParent->scenePos().x() + fPDX;

    //Use this to determine direction of addition of Width.
    int sourceWidthMult = 1;
    int finishWidthMult = 1;

    //If StartLeft == true, line is coming out the left of Start, else; coming out the right.
    bool startLeft = false;
    //If finishLeft == true, line is coming out the left of Finish, else; coming out the right.
    bool finishLeft = false;
    //If usingStart == true, line is using the X coordinate of the Start, else; the X Coordinate of the Finish.
    bool usingStart = false;

    //Calculate if the source is on the left or right of the sourceParentcomponent.
    float sourceX = source->scenePos().x() + (source->getWidth() /2);
    float destinationX = destination->scenePos().x() + (destination->getWidth() /2);

    NodeItem* startParentP = startParent->getParentNodeItem();
    NodeItem* finishParentP = finishParent->getParentNodeItem();
    if(!(startParentP && finishParentP)){
        return;
    }
    if(startParentP == finishParentP || startParentP->isAncestorOf(finishParentP) || finishParentP->isAncestorOf(startParentP)){

        startLeft = false;
        finishLeft = true;
        finishWidthMult = -1;
    }else{

        float sourceParentX = source->getParentNodeItem()->scenePos().x() + (source->getParentNodeItem()->getWidth() /2);;
        float destinationParentX = destination->getParentNodeItem()->scenePos().x() + (destination->getParentNodeItem()->getWidth() /2);;

        if(sourceX < sourceParentX){
            sourceWidthMult = -1;
            startLeft = true;
        }


        if(destinationX < destinationParentX){
            finishWidthMult = -1;
            finishLeft = true;
        }
    }

    bool usingCenter = finishLeft != startLeft;

    sX = sX + (sourceWidthMult * sDX);
    fX = fX + (finishWidthMult * fDX);

    float deltaX = abs(fX - sX);
    float deltaY = abs(fY - sY);

    //Work out the change in distance across the Y Axis.
    float d = 2 * (deltaY / log(deltaY));


    //Work out the end of the start/finish Line segments..
    float sLX = sX + (d * sourceWidthMult);
    float fLX = fX + (d * finishWidthMult);

    //Calculate the center of the Point.



    float mX = ((sLX + fLX) / 2) - 2*circleRadius;
    float mY = ((sY + fY) / 2) - circleRadius;

    //if s and f aren't drawing from the same side as their respective parents.
    if(!usingCenter){
        if(startLeft){
            //Start and Finish both leave to the Left, so minimize;
            if(sLX < fLX){
                usingStart = true;
                mX = sLX;
            }else{
                mX = fLX;
            }
        }else{
            //Start and Finish both leave to the right, so maximize;
            if(sLX > sX){
                if(sLX > fLX){
                    usingStart = true;
                    mX = sLX;
                }else{
                    mX = fLX;
                }
            }
        }
        mX -= circleRadius;
    }

    mX = this->pos().x();
    mY = this->pos().y();

    //Set the Center.
    //setPos(mX, mY);


    //Update based on the Translation of setPos()
    sX -= mX;
    sY -= mY;
    sLX -= mX;

    fX -= mX;
    fY -= mY;
    fLX -= mX;

    //Set mX and mY to the centre of the mid point object.
    mX = circleRadius;
    mY = circleRadius;

    //Calculate the Arrow size, based of the radius of the connection.
    //When the finish point is on the right, we need to reverse the arrowHeadSize.
    int arrowHeadDepth = -finishWidthMult * (circleRadius * 3);
    int arrowHeadHeight = arrowHeadDepth / (1.5);

    int arrowTailDepth = -sourceWidthMult * (circleRadius * 3);
    int arrowTailHeight = arrowTailDepth / (1.5);



    if(lineItems.size() == 6){
        lineItems[0]->setLine(sX, sY, sLX, sY);



        if((usingCenter && (sLX > mX) && startLeft)){// || (!usingCenter && !usingStart)){
            qCritical() << "GROW mY";
            //Grow towards mY first
            lineItems[1]->setLine(sLX, sY, sLX, mY);
            lineItems[2]->setLine(sLX, mY, mX, mY);

        }else{
             qCritical() << "GROW mX";
            //Grow towards mX first

            lineItems[1]->setLine(sLX, sY, mX, sY);
            lineItems[2]->setLine(mX, sY, mX, mY);



        }

        if(usingCenter && (fLX > mX) && finishLeft){ //|| (!usingCenter && !usingStart)){
            qCritical() << "GROW fY";
            //Grow towards fY first
            lineItems[3]->setLine(mX, mY, mX, fY);
            lineItems[4]->setLine(mX, fY, fLX, fY);




        }else{

            qCritical() << "GROW fX";
            //Grow towards fX first
            lineItems[3]->setLine(mX, mY, fLX, mY);
            lineItems[4]->setLine(fLX, mY, fLX, fY);


        }


        lineItems[5]->setLine(fLX, fY, fX, fY);
    }

    //Set the Arrow Head!
    if(arrowHeadLines.size() == 3){
        arrowHeadLines[0]->setLine(fX, fY, fX - arrowHeadDepth, fY - (arrowHeadHeight/2));
        arrowHeadLines[1]->setLine(fX - arrowHeadDepth, fY - (arrowHeadHeight/2), fX - arrowHeadDepth, fY + (arrowHeadHeight/2));
        arrowHeadLines[2]->setLine(fX - arrowHeadDepth, fY + (arrowHeadHeight/2), fX, fY);
    }

    if(arrowTailLines.size() == 3){
        arrowTailLines[0]->setLine(sX, sY - arrowTailHeight/2, sX - arrowTailDepth, sY);
        arrowTailLines[1]->setLine(sX - arrowTailDepth, sY, sX, sY + arrowTailHeight/2);
        arrowTailLines[2]->setLine(sX, sY + arrowTailHeight/2, sX, sY - arrowTailHeight/2);
    }

    prepareGeometryChange();
    update();
}
