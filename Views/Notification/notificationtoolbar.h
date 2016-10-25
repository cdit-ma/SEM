#ifndef NOTIFICATIONTOOLBAR_H
#define NOTIFICATIONTOOLBAR_H

#include "enumerations.h"
#include "../../Controllers/ViewController/viewcontroller.h"

#include <QToolBar>
#include <QToolButton>
#include <QLabel>
#include <QMovie>


class NotificationToolbar : public QToolBar
{
    Q_OBJECT
public:
    explicit NotificationToolbar(ViewController* vc, QWidget *parent = 0);

signals:
    void showMostRecentNotification();
    void toggleDialog();

public slots:
    void themeChanged();

    void notificationReceived();
    void notificationsSeen();

    void lastNotificationDeleted();

    void displayLoadingGif(bool show);
    void updateIconFrame(int);
    void updateTypeCount(NOTIFICATION_TYPE type, int count);

private:
    void setupLayout();
    void updateButtonIcon();

    QAction* toggleNotificationsDialog;
    QAction* showMostRecentAction;
    QToolButton* showMostRecentButton;

    QIcon defaultIcon;
    QIcon notificationIcon;
    QMovie* loadingGif;
    bool loadingGifDisplayed;

    QHash<NOTIFICATION_TYPE, QLabel*> typeCount;

};

#endif // NOTIFICATIONTOOLBAR_H
