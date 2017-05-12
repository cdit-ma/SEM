#ifndef NOTIFICATIONTOOLBAR_H
#define NOTIFICATIONTOOLBAR_H

#include "../../enumerations.h"
#include "../../Controllers/ViewController/viewcontroller.h"
#include "../../Controllers/NotificationManager/notificationmanager.h"

#include <QToolBar>
#include <QLabel>
#include <QMovie>


class NotificationToolbar : public QToolBar
{
    Q_OBJECT
public:
    explicit NotificationToolbar(ViewController* vc, QWidget *parent = 0);

signals:
    void toggleDialog();

public slots:
    void themeChanged();

    void notificationReceived();
    void notificationsSeen();

    void lastNotificationDeleted();

    void displayLoadingGif(bool show = true);
    void updateIconFrame(int);
    void updateSeverityCount(NOTIFICATION_SEVERITY severity, int count);

private:
    void setupLayout();
    void updateButtonIcon();

    QAction* toggleNotificationsDialog;
    QAction* showMostRecentAction;

    QIcon defaultIcon;
    QIcon notificationIcon;
    QMovie* loadingGif;
    bool loadingGifDisplayed;

    QHash<NOTIFICATION_SEVERITY, QLabel*> severityCount;

};

#endif // NOTIFICATIONTOOLBAR_H
