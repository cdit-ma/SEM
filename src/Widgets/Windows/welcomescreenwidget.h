#ifndef WELCOMESCREENWIDGET_H
#define WELCOMESCREENWIDGET_H

#include "../../Controllers/ActionController/actioncontroller.h"

#include <QWidget>
#include <QToolButton>
#include <QToolBar>

class WelcomeScreenWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WelcomeScreenWidget(ActionController *ac, QWidget *parent = 0);

signals:
    void actionTriggered(QAction*);

private slots:
    void themeChanged();
    void recentProjectsUpdated();
private:
    ActionController* actionController;
    QToolButton* recentProjectsLabel;
    QToolBar* recentProjectsToolbar;
};

#endif // WELCOMESCREENWIDGET_H
