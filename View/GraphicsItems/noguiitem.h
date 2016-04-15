#ifndef NOGUIITEM_H
#define NOGUIITEM_H
#include "../../Controller/entityadapter.h"
#include <QObject>

class NoGUIItem : public QObject
{
    Q_OBJECT
public:
    NoGUIItem(EntityAdapter* entityAdapter);
    ~NoGUIItem();

    int getID();
    EntityAdapter* getEntityAdapter();
private:
    EntityAdapter* entityAdapter;
    int ID;
};

#endif // NOGUIITEM_H
