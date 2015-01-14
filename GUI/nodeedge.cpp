#include "nodeedge.h"
#include "graphmlitem.h"

NodeEdge::NodeEdge(Edge* edge, NodeItem* s, NodeItem* d): GraphMLItem(edge)
{
    label = 0;
    IS_VISIBLE = true;
    IS_SELECTED = false;

    IS_INSTANCE_LINK = edge->isInstanceLink();
    IS_IMPL_LINK = edge->isImplLink();
    IS_AGG_LINK = edge->isAggregateLink();
    IS_DEPLOYMENT_LINK = edge->isDeploymentLink();

    source = s;
    destination = d;

    if(IS_INSTANCE_LINK || IS_IMPL_LINK){
        Node* src = edge->getSource();
        Node* dst = edge->getDestination();

        if(dst->getParentNode()->isDefinition() && (src->getParentNode()->isImpl() || src->getParentNode()->isInstance())){
            //Don't show Non-Top Most Instance Links
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
        arrowHeadLines.append(new QGraphicsLineItem(this));
    }

    //Setup Sizes.
    circleRadius = (s->boundingRect().width() + d->boundingRect().width()) / 30;
    width = circleRadius * 2;
    height =  width;

    source->addNodeEdge(this);
    destination->addNodeEdge(this);

    setupBrushes();
    updateLines();
    updateLabel();
    setLabelFont();

    GraphMLData* descriptionData = edge->getData("description");

    if(descriptionData)
        connect(descriptionData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataUpdated(GraphMLData*)));



    this->setVisible(IS_VISIBLE);
    //Set Flags
    setFlag(ItemDoesntPropagateOpacityToChildren);
    setFlag(ItemIgnoresParentOpacity);
    setFlag(ItemIsSelectable);
}

NodeEdge::~NodeEdge()
{
    if(source){
        source->removeNodeEdge(this);
    }
    if(destination){
        destination->removeNodeEdge(this);
    }

    lineItems.clear();
    delete label;
}

QRectF NodeEdge::boundingRect() const
{
    int brushSize = selectedPen.width();
    return QRectF(-brushSize, -brushSize, width + (brushSize *2), height + (brushSize *2));
}

void NodeEdge::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
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

NodeItem *NodeEdge::getSource()
{
    return source;
}

NodeItem *NodeEdge::getDestination()
{
    return destination;
}


void NodeEdge::setOpacity(qreal opacity)
{
    Q_UNUSED(opacity);
}


void NodeEdge::setSelected(bool selected)
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
        if(selected){
            line->setPen(selectedArrowPen);
        }else{
            line->setPen(arrowPen);
        }
        line->update();
    }

    label->setVisible(selected);

    prepareGeometryChange();
    update();
}

void NodeEdge::setVisible(bool visible)
{
    bool setVisible = IS_VISIBLE && (source->isVisible() && destination->isVisible());// && visible;

    foreach(QGraphicsLineItem* line, lineItems){
        line->setVisible(setVisible);
    }
    foreach(QGraphicsLineItem* line, arrowHeadLines){
        line->setVisible(setVisible);
    }


    label->setVisible(setVisible);
    QGraphicsItem::setVisible(setVisible);
}

void NodeEdge::updateEdge()
{
    if(source && destination){
        updateLines();
    }
}

void NodeEdge::graphMLDataUpdated(GraphMLData *data)
{
    if(data){
        QString dataKey = data->getKey()->getName();
        QString dataValue = data->getValue();

        if(dataKey == "description"){
            updateLabel();
        }
    }
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

void NodeEdge::setLabelFont()
{
    QFont font("Arial");
    font.setPointSize(1);
    QFontMetrics fm(font);

    float factor = (width*0.95) / fm.width(QString(16, 'c'));
    font.setPointSizeF(font.pointSizeF() * factor);
    label->setFont(font);

}

void NodeEdge::updateLabel()
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

void NodeEdge::setupBrushes()
{
    int penWidth = 4;

    QColor selectedColor;
    QColor color;

    if(IS_INSTANCE_LINK){
        color = QColor(0, 0, 180);
    }else if(IS_IMPL_LINK){
        color = QColor(0, 180, 0);
    }else if(IS_AGG_LINK){
        color = QColor(180, 0, 0);
    }else if(IS_DEPLOYMENT_LINK){
        color = QColor(180, 0, 180);
    }else{
        color = QColor(50, 50, 50);
    }

    selectedColor = color;
    color.setAlpha(150);
    selectedColor.setAlpha(250);

    brush = QBrush(color);
    selectedBrush = QBrush(selectedColor);

    pen.setColor(color);
    pen.setWidth(penWidth);
    pen.setStyle(Qt::DashLine);
    selectedPen.setColor(selectedColor);
    selectedPen.setWidth(6 * penWidth);

    arrowPen.setColor(selectedColor);
    arrowPen.setStyle(Qt::SolidLine);
    arrowPen.setWidth(penWidth);

    selectedArrowPen.setColor(selectedColor);
    selectedArrowPen.setWidth(6 * penWidth);
}


void NodeEdge::updateLines()
{
    //The Top Left of the Circle will be the center point.

    float sx = source->scenePos().x() + (source->width / 2);
    float sy = source->scenePos().y() + (source->height / 2);
    float dx = destination->scenePos().x() + (destination->width / 2);
    float dy = destination->scenePos().y() + (destination->height / 2);

    //Work out the Center Point.
    float mx = ((sx + dx) / 2) - circleRadius;
    float my = ((sy + dy) / 2) - circleRadius;

    //Set the Center.
    setPos(mx, my);

    sx -= mx;
    sy -= my;

    dx -= mx;
    dy -= my;

    mx = circleRadius;
    my = circleRadius;

    int arrowHeadSize = width / 4;

    if(dx < sx){
        arrowHeadSize *= - 1;
    }

    lineItems[0]->setLine(sx, sy, mx, sy);
    lineItems[1]->setLine(mx, sy, mx, dy);
    lineItems[2]->setLine(mx, dy, dx, dy);

    arrowHeadLines[0]->setLine(dx, dy, dx-arrowHeadSize, dy-(arrowHeadSize/2));
    arrowHeadLines[1]->setLine(dx-arrowHeadSize, dy-(arrowHeadSize/2), dx-arrowHeadSize, dy+(arrowHeadSize/2));
    arrowHeadLines[2]->setLine(dx-arrowHeadSize, dy+(arrowHeadSize/2), dx, dy);

    prepareGeometryChange();
    update();
}
