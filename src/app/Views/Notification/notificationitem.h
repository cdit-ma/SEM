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

    int getID();
    int getEntityID();

    bool isSelected();
    void setSelected(bool selected);

signals:
    void notificationItemClicked(NotificationItem* item);
    void flashEntityItem(int ID);
    void centerEntityItem(int ID);
    void selectAndCenterEntityItem(int ID);

private slots:
    void themeChanged();
    void descriptionChanged();
    void titleChanged();
    void timeChanged();
    void updateIcon();

protected:
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);

private:
    void toggleSelected();
    void setupDescriptionLayout();
    void setupLayout();
    void updateStyleSheet();
    void updateVisibility(bool filterMatched);

    QSharedPointer<NotificationObject> notification;

    QLabel* label_icon = 0;
    QLabel* label_text = 0;
    QLabel* label_time = 0;
    QLabel* label_description = 0;

    QAction* action_delete = 0;
    QToolBar* toolbar = 0;
    
    QString backgroundColor_;
    bool selected_ = false;
    bool doubleClicked_ = false;
};


#endif // NOTIFICATIONITEM_H
