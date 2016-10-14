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
    void removeNotificationItem(int ID);

signals:
    void centerOn(int ID);
    void itemDeleted(int ID);

public slots:
    void toggleVisibility();
    void resetDialog();

private slots:
    void themeChanged();
    void listSelectionChanged();

    void typeActionToggled(int type);

    void clearSelected();
    void clearVisible();

    void notificationItemClicked(QListWidgetItem* item);

private:
    void setupLayout();
    void updateVisibilityCount(int val, bool set = false);

    void removeItem(QListWidgetItem* item);
    void clearAll();

    void updateTypeActions(QList<NOTIFICATION_TYPE> types);
    void updateTypeAction(NOTIFICATION_TYPE type);

    QAction* getTypeAction(NOTIFICATION_TYPE type) const;
    QPair<QString, QString> getActionIcon(NOTIFICATION_TYPE type) const;

    QSignalMapper* typeActionMapper;
    QListWidget* listWidget;
    QToolBar* toolbar;

    QAction* clearSelectedAction;
    QAction* clearVisibleAction;

    QMultiMap<NOTIFICATION_TYPE, QListWidgetItem*> notificationHash;
    QHash<int, QListWidgetItem*> notificationIDHash;
    QHash<NOTIFICATION_TYPE, QAction*> typeActionHash;

    int visibleCount;
};

#endif // NOTIFICATIONDIALOG_H
