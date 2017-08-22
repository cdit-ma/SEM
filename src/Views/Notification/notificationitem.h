#ifndef NOTIFICATIONITEM_H
#define NOTIFICATIONITEM_H

#include "../../Controllers/NotificationManager/notificationmanager.h"
#include "notificationobject.h"

#include <QFrame>
#include <QLabel>
#include <QMovie>
#include <QToolButton>
//#include <QTimer>

class NotificationItem : public QFrame
{
    Q_OBJECT
public:
    explicit NotificationItem(NotificationObject* obj, QWidget* parent = 0);

    int getID();
    int getEntityID();
    NotificationObject* getNotification() const;
    QString getIconPath();
    QString getIconName();
    NOTIFICATION_SEVERITY getSeverity();
    NOTIFICATION_TYPE getType();
    NOTIFICATION_CATEGORY getCategory();

    void setSelected(bool select);

    signals:
    void hoverEnter(int ID);
    void hoverLeave(int ID);
    void itemClicked(NotificationItem* item, bool currentState, bool controlDown);

public slots:
    void themeChanged();

private slots:
    void descriptionChanged();
    void updateIcon();
    void timestampChanged();
    void loading(bool on);

protected:
    void mouseReleaseEvent(QMouseEvent* event);
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);

private:
    void setupLayout();
    void updateStyleSheet();
    void updateVisibility(bool filterMatched);

    NotificationObject* notification = 0;

    QSize icon_size = QSize(24, 24);
    QLabel* label_icon = 0;
    QLabel* label_text = 0;
    QLabel* label_time = 0;
    QAction* action_delete = 0;
    

    QString backgroundColor;
    bool selected = false;
};


#endif // NOTIFICATIONITEM_H
