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

    _isExpanded = true;
    expandEnabled = false;
    selectEnabled = true;
    moveEnabled = true;


    setHoverEnabled(true);
    setFlag(QGraphicsItem::ItemIsSelectable, false);
}

EntityItemNew::~EntityItemNew()
{
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

ViewItem *EntityItemNew::getViewItem() const
{
    return viewItem;
}

int EntityItemNew::getID()
{
    return viewItem->getID();
}

QRectF EntityItemNew::getElementRect(EntityItemNew::ELEMENT_RECT rect) const
{
    QRectF r;

    switch(rect){
        case ER_SELECTION:
            r = currentRect();
            break;
        default:
            break;
    }
    return r;
}

QPainterPath EntityItemNew::getElementPath(EntityItemNew::ELEMENT_RECT rect) const
{
    QPainterPath region;
    region.addRect(getElementRect(rect));
    return region;
}

void EntityItemNew::paintPixmap(QPainter *painter, qreal lod, EntityItemNew::ELEMENT_RECT pos, QString imageAlias, QString imageName, QColor tintColor, bool update)
{
    QRectF imageRect = getElementRect(pos);

    if(!imageRect.isEmpty()){
        //Get the current Image from the Map
        QPixmap image = imageMap[pos];

        //Calculate the required size of the image.
        QSize requiredSize = getPixmapSize(imageRect, lod);

        //If the image size is different to what is cached, we should Update, or we have been told to update.
        if(image.size() != requiredSize || update){
            image = getPixmap(imageAlias, imageName, requiredSize, tintColor);
            //Store the image into the map.
            imageMap[pos] = image;
        }

        //Paint Pixmap
        paintPixmap(painter, imageRect, image);
    }
}

void EntityItemNew::paintPixmap(QPainter *painter, qreal lod, QRectF imageRect, QString imageAlias, QString imageName, QColor tintColor)
{
    //Calculate the required size of the image.
    QSize requiredSize = getPixmapSize(imageRect, lod);
    QPixmap pixmap = getPixmap(imageAlias, imageName, requiredSize, tintColor);
    paintPixmap(painter, imageRect, pixmap);
}

void EntityItemNew::setTooltip(EntityItemNew::ELEMENT_RECT rect, QString tooltip, QCursor cursor)
{
    tooltipMap[rect] = tooltip;
    tooltipCursorMap[rect] = cursor;
}


void EntityItemNew::paintPixmap(QPainter *painter, QRectF imageRect, QPixmap pixmap) const
{
    if(imageRect.isValid()){
        painter->drawPixmap(imageRect.x(), imageRect.y(), imageRect.width(), imageRect.height(), pixmap);
    }
}

QSize EntityItemNew::getPixmapSize(QRectF rect, qreal lod) const
{
    //Calculate the required size of the image.
    QSize requiredSize;
    requiredSize.setWidth(rect.width() * lod * 2);
    requiredSize.setHeight(rect.height() * lod * 2);
    requiredSize = Theme::theme()->roundQSize(requiredSize);
    return requiredSize;
}

QPixmap EntityItemNew::getPixmap(QString imageAlias, QString imageName, QSize requiredSize, QColor tintColor) const
{
    Theme* theme = Theme::theme();

    QPixmap image = theme->getImage(imageAlias, imageName, requiredSize, tintColor);

    if(image.isNull()){
        //Return a default ? Icon
        image = theme->getImage("Actions", "Help", requiredSize, tintColor);
    }
    return image;
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

QPainterPath EntityItemNew::shape() const
{
    return getElementPath(ER_SELECTION);
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
            //Select/deselect this item
            emit req_setSelected(this, setSelected, controlPressed);
        }
    }
}

void EntityItemNew::handleExpand(bool expand)
{
    if(isExpandEnabled()){
        if(expand != _isExpanded){
            emit req_expanded(this, expand);
        }
    }

}

void EntityItemNew::handleHover(bool hovered)
{
    if(isHoverEnabled()){
        if(hovered != _isHovered){
            setHovered(hovered);
            //emit req_hovered(this, hovered);
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

bool EntityItemNew::isSelected() const
{
    return _isSelected;
}

bool EntityItemNew::isActiveSelected() const
{
    return _isActiveSelected;
}

bool EntityItemNew::isHighlighted() const
{
    return _isHightlighted;
}

bool EntityItemNew::isHovered() const
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
        disconnect(viewItem, SIGNAL(dataChanged(QString,QVariant)), this, SLOT(dataChanged(QString,QVariant)));
        disconnect(viewItem, SIGNAL(destructing()), this, SLOT(destruct()));
        viewItem->removeListener(this);
        viewItem = 0;
    }
}

void EntityItemNew::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    bool controlDown = event->modifiers().testFlag(Qt::ControlModifier);

    if(getElementPath(ER_SELECTION).contains(event->pos())){
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



void EntityItemNew::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    bool inItem = getElementPath(ER_SELECTION).contains(event->pos());
    if(isHovered() && !inItem){
        handleHover(false);
    }else if(!isHovered() && inItem){
        handleHover(true);
    }
}

void EntityItemNew::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    bool inItem = getElementPath(ER_SELECTION).contains(event->pos());
    if(isHovered() && !inItem){
        handleHover(false);
    }else if(!isHovered() && inItem){
        handleHover(true);
    }
}

void EntityItemNew::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    if(isHovered()){
        handleHover(false);
    }
}


bool EntityItemNew::isExpanded() const
{
    return _isExpanded;
}

void EntityItemNew::setExpanded(bool expand)
{
    if(_isExpanded != expand){
        _isExpanded = expand;
    }
}


void EntityItemNew::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(isExpandEnabled()){
        if(event->button() == Qt::LeftButton && getElementPath(ER_EXPANDCONTRACT).contains(event->pos())){
            handleExpand(!isExpanded());
        }
    }
}

QPen EntityItemNew::getPen()
{
    QPen pen = defaultPen;
    pen.setJoinStyle(Qt::MiterJoin);
    QColor penColor = defaultPen.color();

    if(isSelected()){
        pen.setStyle(Qt::SolidLine);
        pen.setCosmetic(true);
        pen.setWidthF(SELECTED_LINE_WIDTH);
        penColor = Theme::theme()->getSelectedItemBorderColor();
    }

    if(isHovered()){
        if(!isSelected()){
            //penColor = Qt::darkGray;
            penColor = QColor(115,115,115);
        }
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

bool EntityItemNew::isMoving() const
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

bool EntityItemNew::isExpandEnabled()
{
    return expandEnabled;
}

void EntityItemNew::setExpandEnabled(bool enabled)
{
    if(expandEnabled != enabled){
        expandEnabled = enabled;
    }
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
