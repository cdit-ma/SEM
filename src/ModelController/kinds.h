#ifndef KINDS_H
#define KINDS_H

enum class GRAPHML_KIND
    {
        NONE,
        DATA,
        KEY,
        NODE,
        EDGE,
    };
Q_DECLARE_METATYPE(GRAPHML_KIND)

#endif