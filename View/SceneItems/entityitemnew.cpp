#include "entityitemnew.h"
#include "../theme.h"
#include <QPainter>
#include <QDebug>
#include "nodeitemnew.h"


EntityItemNew::EntityItemNew(ViewItem *viewItem, EntityItemNew* parentItem, KIND kind)
{
    this->viewItem = 0;
    this->parentItem = parentItem;
    this->kind = kind;
    paintIconOverlay = false;
    connectViewItem(viewItem);

    if(parentItem){
        connect(parentItem, SIGNAL(scenePosChanged()), this, SIGNAL(scenePosChanged()));
    }

    //Sets the default border to be dark gray
    QPen defaultPen(QColor(50, 50, 50));
    defaultPen.setCosmetic(true);

    setDefaultPen(defaultPen);

    _isHovered = false;
    _isSelected = false;
    _isActiveSelected = false;
    _isMoving = false;
    _isMouseMoving = false;
    _hasMouseMoved = false;
    _isHighlighted = false;
    ignorePosition = false;



    _isExpanded = false;
    expandEnabled = false;
    selectEnabled = true;
    moveEnabled = true;

    textFont = QFont("Verdana");
    setFontSize(10);

    addRequiredData("readOnly");

    //Setup the initial Body color
    bodyColor = QColor(233,234,237).lighter(110);


    setHoverEnabled(true);
    setFlag(QGraphicsItem::ItemIsSelectable, false);
}

EntityItemNew::~EntityItemNew()
{
    disconnectViewItem();
}

int EntityItemNew::type() const
{
    return ENTITY_ITEM_KIND;
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

NodeItemNew *EntityItemNew::getParentNodeItem() const
{
    if(parentItem && parentItem->isNodeItem()){
        return (NodeItemNew*) parentItem;
    }
    return 0;
}

void EntityItemNew::unsetParent()
{
    parentItem = 0;
    setParentItem(0);
}

bool EntityItemNew::isTopLevelItem() const
{
    return parentItem == 0;
}

bool EntityItemNew::isReadOnly() const
{
    return viewItem->isReadOnly();
}

ViewItem *EntityItemNew::getViewItem() const
{
    return viewItem;
}

void EntityItemNew::setPos(const QPointF &pos)
{
    if(pos != getPos()){
        QPointF deltaPos = pos - this->getPos();
        deltaPos = validateMove(deltaPos);
        if(!deltaPos.isNull()){
            QPointF newPos = getPos() + deltaPos;
            QGraphicsObject::setPos(newPos - getTopLeftOffset());
            emit positionChanged();
            emit scenePosChanged();
        }
    }
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
        RENDER_STATE state = getRenderState(lod);
        //Calculate the required size of the image.
        QSize requiredSize = getPixmapSize(imageRect, lod);

        if(state == RS_BLOCK){
            switch(pos){
            case ER_MAIN_ICON:
                break;
            case ER_SECONDARY_ICON:
                break;
            case ER_EDGE_KIND_ICON:
                break;
            default:
                return;
            }

            paintPixmapRect(painter, imageAlias, imageName, imageRect);
        }else{
            ImageMap image = imageMap[pos];

            //If the image size is different to what is cached, we should Update, or we have been told to update.
            if(update || image.imageSize != requiredSize || image.imageAlias != imageAlias || image.imageName != imageName || image.pixmap.isNull()){
                image.pixmap = getPixmap(imageAlias, imageName, requiredSize, tintColor);
                image.imageAlias = imageAlias;
                image.imageName = imageName;
                image.imageSize = requiredSize;

                //Store the image into the map.
                imageMap[pos] = image;
            }

            //Paint Pixmap
            paintPixmap(painter, imageRect, image.pixmap);
        }
    }
}

void EntityItemNew::paintPixmap(QPainter *painter, qreal lod, QRectF imageRect, QString imageAlias, QString imageName, QColor tintColor)
{
    RENDER_STATE state = getRenderState(lod);
    //Calculate the required size of the image.
    QSize requiredSize = getPixmapSize(imageRect, lod);

    if(requiredSize.width() <= 8 || state == RS_BLOCK){
        paintPixmapRect(painter, imageAlias, imageName, imageRect);
    }else{
        QPixmap pixmap = getPixmap(imageAlias, imageName, requiredSize, tintColor);
        paintPixmap(painter, imageRect, pixmap);
    }
}

void EntityItemNew::paintPixmap(QPainter *painter, qreal lod, QRectF imageRect, QPair<QString, QString> image, QColor tintColor)
{

    paintPixmap(painter, lod, imageRect, image.first, image.second, tintColor);
}

