#ifndef INVISIBLEDOCKWIDGET_H
#define INVISIBLEDOCKWIDGET_H

#include "basedockwidget.h"
class InvisibleDockWidget : public BaseDockWidget
{
    friend class WindowManager;
    Q_OBJECT
protected:
    InvisibleDockWidget(QString title);
    ~InvisibleDockWidget();
    void themeChanged();
};

#endif // INVISIBLEDOCKWIDGET_H
