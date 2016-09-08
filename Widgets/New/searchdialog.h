#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QToolBar>
#include <QToolButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QActionGroup>

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

    void constructKeyButton(QString key, bool checked = false);

    QLabel* queryLabel;
    QLabel* scopeLabel;

    QLineEdit* searchLineEdit;
    QToolButton* searchButton;
    QComboBox* scopeComboBox;

    QToolBar* keysToolBar;
    QVBoxLayout* keysLayout;
    QActionGroup* keysActionGroup;

    int maxToolButtonWidth;
};

#endif // SEARCHDIALOG_H
