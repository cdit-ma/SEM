#ifndef DATA_H
#define DATA_H
#include "key.h"
#include "entity.h"
class EntityFactory; 
class Data : public GraphML
{
    Q_OBJECT
    friend class Entity;
    friend class EntityFactory;
    friend class Key;
    friend class Node;
    friend class Edge;
protected:
    Data(Key* key, QVariant value = QVariant(), bool protect = false);
    ~Data();
    static Data* clone(Data* data);
public:

    void setParent(Entity* parent);
    Entity* getParent();

    void setProtected(bool protect);
    bool isProtected() const;



    bool setValue(QVariant value);
    
    void setParentData(Data* parentData);
    Data* getParentData();
    void unsetParentData();
    void revalidateData();
    void clearValue();
    bool compare(const Data* data) const;

    QList<Data*> getChildData();

    bool isVisualData();

    Key* getKey();
    QString getKeyName() const;
    QVariant getValue() const;

    QString toGraphML(int indentDepth);
    QString toString();
protected:
    void store_value();
    void restore_value();

    bool forceValue(QVariant value);
    void addChildData(Data* childData);
    void removeChildData(Data* childData);
signals:
    void dataChanged(QVariant data);
private slots:
    void parentDataChanged(int ID, QString keyName, QVariant data);
private:
    bool _setValue(QVariant value, bool validate = true);
    void updateChildren(bool changed = true);
    Entity* _parent;
    Key* _key;
    Data* _parentData;
    int _parentDataID;
    QString _keyName;

    bool _isProtected;
    bool _isDataLinked;
    QVariant _value;
    QVariant old_value;
    QHash<int, Data*> _childData;

    // GraphML interface

};

#endif // DATA_H
