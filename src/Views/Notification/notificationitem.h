#ifndef NOTIFICATIONITEM_H
#define NOTIFICATIONITEM_H

#include <QFrame>
#include <QLabel>
#include <QAction>
#include <QToolBar>

#include "../../Controllers/NotificationManager/notificationenumerations.h"

class NotificationObject;
class NotificationItem : public QFrame
{
    Q_OBJECT
public:
    explicit NotificationItem(QSharedPointer<NotificationObject> obj, QWidget* parent = 0);
    ~NotificationItem();

    int getID();
    int getEntityID();
    QSharedPointer<NotificationObject> getNotification() const;
    void setSelected(bool select);
signals:
    void highlightEntity(int entityID, bool highlight);
    void itemClicked(NotificationItem* item);
private slots:
    void themeChanged();
    void descriptionChanged();
    void updateIcon();
    void timeChanged();
protected:
    void mouseReleaseEvent(QMouseEvent* event);

private:
    void setupLayout();
    void updateStyleSheet();
    void updateVisibility(bool filterMatched);

    QSharedPointer<NotificationObject> notification;

    QLabel* label_icon = 0;
    QLabel* label_text = 0;
    QLabel* label_time = 0;
    QAction* action_delete = 0;
    QToolBar* toolbar = 0;
    

    QString backgroundColor;
    bool selected = false;
};


#endif // NOTIFICATIONITEM_H
