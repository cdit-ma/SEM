#include "entityitem.h"

#include <QPainter>
#include <QtMath>
#include <QStyleOption>
#include <QTransform>

#include "Node/nodeitem.h"



EntityItem::EntityItem(ViewItem *view_item, EntityItem* parent_item, KIND kind)
{
    this->parent_item = parent_item;
    this->kind = kind;

    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setAcceptHoverEvents(true);
    setExpanded(true);
    setDefaultZValue(1);

    connectViewItem(view_item);


    //setup the lock state
    setIconVisible(EntityRect::LOCKED_STATE_ICON, {"Icons", "lockClosed"}, false);


    addRequiredData("readOnly");
    addHoverFunction(EntityRect::SHAPE, std::bind(&NodeItem::shapeHover, this, std::placeholders::_1, std::placeholders::_2));
}

QList<EntityItem::EntityRect> EntityItem::GetEntityRects(){
    return {
        EntityRect::SHAPE,
        EntityRect::MOVE,
        EntityRect::EXPAND_CONTRACT,
        EntityRect::HEADER,
        EntityRect::BODY,

        EntityRect::PRIMARY_TEXT,
        EntityRect::SECONDARY_TEXT,
        EntityRect::TERTIARY_TEXT,
        
        EntityRect::MAIN_ICON,
        EntityRect::MAIN_ICON_OVERLAY,
        EntityRect::SECONDARY_ICON,
        EntityRect::TERTIARY_ICON,
        
        EntityRect::LOCKED_STATE_ICON,
        EntityRect::NOTIFICATION_RECT,
        EntityRect::RESIZE_ARROW_ICON,

        EntityRect::CONNECT_SOURCE,
        EntityRect::CONNECT_TARGET
    };
}

EntityItem::~EntityItem()
{
    disconnect(this);
    disconnectViewItem();
}

bool EntityItem::isHidden() const
{
    return !isVisible();
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
    return parent_item;
}

NodeItem *EntityItem::getParentNodeItem() const
{
    if(parent_item && parent_item->isNodeItem()){
        return (NodeItem*) parent_item;
    }
    return 0;
}

void EntityItem::unsetParent()
{
    parent_item = 0;
    setParentItem(0);
}


bool EntityItem::isReadOnly() const
{
    return view_item->isReadOnly();
}

ViewItem *EntityItem::getViewItem() const
{
    return view_item;
}

void EntityItem::setPos(const QPointF &pos)
{
    if(pos != getPos()){
        QPointF deltaPos = pos - getPos();
        deltaPos = validateMove(deltaPos);
        if(!deltaPos.isNull()){
            QPointF newPos = getPos() + deltaPos;
            QGraphicsObject::setPos(newPos - getTopLeftOffset());
            emit positionChanged();
        }
    }
}

int EntityItem::getID()
{
    return view_item->getID();
}

QRectF EntityItem::getElementRect(EntityRect rect) const
{
    QRectF r;

    switch(rect){
        case EntityRect::SHAPE:

            r = currentRect();
            break;
        case EntityRect::MOVE:
            r = currentRect();
            break;
        default:
            break;
    }
    return r;
}

QPainterPath EntityItem::getElementPath(EntityRect rect) const
{
    QPainterPath region;
    region.addRect(getElementRect(rect));
    return region;
}

