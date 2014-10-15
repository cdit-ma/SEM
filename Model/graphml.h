#ifndef GRAPHML_H
#define GRAPHML_H

#include <QVector>
#include <QString>
#include <QObject>

//Forward Declaration
class GraphMLData;
class GraphMLKey;

//Abstract Base class for ALL GraphML objects used in a graph. Extended by GraphMLContainer.
//Some pure Virtual methods. Cannot be directly Instantiated.
class GraphML : public QObject{
    Q_OBJECT
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

    QString getDataValue(QString keyName);

    GraphMLData* getData(QString keyName);

    GraphMLData* getData(GraphMLKey* key);

    //Get a list of Data objects contained by this GraphML
    QVector<GraphMLData *> getData();

    //Remove all Data
    void removeData();

    //Attach a data object contained by this GraphML
    void attachData(GraphMLData* data);

    //Remove a data object contained by this GraphML;
    void removeData(GraphMLData* data);

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


signals:
    //Used to inform models if data has been removed.
    void dataRemoved(GraphMLData* data);
    void dataAdded(GraphMLData* data);
public slots:
    void updateDataValue(QString key, QString value);

protected:
    QVector<GraphMLData *> attachedData;

    void setID(QString id);
private:
    //The name of this graphml object.
    QString name;

    //QString
    QString deletingXML;

    //The ID of this graphml object.
    QString id;

    //The unique ID of this graphml object.
    qint32 Uid;

    //The unique kind ID of this graph object.
    KIND kind;
};
#endif // GRAPHML_H


