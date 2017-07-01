#ifndef DATA_H
#define DATA_H
#include "key.h"
#include "entity.h"
#include <QSet>

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
    Entity* getParent();

    void setProtected(bool protect);
    bool isProtected() const;



    bool setValue(QVariant value);
    

    bool bindData(Data* data);
    bool unbindData(Data* data);
    void registerParent(Entity* parent);

    Data* getParentData();
    
    void revalidateData();
    void clearValue();
    bool compare(const Data* data) const;


    Key* getKey();
    QString getKeyName() const;
    QVariant getValue() const;

    QString toGraphML(int indentDepth);
    QString toString();
protected:
    void store_value();
    void restore_value();

    bool forceValue(QVariant value);

    void setParentData(Data* data);
    void setParent(Entity* parent);
private:
    bool addChildData(Data* data);
    bool removeChildData(Data* data);
signals:
    void dataChanged(QVariant data);
private slots:
    void parentDataChanged(int ID, QString keyName, QVariant data);
private:
    bool _setValue(QVariant value, bool validate = true);
    void updateChildren(bool changed = true);
    
    Entity* parent = 0;
    Key* key = 0;
    Data* parent_data = 0;
    int parent_data_id = -1;
    
    QString key_name;

    bool is_protected = false;
    bool is_data_linked = false;
    
    
    QVariant value;
    QVariant old_value;

    QSet<Data*> child_data;
};

#endif // DATA_H
