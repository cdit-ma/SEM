//
// Created by Cathlyn Aston on 19/11/20.
//

#include "delegateanchor.h"

using namespace Pulse::View;

DelegateAnchor::DelegateAnchor(QGraphicsItem* parent)
    : QGraphicsObject(parent) {}


void DelegateAnchor::transferToAdopter(EdgeAdopter* adopter)
{
    if (adopter == nullptr) {
        throw std::invalid_argument("DelegateAnchor::transferToAdopter - The edge adopter is null");
    }
    for (auto anchor : attached_anchors_) {
        anchor->transferToAdopter(adopter);
    }
}

void DelegateAnchor::edgeAnchorMoved(const QPointF& pos)
{
    for (auto anchor : attached_anchors_) {
        anchor->edgeAnchorMoved(pos);
    }
}

void DelegateAnchor::edgeAnchorVisibilityChanged(bool visible)
{
    for (auto anchor : attached_anchors_) {
        anchor->edgeAnchorVisibilityChanged(visible);
    }
}

void DelegateAnchor::adoptEdges(EdgeAnchor* anchor)
{
    checkPreCondition(anchor);
    if (attached_anchors_.contains(anchor)) {
        throw std::invalid_argument("DelegateAnchor::adoptEdges - The edge anchor is already attached");
    }
    attached_anchors_.append(anchor);
}

void DelegateAnchor::returnEdges(EdgeAnchor* anchor)
{
    checkPreCondition(anchor);
    if (!attached_anchors_.contains(anchor)) {
        throw std::invalid_argument("DelegateAnchor::returnEdges - The edge anchor is not attached");
    }
    attached_anchors_.removeAll(anchor);
}

void DelegateAnchor::checkPreCondition(EdgeAnchor* anchor)
{
    if (anchor == nullptr) {
        throw std::invalid_argument("DelegateAnchor::checkPreCondition - The edge anchor is null");
    }
}