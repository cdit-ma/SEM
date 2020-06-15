#ifndef CENTRALWINDOW_H
#define CENTRALWINDOW_H

#include "viewwindow.h"
#include <QMenu>

class CentralWindow : public ViewWindow
{
    Q_OBJECT
    friend class WindowManager;

protected:
    explicit CentralWindow(BaseWindow* parent_window);

public:
    QMenu* createPopupMenu() override;

private:
    BaseWindow* parent_window = nullptr;
};

#endif // CENTRALWINDOW_H