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
    explicit ShortcutDialog(QWidget* parent = nullptr);

    void addShortcut(QString shortcut, const QString& description, const QString& alias = "", const QString& image = "");
    void addTitle(const QString& label, const QString& alias = "", const QString& image = "");

    void resizeTable();

private slots:
    void themeChanged();

private:
    void setupLayout();

    QTableWidget* tableWidget = nullptr;
    QVBoxLayout* layout = nullptr;
};

#endif // SHORTCUTDIALOG_H