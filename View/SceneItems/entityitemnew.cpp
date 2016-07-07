#include "entityitemnew.h"
#include "../theme.h"
#include <QPainter>
#include <QDebug>

EntityItemNew::EntityItemNew(ViewItem *viewItem, EntityItemNew* parentItem, KIND kind)
{
    this->viewItem = 0;
    this->parentItem = parentItem;
    this->kind = kind;
    connectViewItem(viewItem);

    //Sets the default border to be dark gray
    setDefaultPen(QPen(QColor(50, 50, 50)));

    _isHovered = false;
    _isSelected = false;
    _isActiveSelected = false;
    _isMoving = false;
    selectEnabled = true;
    moveEnabled = true;


    setHoverEnabled(true);
    setFlag(QGraphicsItem::ItemIsSelectable, false);
}

EntityItemNew::~EntityItemNew()
{
    qCritical() << "DISCONNECTING";
    disconnectViewItem();
}

EntityItemNew::RENDER_STATE EntityItemNew::getRenderState(qreal lod) const
{
    if(lod >= 1.0){
        return RS_FULL;
    }else if(lod >= (2.0/3.0)){
        return RS_REDUCED;
    }else if(lod >= (1.0/3.0)){
        return RS_MINIMAL;
    }else{
        return RS_BLOCK;
    }
}

VIEW_STATE EntityItemNew::getViewState() const
{
    //TODO
    return VIEW_STATE::VS_NONE;
}

EntityItemNew *EntityItemNew::getParent() const
{
    return parentItem;
}

int EntityItemNew::getID()
{
    return viewItem->getID();
}

void EntityItemNew::paintPixmap(QPainter *painter, qreal lod, EntityItemNew::ELEMENT_RECT pos, QString alias, QString imageName, bool update, QColor tintColor)
{
    QRectF place = getElementRect(pos);

    if(!place.isEmpty()){
        Theme* theme = Theme::theme();

        //Get the current Image from the Map
        QPixmap image = imageMap[pos];

        //Calculate the required size of the image.
        QSize requiredSize;
        requiredSize.setWidth(place.width() * lod * 2);
        requiredSize.setHeight(place.height() * lod * 2);
        requiredSize = theme->roundQSize(requiredSize);

        //If the image size is different to what is cached, we should Update, or we have been told to update.
        if(image.size() != requiredSize || update){
            //Try get the image the user asked for.
            image = theme->getImage(alias, imageName, requiredSize, tintColor);

            if(image.isNull()){
                //Use a help icon.
                image = theme->getImage("Actions", "Help", requiredSize, tintColor);
            }
            //Store the image into the map.
            imageMap[pos] = image;
        }

        //Paint Pixmap
        painter->drawPixmap(place.x(), place.y(), place.width(), place.height(), image);
    }
}

QRectF EntityItemNew::translatedBoundingRect() const
{
    QRectF rect = boundingRect();
    rect.translate(pos());
    return rect;
}

/**
 * @brief EntityItemNew::intersectsRectInScene - Checks to see if a Rectangle in Scene Coordinated contains this EntityItems's SceneRect
 * @param rectInScene - A rectangle in scene coordinate space
 * @return
 */
bool EntityItemNew::intersectsRectInScene(QRectF rectInScene) const
{
    return rectInScene.contains(sceneBoundingRect());
}

void EntityItemNew::addRequiredData(QString keyName)
{
    if(!requiredDataKeys.contains(keyName)){
        requiredDataKeys.append(keyName);
    }
}

void EntityItemNew::reloadRequiredData()
{
    foreach(QString keyName, requiredDataKeys){
        QVariant dataValue = viewItem->getData(keyName);
        if(!dataValue.isNull()){
            dataChanged(keyName, dataValue);
        }
    }
}

QRectF EntityItemNew::sceneBoundingRect() const
{
    return QGraphicsObject::sceneBoundingRect();
}

QRectF EntityItemNew::moveRect() const
{
    return currentRect();
}

void EntityItemNew::adjustPos(QPointF delta)
{
    setPos(pos() + delta);
}


void EntityItemNew::setData(QString keyName, QVariant value)
{
    emit req_setData(viewItem, keyName, value);
}

QVariant EntityItemNew::getData(QString keyName)
{
    return viewItem->getData(keyName);
}

bool EntityItemNew::hasData(QString keyName)
{
    return viewItem->hasData(keyName);
}

bool EntityItemNew::isDataEditable(QString keyName)
{
    //TODO Get view to handle this!
    return false;
}

void EntityItemNew::handleSelection(bool setSelected, bool controlPressed)
{
    if(isSelectionEnabled()){
        if(isSelected() && controlPressed){
            //We should deselect on Control + Click
            setSelected = false;
        }

        if(isSelected() != setSelected){
            if(setSelected && !controlPressed){
                //Clear before selection.
                emit req_clearSelection();
            }
            //Select/deselect this item
            emit req_setSelected(viewItem, setSelected);
        }
    }
}

