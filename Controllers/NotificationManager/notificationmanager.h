#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include "enumerations.h"
#include "../ViewController/viewcontroller.h"
#include "../ActionController/actioncontroller.h"
#include "../../Widgets/Dialogs/notificationdialog.h"

#include <QObject>
#include <QWidget>
#include <QToolBar>
#include <QToolButton>
#include <QLabel>

class NotificationManager : public QObject
{
    Q_OBJECT
public:
    explicit NotificationManager(ViewController* vc, QWidget *parent = 0);
    QToolBar* getNotificationWidget();

signals:
    void notificationAdded(QString iconPath, QString iconName, QString description);

public slots:
    void themeChanged();

    void notificationReceived(NOTIFICATION_TYPE type, QString title, QString description, QString iconPath, QString iconName, int ID);
    void notificationsSeen();

    void showLastNotification();
    void deleteNotification(int ID);

    void displayLoadingGif(bool show);
    void updateIconFrame(int);
    void updateButtonIcon();
    void updateTypeCount(NOTIFICATION_TYPE type, int count);

private:
    void constructNotificationWidget();

    struct Notification {
        NOTIFICATION_TYPE type;
        QString title;
        QString description;
        QString iconPath;
        QString iconName;
        int entityID;
        int ID;
    };

    static int _NotificationID;

    ViewController* viewController;

    QWidget* parentWidget;
    RootAction* toggleNotificationsDialog;
    NotificationDialog* notificationDialog;

    QToolBar* notificationWidget;
    QToolButton* showLastNotificationButton;
    QAction* showLastNotificationAction;

    QIcon defaultIcon;
    QIcon notificationIcon;
    QMovie* loadingGif;
    bool loadingGifDisplayed;

    Notification lastNotification;

    QHash<int, Notification> notifications;
    QHash<NOTIFICATION_TYPE, QLabel*> typeCount;

};

#endif // NOTIFICATIONMANAGER_H
