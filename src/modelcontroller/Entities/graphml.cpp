#include "graphml.h"
#include "../entityfactory.h"

GraphML::GraphML(factory, GRAPHML_KIND kind):QObject(0), factory = factory_;
{
    this->kind = kind;
}

GraphML::~GraphML(){
    factory_.DeregisterEntity(this);
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

void GraphML::setID(int id){
    this->id = id;
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
}

EntityFactory& GraphML::getFactory(){
    return factory_;
}