void EntityItem::paintPixmap(QPainter *painter, qreal lod, EntityRect pos, const QString& imagePath, const QString& imageName, QColor tintColor)
{
    QRectF imageRect = getElementRect(pos);

    if(!imageRect.isEmpty()){
        RENDER_STATE state = getRenderState(lod);

        if(state == RENDER_STATE::BLOCK){
            //Only allow the Main Icon/Secondary Icon and Edge Kind Icon to be drawn in block state.
            switch(pos){
            case EntityRect::MAIN_ICON:
            case EntityRect::SECONDARY_ICON:
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

void EntityItem::paintPixmap(QPainter *painter, qreal lod, const QRectF& image_rect, const QPair<QString, QString>& image, QColor tintColor){
    paintPixmap(painter, lod, image_rect, image.first, image.second, tintColor);
}

void EntityItem::paintPixmap(QPainter *painter, qreal lod, EntityRect pos, const QPair<QString, QString>& image, QColor tintColor)
{
    paintPixmap(painter, lod, pos, image.first, image.second, tintColor);
}

StaticTextItem* EntityItem::getTextItem(EntityRect pos){
    auto text_item = textMap.value(pos, 0);
    if(!text_item){
        //If we haven't got one, construct a text tiem
        text_item = new StaticTextItem(Qt::AlignLeft | Qt::AlignVCenter);
        textMap[pos] = text_item;
    }
    return text_item;
}

void EntityItem::renderText(QPainter *painter, qreal lod, EntityRect pos, QString text, int textOptions)
{
    auto text_item = getTextItem(pos);
    if(text_item){
        //painter->fillRect(getElementRect(pos), QColor(0,0,255,20));
        text_item->RenderText(painter, getRenderState(lod), getElementRect(pos) + QMarginsF(-1,0,0,0), text);
    }
}

void EntityItem::addHoverFunction(EntityRect rect, std::function<void (bool, QPointF)> func){
    hover_function_map.insert(rect, func);
}

void EntityItem::removeHoverFunction(EntityRect rect){
    hover_function_map.remove(rect);
}

void EntityItem::paintPixmap(QPainter *painter, QRectF imageRect, QPixmap pixmap) const
{
    if(imageRect.isValid()){
        painter->drawPixmap(imageRect, pixmap, pixmap.rect());
    }
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
    if(!tintColor.isValid()){
        tintColor = getTextColor();
    }
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


bool EntityItem::isDataProtected(QString key_name) const
{
    if(view_item){
        return view_item->isDataProtected(key_name);
    }
    return true;
}
bool EntityItem::isDataRequired(const QString& key_name) const{
    return required_data_keys.contains(key_name);
}
void EntityItem::addRequiredData(const QString& key_name)
{
    required_data_keys.insert(key_name);
}

void EntityItem::removeRequiredData(const QString& key_name)
{
    required_data_keys.remove(key_name);
}


void EntityItem::reloadRequiredData()
{
    for(const auto& key_name : required_data_keys){
        if(view_item->hasData(key_name)){
            auto data_value = view_item->getData(key_name);
            if(!data_value.isNull()){
                dataChanged(key_name, data_value);
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


QSize EntityItem::smallIconSize() const
{
    return QSize(8,8);
}

QPainterPath EntityItem::shape() const
{
    return getElementPath(EntityRect::SHAPE);
}


void EntityItem::setIgnorePosition(bool ignore)
{
    is_ignoring_positon = ignore;
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
    return is_ignoring_positon;
}

void EntityItem::dataChanged(const QString& key_name, const QVariant& data)
{
    if(isDataRequired(key_name)){
        if(key_name == "x" || key_name == "y"){
            qreal realData = data.toReal();
            QPointF oldPos = getPos();
            QPointF newPos = oldPos;

            if(key_name == "x"){
                newPos.setX(realData);
            }else if(key_name == "y"){
                newPos.setY(realData);
            }

            if(newPos != oldPos){
                setPos(newPos);
                setPos(getNearestGridPoint());
            }
        }else if(key_name == "readOnly"){
            bool isReadOnly = data.toBool();
            setIconVisible(EntityRect::LOCKED_STATE_ICON, isReadOnly);
        }
    }
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


QVariant EntityItem::getData(const QString& key_name) const
{
    return view_item->getData(key_name);
}


bool EntityItem::hasData(const QString& key_name) const
{
    return view_item->hasData(key_name);
}

qreal EntityItem::getDefaultZValue() const
{
    return default_z_value;
}

void EntityItem::handleSelection(bool append)
{
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

void EntityItem::removeData(QString keyName)
{
    if(hasData(keyName)){
        emit req_removeData(getViewItem(), keyName);
    }
}

void EntityItem::setDefaultZValue(qreal z)
{
    default_z_value = z;
    updateZValue();
}

void EntityItem::handleExpand(bool expand)
{
    if(isExpandEnabled()){
        if(expand != is_expanded){
            emit req_expanded(this, expand);
        }
    }
}

void EntityItem::shapeHover(bool handle, const QPointF&){
    setHovered(handle);
}


void EntityItem::setMoveStarted()
{
    positionPreMove = getPos();
    is_moving = true;
}

bool EntityItem::setMoveFinished()
{
    is_moving = false;
    return getPos() != positionPreMove;
}


bool EntityItem::isSelected() const
{
    return is_selected;
}

bool EntityItem::isActiveSelected() const
{
    return is_active_selected;
}

bool EntityItem::isHighlighted() const
{
    return is_highlighted;
}

bool EntityItem::isHovered() const
{
    return is_hovered;
}

void EntityItem::updateIcon(){
    auto view_item = getViewItem();
    if(view_item){
        setIconVisible(EntityRect::MAIN_ICON, view_item->getIcon(), true);
    }else{
        setIconVisible(EntityRect::MAIN_ICON, false);
    }
}

void EntityItem::connectViewItem(ViewItem *view_item)
{
    this->view_item = view_item;

    view_item->registerObject(this);
    connect(view_item, &ViewItem::dataAdded, this, &EntityItem::dataChanged);
    connect(view_item, &ViewItem::dataChanged, this, &EntityItem::dataChanged);
    connect(view_item, &ViewItem::dataRemoved, this, &EntityItem::dataRemoved);
    connect(view_item, &ViewItem::iconChanged, this, &EntityItem::updateIcon);
    updateIcon();
}

void EntityItem::disconnectViewItem()
{
    if(view_item){
        disconnect(view_item, &ViewItem::dataAdded, this, &EntityItem::dataChanged);
        disconnect(view_item, &ViewItem::dataChanged, this, &EntityItem::dataChanged);
        disconnect(view_item, &ViewItem::dataRemoved, this, &EntityItem::dataRemoved);
        disconnect(view_item, &ViewItem::iconChanged, this, &EntityItem::updateIcon);
        view_item->unregisterObject(this);
        view_item = 0;
        updateIcon();
    }
}

void EntityItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    bool controlDown = event->modifiers().testFlag(Qt::ControlModifier);


    if(event->button() == Qt::LeftButton && getElementPath(EntityRect::SHAPE).contains(event->pos())){
        handleSelection(controlDown);
    }

    if(event->button() == Qt::MiddleButton){
        emit req_centerItem(this);
    }

    if(isMoveEnabled() && event->button() == Qt::LeftButton && getElementPath(EntityRect::MOVE).contains(event->pos())){
        //Check for movement.
        is_mouse_moving = true;
        has_mouse_moving = false;
        previousMovePoint = event->scenePos();
    }
}

void EntityItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(is_mouse_moving){
        if(!has_mouse_moving){
            emit req_StartMove();
            has_mouse_moving = true;
        }
        QPointF deltaPos = event->scenePos() - previousMovePoint;
        previousMovePoint = event->scenePos();
        emit req_Move(deltaPos);
    }
}

void EntityItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    if(is_mouse_moving){
        is_mouse_moving = false;
        emit req_FinishMove();
    }
}


void EntityItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event){
    hoverMoveEvent(event);
}

void EntityItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    

    const QPointF& pos = event->pos();
    QList<EntityRect> hovered_rects;
    QList<EntityRect> unhovered_rects;

    for(auto rect : GetEntityRects()){
        auto in_path = getElementPath(rect).contains(pos);
        auto was_hovered = isHovered(rect);

        if(in_path){
            setAreaHovered(rect, true);
            hovered_rects += rect;
        }else{
            if(was_hovered){
                setAreaHovered(rect, false);
                unhovered_rects += rect;
            }
        }
    }
    
    

    //Handle Unhovering first
    for(auto rect : unhovered_rects){
        if(hover_function_map.contains(rect)){
            hover_function_map[rect](false, pos);
        }
    }

    //Clear the tooltips
    tooltip_stack.clear();
    cursor_stack.clear();

    for(auto rect : hovered_rects){
        if(hover_function_map.contains(rect)){
            hover_function_map[rect](true, pos);
        }
    }

    if(tooltip_stack.size()){
        setToolTip(tooltip_stack.top());
    }else{
        setToolTip("");
    }

    if(cursor_stack.size()){
        setCursor(cursor_stack.top());
    }else{
        unsetCursor();
    }

    QGraphicsObject::hoverMoveEvent(event);
}

bool EntityItem::isHovered(EntityRect area) const{
    return hovered_areas.contains(area);
}

void EntityItem::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
    for(auto rect : hover_function_map.uniqueKeys()){
        auto was_hovered = isHovered(rect);
        if(was_hovered){
            if(hover_function_map.value(rect)){
                hover_function_map.value(rect)(false, event->pos());
            }
            setAreaHovered(rect, false);
        }
    }
    QGraphicsObject::hoverLeaveEvent(event);
}


bool EntityItem::isExpanded() const
{
    return is_expanded;
}

void EntityItem::setExpanded(bool expand)
{
    if(is_expanded != expand){
        is_expanded = expand;
        emit positionChanged();
    }
}


void EntityItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(isExpandEnabled()){
        if(event->button() == Qt::LeftButton && getElementPath(EntityRect::EXPAND_CONTRACT).contains(event->pos())){
            handleExpand(!isExpanded());
        }
    }
}

void EntityItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    Q_UNUSED(option)

    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    RENDER_STATE state = getRenderState(lod);

    if(state > RENDER_STATE::BLOCK){
        painter->save();
        //Paint the Outline path.
        painter->setPen(getPen());
        painter->setBrush(Qt::NoBrush);

        painter->drawRect(getElementRect(EntityRect::SHAPE) );
        painter->restore();
    }

    if(state == RENDER_STATE::BLOCK){
        QBrush brush(Qt::SolidPattern);
        painter->setPen(Qt::NoPen);

        if(isSelected()){
            brush.setColor(getPen().color());
            painter->setBrush(brush);
            painter->drawPath(getElementPath(EntityRect::SHAPE));
        }else{
            brush.setColor(getBodyColor());
        }
    }

    auto ICON = EntityRect::MAIN_ICON;
    if(isIconVisible(ICON)){
        paintPixmap(painter, lod, ICON, getIcon(ICON));
    }

    if(state > RENDER_STATE::BLOCK){
        ICON = EntityRect::MAIN_ICON_OVERLAY;
        //Paint extra icons
        if(isIconVisible(ICON)){
            auto rect = getElementRect(ICON);
            auto inner_rect = rect.adjusted(.75,.75,-.75,-.75);
            painter->setPen(Qt::NoPen);
            painter->setBrush(getBodyColor());
            painter->drawEllipse(rect);

            paintPixmap(painter, lod, inner_rect, getIcon(ICON));
        }

        ICON = EntityRect::SECONDARY_ICON;
        if(isIconVisible(ICON)){
            //painter->fillRect(getElementRect(ICON), QColor(255,0,0,20));
            paintPixmap(painter, lod, ICON, getIcon(ICON));
        }

        ICON = EntityRect::TERTIARY_ICON;
        if(isIconVisible(ICON)){
            //painter->fillRect(getElementRect(ICON), QColor(255,0,0,20));
            paintPixmap(painter, lod, ICON, getIcon(ICON));
        }

        ICON = EntityRect::LOCKED_STATE_ICON;
        if(isIconVisible(ICON)){
            //painter->fillRect(getElementRect(ICON), QColor(255,0,0,20));
            paintPixmap(painter, lod, ICON, getIcon(ICON));
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
    return is_moving;
}

int EntityItem::getGridSize() const
{
    return 5;
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
    if(is_hovered != isHovered){
        is_hovered = isHovered;
        if(is_hovered && getParent()){
            getParent()->setHovered(false);
        }
        update();
    }
}

void EntityItem::setHighlighted(bool isHighlight)
{
    if(is_highlighted != isHighlight){
        is_highlighted = isHighlight;
        update();
    }
}

void EntityItem::setSelected(bool selected)
{
    if(is_selected != selected){
        is_selected = selected;

        updateZValue();
        update();
    }
}

void EntityItem::setActiveSelected(bool active)
{
    if(is_active_selected != active){
        is_active_selected = active;
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


QColor EntityItem::getAltBodyColor() const{
    return alt_body_color;
}
void EntityItem::setAltBodyColor(QColor color){
    alt_body_color = color;
    update();
}

QColor EntityItem::getAltTextColor() const{
    return alt_text_color;
}

void EntityItem::setAltTextColor(QColor color){
    alt_text_color = color;
    update();
}

QColor EntityItem::getBaseBodyColor() const
{
    return bodyColor;
}

QColor EntityItem::getHighlightTextColor() const{
    return highlight_text_color;
}

void EntityItem::setHighlightTextColor(QColor color){
    highlight_text_color = color;
    update();
}

QColor EntityItem::getTextColor() const{
    if(isHighlighted()){
        return getHighlightTextColor();
    }
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
    if(isHighlighted()){
        return highlight_color;
    }else{
        return header_color;
    }
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

bool EntityItem::isExpandEnabled()
{
    return is_expand_enabled;
}

void EntityItem::setExpandEnabled(bool enabled)
{
    if(is_expand_enabled != enabled){
        if(enabled){
            addRequiredData("isExpanded");
        }else{
            removeRequiredData("isExpanded");
            setExpanded(false);
        }
        is_expand_enabled = enabled;
    }
    
}

void EntityItem::setMoveEnabled(bool enabled)
{
    if(is_move_enabled != enabled){
        is_move_enabled = enabled;
    }
}

bool EntityItem::isMoveEnabled()
{
    return is_move_enabled;
}

bool EntityItem::gotIcon(const EntityRect rect) const{
    return icon_map.contains(rect);
}

const QPair<QString, QString>& EntityItem::getIcon(const EntityRect rect){
    return icon_map[rect];
}

void EntityItem::setIcon(const EntityRect rect, const QPair<QString, QString>& icon){
    icon_map.insert(rect, icon);
    update();
}

void EntityItem::setIconVisible(const EntityRect rect, const QPair<QString, QString>& icon, bool visible){
    setIcon(rect, icon);
    setIconVisible(rect, visible);
}

void EntityItem::setIconVisible(const EntityRect rect, bool visible){
    if(visible && !visible_icons.contains(rect)){
        visible_icons.insert(rect);
    }else if(!visible && visible_icons.contains(rect)){
        visible_icons.remove(rect);
    }else{
        return;
    }
    update();
}

bool EntityItem::isIconVisible(const EntityRect rect) const{
    return gotIcon(rect) && visible_icons.contains(rect);
}

void EntityItem::setAreaHovered(EntityRect rect, bool is_hovered){
    if(is_hovered){
        if(!hovered_areas.contains(rect)){
            hovered_areas.insert(rect);
            update();
        }
    }else{
        if(hovered_areas.contains(rect)){
            hovered_areas.remove(rect);
            update();
        }
    }
}

void EntityItem::AddTooltip(const QString& tooltip){
    tooltip_stack.push(tooltip);
}

void EntityItem::AddCursor(const QCursor& cursor){
    cursor_stack.push(cursor);
}