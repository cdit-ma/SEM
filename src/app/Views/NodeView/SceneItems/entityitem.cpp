#include "entityitem.h"

#include <QPainter>
#include <QtMath>
#include <QStyleOption>
#include <QTransform>

#include "Node/nodeitem.h"


EntityItem::EntityItem(ViewItem *viewItem, EntityItem* parentItem, KIND kind)
{
    this->defaultZValue = 0;
    this->viewItem = 0;
    this->parentItem = parentItem;
    this->kind = kind;
    paintIconOverlay = false;
    paintTertiaryIcon = false;
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
    setDefaultZValue(1);
}

EntityItem::~EntityItem()
{
    disconnectViewItem();
}

int EntityItem::type() const
{
    return ENTITY_ITEM_KIND;
}

bool EntityItem::isHidden() const
{
    return !isVisible();
}

void EntityItem::setTertiaryIcon(QString path, QString image)
{
    tertiaryIconPath.first = path;
    tertiaryIconPath.second = image;
    update();
}

void EntityItem::setTertiaryIconVisible(bool visible)
{
    paintTertiaryIcon = visible;
    update();
}

bool EntityItem::isTertiaryIconVisible() const
{
    return paintTertiaryIcon;
}

QPair<QString, QString> EntityItem::getTertiaryIcon() const
{
    return tertiaryIconPath;
}

RENDER_STATE EntityItem::getRenderState(qreal lod) const
{
    if(lod >= 1.0){
        return RENDER_STATE::FULL;
    }else if(lod >= .75){
        return RENDER_STATE::REDUCED;
    }else if(lod >= .5){
        return RENDER_STATE::MINIMAL;
    }else{
        return RENDER_STATE::BLOCK;
    }
}

EntityItem *EntityItem::getParent() const
{
    return parentItem;
}

NodeItem *EntityItem::getParentNodeItem() const
{
    if(parentItem && parentItem->isNodeItem()){
        return (NodeItem*) parentItem;
    }
    return 0;
}

void EntityItem::unsetParent()
{
    parentItem = 0;
    setParentItem(0);
}

bool EntityItem::isTopLevelItem() const
{
    return parentItem == 0;
}

bool EntityItem::isReadOnly() const
{
    return viewItem->isReadOnly();
}

ViewItem *EntityItem::getViewItem() const
{
    return viewItem;
}

void EntityItem::setPos(const QPointF &pos)
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

int EntityItem::getID()
{
    return viewItem->getID();
}

QRectF EntityItem::getElementRect(EntityItem::ELEMENT_RECT rect) const
{
    QRectF r;

    switch(rect){
        case ER_SELECTION:
            r = currentRect();
            break;
        case ER_MOVE:
            r = currentRect();
            break;
        default:
            break;
    }
    return r;
}

QPainterPath EntityItem::getElementPath(EntityItem::ELEMENT_RECT rect) const
{
    QPainterPath region;
    region.addRect(getElementRect(rect));
    return region;
}

void EntityItem::paintPixmap(QPainter *painter, qreal lod, EntityItem::ELEMENT_RECT pos, const QString& imagePath, const QString& imageName, QColor tintColor)
{
    QRectF imageRect = getElementRect(pos);

    if(!imageRect.isEmpty()){
        RENDER_STATE state = getRenderState(lod);

        if(state == RENDER_STATE::BLOCK){
            //Only allow the Main Icon/Secondary Icon and Edge Kind Icon to be drawn in block state.
            switch(pos){
            case ER_MAIN_ICON:
            case ER_SECONDARY_ICON:
            case ER_EDGE_KIND_ICON:
                break;
            default:
                return;
            }

            paintPixmapRect(painter, imagePath, imageName, imageRect);
        }else{
            auto required_size = getPixmapSize(imageRect, lod);
            auto pixmap = getPixmap(imagePath, imageName, required_size, tintColor);
            paintPixmap(painter, imageRect, pixmap);
        }
    }
}

