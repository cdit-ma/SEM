#ifndef NOTIFICATIONTOOLBAR_H
#define NOTIFICATIONTOOLBAR_H

#include "../../enumerations.h"
#include "../../Controllers/ViewController/viewcontroller.h"
#include "../../Controllers/NotificationManager/notificationmanager.h"
#include "../../Controllers/NotificationManager/notificationenumerations.h"

#include <QToolBar>
#include <QLabel>
#include <QMovie>


class NotificationToolbar : public QToolBar
{
    Q_OBJECT
public:
    explicit NotificationToolbar(QWidget *parent = 0);

signals:
    void toggleDialog();
    void showLastNotification();
public slots:
    void themeChanged();

    void notificationReceived();
    void notificationsSeen();

    void lastNotificationDeleted();

    void displayLoadingGif(bool show = true);
    void updateIconFrame(int);
    void updateSeverityCount(Notification::Severity severity, int count);

private:
    void setupLayout();
    void initialiseToolbar();
    void updateButtonIcon();

    QAction* toggleNotificationsDialog;
    QAction* showMostRecentAction;

    QIcon defaultIcon;
    QIcon notificationIcon;
    QMovie* loadingGif;
    bool loadingGifDisplayed;

    QHash<Notification::Severity, QLabel*> severityCount;

};

#endif // NOTIFICATIONTOOLBAR_H
