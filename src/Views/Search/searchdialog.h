#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QScrollArea>
#include <QLabel>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QSplitter>
#include <QMovie>

#include "../../theme.h"
#include "../../Controllers/ViewController/viewitem.h"
#include "../../Utils/filtergroup.h"
#include "searchitemwidget.h"

class SearchDialog : public QWidget
{
    Q_OBJECT

public:
    enum SEARCH_FILTER{ASPECTS_FILTER, DATA_FILTER};

    explicit SearchDialog(QWidget *parent = 0);

    void searchResults(QString query, QMap<QString, ViewItem*> results);

signals:
    void itemHoverEnter(int ID);
    void itemHoverLeave(int ID);

    void centerOnViewItem(int ID);
    void popupViewItem(int ID);

    void searchButtonClicked();
    void refreshButtonClicked();

    void filterCleared(int filter);
    void filtersChanged(int filter, QList<QVariant> checkedKeys);

public slots:
    void on_themeChanged();
    void on_filtersChanged(QList<QVariant> checkedKeys);

    void searchItemSelected(int ID);
    void viewItemDestructed(int ID);

    void centerOnSelectedItem();
    void popupSelectedItem();

    void loading(bool on);
    void resetPanel();

private:
    void setupLayout();
    void setupFilterGroups();

    void clearSearchItems();

    SearchItemWidget* constructSearchItem(ViewItem* item);

    QLabel* queryLabel;
    QLabel* searchLabel;
    QLabel* scopeLabel;
    QLabel* infoLabel;

    QMovie* loadingGif;

    QToolButton* centerOnButton;
    QToolButton* popupButton;
    QToolButton* searchButton;
    QToolButton* refreshButton;

    QToolBar* filtersToolbar;
    QToolBar* topToolbar;
    QToolBar* bottomToolbar;
    QSplitter* displaySplitter;

    FilterGroup* aspectFilterGroup;
    FilterGroup* dataFilterGroup;
    QAction* dataGroupAction;

    QVBoxLayout* resultsLayout;
    QHash<int, SearchItemWidget*> searchItems;

    int selectedSearchItemID;
    QString queryText;
};

#endif // SEARCHDIALOG_H
