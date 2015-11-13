#ifndef GRAPHML_H
#define GRAPHML_H

#include <QVector>
#include <QString>
#include <QObject>

//Forward Declaration
class GraphMLData;
class GraphMLKey;

//Abstract Base class for ALL GraphML objects used in a graph. Extended by Node.
//Some pure Virtual methods. Cannot be directly Instantiated.
class GraphML : public QObject{
    Q_OBJECT
public:
    enum KIND{NONE, GRAPH, NODE, EDGE, ALL, KEY, DATA};

    //Super class constructor; Relative ID and name provided. Used only to be called by the extended classes.
    GraphML(KIND kind, QString name="");

    bool isNode();
    bool isEdge();
    bool isDeleting();

    //The destructor.
    ~GraphML();

    //Set the name of this graphml object
    void setName(QString name);

    GraphML::KIND getKind() const;

    //Get the name of this graphml object
    QString getName() const;

    int getID() const;

    virtual QList<GraphMLKey*> getKeys(int depth = 0);

    QString getDataValue(QString keyName);
    qreal getDataNumberValue(QString keyName);



    GraphMLData* getData(QString keyName);
    GraphMLData* getData(int ID);

    GraphMLData* getData(GraphMLKey* key);

    bool containsData(GraphMLData* data);

    bool wasGenerated();
    void setGenerated(bool isGenerated);

    //Get a list of Data objects contained by this GraphML
    QList<GraphMLData *> getData();



    //Attach a data object contained by this GraphML
    void attachData(GraphMLData* data, bool privateData=false);
    void attachData(QList<GraphMLData* > data);

    //Remove a data object contained by this GraphML;
    void removeData(GraphMLData* data);
    void removeData(QString keyName);


    //Returns a string graphml representation of this graphml object
    //Pure Virtual => Must be overwritten by subclasses!
    virtual QString toGraphML(qint32 indentationLevel=0)=0;

    //Returns a string representation of this graphml object.
    virtual QString toString()=0;

    //The unique ID counter;
    static qint32 _Uid;


signals:
    void deleting();
    //Used to inform models if data has been removed.
    void dataRemoved(int ID);
    void dataRemoved(GraphMLData* data);
    void dataAdded(GraphMLData* data);
    void model_DisplayMessage(QString title, QString message, int ID);
public slots:
    void updateDataValue(QString key, QString value);
    void updateDataValue(QString key, qreal value);

protected:
    QList<GraphMLData *> attachedData;

private:
    //Remove all Data
    void removeData();

    //The name of this graphml object.
    QString name;

    //QString
    QString deletingXML;

    bool generated;

    bool _deleting;

    //The ID of this graphml object.
    QString id;

    //The unique ID of this graphml object.
    qint32 Uid;

    //The unique kind ID of this graph object.
    KIND kind;
};
#endif // GRAPHML_H


