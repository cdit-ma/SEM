#ifndef EDGEKINDS_H
#define EDGEKINDS_H
#include <QObject>

enum class EDGE_KIND{
    NONE,
    DEFINITION,
    AGGREGATE,
    DEPLOYMENT,
    DATA,
    ASSEMBLY,
    QOS,
    TRIGGER
};

enum class EDGE_DIRECTION{
    SOURCE,
    TARGET
};

inline uint qHash(EDGE_KIND key, uint seed)
{
    return ::qHash(static_cast<uint>(key), seed);
};

inline uint qHash(EDGE_DIRECTION key, uint seed)
{
    return ::qHash(static_cast<uint>(key), seed);
};


Q_DECLARE_METATYPE(EDGE_KIND);
Q_DECLARE_METATYPE(EDGE_DIRECTION);

#endif