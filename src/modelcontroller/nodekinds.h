#ifndef NODEKINDS_H
#define NODEKINDS_H

#include <QObject>
#include <QHash>

enum class ProtectedState{
    PROTECTED,
    UNPROTECTED,
    IGNORED
};

enum class NODE_KIND
    {
        NONE,
        MODEL,
        INTERFACE_DEFINITIONS,
        BEHAVIOUR_DEFINITIONS,
        DEPLOYMENT_DEFINITIONS,
        ASSEMBLY_DEFINITIONS,
        HARDWARE_DEFINITIONS,
        WORKER_DEFINITIONS,

        AGGREGATE,
        AGGREGATE_INSTANCE,
        MEMBER,
        VECTOR_INSTANCE,
        COMPONENT,
        MEMBER_INSTANCE,
        ATTRIBUTE,
        PORT_PUBLISHER,
        PORT_SUBSCRIBER,
        IDL,
        VECTOR,
        ENUM,
        ENUM_INSTANCE,
        ENUM_MEMBER,
        NAMESPACE,

        SERVER_INTERFACE,
        PORT_REPLIER,
        PORT_REQUESTER,
        PORT_REPLIER_IMPL,
        PORT_REPLIER_INST,
        PORT_REQUESTER_IMPL,
        PORT_REQUESTER_INST,
        INPUT_PARAMETER_GROUP,
        RETURN_PARAMETER_GROUP,
        INPUT_PARAMETER_GROUP_INSTANCE,
        RETURN_PARAMETER_GROUP_INSTANCE,
        VOID_TYPE,

        
        


        COMPONENT_ASSEMBLY,
        MANAGEMENT_COMPONENT,
        ATTRIBUTE_INSTANCE,
        COMPONENT_INSTANCE,
        PORT_SUBSCRIBER_DELEGATE,
        PORT_PUBLISHER_DELEGATE,
        
        PORT_SUBSCRIBER_INST,
        PORT_PUBLISHER_INST,
        
        PORT_PERIODIC_INST,
        HARDWARE_CLUSTER,
        HARDWARE_NODE,
        OPENCL_PLATFORM,
        OPENCL_DEVICE,
        DEPLOYMENT_CONTAINER,
        CLASS,
        CLASS_INSTANCE,
        FUNCTION,
        TRANSITION_FUNCTION,

        ATTRIBUTE_IMPL,
        BRANCH,
        IF_STATEMENT,
        IF_CONDITION,
        ELSEIF_CONDITION,
        ELSE_CONDITION,
        COMPONENT_IMPL,
        PORT_SUBSCRIBER_IMPL,
        PORT_PUBLISHER_IMPL,
        INPUT_PARAMETER,
        RETURN_PARAMETER,
        VARIABLE_PARAMETER,
        VARIADIC_PARAMETER,
        PORT_PERIODIC,
        FUNCTION_CALL,
        TERMINATION,
        VARIABLE,
        WHILE_LOOP,
        FOR_LOOP,
        CONDITION,
        CODE,
        HEADER,
        FOR_CONDITION,
        VECTOR_ITTERATOR_CONDITION,
        SETTER,
        LOGGINGSERVER,
        LOGGINGPROFILE,
        DEPLOYMENT_ATTRIBUTE,
        EXTERNAL_TYPE,
        BOOLEAN_EXPRESSION,

        QOS_DDS_PROFILE,
        QOS_DDS_POLICY_DEADLINE,
        QOS_DDS_POLICY_DESTINATIONORDER,
        QOS_DDS_POLICY_DURABILITY,
        QOS_DDS_POLICY_DURABILITYSERVICE,
        QOS_DDS_POLICY_ENTITYFACTORY,
        QOS_DDS_POLICY_GROUPDATA,
        QOS_DDS_POLICY_HISTORY,
        QOS_DDS_POLICY_LATENCYBUDGET,
        QOS_DDS_POLICY_LIFESPAN,
        QOS_DDS_POLICY_LIVELINESS,
        QOS_DDS_POLICY_OWNERSHIP,
        QOS_DDS_POLICY_OWNERSHIPSTRENGTH,
        QOS_DDS_POLICY_PARTITION,
        QOS_DDS_POLICY_PRESENTATION,
        QOS_DDS_POLICY_READERDATALIFECYCLE,
        QOS_DDS_POLICY_RELIABILITY,
        QOS_DDS_POLICY_RESOURCELIMITS,
        QOS_DDS_POLICY_TIMEBASEDFILTER,
        QOS_DDS_POLICY_TOPICDATA,
        QOS_DDS_POLICY_TRANSPORTPRIORITY,
        QOS_DDS_POLICY_USERDATA,
        QOS_DDS_POLICY_WRITERDATALIFECYCLE,

        EXTERNAL_PUBSUB_DELEGATE,
        SHARED_DATATYPES,
        PORT_REQUEST_DELEGATE,
        PORT_PUBSUB_DELEGATE,
        EXTERNAL_SERVER_DELEGATE
    };
