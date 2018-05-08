#ifndef EXPORT_ID_KEY_H
#define EXPORT_ID_KEY_H
#include "../key.h"

class ExportIDKey : public Key
{
    Q_OBJECT
public:
    ExportIDKey(EntityFactory& factory);
    //If it's a valid UUID, use it, if its another string MD5 it, else generate a new one.
    static QString GetUUIDOfValue(const QString str_value);

    QVariant validateDataChange(Data* data, QVariant dataValue);
private:
    static QString getMD5UUID(const QString str);
};

#endif // EXPORT_ID_KEY_H