void EntityItemNew::handleHover(bool hovered)
{
    if(isHoverEnabled()){
        if(hovered != _isHovered){
            emit req_hovered(this, _isHovered);
        }
    }
}

void EntityItemNew::setSelectionEnabled(bool enabled)
{
    selectEnabled = enabled;
}

void EntityItemNew::setHoverEnabled(bool enabled)
{
    setAcceptHoverEvents(enabled);
    hoverEnabled = enabled;
}

bool EntityItemNew::isHoverEnabled()
{
    return hoverEnabled;
}

bool EntityItemNew::isSelected()
{
    return _isSelected;
}

bool EntityItemNew::isActiveSelected()
{
    return _isActiveSelected;
}

bool EntityItemNew::isHighlighted()
{
    return _isHightlighted;
}

bool EntityItemNew::isHovered()
{
    return _isHovered;
}

void EntityItemNew::connectViewItem(ViewItem *viewItem)
{
    this->viewItem = viewItem;

    viewItem->addListener(this);
    connect(viewItem, SIGNAL(dataChanged(QString,QVariant)), this, SLOT(dataChanged(QString,QVariant)));
    connect(viewItem, SIGNAL(destructing()), this, SLOT(deleteLater()));
}

void EntityItemNew::disconnectViewItem()
{
    if(viewItem){
        viewItem->removeListener(this);
        disconnect(viewItem, SIGNAL(dataChanged(QString,QVariant)), this, SLOT(dataChanged(QString,QVariant)));
        disconnect(viewItem, SIGNAL(destructing()), this, SLOT(destruct()));
        viewItem = 0;
    }
}

void EntityItemNew::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qCritical() << "ENTITY ITEM MOUSE PRESS";
    bool controlDown = event->modifiers().testFlag(Qt::ControlModifier);
    if(currentRect().contains(event->pos())){
        handleSelection(true, controlDown);
    }


    if(isMoveEnabled()){
        if(event->button() == Qt::LeftButton && moveRect().contains(event->pos())){
            //Check for movement.
            _isMoving = true;
            previousMovePoint = event->scenePos();
        }
    }
}

void EntityItemNew::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{

    if(_isMoving){
        qCritical() << "ENTITY ITEM MOUSE MOVE";
        QPointF deltaPos = event->scenePos() - previousMovePoint;
        previousMovePoint = event->scenePos();
        emit req_adjustPos(deltaPos);
    }
}

void EntityItemNew::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(_isMoving){
        _isMoving = false;
        emit req_adjustPosFinished();
    }
}

/*

void EntityItemNew::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    handleHover(true);
}

void EntityItemNew::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    handleHover(false);
}

void EntityItemNew::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    handleHover(true);
}*/

QPen EntityItemNew::getPen(qreal lod)
{
    QPen pen = defaultPen;
    QColor penColor = defaultPen.color();

    qreal selectedPenWidth = qMax(SELECTED_LINE_WIDTH / lod, 1.0);

    if(isSelected()){
        pen.setStyle(Qt::SolidLine);
        penColor = Theme::theme()->getSelectedItemBorderColor();

        pen.setWidthF(selectedPenWidth);
    }

    if(isHovered()){
        pen.setWidthF(selectedPenWidth);
        penColor = penColor.lighter(130);
    }

    pen.setColor(penColor);

    return pen;
}

void EntityItemNew::setDefaultPen(QPen pen)
{
    defaultPen = pen;
}

bool EntityItemNew::isNodeItem()
{
    return kind == EntityItemNew::NODE;
}

bool EntityItemNew::isEdgeItem()
{
    return kind == EntityItemNew::EDGE;
}

bool EntityItemNew::isMoving()
{
    return _isMoving;
}

QPair<QString, QString> EntityItemNew::getIconPath()
{
    if(viewItem){
        return viewItem->getIcon();
    }
    return QPair<QString, QString>();
}

void EntityItemNew::destruct()
{
    disconnectViewItem();
    delete this;
}

void EntityItemNew::setHovered(bool isHovered)
{
    if(_isHovered != isHovered){
        _isHovered = isHovered;
        update();
    }
}

void EntityItemNew::setHighlighted(bool isHighlight)
{
    if(_isHightlighted != isHighlight){
        _isHightlighted = isHighlight;
        update();
    }
}

void EntityItemNew::setSelected(bool selected)
{
    if(_isSelected != selected){
        _isSelected = selected;
        update();
    }
}

void EntityItemNew::setActiveSelected(bool active)
{
    if(_isActiveSelected != active){
        _isActiveSelected = active;
        update();
    }
}

bool EntityItemNew::isSelectionEnabled()
{
    return selectEnabled;
}

void EntityItemNew::setMoveEnabled(bool enabled)
{
    if(moveEnabled != enabled){
        moveEnabled = enabled;
    }
}

bool EntityItemNew::isMoveEnabled()
{
    return moveEnabled;
}
