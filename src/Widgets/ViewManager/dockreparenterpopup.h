#ifndef DOCKREPARENTERPOPUP_H
#define DOCKREPARENTERPOPUP_H

#include <QHash>
#include <QLabel>
#include <QToolBar>
#include <QHBoxLayout>
#include <QToolButton>

#include "../../Widgets/Dialogs/popupwidget.h"

class BaseDockWidget;
class BaseWindow;

class DockReparenterPopup : public PopupWidget
{
    Q_OBJECT
public:
    explicit DockReparenterPopup();

    bool ReparentDockWidget(BaseDockWidget* dock_widget);

protected:
    void reject();
private:
    void windowTriggered(int window_id);

    QToolButton* getWindowAction(BaseWindow* window);
    void themeChanged();
    void setupLayout(); 

    BaseDockWidget* dock_widget = 0;

    QAction* close_action = 0;
    QHash<int, QToolButton*> button_lookup;
    QHBoxLayout* h_layout = 0;
    QToolBar* toolbar = 0;
};

#endif //DOCKREPARENTERPOPUP_H