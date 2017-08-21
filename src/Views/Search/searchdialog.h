#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QScrollArea>
#include <QLabel>
#include <QToolBar>
#include <QVBoxLayout>
#include <QSplitter>


#include "../../Controllers/ViewController/viewitem.h"
#include "../../Utils/filtergroup.h"
#include "searchitemwidget.h"
#include "../../Widgets/optiongroupbox.h"

class SearchDialog : public QFrame
{
    Q_OBJECT
public:
    explicit SearchDialog(QWidget *parent = 0);

    void DisplaySearchResults(QString query, QMap<QString, ViewItem*> results);
signals:
    void itemHoverEnter(int ID);
    void itemHoverLeave(int ID);

    void SearchQuery(QString query);
    void SearchPopup();

    void CenterOn(int ID);
    void Popup(int ID);
private slots:
    void themeChanged();
    void filtersChanged();

    void searchItemSelected(int ID);
    void resetPanel();
public slots:
    void viewItemDestructed(int ID);
private:
    void setQuery(QString query);
    void setupLayout();
    void setupFilters();

    void clearSearchItems();

    SearchItemWidget* constructSearchItem(ViewItem* item);

    QLabel* search_label = 0;
    QLabel* query_label = 0;
    
    QLabel* info_label = 0;

    QWidget* filters_widget = 0;
    QWidget* results_widget = 0;
    QVBoxLayout* results_layout = 0;
    QVBoxLayout* filters_layout = 0;

    QToolBar* top_toolbar = 0;

    QAction* center_action = 0;
    QAction* popup_action = 0;
    QAction* search_action = 0;
    QAction* refresh_action = 0;
    
    QSplitter* splitter = 0;

    OptionGroupBox* aspect_filters = 0;
    OptionGroupBox* data_filters = 0;

    QHash<int, SearchItemWidget*> search_items;

    int selected_id = -1;
    QString query_text;
};

#endif // SEARCHDIALOG_H
