//
// Created by Cathlyn Aston on 28/10/20.
//

#ifndef PULSE_MODEL_ENTITY_H
#define PULSE_MODEL_ENTITY_H

#include <QObject>

namespace Pulse::Model {

class Entity : public QObject {
    Q_OBJECT

public:
    virtual ~Entity() = 0;

signals:
    // change to nameChanged()
    void labelChanged(const QString& label);
    void iconChanged(const QString& icon_path, const QString& icon_name);

    // send a signal to flash/unflash entities at given time from Timer class
    //
};

inline Entity::~Entity() = default;

} // end Pulse::Model namespace
#endif // PULSE_MODEL_ENTITY_H


// Create Timer class that listens to Pulse's Time Progress bar in the view
// send time signal to entities

// Try composition of specific model classes  (ie. Pulse/Charts)