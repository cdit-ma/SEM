#ifndef DATA_H
#define DATA_H
#include "key.h"
#include "entity.h"
#include <QSet>
#include <QStack>

class Data : public GraphML
{
    Q_OBJECT
    friend class Entity;
    friend class EntityFactory;

    friend class Key;
    friend class Node;
    friend class Edge;

protected:
    Data(EntityFactoryBroker& broker, Key* key, QVariant value = QVariant(), bool protect = false);
    ~Data();
public:
    static bool CompareData(const Data* a, const Data* b);
    static bool SortByKey(const Data* a, const Data* b);
    Entity* getParent();

    void setProtected(bool protect);
    bool isProtected() const;



    bool setValue(QVariant value);
    
    bool linkData(Data* data, bool setup_bind);
    bool linkData(Data* data);
    bool unlinkData(Data* data);


    void registerParent(Entity* parent);
    
    
    bool revalidateData();


    Key* getKey() const;
    QString getKeyName() const;
    QVariant getValue() const;

    void ToGraphmlStream(QTextStream& stream, int indend_depth);

    QString toString() const;

    void addValidValue(QVariant value);
    void addValidValues(QList<QVariant> values);

    QList<QVariant> getValidValues();
protected:

    void store_value();
    void restore_value();

    bool forceValue(QVariant value);

    void setParent(Entity* parent);
private:
    void addParentData(Data* data);
    void removeParentData(Data* data);
private:
    bool _setData(QVariant value);
    bool addChildData(Data* data);
    bool removeChildData(Data* data);
signals:
    void dataChanged(QVariant data);
private:
    bool _setValue(QVariant value, bool validate = true);
    void updateChildren(bool changed = true);
    
    Entity* parent = 0;
    Key* key = 0;

    QSet<Data*> parent_datas;
    QSet<Data*> child_datas;
    
    QString key_name;

    bool is_protected = false;
    bool is_data_linked = false;

    QList<QVariant> valid_values_;
    
    
    QVariant value;
    QStack<QVariant> old_values_;
};

#endif // DATA_H
