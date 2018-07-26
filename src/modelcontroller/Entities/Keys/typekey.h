#ifndef TYPE_KEY_H
#define TYPE_KEY_H
#include "../key.h"
#include "../../nodekinds.h"
#include <QSet>

class EntityFactoryRegistryBroker;
class Node;


class EntityFactoryRegistryBroker;
class TypeKey : public Key
{
    Q_OBJECT
public:
    TypeKey(EntityFactoryBroker& factory);
    QVariant validateDataChange(Data* data, QVariant dataValue);

    static void BindInnerAndOuterTypes(Node* src, Node* dst, bool bind);
    static void BindTypes(Node* src, Node* dst, bool bind);
    static void BindNamespaceAndLabelToType(Node* node, bool bind);

    static QList<QVariant> GetValidPrimitiveTypes();
    static QList<QVariant> GetValidNumberTypes();
    static QVariant GetDefaultPrimitiveType();

    static QString GetCPPPrimitiveType(const QString& type);
    

    static bool CompareTypes(Node* node_1 , Node* node_2);
private:
    static const QSet<QString>& GetPrimitiveTypes();
    static const QSet<QString>& GetNumberTypes();

    QSet<NODE_KIND> combine_namespace_kinds;
};

#endif //TYPE_KEY_H
