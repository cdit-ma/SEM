#ifndef EXPORT_ID_KEY_H
#define EXPORT_ID_KEY_H
#include "../key.h"
#include <functional>

class Entity;
class EntityFactoryRegistryBroker;
class ExportIDKey : public Key
{
    Q_OBJECT
public:
    ExportIDKey(EntityFactoryBroker& factory, std::function<void (Entity*, QString, QString)> uuid_notifier);
    //If it's a valid UUID, use it, if its another string MD5 it, else generate a new one.
    static QString GetUUIDOfValue(const QString& str_value);

    QVariant validateDataChange(Data* data, QVariant dataValue);
    bool setData(Data* data, QVariant dataValue);
private:
    static QString getMD5UUID(const QString& str);
    
    std::function<void (Entity*, QString, QString)> uuid_notifier_;

};

#endif // EXPORT_ID_KEY_H
