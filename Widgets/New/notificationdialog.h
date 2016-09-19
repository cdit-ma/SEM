#ifndef NOTIFICATIONDIALOG_H
#define NOTIFICATIONDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QListWidgetItem>
#include <QToolButton>
#include <QComboBox>
#include <QActionGroup>

class NotificationDialog : public QDialog
{
    Q_OBJECT
public:
    enum NOTIFICATION_TYPE{INFO, WARNING, CRITICAL};
    explicit NotificationDialog(QWidget *parent = 0);

    void addListItem(QString description, NOTIFICATION_TYPE type = INFO);
    void addListAction(QString description, NOTIFICATION_TYPE type = INFO);

signals:

public slots:
    void themeChanged();
    void displayTypeChanged(int type);

    void clearAll();
    void removeListItem(QListWidgetItem* item);

    void listItemClicked(QListWidgetItem* item);

private:
    QToolButton* clearButton;
    QComboBox* typeComboBox;
    QListWidget* listWidget;
    QListWidget* closeButtonListWidget;

    QActionGroup* infoActionGroup;
    QActionGroup* warningActionGroup;
    QActionGroup* criticalActionGroup;

    QMap<QActionGroup*, QListWidgetItem*> closeButtons;
};

#endif // NOTIFICATIONDIALOG_H
