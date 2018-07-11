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
    friend class Key;

    Q_OBJECT
public:
    static bool SortByIndex(const Entity* a, const Entity* b);
protected:
    Entity(EntityFactoryBroker& factory, GRAPHML_KIND graphml_kind);
    ~Entity();
    virtual void DataAdded(Data* data){};
    void revalidateData();
public:
    QSet<Key *> getKeys() const;

    bool addData(Data* data);
    bool addData(QList<Data*>& data_list);
    Data* getData(const QString& key_name) const;
    Data* getData(Key* key) const;
    QList<Data *> getData() const;
    
    bool removeData(Key* key);
    bool removeData(Data* data);
    bool removeData(const QString& key_name);

    
    
    bool isLabelFunctional() const;
    bool isImplicitlyConstructed() const;
    void _dataChanged(Data* data);
    void _dataRemoved(Data* data);
    QStringList getKeyNames() const;

    bool gotData() const;
    bool gotData(const QString& key_name) const;
    bool gotData(Key* key) const;



    bool isNode() const;
    bool isEdge() const;

    bool isReadOnly() const;

    QVariant getDataValue(const QString& key_name) const;
    QVariant getDataValue(Key* key) const;
    bool setDataValue(const QString& key_name, QVariant value);
    bool setDataValue(Key* key, QVariant value);

    QStringList getProtectedKeys();

    virtual QString toGraphML(int indentDepth=0, bool function_only = false) = 0;
    QString toString() const;
    void setImplicitlyConstructed(bool implicitly_constructed = true);
protected:
    void setLabelFunctional(bool functional = true);
signals:
    void dataChanged(int ID, QString key_name, QVariant data, bool is_protected);
    void dataRemoved(int ID, QString key_name);
private:
    Key* getKey(const QString& key_name) const;
    bool is_label_functional_ = true;
    bool is_implicitly_constructed_ = false;
    
    QHash<Key*, Data*> data_map_;
    QHash<QString, Key*> key_lookup_;
};

#endif // ENTITY_H
