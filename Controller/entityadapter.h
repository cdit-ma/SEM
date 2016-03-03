#ifndef ENTITYADAPTER_H
#define ENTITYADAPTER_H
#include "../Model/entity.h"
#include <QObject>
#include <QVector>
#include "../enumerations.h"
class NewController;
class EntityAdapter: public QObject
{
    Q_OBJECT
    friend class NewController;
public:
    EntityAdapter(Entity* entity);
    ~EntityAdapter();
    int getID();
    bool isNodeAdapter();
    bool isEdgeAdapter();
    bool isReadOnly();

    const QVariant getDataValue(QString keyName);
    bool isDataProtected(QString keyName);

    QStringList getKeys();
    QStringList getValidValuesForKey(QString keyName);
    bool hasData(QString keyName);
    QVariant::Type getKeyType(QString keyName);


    QString toString();
    bool isValid();

    void addListener(QObject* object);
    void removeListener(QObject* object);
    bool hasListeners();

signals:

    void dataAdded(QString keyName, QVariant data);
    void dataChanged(QString keyName, QVariant data);
    void dataRemoved(QString keyName);

    void readOnlySet(int ID, bool isReadOnly);
private:
    void invalidate();
    bool _isValid;
    Entity* _entity;
    bool _isNode;
    int _ID;
    QVector<QObject*> _listeners;
};

#endif // ENTITYADAPTER_H
