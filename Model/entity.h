#ifndef ENTITY_H
#define ENTITY_H
#include "graphml.h"
#include <QHash>
#include <QVariant>

class Data;
class Key;
class Entity: public GraphML
{
    Q_OBJECT
public:
    enum ENTITY_KIND{EK_NONE, EK_ALL, EK_NODE, EK_EDGE};

    static QString getEntityKindName(const ENTITY_KIND entityKind);
    static ENTITY_KIND getEntityKind(const QString entityString);


    Entity(ENTITY_KIND kind);
    ~Entity();
    ENTITY_KIND getEntityKind() const;

    virtual bool isInModel() = 0;
    bool addData(Data* data);
    bool addData(QList<Data*> dataList);


    Data* getData(QString keyName);
    Data* getData(Key* key);
    Data* getData(int ID);
    QList<Data *> getData();
    QList<Key *> getKeys(int depth = -1);

    bool hasData(QString keyName = "");

    QStringList getKeyNames();

    QString getEntityName();
    bool isNode();
    bool isEdge();
    bool isReadOnly();
    bool isDataProtected(QString keyName);
    bool isSnippetReadOnly();


    QVariant getDataValue(QString keyName);
    void setDataValue(QString keyName, QVariant value);


    QStringList getProtectedKeys();
    bool removeData(Key* key);
    bool removeData(Data* data);
    bool removeData(QString keyName);

    virtual QString toGraphML(int indentDepth=0) = 0;
    QString toString();
signals:
    void dataChanged(int ID, QString keyName, QVariant data);
    void dataRemoved(int ID, QString keyName);

    void propertyChanged(int ID, QString propertyName, QVariant data);
    void propertyRemoved(int ID, QString propertyName);

    void readOnlySet(int, bool isReadOnly);
    void terminating();
public slots:
    void dataProtected(bool protect);
private slots:
    void thisDataChanged(QString keyName);
private:
    int getDataIDFromKeyName(QString keyName);
    int getKeyIDFromKeyName(QString keyName);
    int getDataIDFromKeyID(int keyID);
    Data* getDataFromDataID(int dataID);

    QStringList unprotectedKeys;
    Entity::ENTITY_KIND entityKind;
    //Data ID -> Data
    QHash<int, Data*> lookupDataID2Data;
    //Key ID -> Data ID
    QHash<int, int> lookupKeyID2DataID;
    //Key Name -> Key ID
    QHash<QString, int> lookupKeyName2KeyID;
};

#endif // ENTITY_H
