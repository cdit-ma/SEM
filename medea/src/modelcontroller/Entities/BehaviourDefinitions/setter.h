#ifndef SETFUNCTION_H
#define SETFUNCTION_H
#include "../InterfaceDefinitions/datanode.h"


class DataNode;
class EntityFactoryRegistryBroker;
class Setter : public DataNode
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
	Setter(EntityFactoryBroker& factory, bool is_temp_node);
public:
    bool canAdoptChild(Node* child);

	DataNode* GetLhs();
	DataNode* GetRhs();
	DataNode* GetOperator();
private:
	void operatorChanged();
	void updateLabel();

	DataNode* lhs_ = 0;
	DataNode* operator_ = 0;
	DataNode* rhs_ = 0;
};

#endif //SETFUNCTION_H