void EntityItemNew::paintPixmap(QPainter *painter, qreal lod, EntityItemNew::ELEMENT_RECT pos, QPair<QString, QString> image, QColor tintColor)
{
    paintPixmap(painter, lod, pos, image.first, image.second, tintColor);
}

void EntityItemNew::setTooltip(EntityItemNew::ELEMENT_RECT rect, QString tooltip, QCursor cursor)
{
    tooltipMap[rect] = tooltip;
    tooltipCursorMap[rect] = cursor;
}


void EntityItemNew::paintPixmap(QPainter *painter, QRectF imageRect, QPixmap pixmap) const
{
    if(imageRect.isValid()){

        painter->drawPixmap(imageRect,pixmap, pixmap.rect());
    }
}

void EntityItemNew::setIconOverlay(QString alias, QString imageName)
{
    if(!paintIconOverlay || iconOverlayIconPath.first != alias || iconOverlayIconPath.second != imageName){
        iconOverlayIconPath.first = alias;
        iconOverlayIconPath.second = imageName;
        update();
    }
}

bool EntityItemNew::setIconOverlayVisible(bool visible)
{
    if(paintIconOverlay != visible){
        paintIconOverlay = visible;
        update();
    }
}


void EntityItemNew::renderText(QPainter *painter, qreal lod, QRectF textRect, QString text, int fontSize) const
{
    if(!textRect.isValid() || text.isEmpty()){
        return;
    }
    if(fontSize <=0){
        fontSize = this->fontSize;
    }

    painter->setClipRect(boundingRect());

    QFont font = textFont;
    font.setPixelSize(fontSize * 4);

    qreal osfontSize = fontSize * 4;
    qreal maxFontSize = fontSize;
    qreal minFontSize = fontSize / 2.0;


    painter->save();
    painter->setClipRect(boundingRect());
    painter->setPen(Qt::black);
    painter->setFont(font);
    qreal requiredWidth = painter->fontMetrics().width(text);

    qreal actualWidth = textRect.width();
    qreal actualHeight = textRect.height();

    qreal scale = actualWidth / requiredWidth;

    qreal renderedFontHeight = scale * osfontSize;

    if(renderedFontHeight > actualHeight){
       scale /= renderedFontHeight / actualHeight;
    }

    renderedFontHeight = scale * osfontSize;

    if(renderedFontHeight > maxFontSize){
        scale /= renderedFontHeight / maxFontSize;
    }
    if(renderedFontHeight < minFontSize){
        scale /= renderedFontHeight / minFontSize;
    }
    renderedFontHeight = scale * osfontSize * lod;


    painter->scale(scale, scale);
    textRect.setWidth(textRect.width() / scale);
    textRect.setHeight(textRect.height() / scale);
    textRect.moveTopLeft(textRect.topLeft() / scale);

    int options = Qt::AlignVCenter | Qt::AlignLeft | Qt::TextWrapAnywhere;
    if(renderedFontHeight > 5){
        painter->drawText(textRect, options, text);
    }else{
        QRectF rect = painter->fontMetrics().boundingRect(textRect.toRect(), options, text);
        painter->setBrush(QColor(120,120,120,120));
        painter->setPen(Qt::NoPen);
        rect.moveCenter(textRect.center());
        rect.moveLeft(textRect.left());
        painter->drawRect(rect);
    }
    painter->restore();
}

