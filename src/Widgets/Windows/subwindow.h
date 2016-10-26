#ifndef SUBWINDOW_H
#define SUBWINDOW_H

#include "viewwindow.h"
class SubWindow : public ViewWindow
{
    Q_OBJECT
    friend class WindowManager;
protected:
    SubWindow();
    ~SubWindow();
};

#endif // SUBWINDOW_H
