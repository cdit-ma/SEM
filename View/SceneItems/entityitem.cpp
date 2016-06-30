#include "entityitem.h"


EntityItemNew::EntityItemNew(ViewItem *viewItem, EntityItem *parentItem, KIND kind)
{
    viewItem = 0;
    this->kind = kind;
    connectViewItem(viewItem);

    _isHovered = false;
    _isSelected = false;
    _isActiveSelected = false;

    setHoverEnabled(true);
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

EntityItemNew *EntityItemNew::getParent()
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

QRectF EntityItemNew::boundingRect() const
{
    return QGraphicsObject::boundingRect();
}

void EntityItemNew::setData(QString keyName, QVariant value)
{
    emit req_setData(this, keyName, value);
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
    return _isHighlighted;
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
        disconnect(viewItem, SIGNAL(destructing()), this, SLOT(deleteLater()));
        viewItem = 0;
    }
}

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
}

QPen EntityItemNew::getPen(qreal lod)
{
    //TODO
    return QPen();
}

bool EntityItemNew::isNodeItem()
{
    return kind == EntityItemNew::NODE;
}

bool EntityItemNew::isEdgeItem()
{
    return kind == EntityItemNew::EDGE;
}

void EntityItemNew::setHovered(bool isHovered)
{
    _isHovered = isHovered;
}

void EntityItemNew::setHighlighted(bool isHighlight)
{
    _isHightlighted = isHighlight;
}

void EntityItemNew::setSelected(bool selected)
{
    _isSelected = selected;
}

void EntityItemNew::setActiveSelected(bool active)
{
    _isActiveSelected = active;
}

bool EntityItemNew::isSelectionEnabled()
{
    return selectEnabled;
}
