#ifndef VERSION_KEY_H
#define VERSION_KEY_H
#include "../key.h"

class EntityFactoryRegistryBroker;
class Node;


class EntityFactoryRegistryBroker;
class VersionKey : public Key
{
    Q_OBJECT
public:
    VersionKey(EntityFactoryBroker& factory);
    QVariant validateDataChange(Data* data, QVariant dataValue);

    static int CompareVersion(const QString& current_version, const QString& compare_version);
    static bool IsVersionValid(const QString& version);
private:
    static const QString& GetDefaultVersion();
};

#endif //VERSION_KEY_H
