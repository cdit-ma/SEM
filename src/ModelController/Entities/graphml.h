#ifndef GRAPHML_H
#define GRAPHML_H
#include <QObject>
#include "../kinds.h"

class EntityFactory;

class GraphML: public QObject{
    Q_OBJECT

    //The factory can access protected constructors
    friend class EntityFactory;
protected:
    GraphML(GRAPHML_KIND kind);
    virtual ~GraphML();
    int setID(int id);
    void setFactory(EntityFactory* factory);
    EntityFactory* getFactory();
public:
    GRAPHML_KIND getGraphMLKind() const;
    int getID() const; 

    //Pure Virtual
    virtual QString toGraphML(int indentDepth=0)=0;
    virtual QString toString()=0;
private:
    int id = -1;
    GRAPHML_KIND kind = GRAPHML_KIND::NONE;
    EntityFactory* factory_ = 0;
};
#endif // GRAPHML_H


