//
// Created by Cathlyn Aston on 24/9/20.
//

#ifndef PULSE_VIEW_ENTITYINFODISPLAY_H
#define PULSE_VIEW_ENTITYINFODISPLAY_H

#include <QString>

namespace Pulse::View {

/**
 * @brief An EntityInfoDisplay listens to and handles changes to the Entity's name and icon
 */
class EntityInfoDisplay {
public:
    virtual ~EntityInfoDisplay() = 0;

    virtual void changeIcon(const QString& icon_path, const QString& icon_name) = 0;
    virtual void changeName(const QString& name) = 0;
};

inline EntityInfoDisplay::~EntityInfoDisplay() = default;

} // end Pulse::View namespace
#endif // PULSE_VIEW_ENTITYINFODISPLAY_H