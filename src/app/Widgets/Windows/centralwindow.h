#ifndef CENTRALWINDOW_H
#define CENTRALWINDOW_H

#include "viewwindow.h"
#include <QMenu>

class CentralWindow : public ViewWindow
{
    Q_OBJECT
    friend class WindowManager;
protected:
    CentralWindow(BaseWindow* parent_window);
    ~CentralWindow();
public:
    QMenu* createPopupMenu();
private:
    BaseWindow* parent_window = 0;
};

#endif // CENTRALWINDOW_H
