#ifndef CENTRALWINDOW_H
#define CENTRALWINDOW_H

#include "viewwindow.h"
class CentralWindow : public ViewWindow
{
    Q_OBJECT
    friend class WindowManager;
protected:
    CentralWindow();
    ~CentralWindow();
};

#endif // CENTRALWINDOW_H