QSize EntityItemNew::getPixmapSize(QRectF rect, qreal lod) const
{
    //Calculate the required size of the image.
    QSize requiredSize;
    requiredSize.setWidth(rect.width() * 2 * lod);
    requiredSize.setHeight(rect.height() * 2 * lod);
    requiredSize = Theme::roundQSize(requiredSize);
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
    //we should use the bounding rect coordinates!
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

bool EntityItemNew::isDataProtected(QString keyName) const
{
    if(viewItem){
        return viewItem->isDataProtected(keyName);
    }
    return true;
}

void EntityItemNew::addRequiredData(QString keyName)
{
    if(!requiredDataKeys.contains(keyName)){
        requiredDataKeys.append(keyName);
    }
}

void EntityItemNew::removeRequiredData(QString keyName)
{
    if(requiredDataKeys.contains(keyName)){
        requiredDataKeys.removeAll(keyName);
    }
}

QStringList EntityItemNew::getRequiredDataKeys()
{
    return requiredDataKeys;
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

QRectF EntityItemNew::viewRect() const
{
    return boundingRect();
}

QRectF EntityItemNew::sceneViewRect() const
{
    return mapToScene(viewRect()).boundingRect();
}

QSize EntityItemNew::iconSize() const
{
    return QSize(32,32);
}

QSize EntityItemNew::smallIconSize() const
{
    return QSize(8,8);
}

QPainterPath EntityItemNew::shape() const
{
    return getElementPath(ER_SELECTION);
}

QPainterPath EntityItemNew::sceneShape() const
{
    return mapToScene(shape());
}

void EntityItemNew::setIgnorePosition(bool ignore)
{
    ignorePosition = ignore;
    if(ignore){
        removeRequiredData("x");
        removeRequiredData("y");
    }else{
        addRequiredData("x");
        addRequiredData("y");
    }
}

bool EntityItemNew::isIgnoringPosition()
{
    return ignorePosition;
}

void EntityItemNew::dataChanged(QString keyName, QVariant data)
{
    if(getRequiredDataKeys().contains(keyName)){
        if(keyName == "x" || keyName == "y"){
            qreal realData = data.toReal();
            QPointF oldPos = getPos();
            QPointF newPos = oldPos;

            if(keyName == "x"){
                newPos.setX(realData);
            }else if(keyName == "y"){
                newPos.setY(realData);
            }

            if(newPos != oldPos){
                setPos(newPos);
                setPos(getNearestGridPoint());
            }
        }
    }
}

void EntityItemNew::propertyChanged(QString propertyName, QVariant data)
{

}

void EntityItemNew::dataRemoved(QString keyName)
{

}

void EntityItemNew::adjustPos(QPointF delta)
{
    setPos(getPos() + delta);
}

QPointF EntityItemNew::getPos() const
{
    return pos() + getTopLeftOffset();
}

QPointF EntityItemNew::getTopLeftOffset() const{
    return QPointF();
}

QPointF EntityItemNew::validateMove(QPointF delta)
{
    if(getParentNodeItem()){
        QPointF adjustedPos = getPos() + delta;
        QPointF minPos = getParentNodeItem()->gridRect().topLeft();

        //Minimize on the minimum position this item can go.
        if(adjustedPos.x() < minPos.x()){
            adjustedPos.rx() = minPos.x();
        }
        if(adjustedPos.y() < minPos.y()){
            adjustedPos.ry() = minPos.y();
        }

        //Offset by the pos() to get the restricted delta.
        delta = adjustedPos - getPos();
    }
    return delta;
}


void EntityItemNew::setData(QString keyName, QVariant value)
{
    emit req_setData(viewItem, keyName, value);
}

QVariant EntityItemNew::getData(QString keyName) const
{
    return viewItem->getData(keyName);
}

QVariant EntityItemNew::getProperty(QString propertyName) const
{
    return viewItem->getProperty(propertyName);
}

bool EntityItemNew::hasData(QString keyName) const
{
    return viewItem->hasData(keyName);
}

bool EntityItemNew::isDataEditable(QString keyName)
{
    //TODO Get view to handle this!
    return false;
}

void EntityItemNew::handleSelection(bool append)
{
    if(isSelectionEnabled()){
        bool setActive = false;

        if(isSelected() && !append){
            setActive = true;
        }

        if(setActive){
            //If it isnt actively selected, we shouldn't unselect
            if(!isActiveSelected()){
                emit req_activeSelected(getViewItem());
            }
        }else{
            emit req_selected(getViewItem(), append);
        }
    }
}

void EntityItemNew::removeData(QString keyName)
{
    if(hasData(keyName)){
        emit req_removeData(getViewItem(), keyName);
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

void EntityItemNew::setMoveStarted()
{
    positionPreMove = getPos();
    _isMoving = true;
}

bool EntityItemNew::setMoveFinished()
{
    _isMoving = false;
    return getPos() != positionPreMove;
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
    return _isHighlighted;
}

bool EntityItemNew::isHovered() const
{
    return _isHovered;
}

void EntityItemNew::connectViewItem(ViewItem *viewItem)
{
    this->viewItem = viewItem;

    viewItem->registerObject(this);
    connect(viewItem, &ViewItem::dataAdded, this, &EntityItemNew::dataChanged);
    connect(viewItem, &ViewItem::dataChanged, this, &EntityItemNew::dataChanged);
    connect(viewItem, &ViewItem::dataRemoved, this, &EntityItemNew::dataRemoved);
    connect(viewItem, &ViewItem::propertyChanged, this, &EntityItemNew::propertyChanged);
}

void EntityItemNew::disconnectViewItem()
{
    if(viewItem){
        disconnect(viewItem, &ViewItem::dataAdded, this, &EntityItemNew::dataChanged);
        disconnect(viewItem, &ViewItem::dataChanged, this, &EntityItemNew::dataChanged);
        disconnect(viewItem, &ViewItem::dataRemoved, this, &EntityItemNew::dataRemoved);
        viewItem->unregisterObject(this);
        viewItem = 0;
    }
}

void EntityItemNew::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    bool controlDown = event->modifiers().testFlag(Qt::ControlModifier);


    if(event->button() == Qt::LeftButton && getElementPath(ER_SELECTION).contains(event->pos())){
        handleSelection(controlDown);
    }

    if(event->button() == Qt::MiddleButton){
        emit req_centerItem(this);
    }

    if(isMoveEnabled() && event->button() == Qt::LeftButton && getElementPath(ER_MOVE).contains(event->pos())){
        //Check for movement.
        _isMouseMoving = true;
        _hasMouseMoved = false;
        previousMovePoint = event->scenePos();
    }
}

void EntityItemNew::setFontSize(int fontSize)
{
    textFont.setPixelSize(fontSize * 2);
    this->fontSize = fontSize;
}


void EntityItemNew::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(_isMouseMoving){
        if(!_hasMouseMoved){
            emit req_StartMove();
            _hasMouseMoved = true;
        }
        QPointF deltaPos = event->scenePos() - previousMovePoint;
        previousMovePoint = event->scenePos();
        emit req_Move(deltaPos);
    }
}

void EntityItemNew::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(_isMouseMoving){
        _isMouseMoving = false;
        emit req_FinishMove();
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
        emit positionChanged();
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

void EntityItemNew::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    RENDER_STATE state = getRenderState(lod);

    if(state == RS_BLOCK){
        painter->save();
        painter->setClipRect(boundingRect());
        QBrush brush(Qt::SolidPattern);


        if(isSelected()){
            brush.setColor(getPen().color());
        }else{
            brush.setColor(getBodyColor());
            //brush.setColor(getBaseBodyColor());
        }
        painter->setBrush(brush);
        painter->setPen(Qt::NoPen);
        painter->drawPath(getElementPath(ER_SELECTION));
        painter->restore();
    }


    painter->save();
    painter->setClipPath(getElementPath(ER_SELECTION));
    //Paint the pixmap!
    QPair<QString, QString> icon = getIconPath();
    paintPixmap(painter, lod, ER_MAIN_ICON, icon.first, icon.second);
    painter->restore();

    if(state > RS_BLOCK && paintIconOverlay){
        paintPixmap(painter, lod, ER_MAIN_ICON_OVERLAY, iconOverlayIconPath.first, iconOverlayIconPath.second);
    }
}

QPen EntityItemNew::getPen()
{
    QPen pen = defaultPen;
    pen.setJoinStyle(Qt::MiterJoin);
    QColor penColor = defaultPen.color();

    if(isSelected()){
        if(pen.style() == Qt::NoPen){
            pen.setStyle(Qt::SolidLine);
        }
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

    if(!isActiveSelected()){
        penColor = penColor.lighter();
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

int EntityItemNew::getGridSize() const
{
    return 10;
}

int EntityItemNew::getMajorGridCount() const
{
    return 5;
}

QPair<QString, QString> EntityItemNew::getIconPath()
{
    if(viewItem){
        return viewItem->getIcon();
    }
    return QPair<QString, QString>();
}

QPointF EntityItemNew::getNearestGridPoint(QPointF newPos)
{
    if(newPos.isNull()){
        newPos = getPos();
    }
    qreal gridSize = getGridSize();
    QPointF point = mapToScene(mapFromParent(newPos));
    qreal closestX = qRound(point.x() / gridSize) * gridSize;
    qreal closestY = qRound(point.y() / gridSize) * gridSize;
    QPointF delta = QPointF(closestX, closestY) - point;
    return newPos + delta;
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
    if(_isHighlighted != isHighlight){
        _isHighlighted = isHighlight;
        update();
    }
}

void EntityItemNew::setSelected(bool selected)
{
    if(_isSelected != selected){
        _isSelected = selected;
        qreal z = zValue();
        z += (selected ? 1: -1);
        z = qMax(0.0, z);
        setZValue(z);
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

void EntityItemNew::paintPixmapRect(QPainter* painter, QString imageAlias, QString imageName, QRectF rect)
{
    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(Theme::theme()->getMainImageColor(imageAlias, imageName));
    painter->drawRect(rect);
    painter->restore();
}

QColor EntityItemNew::getBaseBodyColor() const
{
    return bodyColor;
}

QColor EntityItemNew::getBodyColor() const
{
    if(isHighlighted()){
        return Theme::theme()->getHighlightColor();
    } else {
        return bodyColor;
    }
}

void EntityItemNew::setBaseBodyColor(QColor color)
{
    if(bodyColor != color){
        bodyColor = color;
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
        if(enabled){
            addRequiredData("isExpanded");
        }else{
            removeRequiredData("isExpanded");
            setExpanded(false);
        }
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
