//
// Created by Cathlyn Aston on 20/10/20.
//

#ifndef PULSE_VIEW_ENTITYCONTAINER_H
#define PULSE_VIEW_ENTITYCONTAINER_H

#include "../Entity/entity.h"

namespace Pulse::View {

/**
 * @brief An EntityContainer can contain Entities and be able to be contracted and expanded
 */
class EntityContainer {
public:
    virtual ~EntityContainer() = 0;

    virtual void add(Entity* entity) = 0;
    virtual void remove(Entity* entity) = 0;

    virtual void expand() = 0;
    virtual void contract() = 0;
};

inline EntityContainer::~EntityContainer() = default;

} // end Pulse::View namespace
#endif // PULSE_VIEW_ENTITYCONTAINER_H