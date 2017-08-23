#ifndef NOTIFICATIONITEM_H
#define NOTIFICATIONITEM_H

#include <QFrame>
#include <QLabel>
#include <QAction>

#include "../../Controllers/NotificationManager/notificationenumerations.h"

class NotificationObject;
class NotificationItem : public QFrame
{
    Q_OBJECT
public:
    explicit NotificationItem(NotificationObject* obj, QWidget* parent = 0);
    ~NotificationItem();

    int getID();
    int getEntityID();
    NotificationObject* getNotification() const;
    void setSelected(bool select);
signals:
    void highlightEntity(int entityID, bool highlight);
    void itemClicked(NotificationItem* item);
private slots:
    void themeChanged();
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
