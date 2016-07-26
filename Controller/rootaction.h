#ifndef ROOTACTION_H
#define ROOTACTION_H

#include <QAction>
class RootAction : public QAction
{
    Q_OBJECT
public:
    RootAction(QString text, QObject* parent = 0);

    QAction* getStealthAction();
private slots:
    void actionChanged();
private:
    QAction* stealthAction;
};

#endif // ROOTACTION_H
