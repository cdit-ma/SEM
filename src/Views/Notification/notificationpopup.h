#ifndef NOTIFICATIONPOPUP_H
#define NOTIFICATIONPOPUP_H

#include <QLabel>
#include <QTimer>

#include "../../Widgets/Dialogs/popupwidget.h"

class NotificationObject;
class NotificationPopup : public PopupWidget
{
    Q_OBJECT
public:
    explicit NotificationPopup();
    void DisplayNotification(NotificationObject* notification);

private:
    void themeChanged();
    void setupLayout();

    QLabel* icon = 0;
    QLabel* label = 0;
    QTimer* timer = 0;
    QWidget* widget = 0;
};

#endif //NOTIFICATIONPOPUP_H
