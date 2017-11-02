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

    
bool GraphML::SortByID(const GraphML* a, const GraphML* b){
    return a->getID() < b->getID();
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

int GraphML::setID(int id){
    //Only allow setting once
    if(this->id < 0){
        this->id = id;
    }
    return this->id;
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
}

EntityFactory* GraphML::getFactory(){
    return factory_;
}
