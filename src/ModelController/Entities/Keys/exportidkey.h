#ifndef EXPORT_ID_KEY_H
#define EXPORT_ID_KEY_H
#include "../key.h"

class ExportIDKey : public Key
{
    Q_OBJECT
public:
    ExportIDKey();
    QVariant validateDataChange(Data* data, QVariant dataValue);
private:
    static QString getMD5UUID(const QString str);
};

#endif // EXPORT_ID_KEY_H
