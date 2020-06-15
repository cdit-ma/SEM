#ifndef NOTIFICATIONTOOLBAR_H
#define NOTIFICATIONTOOLBAR_H

#include "../../Controllers/NotificationManager/notificationenumerations.h"

#include <QToolBar>
#include <QLabel>
#include <QMovie>

class NotificationToolbar : public QToolBar
{
    Q_OBJECT
    
public:
    explicit NotificationToolbar(QWidget *parent = nullptr);
    
signals:
    void showSeverity(Notification::Severity severity);
    void showPanel();
    
private slots:
    void themeChanged();
    void notificationAdded();
    void notificationsSeen();
    void loadingGifTicked();
    
private:
    void setLoadingMode(bool loading);

    void updateCount();
    void setupLayout();
    void updateButtonIcon();

    const int max_count = 99;
    
    QAction* show_most_recent_action = nullptr;
    QMovie* loading_gif = nullptr;
    QIcon default_icon;
    
    QHash<Notification::Severity, QAction*> severity_actions;
};

#endif // NOTIFICATIONTOOLBAR_H