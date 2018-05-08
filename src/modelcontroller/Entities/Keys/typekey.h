#ifndef TYPE_KEY_H
#define TYPE_KEY_H
#include "../key.h"
#include <QSet>

class Node;
class EntityFactory;

class TypeKey : public Key
{
    Q_OBJECT
public:
    TypeKey(EntityFactory& factory);
    QVariant validateDataChange(Data* data, QVariant dataValue);

    static void BindInnerAndOuterTypes(Node* src, Node* dst, bool bind);
    static void BindTypes(Node* src, Node* dst, bool bind);
    static void BindNamespaceAndLabelToType(Node* node, bool bind);

    static QList<QVariant> GetValidPrimitiveTypes();

    static bool CompareTypes(Node* node_1 , Node* node_2);
private:
    QSet<NODE_KIND> combine_namespace_kinds;
};

#endif //TYPE_KEY_H
