#ifndef SHORTCUTDIALOG_H
#define SHORTCUTDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
class ShortcutDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ShortcutDialog(QWidget *parent = 0);
    void addShortcut(QString shortcut, QString description="");
    void addTitle(QString label, QIcon icon);

private:
    void setupLayout();

    QTableWidget* tableWidget;
    QVBoxLayout* layout;

};

#endif // SHORTCUTDIALOG_H
