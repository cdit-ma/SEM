#ifndef TOOLBARWIDGETMENU_H
#define TOOLBARWIDGETMENU_H

#include "toolbarwidgetaction.h"

#include <QMenu>

class ToolbarWidgetMenu : public QMenu
{
    Q_OBJECT
public:
    explicit ToolbarWidgetMenu(ToolbarWidgetAction* action, QWidget *parent = 0);

    QAction* exec();

    void addWidgetAction(ToolbarWidgetAction* action);
    QList<ToolbarWidgetAction*> getWidgetActions();

protected:
    virtual void enterEvent(QEvent*);
    virtual void leaveEvent(QEvent*);

signals:
    void closeParentMenu();

public slots:
    bool close();
    void closeMenu();
    void connectChildMenu(QMenu* menu);

private:
    ToolbarWidgetAction* parentAction;
    QList<ToolbarWidgetAction*> widgetActions;
    bool eventFromMenu;

};

#endif // TOOLBARWIDGETMENU_H
