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
    void reject() final;
    
private:
    void windowTriggered(int window_id);
    void unsetDockWidget();

    QToolButton* getWindowAction(BaseWindow* window);
    
    void themeChanged();
    void setupLayout(); 

    BaseDockWidget* dock_widget = nullptr;

    QAction* close_action = nullptr;
    QHBoxLayout* h_layout = nullptr;
    QToolBar* toolbar = nullptr;

    QHash<int, QToolButton*> button_lookup;
};

#endif // DOCKREPARENTERPOPUP_H