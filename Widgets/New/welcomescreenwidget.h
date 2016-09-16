#ifndef WELCOMESCREENWIDGET_H
#define WELCOMESCREENWIDGET_H

#include "../../Controller/actioncontroller.h"

#include <QWidget>
#include <QToolBar>

class WelcomeScreenWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WelcomeScreenWidget(ActionController *ac, QWidget *parent = 0);

signals:
    void actionTriggered(QAction*);

public slots:
    void themeChanged();
    void recentProjectsUpdated();

private:
    ActionController* actionController;
    QToolBar* recentProjectsToolbar;
};

#endif // WELCOMESCREENWIDGET_H
