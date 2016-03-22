#include "graphml.h"
int GraphML::_IDCounter = 0;

/**
 * @brief GraphML::GraphML
 * The GraphML class is the lightest weight element in the Model. It is used to ensure that every item has an unique ID and can be used to up cast.
 * @param kind The kind of this element.
 */

void GraphML::resetID()
{
      GraphML::_IDCounter = 0;
}

GraphML::GraphML(GraphML::GRAPHML_KIND kind):QObject(0)
{
    this->id = -1;

    if(kind != GraphML::GK_DATA){
        setID();
    }
    this->kind = kind;
}

/**
 * @brief GraphML::getKind
 * Returns the GraphML::GRAPHML_KIND of the element. Used for upcasting.
 * @return The kind of the GraphML Object
 */
GraphML::GRAPHML_KIND GraphML::getGraphMLKind() const
{
    return kind;
}

void GraphML::setID()
{
    if(id < 0){
        this->id = ++_IDCounter;
    }
}

/**
 * @brief GraphML::getID
 * Returns the unique ID of this object.
 * @return
 */
int GraphML::getID()
{
    return id;
}
