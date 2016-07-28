#ifndef ACTIONGROUP_H
#define ACTIONGROUP_H

#include <QObject>
#include <QActionGroup>
//#include "../../Controller/rootaction.h"
class RootAction;

class ActionGroup : public QActionGroup
{
    Q_OBJECT
public:
    ActionGroup(QObject *parent = 0);

    RootAction* getGroupVisibilityAction();

    QList<RootAction*> getRootActions();
    void addSeperator();
    void updateSpacers();
    void addAction(QAction *a);

    void removeAction(QAction* a);
private slots:
    void updateMasterAction();
private:
    RootAction* masterAction;
};

#endif // ACTIONGROUP_H
