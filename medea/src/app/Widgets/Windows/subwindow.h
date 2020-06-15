#ifndef SUBWINDOW_H
#define SUBWINDOW_H

#include "viewwindow.h"

class SubWindow : public ViewWindow
{
    Q_OBJECT
    friend class WindowManager;

protected:
    explicit SubWindow(BaseWindow* parent_window);
};

#endif // SUBWINDOW_H