#ifndef EDGEKINDS_H
#define EDGEKINDS_H

enum class EDGE_KIND{
    NONE,
    DEFINITION,
    AGGREGATE,
    WORKFLOW,
    ASSEMBLY,
    DATA,
    DEPLOYMENT,
    QOS
};

inline uint qHash(EDGE_KIND key, uint seed)
{
    return ::qHash(static_cast<uint>(key), seed);
};


Q_DECLARE_METATYPE(EDGE_KIND);
#endif