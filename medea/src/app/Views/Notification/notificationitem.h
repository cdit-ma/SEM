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
    explicit NotificationItem(QSharedPointer<NotificationObject> obj, QWidget* parent = nullptr);

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
    void updateActionDeleteEnabled();

protected:
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    void toggleSelected();
    void setupDescriptionLayout();
    void setupLayout();
    void updateStyleSheet();

    QSharedPointer<NotificationObject> notification;

    QLabel* label_icon = nullptr;
    QLabel* label_text = nullptr;
    QLabel* label_time = nullptr;
    QLabel* label_description = nullptr;

    QAction* action_delete = nullptr;
    QToolBar* toolbar = nullptr;
    
    QString backgroundColor_;
    
    bool selected_ = false;
    bool doubleClicked_ = false;
};

#endif // NOTIFICATIONITEM_H