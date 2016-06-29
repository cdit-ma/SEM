#ifndef EDGEITEMNEW_H
#define EDGEITEMNEW_H

#include "../edgeviewitem.h"
#include "entityitem.h"

class EdgeItemNew : public EntityItemNew{
    Q_OBJECT

public:
    enum KIND{DEFAULT};

    EdgeItemNew(EdgeViewItem* viewItem, EntityItemNew* parent, KIND edge_kind = DEFAULT);


private:
    EdgeViewItem* edgeViewItem;

};

#endif // EDGEITEMNEW_H
