#ifndef INVISIBLEWINDOW_H
#define INVISIBLEWINDOW_H

#include "basewindow.h"
#include <QMenu>

class InvisibleWindow : public BaseWindow
{
    Q_OBJECT
    friend class WindowManager;

protected:
    explicit InvisibleWindow(BaseWindow* parent_window);

public:
    QMenu* createPopupMenu() override;

private:
    void updateVisibility();

    BaseWindow* parent_window = nullptr;
};

#endif // INVISIBLEWINDOW_H