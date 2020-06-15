#ifndef DATAUPDATE_H
#define DATAUPDATE_H
#include <QString>

struct DataUpdate{
    QString key_name;
    QVariant value;
    bool is_protected;
};

Q_DECLARE_METATYPE(DataUpdate)
#endif //DATAUPDATE_H