#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>
#include <QScrollArea>
#include <QLabel>
#include <QLineEdit>
#include <QToolBar>
#include <QToolButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QActionGroup>

#include "../../View/viewitem.h"
#include "searchitemwidget.h"

class SearchDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SearchDialog(QWidget *parent = 0);

    void searchResults(QString query, QMap<QString, ViewItem*> results);

signals:
    void itemHoverEnter(int ID);
    void itemHoverLeave(int ID);
    void keyButtonChecked(QString key);
    void centerOnViewItem(int ID);

public slots:
    void themeChanged();
    void keyButtonChecked(bool checked);

private:
    void setupLayout();
    void clear();

    SearchItemWidget* constructSearchItem(ViewItem* item);
    void constructKeyButton(QString key, QString text = "", bool checked = false);

    QLabel* queryLabel;
    QLabel* scopeLabel;
    QLabel* infoLabel;

    QLineEdit* searchLineEdit;
    QToolButton* searchButton;
    QComboBox* scopeComboBox;

    QScrollArea* keysArea;
    QToolBar* keysToolBar;
    QVBoxLayout* keysLayout;
    QActionGroup* keysActionGroup;

    QVBoxLayout* resultsLayout;
    QList<SearchItemWidget*> searchItems;
};

#endif // SEARCHDIALOG_H