void EntityItem::paintPixmap(QPainter *painter, qreal lod, const QRectF& image_rect, const QString& imagePath, const QString& imageName, QColor tintColor){
    if(!image_rect.isEmpty()){
        auto required_size = getPixmapSize(image_rect, lod);
        
        auto pixmap = getPixmap(imagePath, imageName, required_size, tintColor);
        paintPixmap(painter, image_rect, pixmap);
    }
}

void EntityItem::paintPixmap(QPainter *painter, qreal lod, EntityItem::ELEMENT_RECT pos, const QPair<QString, QString>& image, QColor tintColor)
{
    paintPixmap(painter, lod, pos, image.first, image.second, tintColor);
}

StaticTextItem* EntityItem::getTextItem(ELEMENT_RECT pos){
    auto text_item = textMap.value(pos, 0);
    if(!text_item){
        //If we haven't got one, construct a text tiem
        text_item = new StaticTextItem(Qt::AlignLeft | Qt::AlignVCenter);
        textMap[pos] = text_item;
    }
    return text_item;
}

void EntityItem::renderText(QPainter *painter, qreal lod, EntityItem::ELEMENT_RECT pos, QString text, int textOptions)
{
    painter->save();
    
    auto text_item = getTextItem(pos);

    painter->setPen(text_color);
    
    text_item->RenderText(painter, getRenderState(lod), getElementRect(pos), text);
    painter->restore();
}

void EntityItem::setTooltip(EntityItem::ELEMENT_RECT rect, QString tooltip, QCursor cursor)
{
    tooltipMap[rect] = tooltip;
    tooltipCursorMap[rect] = cursor;
}


void EntityItem::paintPixmap(QPainter *painter, QRectF imageRect, QPixmap pixmap) const
{
    if(imageRect.isValid()){
        painter->drawPixmap(imageRect, pixmap, pixmap.rect());
    }
}

void EntityItem::setIconOverlay(QString alias, QString imageName)
{
    if(!paintIconOverlay || iconOverlayIconPath.first != alias || iconOverlayIconPath.second != imageName){
        iconOverlayIconPath.first = alias;
        iconOverlayIconPath.second = imageName;
        update();
    }
}

void EntityItem::setIconOverlayVisible(bool visible)
{
    if(paintIconOverlay != visible){
        paintIconOverlay = visible;
        update();
    }
}

bool EntityItem::isIconOverlayVisible() const
{
    return paintIconOverlay;
}

QSize EntityItem::getPixmapSize(QRectF rect, qreal lod) const
{
    //Calculate the required size of the image.
    QSize requiredSize;
    requiredSize.setWidth(rect.width() * 2 * lod);
    requiredSize.setHeight(rect.height() * 2 * lod);
    requiredSize = Theme::roundQSize(requiredSize);
    
    //Max out at 128 pixels
    if(requiredSize.width() > 128){
        requiredSize.setWidth(128);
        requiredSize.setHeight(128);
    }
    return requiredSize;
}

QPixmap EntityItem::getPixmap(const QString& imageAlias, const QString& imageName, QSize requiredSize, QColor tintColor) const
{
    Theme* theme = Theme::theme();
    tintColor = Theme::theme()->getMenuIconColor();
    QPixmap image = theme->getImage(imageAlias, imageName, requiredSize, tintColor);
    return image;
}


QRectF EntityItem::translatedBoundingRect() const
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
bool EntityItem::intersectsRectInScene(QRectF rectInScene) const
{
    return rectInScene.contains(sceneBoundingRect());
}

bool EntityItem::isDataProtected(QString keyName) const
{
    if(viewItem){
        return viewItem->isDataProtected(keyName);
    }
    return true;
}

void EntityItem::addRequiredData(QString keyName)
{
    if(!requiredDataKeys.contains(keyName)){
        requiredDataKeys.append(keyName);
    }
}

