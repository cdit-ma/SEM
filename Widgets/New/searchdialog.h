#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>
#include <QToolButton>
#include <QVBoxLayout>

class SearchDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SearchDialog(QWidget *parent = 0);

signals:

public slots:
    void themeChanged();

private:
    void setupLayout();

    void constructKeyButton(QString key);

    QToolButton* searchButton;
    QVBoxLayout* keysLayout;
};

#endif // SEARCHDIALOG_H
