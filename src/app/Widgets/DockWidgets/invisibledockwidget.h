#ifndef INVISIBLEDOCKWIDGET_H
#define INVISIBLEDOCKWIDGET_H

#include "basedockwidget.h"
#include "../Windows/basewindow.h"

class InvisibleDockWidget : public BaseDockWidget
{
    friend class WindowManager;
    Q_OBJECT

protected:
    InvisibleDockWidget(QString title, QWidget* parent = nullptr);
};

#endif // INVISIBLEDOCKWIDGET_H
