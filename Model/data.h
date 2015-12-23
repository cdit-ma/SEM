#ifndef DATA_H
#define DATA_H
#include "key.h"
#include "entity.h"
class Data : public GraphML
{
    Q_OBJECT
public:
    Data(Key* key, QVariant value = QVariant(), bool protect = false);
    static Data* clone(Data* data);

    void setParent(Entity* parent);
    Entity* getParent();

    void setProtected(bool protect);
    bool isProtected() const;

    bool setValue(QVariant value);

    void setParentData(Data* parentData, bool protect = true);
    Data* getParentData();
    void unsetParentData();

    void clearValue();
    bool compare(const Data* data) const;

    QList<Data*> getChildData();


    Key* getKey();
    QString getKeyName();
    QVariant getValue() const;

    QString toGraphML(int indentDepth);
    QString toString();
protected:
    void addChildData(Data* childData);
    void removeChildData(Data* childData);
private slots:
    void parentDataChanged(int ID, QString keyName, QVariant data);
signals:
    void dataChanged(int ID, QString keyName, QVariant data);
private:
    Entity* _parent;
    Key* _key;
    Data* _parentData;
    int _parentDataID;
    QString _keyName;

    bool _isProtected;
    QVariant _value;
    QHash<int, Data*> _childData;

    // GraphML interface

};

#endif // DATA_H
