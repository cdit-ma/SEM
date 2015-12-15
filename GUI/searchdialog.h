#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include "searchitem.h"
#include "medeawindow.h"

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
    SearchDialog(QSize minimumSize, MedeaWindow* window);

    bool addSearchItems(QList<GraphMLItem*> searchResult);
    void updateHedearLabels(QString search, QStringList aspects, QStringList kinds);

signals:
    void searchDialog_refresh();
    void searchDialog_clickToCenter(bool b);
    void searchDialog_doubleClickToExpand(bool b);

public slots:
    void show();
    void returnFocus();

private slots:
    void sortItems(bool checked);

protected:
    void closeEvent(QCloseEvent* event);

private:
    QLabel* constructHeadearLabel(QString labelText, QVBoxLayout *vLayout, int fixedWidth);

    void setupLayout();
    void setupMenus(QHBoxLayout *layout);

    void addSearchItem(SearchItem* searchItem);
    void sortItems(QString sortKey = "");

    void clear();

    MedeaWindow* parentWindow;
    QVBoxLayout* resultsLayout;

    QLabel* searchLabel;
    QLabel* aspectsLabel;
    QLabel* kindsLabel;
    QLabel* notFoundLabel;

    QMenu* sortMenu;
    QMenu* settingsMenu;

    QFont defaultFont;
    QFont searchFont;

    QList<SearchItem*> searchItems;
    QList<GraphMLItem*> resultGraphmlItems;

    QString currentSortKey;

    int MAX_ITEM_WIDTH;

};

#endif // SEARCHDIALOG_H
