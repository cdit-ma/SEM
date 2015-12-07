#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include "searchitem.h"

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QLabel>
#include <QMenu>
#include <QWidgetAction>
#include <QRadioButton>
#include <QCheckBox>

class SearchDialog : public QDialog
{
    Q_OBJECT

public:
    SearchDialog(QSize minimumSize, QWidget *parent);

    void insertSearchItem(SearchItem* searchItem);
    void clear();

signals:
    void searchDialog_clickToCenter(bool b);
    void searchDialog_doubleClickToExpand(bool b);

private slots:
    void sortItems();

private:
    void setupMenus(QLayout *layout);

    QVBoxLayout* resultsLayout;
    QList<SearchItem*> searchItems;

    QMenu* sortMenu;
    QMenu* settingsMenu;
};

#endif // SEARCHDIALOG_H
