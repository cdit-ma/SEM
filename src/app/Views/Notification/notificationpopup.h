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
    void DisplayNotification(QSharedPointer<NotificationObject> notification);
    void Hide();

private:
    void themeChanged();
    void setupLayout();
    bool eventFilter(QObject* object, QEvent* event);

    QSharedPointer<NotificationObject> current_notification;
    QLabel* icon = 0;
    QLabel* label = 0;
    QTimer* timer = 0;
    QWidget* widget = 0;
};

#endif //NOTIFICATIONPOPUP_H