inline uint qHash(NODE_KIND key, uint seed)
    {
        return ::qHash(static_cast<uint>(key), seed);
    }
Q_DECLARE_METATYPE(NODE_KIND);




//Enum for Node Types
enum class NODE_TYPE{
    DEFINITION              = 1 << 0,   //bin. 0000 0000 0000 0000 0000 0000 0000 0001
    INSTANCE                = 1 << 1,   //bin. 0000 0000 0000 0000 0000 0000 0000 0010
    IMPLEMENTATION          = 1 << 2,   //bin. 0000 0000 0000 0000 0000 0000 0000 0100
    ASPECT                  = 1 << 3,   //bin. 0000 0000 0000 0000 0000 0000 0000 1000
    BEHAVIOUR               = 1 << 4,   //bin. 0000 0000 0000 0000 0000 0000 0001 0000
    DATA                    = 1 << 5,   //bin. 0000 0000 0000 0000 0000 0000 0010 0000
    EVENTPORT               = 1 << 6,   //bin. 0000 0000 0000 0000 0000 0000 0100 0000
    EVENTPORT_ASSEMBLY      = 1 << 7,   //bin. 0000 0000 0000 0000 0000 0000 1000 0000
    HARDWARE                = 1 << 8,   //bin. 0000 0000 0000 0000 0000 0001 0000 0000
    QOS                     = 1 << 9,   //bin. 0000 0000 0000 0000 0000 0010 0000 0000
    BEHAVIOUR_ELEMENT       = 1 << 10,  //bin. 0000 0000 0000 0000 0000 0100 0000 0000
    DDS                     = 1 << 11,  //bin. 0000 0000 0000 0000 0000 1000 0000 0000
    PARAMETER               = 1 << 12,  //bin. 0000 0000 0000 0000 0001 0000 0000 0000
    CONDITION               = 1 << 13,  //bin. 0000 0000 0000 0000 0010 0000 0000 0000
    LOGGING                 = 1 << 14,  //bin. 0000 0000 0000 0000 0100 0000 0000 0000
    BEHAVIOUR_CONTAINER     = 1 << 15,  //bin. 0000 0000 0000 0000 0100 0000 0000 0000
    TOP_BEHAVIOUR_CONTAINER = 1 << 16,  //bin. 0000 0000 0000 0000 1000 0000 0000 0000
};

inline uint qHash(NODE_TYPE key, uint seed)
    {
        return ::qHash(static_cast<uint>(key), seed);
    }

Q_DECLARE_METATYPE(NODE_TYPE);


enum class VIEW_ASPECT{
    NONE,
    INTERFACES,
    BEHAVIOUR,
    ASSEMBLIES,
    HARDWARE,
    WORKERS
};

inline QString getViewAspectName(VIEW_ASPECT aspect){
    switch(aspect){
        case VIEW_ASPECT::INTERFACES:
            return "Interfaces";
        case VIEW_ASPECT::BEHAVIOUR:
            return "Behaviour";
        case VIEW_ASPECT::ASSEMBLIES:
            return "Assemblies";
        case VIEW_ASPECT::HARDWARE:
            return "Hardware";    
        case VIEW_ASPECT::WORKERS:
            return "Workers";    
        default:
            return "None";
    }
};

  
inline uint qHash(VIEW_ASPECT key, uint seed)
{
    return ::qHash(static_cast<uint>(key), seed);
}
Q_DECLARE_METATYPE(VIEW_ASPECT);
#endif