#ifndef ACTIONGROUP_H
#define ACTIONGROUP_H

#include <QActionGroup>

class ActionGroup : public QActionGroup
{
    Q_OBJECT
    
public:
    explicit ActionGroup(QObject *parent = nullptr);

    void addSeparator();
    void updateSpacers();
};

// NOTE: After deleting updateMasterAction(), interesting how the following connect doesn't spit out a compile or
//  build error even after a clean build and clearing the cmake cache
//  connect(a, SIGNAL(changed()), this, SLOT(updateMasterAction()));

#endif // ACTIONGROUP_H