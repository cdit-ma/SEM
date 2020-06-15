#ifndef TOOLDOCKWIDGET_H
#define TOOLDOCKWIDGET_H

#include "basedockwidget.h"

class ToolDockWidget : public BaseDockWidget
{
    friend class WindowManager;
    Q_OBJECT

protected:
    explicit ToolDockWidget(const QString& title, QWidget* parent = nullptr);

    void themeChanged() override;
};

#endif // TOOLDOCKWIDGET_H