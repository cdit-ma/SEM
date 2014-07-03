#ifndef GRAPHML_H
#define GRAPHML_H

#include <QVector>
#include <QString>

//Forward Declaration
class GraphMLData;

//Abstract Base class for ALL GraphML objects used in a graph. Extended by GraphMLContainer.
//Some pure Virtual methods. Cannot be directly Instantiated.
class GraphML{

public:
    enum KIND{NONE, GRAPH, NODE, EDGE, ALL, KEY, DATA};

    //Super class constructor; Relative ID and name provided. Used only to be called by the extended classes.
    GraphML(KIND kind, QString name="");

    //The destructor.
    ~GraphML();

    //Set the name of this graphml object
    void setName(QString name);

    GraphML::KIND getKind() const;

    //Get the name of this graphml object
    QString getName() const;

    QString getID() const;

    QString getData(QString keyName);
    //Get a list of Data objects contained by this GraphML
    QVector<GraphMLData *> getData();

    //Attach a data object contained by this GraphML
    void attachData(GraphMLData* data);

    void attachData(QVector<GraphMLData* > data);

    //Get the Unique ID of this object
    qint32 getUID() const;

    //Returns a string graphml representation of this graphml object
    //Pure Virtual => Must be overwritten by subclasses!
    virtual QString toGraphML(qint32 indentationLevel=0)=0;

    //Returns a string representation of this graphml object.
    virtual QString toString()=0;

    //The unique ID counter;
    static qint32 _Uid;
protected:
    QVector<GraphMLData *> attachedData;

    void setID(QString id);
private:
    //The name of this graphml object.
    QString name;

    //The ID of this graphml object.
    QString id;

    //The unique ID of this graphml object.
    qint32 Uid;

    //The unique kind ID of this graph object.
    KIND kind;
};
#endif // GRAPHML_H


