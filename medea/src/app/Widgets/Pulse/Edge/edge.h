//
// Created by Cathlyn Aston on 19/11/20.
//

#ifndef PULSE_VIEW_EDGE_H
#define PULSE_VIEW_EDGE_H

#include "../EdgeAnchor/edgeanchor.h"

#include <QPointF>
//#include <QObject>

namespace Pulse::View {

class Edge { //: public QObject {
    //Q_OBJECT

public:
    virtual ~Edge() = 0;

//public slots:
    virtual void onSourceMoved(const QPointF& pos) = 0;
    virtual void onDestinationMoved(const QPointF& pos) = 0;

    virtual void onSourceVisibilityChanged(bool visible) = 0;
    virtual void onDestinationVisibilityChanged(bool visible) = 0;
};

inline Edge::~Edge() = default;

} // end Pulse::View namespace
#endif // PULSE_VIEW_EDGE_H