void EntityItem::removeRequiredData(QString keyName)
{
    if(requiredDataKeys.contains(keyName)){
        requiredDataKeys.removeAll(keyName);
    }
}

QStringList EntityItem::getRequiredDataKeys()
{
    return requiredDataKeys;
}

void EntityItem::reloadRequiredData()
{
    foreach(QString keyName, requiredDataKeys){
        if(viewItem->hasData(keyName)){
            QVariant dataValue = viewItem->getData(keyName);
            if(!dataValue.isNull()){
                dataChanged(keyName, dataValue);
            }
        }
    }
}

QRectF EntityItem::viewRect() const
{
    return boundingRect();
}

QRectF EntityItem::sceneViewRect() const
{
    return mapToScene(viewRect()).boundingRect();
}

QSize EntityItem::iconSize() const
{
    return QSize(32,32);
}

QSize EntityItem::smallIconSize() const
{
    return QSize(8,8);
}

QPainterPath EntityItem::shape() const
{
    return getElementPath(ER_SELECTION);
}

QPainterPath EntityItem::sceneShape() const
{
    return mapToScene(shape());
}

void EntityItem::setIgnorePosition(bool ignore)
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

bool EntityItem::isIgnoringPosition()
{
    return ignorePosition;
}

void EntityItem::dataChanged(QString keyName, QVariant data)
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

void EntityItem::propertyChanged(QString, QVariant)
{
}

void EntityItem::dataRemoved(QString)
{

}

void EntityItem::adjustPos(QPointF delta)
{
    setPos(getPos() + delta);
}

QPointF EntityItem::getPos() const
{
    return pos() + getTopLeftOffset();
}

QPointF EntityItem::getTopLeftOffset() const{
    return QPointF();
}

QPointF EntityItem::validateMove(QPointF delta)
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


void EntityItem::setData(QString keyName, QVariant value)
{
    emit req_setData(viewItem, keyName, value);
}

QVariant EntityItem::getData(QString keyName) const
{
    return viewItem->getData(keyName);
}


bool EntityItem::hasData(QString keyName) const
{
    return viewItem->hasData(keyName);
}

qreal EntityItem::getDefaultZValue() const
{
    return defaultZValue;
}

void EntityItem::handleSelection(bool append)
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

void EntityItem::removeData(QString keyName)
{
    if(hasData(keyName)){
        emit req_removeData(getViewItem(), keyName);
    }
}

void EntityItem::setDefaultZValue(qreal z)
{
    defaultZValue = z;
    updateZValue();
}

void EntityItem::handleExpand(bool expand)
{
    if(isExpandEnabled()){
        if(expand != _isExpanded){
            emit req_expanded(this, expand);
        }
    }

}

void EntityItem::handleHover(bool hovered)
{
    if(isHoverEnabled()){
        if(hovered != _isHovered){
            setHovered(hovered);
        }
    }
}

void EntityItem::setSelectionEnabled(bool enabled)
{
    selectEnabled = enabled;
}

void EntityItem::setMoveStarted()
{
    positionPreMove = getPos();
    _isMoving = true;
}

bool EntityItem::setMoveFinished()
{
    _isMoving = false;
    return getPos() != positionPreMove;
}

void EntityItem::setHoverEnabled(bool enabled)
{
    setAcceptHoverEvents(enabled);
    hoverEnabled = enabled;
}

bool EntityItem::isHoverEnabled()
{
    return hoverEnabled;
}

bool EntityItem::isSelected() const
{
    return _isSelected;
}

bool EntityItem::isActiveSelected() const
{
    return _isActiveSelected;
}

bool EntityItem::isHighlighted() const
{
    return _isHighlighted;
}

bool EntityItem::isHovered() const
{
    return _isHovered;
}

void EntityItem::connectViewItem(ViewItem *viewItem)
{
    this->viewItem = viewItem;

    viewItem->registerObject(this);
    connect(viewItem, &ViewItem::dataAdded, this, &EntityItem::dataChanged);
    connect(viewItem, &ViewItem::dataChanged, this, &EntityItem::dataChanged);
    connect(viewItem, &ViewItem::dataRemoved, this, &EntityItem::dataRemoved);
    connect(viewItem, &ViewItem::propertyChanged, this, &EntityItem::propertyChanged);
}

