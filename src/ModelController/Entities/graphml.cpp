#include "graphml.h"
#include "../entityfactory.h"

GraphML::GraphML(GRAPHML_KIND kind):QObject(0)
{
    this->kind = kind;
}

GraphML::~GraphML(){
    if(factory_){
        factory_->DeregisterEntity(this);
    }
}

/**
 * @brief GraphML::getKind
 * Returns the GRAPHML_KIND of the element. Used for upcasting.
 * @return The kind of the GraphML Object
 */
GRAPHML_KIND GraphML::getGraphMLKind() const
{
    return kind;
}

void GraphML::setID(int id){
    //Only allow setting once
    if(this->id < 0){
        this->id = id;
    }
}

/**
 * @brief GraphML::getID
 * Returns the unique ID of this object.
 * @return
 */
int GraphML::getID() const
{
    return id;
}

void GraphML::setFactory(EntityFactory* factory){
    factory_ = factory;

    if(factory_){
        factory_->RegisterEntity(this);
    }
}

EntityFactory* GraphML::getFactory(){
    return factory_;
}
