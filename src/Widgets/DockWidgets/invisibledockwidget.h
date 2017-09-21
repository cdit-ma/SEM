#ifndef INVISIBLEDOCKWIDGET_H
#define INVISIBLEDOCKWIDGET_H

#include "basedockwidget.h"
#include "../Windows/basewindow.h"

class InvisibleDockWidget : public BaseDockWidget
{
    friend class WindowManager;
    Q_OBJECT

protected:
    InvisibleDockWidget(QString title);
    ~InvisibleDockWidget();
    void themeChanged();

public slots:
    void connectChildDockWidget(BaseDockWidget* dockWidget);
    void childDockWidgetVisibilityChanged(bool visible);

    // BaseDockWidget interface
public:
    void setWidget(QWidget *widget);

private:
    BaseWindow* window = 0;
};

#endif // INVISIBLEDOCKWIDGET_H
