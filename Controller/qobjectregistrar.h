#ifndef QOBJECTREGISTRAR_H
#define QOBJECTREGISTRAR_H

#include <QObject>

class QObjectRegistrar : public QObject
{
    Q_OBJECT
public:
    QObjectRegistrar(QObject *parent = 0);

    void registerObject(QObject* object);
    void unregisterObject(QObject* object);
    bool hasRegisteredObjects();
signals:
    void lastRegisteredObjectRemoved();
private slots:
    void unregisterObject();
private:
    void _registerObject(QObject* object);
    void _unregisterObject(QObject* object);
    QList<QObject*> registeredObjects;
};

#endif // QOBJECTREGISTRAR_H
