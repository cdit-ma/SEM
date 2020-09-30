//
// Created by Cathlyn Aston on 24/9/20.
//

#ifndef PULSE_VIEW_ENTITY_H
#define PULSE_VIEW_ENTITY_H

#include <QObject>

namespace Pulse::View {

class Entity {
public:
    virtual ~Entity() = 0;

    // TODO: Create an interface for Pulse::Model that sends signals when the model has changed
    //  Then use that here instead of a QObject
    virtual void connectModelData(QObject* model_data) = 0;
    virtual void onModelDeleted() = 0;
};

} // end Pulse::View namespace
#endif // PULSE_VIEW_ENTITY_H
