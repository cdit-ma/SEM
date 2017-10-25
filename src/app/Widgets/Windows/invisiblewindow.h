#ifndef INVISIBLEWINDOW_H
#define INVISIBLEWINDOW_H

#include "viewwindow.h"
#include <QMenu>
class InvisibleWindow : public BaseWindow
{
    Q_OBJECT
    friend class WindowManager;
protected:
    InvisibleWindow(BaseWindow* parent_window);
private:
    void updateVisibility();
public:
    QMenu* createPopupMenu();
private:
    BaseWindow* parent_window = 0;
};

#endif // INVISIBLEWINDOW_H
