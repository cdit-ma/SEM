#ifndef NOTIFICATIONDIALOG_H
#define NOTIFICATIONDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QListWidgetItem>
#include <QToolButton>
#include <QComboBox>
#include <QActionGroup>
#include <QSignalMapper>
#include <QToolBar>
#include "enumerations.h"

class NotificationDialog : public QDialog
{
    enum ITEM_ROLES{
        IR_TYPE = Qt::UserRole + 1,
        IR_ICONPATH = Qt::UserRole + 2,
        IR_ICONNAME = Qt::UserRole + 3,
        IR_ID = Qt::UserRole + 4
    };

    Q_OBJECT
public:
    explicit NotificationDialog(QWidget *parent = 0);

    void addNotificationItem(NOTIFICATION_TYPE type, QString title, QString description, QPair<QString, QString> iconPath, int ID);
signals:
    void notificationAdded();
    void centerOn(int ID);
public slots:
    void toggleVisibility();
private slots:
    void themeChanged();

    void typeActionToggled(int type);

    void clearAll();
    void clearVisible();



    void notificationItemClicked(QListWidgetItem* item);

private:
    void updateTypeAction(NOTIFICATION_TYPE type);
    void setupLayout();

    void removeItem(QListWidgetItem* item);

    QAction* getTypeAction(NOTIFICATION_TYPE type) const;
    QPair<QString, QString> getActionIcon(NOTIFICATION_TYPE type) const;

    QSignalMapper* typeActionMapper;
    QListWidget* listWidget;
    QToolBar* toolbar;

    QAction* clearAllAction;
    QAction* clearVisibleAction;

    QMultiMap<NOTIFICATION_TYPE, QListWidgetItem*> notificationHash;
    QHash<NOTIFICATION_TYPE, QAction*> typeActionHash;
};

#endif // NOTIFICATIONDIALOG_H
