#ifndef TYPE_KEY_H
#define TYPE_KEY_H
#include "../key.h"
#include <QSet>

class Node;

class TypeKey : public Key
{
    Q_OBJECT
public:
    TypeKey();
    QVariant validateDataChange(Data* data, QVariant dataValue);

    static void BindInnerAndOuterTypes(Node* src, Node* dst, bool bind);
    static void BindTypes(Node* src, Node* dst, bool bind);
    static void BindNamespaceAndLabelToType(Node* node, bool bind);
private:
    QSet<NODE_KIND> combine_namespace_kinds;
};

#endif //TYPE_KEY_H