void EntityItem::disconnectViewItem()
{
    if(viewItem){
        disconnect(viewItem, &ViewItem::dataAdded, this, &EntityItem::dataChanged);
        disconnect(viewItem, &ViewItem::dataChanged, this, &EntityItem::dataChanged);
        disconnect(viewItem, &ViewItem::dataRemoved, this, &EntityItem::dataRemoved);
        viewItem->unregisterObject(this);
        viewItem = 0;
    }
}

void EntityItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
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

void EntityItem::setFontSize(int fontSize)
{
    textFont.setPixelSize(fontSize * 2);
    this->fontSize = fontSize;
}


void EntityItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
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

void EntityItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    if(_isMouseMoving){
        _isMouseMoving = false;
        emit req_FinishMove();
    }
}



void EntityItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    bool inItem = getElementPath(ER_SELECTION).contains(event->pos());
    if(isHovered() && !inItem){
        handleHover(false);
    }else if(!isHovered() && inItem){
        handleHover(true);
    }
}

void EntityItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    bool inItem = getElementPath(ER_SELECTION).contains(event->pos());
    if(isHovered() && !inItem){
        handleHover(false);
    }else if(!isHovered() && inItem){
        handleHover(true);
    }
}

void EntityItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *)
{
    if(isHovered()){
        handleHover(false);
    }
}


bool EntityItem::isExpanded() const
{
    return _isExpanded;
}

void EntityItem::setExpanded(bool expand)
{
    if(_isExpanded != expand){
        _isExpanded = expand;
        emit positionChanged();
    }
}


void EntityItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(isExpandEnabled()){
        if(event->button() == Qt::LeftButton && getElementPath(ER_EXPANDCONTRACT).contains(event->pos())){
            handleExpand(!isExpanded());
        }
    }
}

void EntityItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    Q_UNUSED(option)

    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    RENDER_STATE state = getRenderState(lod);

    if(state == RENDER_STATE::BLOCK){
        QBrush brush(Qt::SolidPattern);
        painter->setPen(Qt::NoPen);

        if(isSelected()){
            brush.setColor(getPen().color());
            painter->setBrush(brush);
            painter->drawPath(getElementPath(ER_SELECTION));
        }else{
            brush.setColor(getBodyColor());
        }

        //Paint the pixmap!
    }
      
    paintPixmap(painter, lod, ER_MAIN_ICON, getIconPath());

    if(state > RENDER_STATE::BLOCK){
        if(paintIconOverlay){
            paintPixmap(painter, lod, ER_MAIN_ICON_OVERLAY, iconOverlayIconPath);
        }
        if(paintTertiaryIcon){
            paintPixmap(painter, lod, ER_TERTIARY_ICON, tertiaryIconPath);
        }
        if(isReadOnly()){
            paintPixmap(painter, lod, ER_LOCKED_STATE, "Icons", "lockClosed");
        }
        if(paint_notification){
            painter->save();
            //QBrush brush(Qt::SolidPattern);
            //painter->setPen(Qt::NoPen);
            //brush.setColor(getBodyColor());
            //painter->setBrush(brush);
            //painter->drawRect(getElementRect(ER_NOTIFICATION));
            //paintPixmap(painter, lod, ER_NOTIFICATION, notification_icon, notification_color);
            painter->restore();
        }
    }
}

QPen EntityItem::getDefaultPen() const{
    return defaultPen;
}

QPen EntityItem::getPen()
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
        penColor = Theme::theme()->getHighlightColor();
    }

    if(!isHovered()){
        if(!isSelected() || (isSelected() && !isActiveSelected())){
            penColor = penColor.lighter(140);
        }
    }

    pen.setColor(penColor);
    return pen;
}

