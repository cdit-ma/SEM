#ifndef ACTIONGROUP_H
#define ACTIONGROUP_H

#include <QActionGroup>

class RootAction;
class ActionGroup : public QActionGroup
{
    Q_OBJECT
    
public:
    ActionGroup(QObject *parent = nullptr);

    void addSeparator();
    void updateSpacers();
    
    // NOTE: These are non-virtual functions
    // QActionGroup's implementations are called within the function
    QAction* addAction(QAction *a);
    void removeAction(QAction* a);

private slots:
    void updateMasterAction();
    void actionToggled(bool toggled);

private:
    RootAction* masterAction = nullptr;
    int checkedActionCount = 0;
};

#endif // ACTIONGROUP_H
