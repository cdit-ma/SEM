#ifndef ACTIONGROUP_H
#define ACTIONGROUP_H

#include <QObject>
#include <QActionGroup>

class ActionGroup : public QActionGroup
{
    Q_OBJECT
public:
    ActionGroup(QObject *parent = 0);

    void addSeperator();
    void updateSpacers();
};

#endif // ACTIONGROUP_H
