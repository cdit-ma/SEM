#include "graphicslayoutitem.h"

using namespace MEDEA;

/**
 * @brief GraphicsLayoutItem::GraphicsLayoutItem
 */
GraphicsLayoutItem::GraphicsLayoutItem()
    : QGraphicsLayoutItem() {}


/**
 * @brief GraphicsLayoutItem::getDefaultWidth
 * @return
 */
int GraphicsLayoutItem::getDefaultWidth() const
{
    return DEFAULT_GRAPHICS_ITEM_WIDTH;
}


/**
 * @brief GraphicsLayoutItem::getDefaultHeight
 * @return
 */
int GraphicsLayoutItem::getDefaultHeight() const
{
    return DEFAULT_GRAPHICS_ITEM_HEIGHT;
}


/**
 * @brief GraphicsLayoutItem::getPadding
 * @return
 */
int GraphicsLayoutItem::getPadding() const
{
    return DEFAULT_GRAPHICS_ITEM_PADDING;
}
