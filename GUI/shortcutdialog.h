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
    void addShortcut(QString shortcut, QString description, QString alias="", QString image="");
    void addTitle(QString label, QString alias="", QString image="");
    void resizeTable();

private slots:
    void themeChanged();
private:
    void setupLayout();

    QTableWidget* tableWidget;
    QVBoxLayout* layout;

};

#endif // SHORTCUTDIALOG_H
