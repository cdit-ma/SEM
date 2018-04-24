#ifndef ENTITY_H
#define ENTITY_H
#include "graphml.h"

#include <QHash>
#include <QVariant>

class Data;
class Key;
class Entity: public GraphML
{
    friend class Data;
    friend class EntityFactory;
    Q_OBJECT
protected:
    Entity(GRAPHML_KIND kind);
    ~Entity();
public:
    virtual bool isInModel() = 0;

    bool addData(Data* data);
    bool addData(QList<Data*> dataList);


protected:

    void _dataChanged(Data* data);
    void _dataRemoved(Data* data);
    virtual void DataAdded(Data* data){};
public:
    Data* getData(QString keyName) const;
    Data* getData(Key* key) const;

private:
    Key* getKey(QString keyName) const;

public:
    QList<Data *> getData() const;
    QList<Key *> getKeys() const;
    QStringList getKeyNames() const;
    bool gotData(QString keyName = "") const;
    bool gotData(Key* key) const;

    bool removeData(Key* key);
    bool removeData(Data* data);
    bool removeData(QString keyName);


    bool isNode() const;
    bool isEdge() const;

    bool isReadOnly() const;
    bool isSnippetReadOnly() const;


    QVariant getDataValue(QString keyName) const;
    QVariant getDataValue(Key* key) const;
    bool setDataValue(QString keyName, QVariant value);
    bool setDataValue(Key* key, QVariant value);


    QStringList getProtectedKeys();

    virtual QString toGraphML(int indentDepth=0, bool function_only = false) = 0;
    QString toString() const;
    
signals:
    void dataChanged(int ID, QString keyName, QVariant data);
    void dataRemoved(int ID, QString keyName);

    void terminating();
private:
    QStringList unprotectedKeys;

    QHash<Key*, Data*> dataLookup;
    QHash<QString, Key*> keyLookup;

    bool position_enabled = true;
    bool size_enabled = false;
    bool expand_enabled = true;
};

#endif // ENTITY_H
