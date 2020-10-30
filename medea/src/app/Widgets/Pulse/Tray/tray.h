//
// Created by Cathlyn Aston on 29/10/20.
//

#ifndef PULSE_VIEW_TRAY_H
#define PULSE_VIEW_TRAY_H

#include <QGraphicsWidget>

namespace Pulse::View {

class Tray {
public:
    virtual ~Tray() = 0;
    virtual void addItem(QGraphicsWidget* widget) = 0;
};

inline Tray::~Tray() = default;

} // end Pulse::View
#endif // PULSE_VIEW_TRAY_H
