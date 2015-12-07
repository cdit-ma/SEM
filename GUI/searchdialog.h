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

    void updateHedearLabels(QString search, QStringList aspects, QStringList kinds);

signals:
    void searchDialog_refresh();
    void searchDialog_clickToCenter(bool b);
    void searchDialog_doubleClickToExpand(bool b);

public slots:
    void show();

private slots:
    void sortItems();

private:
    QLabel* constructHeadearLabel(QString labelText, QVBoxLayout *vLayout, int fixedWidth);
    void setupMenus(QLayout *layout);

    QVBoxLayout* resultsLayout;
    QList<SearchItem*> searchItems;

    QLabel* searchLabel;
    QLabel* aspectsLabel;
    QLabel* kindsLabel;
    QLabel* notFoundLabel;

    QMenu* sortMenu;
    QMenu* settingsMenu;

    QFont defaultFont;
    QFont searchFont;

    int MAX_ITEM_WIDTH;
};

#endif // SEARCHDIALOG_H
