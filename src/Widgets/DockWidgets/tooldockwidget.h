#ifndef TOOLDOCKWIDGET_H
#define TOOLDOCKWIDGET_H

#include "basedockwidget.h"
class ToolDockWidget : public BaseDockWidget
{
    friend class WindowManager;
    Q_OBJECT
protected:
    ToolDockWidget(QString title);
    ~ToolDockWidget();
    void themeChanged();
};

#endif // TOOLDOCKWIDGET_H
