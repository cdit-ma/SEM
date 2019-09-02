#include "pathitem.h"

using namespace MEDEA;

/**
 * @brief PathItem::PathItem
 * @param path
 * @param parent
 */
PathItem::PathItem(const QPainterPath &path, QGraphicsItem* parent)
    : QGraphicsPathItem(path, parent)
{

}


/**
 * @brief PathItem::boundingRect
 * @return
 */
QRectF PathItem::boundingRect() const
{
    return QRectF();
}


/**
 * @brief PathItem::paint
 * @param painter
 * @param option
 * @param widget
 */
void PathItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

}
