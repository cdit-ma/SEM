#ifndef ACTIONBUTTON_H
#define ACTIONBUTTON_H

#include <QToolButton>
#include <QAction>

class ActionButton : public QToolButton
{
    Q_OBJECT
public:
    explicit ActionButton(QAction* action);

signals:

public slots:
    void actionChanged();
private:
    QAction* action;
};

#endif // ACTIONBUTTON_H
