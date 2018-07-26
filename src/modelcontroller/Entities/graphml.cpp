#include "graphml.h"
#include "../entityfactorybroker.h"
#include "../entityfactoryregistrybroker.h"

GraphML::GraphML(EntityFactoryBroker& broker, GRAPHML_KIND kind):QObject(0), broker_(broker)
{
    this->kind = kind;
}

GraphML::~GraphML(){
    broker_.DeregisterGraphML(this);
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


EntityFactoryBroker& GraphML::getFactoryBroker(){
    return broker_;
}

QString GraphML::toGraphML(int indent_depth){
    QString xml;
    QTextStream stream(&xml);
    ToGraphmlStream(stream, indent_depth);
    return xml;
}