#ifndef GRAPHML_H
#define GRAPHML_H
#include <QObject>
#include "../kinds.h"

class EntityFactory;
class EntityFactoryBroker;

class GraphML: public QObject{
    Q_OBJECT

    //The factory can access protected constructors
    friend class EntityFactory;
protected:
    GraphML(EntityFactoryBroker& broker, GRAPHML_KIND kind);
    virtual ~GraphML();
    void setID(int id);
    EntityFactoryBroker& getFactoryBroker();
public:
    static bool SortByID(const GraphML* a, const GraphML* b);

    GRAPHML_KIND getGraphMLKind() const;
    int getID() const; 

    //Pure Virtual
    virtual QString toGraphML(int indentDepth=0, bool functional_export = false)=0;
    virtual QString toString() const = 0;
private:
    int id = -1;
    GRAPHML_KIND kind = GRAPHML_KIND::NONE;
    EntityFactoryBroker& broker_;
};
#endif // GRAPHML_H


