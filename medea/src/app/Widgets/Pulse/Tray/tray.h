//
// Created by Cathlyn Aston on 29/10/20.
//

#ifndef PULSE_VIEW_TRAY_H
#define PULSE_VIEW_TRAY_H

#include <QGraphicsWidget>

namespace Pulse::View {

/**
 * @brief A Tray is simply a holder for QGraphicsWidgets
 */
class Tray {
public:
    virtual ~Tray() = 0;

    virtual void addItem(QGraphicsWidget* widget) = 0;
    virtual void removeItem(QGraphicsWidget* widget) = 0;

    virtual bool isEmpty() const = 0;
};

inline Tray::~Tray() = default;

} // end Pulse::View namespace
#endif // PULSE_VIEW_TRAY_H