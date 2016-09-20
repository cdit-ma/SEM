#ifndef NOTIFICATIONDIALOG_H
#define NOTIFICATIONDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QListWidgetItem>
#include <QToolButton>
#include <QComboBox>
#include <QActionGroup>

#include "enumerations.h"

class NotificationDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NotificationDialog(QWidget *parent = 0);

signals:
    void notificationAdded();

public slots:
    void themeChanged();
    void displayTypeChanged(int type);

    void clearAll();
    void clearSelected();

    void addNotificationItem(NOTIFICATION_TYPE type, QString title, QString description, QPair<QString, QString> iconPath);

    void listItemClicked(QListWidgetItem* item);

private:
    void addListAction(QString description, NOTIFICATION_TYPE type = NT_INFO);
    void addListItem(NOTIFICATION_TYPE type, QIcon icon, QString title, QString description);
    void removeListItem(QListWidgetItem* item);

    QToolButton* clearSelectedButton;
    QToolButton* clearAllButton;

    QComboBox* typeComboBox;

    QListWidget* listWidget;
    QListWidget* typeIconListWidget;

    QActionGroup* infoActionGroup;
    QActionGroup* warningActionGroup;
    QActionGroup* criticalActionGroup;

    QHash<QListWidgetItem*, QPair<QString, QString>> icons;

};

#endif // NOTIFICATIONDIALOG_H
