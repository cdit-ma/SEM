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

    bool eventFilter(QObject* object, QEvent* event) override;

    QSharedPointer<NotificationObject> current_notification;

    QLabel* icon = nullptr;
    QLabel* label = nullptr;

    QTimer* timer = nullptr;
    QWidget* widget = nullptr;
};

#endif //NOTIFICATIONPOPUP_H