void EntityItem::setDefaultPen(QPen pen)
{
    defaultPen = pen;
}

bool EntityItem::isNodeItem()
{
    return kind == EntityItem::NODE;
}

bool EntityItem::isEdgeItem()
{
    return kind == EntityItem::EDGE;
}

bool EntityItem::isMoving() const
{
    return _isMoving;
}

int EntityItem::getGridSize() const
{
    return 10;
}

QPair<QString, QString> EntityItem::getIconPath()
{
    if(viewItem){
        return viewItem->getIcon();
    }
    return QPair<QString, QString>();
}

QPointF EntityItem::getNearestGridPoint(QPointF newPos)
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

void EntityItem::destruct()
{
    disconnectViewItem();
    delete this;
}

void EntityItem::setHovered(bool isHovered)
{
    if(_isHovered != isHovered){
        _isHovered = isHovered;
        if(_isHovered && getParent()){
            getParent()->setHovered(false);
        }
        update();
    }
}

void EntityItem::setHighlighted(bool isHighlight)
{
    if(_isHighlighted != isHighlight){
        _isHighlighted = isHighlight;
        update();
    }
}

void EntityItem::setSelected(bool selected)
{
    if(_isSelected != selected){
        _isSelected = selected;

        updateZValue();
        update();
    }
}

void EntityItem::setActiveSelected(bool active)
{
    if(_isActiveSelected != active){
        _isActiveSelected = active;
        updateZValue();
        update();
    }
}

void EntityItem::updateZValue(bool childSelected, bool childActive)
{
    bool raise = childSelected || isSelected();
    childActive |= isActiveSelected();

    qreal z = fabs(getDefaultZValue());

    if(z == 0 && raise){
        z = 1;
        z *= raise ? 2: -2;
    }

    z *= childActive ? 2 : 1;

    setZValue(z);

    if(getParent()){
        getParent()->updateZValue(raise, childActive);
    }
}

void EntityItem::paintPixmapRect(QPainter* painter, QString imageAlias, QString imageName, QRectF rect)
{
    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(Theme::theme()->getMainImageColor(imageAlias, imageName));
    painter->drawRect(rect);
    painter->restore();
}

QColor EntityItem::getBaseBodyColor() const
{
    return bodyColor;
}

QColor EntityItem::getTextColor() const{
    return text_color;
}

void EntityItem::setTextColor(QColor color){
    text_color = color;
    update();
}

void EntityItem::setHeaderColor(QColor color){
    header_color = color;
    update();
}

QColor EntityItem::getHeaderColor() const{
    return header_color;
}


QColor EntityItem::getHighlightColor() const{
    return highlight_color;
}

void EntityItem::setHighlightColor(QColor color){
    highlight_color = color;
    update();
}

QColor EntityItem::getBodyColor() const
{
    if(isHighlighted()){
        return highlight_color;
    } else {
        return bodyColor;
    }
}

void EntityItem::setBaseBodyColor(QColor color)
{
    if(bodyColor != color){
        bodyColor = color;
        update();
    }
}

bool EntityItem::isSelectionEnabled()
{
    return selectEnabled;
}

bool EntityItem::isExpandEnabled()
{
    return expandEnabled;
}

void EntityItem::setExpandEnabled(bool enabled)
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

void EntityItem::setMoveEnabled(bool enabled)
{
    if(moveEnabled != enabled){
        moveEnabled = enabled;
    }
}

bool EntityItem::isMoveEnabled()
{
    return moveEnabled;
}


void EntityItem::AddNotification(QString image_path, QString image_name, QColor color){
    if(!paint_notification || notification_icon.first != image_path || notification_icon.second != image_name){
        notification_icon.first = image_path;
        notification_icon.second = image_name;
        notification_color = color;
        paint_notification = true;
        update();
    }
}

void EntityItem::ClearNotification(){
    if(paint_notification){
        paint_notification = false;
        update();
    }
}