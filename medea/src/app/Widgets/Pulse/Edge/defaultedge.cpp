//
// Created by Cathlyn Aston on 13/11/20.
//

#include "defaultedge.h"

#include <stdexcept>
#include <QPainter>
#include <QDebug>

using namespace Pulse::View;

DefaultEdge::DefaultEdge(NaturalAnchor* src_anchor, NaturalAnchor* dst_anchor, QGraphicsItem* parent)
    : QGraphicsObject(parent)
{
    if (src_anchor == nullptr) {
        throw std::invalid_argument("Edge::Edge - The source anchor is null");
    } else {
        natural_src_anchor_ = src_anchor;
        source_pos_ = src_anchor->pos();
        connect(src_anchor, &NaturalAnchor::edgeAnchorMoved, this, &DefaultEdge::onSourceMoved);
        connect(src_anchor, &NaturalAnchor::edgeAnchorVisibilityChanged, this, &DefaultEdge::onSourceVisibilityChanged);
    }
    if (dst_anchor == nullptr) {
        throw std::invalid_argument("Edge::Edge - The destination anchor is null");
    } else {
        natural_dst_anchor_ = dst_anchor;
        destination_pos_ = dst_anchor->pos();
        connect(dst_anchor, &NaturalAnchor::edgeAnchorMoved, this, &DefaultEdge::onDestinationMoved);
        connect(dst_anchor, &NaturalAnchor::edgeAnchorVisibilityChanged, this, &DefaultEdge::onDestinationVisibilityChanged);
    }
}

re::types::Uuid DefaultEdge::getID() const
{
    return id_;
}

QRectF DefaultEdge::boundingRect() const
{
    return QRectF();
}

void DefaultEdge::onSourceMoved(const QPointF& pos)
{
    // update the src point
    qDebug() << "DefaultEdge::onSourceMoved";
    source_pos_ = pos;
    update();
}

void DefaultEdge::onDestinationMoved(const QPointF& pos)
{
    // update the dst point
    qDebug() << "DefaultEdge::onDestinationMoved";
    destination_pos_ = pos;
    update();
}

void DefaultEdge::onSourceVisibilityChanged(bool visible)
{
    // check if at least one of the src and dst is visible; if no, hide edge
    qDebug() << "DefaultEdge::onSourceVisibilityChanged";
}

void DefaultEdge::onDestinationVisibilityChanged(bool visible)
{
    // check if at least one of the src and dst is visible; if no, hide edge
    qDebug() << "DefaultEdge::onDestinationVisibilityChanged";
}

void DefaultEdge::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    // draw edge
    painter->setPen(Qt::white);
    painter->drawLine(source_pos_, destination_pos_);
}
