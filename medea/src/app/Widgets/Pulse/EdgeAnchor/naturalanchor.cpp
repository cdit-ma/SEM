//
// Created by Cathlyn Aston on 19/11/20.
//

#include "naturalanchor.h"
#include "edgeadopter.h"
#include "../pulseviewutils.h"
#include "../../../theme.h"

#include <QPainter>

using namespace Pulse::View;

NaturalAnchor::NaturalAnchor(QGraphicsItem* parent)
    : QGraphicsObject(parent)
{
    connect(this, &NaturalAnchor::visibleChanged, [this]() { edgeAnchorVisibilityChanged(isVisible()); } );
    connect(this, &NaturalAnchor::xChanged, []() { qDebug() << "x changed"; } );
    connect(Theme::theme(), &Theme::theme_Changed, [this]() {
        anchor_color_ = Theme::theme()->getMenuIconColor();
        update();
    });
}

void NaturalAnchor::transferToAdopter(EdgeAdopter* adopter)
{
    if (adopter == nullptr) {
        throw std::invalid_argument("NaturalAnchor::transferToAdopter - The edge adopter is null");
    }
    adopter->adoptEdges(this);
    active_adopter_ = adopter;
}

void NaturalAnchor::reclaimEdges()
{
    if (active_adopter_ != nullptr) {
        active_adopter_->returnEdges(this);
    }
    active_adopter_ = nullptr;
}

void NaturalAnchor::connectEdge(Edge* edge)
{
    if (edge == nullptr) {
        throw std::invalid_argument("NaturalAnchor::connectEdge - Edge is null");
    }
    edges_.push_back(edge);
}

void NaturalAnchor::disconnectEdges()
{
    edges_.clear();
}

QRectF NaturalAnchor::boundingRect() const
{
    return QRectF(-5, -5, 10, 10);
}

void NaturalAnchor::paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->fillRect(boundingRect(), anchor_color_);
}