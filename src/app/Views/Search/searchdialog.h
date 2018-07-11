#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QScrollArea>
#include <QLabel>
#include <QToolBar>
#include <QVBoxLayout>
#include <QSplitter>
#include <QSet>

#include "../../Controllers/ViewController/viewitem.h"
#include "../../Utils/filtergroup.h"
#include "searchitemwidget.h"
#include "../../Widgets/optiongroupbox.h"

class SearchDialog : public QFrame
{
    Q_OBJECT
public:
    explicit SearchDialog(QWidget *parent = 0);

    void DisplaySearchResults(QString query, QHash<QString, ViewItem*> results);
signals:

    void SearchQuery(QString query);
    void SearchPopup();
    void HighlightEntity(int ID, bool highlight);
    void CenterOn(int ID);
    void Popup(int ID);
private slots:
    void themeChanged();
    void filtersChanged();

    void searchItemSelected(int ID);
    void resetPanel();
public slots:
    void viewItemDestructed(int ID, ViewItem* item);
private:
    void scrollBarValueChanged();
    void loadNextResults();
    void setQuery(QString query);
    void setupLayout();
    void setupFilters();

    void clearSearchItems();

    SearchItemWidget* constructSearchItem(ViewItem* item);
    SearchItemWidget* getSearchItem(ViewItem* item);

    QLabel* search_label = 0;
    QLabel* query_label = 0;
    
    QLabel* info_label = 0;
    QLabel* status_label = 0;

    QWidget* filters_widget = 0;
    QWidget* results_widget = 0;
    QWidget* result_status_widget = 0;
    QVBoxLayout* results_layout = 0;
    QVBoxLayout* filters_layout = 0;

    QToolBar* top_toolbar = 0;
    QToolBar* bottom_toolbar = 0;

    QToolButton* load_more_button = 0;
    QAction* center_action = 0;
    QAction* popup_action = 0;
    QAction* search_action = 0;
    QAction* refresh_action = 0;
    QAction* reset_filters_action = 0;
    
    QSplitter* splitter = 0;

    QScrollArea* filters_scroll = 0;
    QScrollArea* results_scroll = 0;

    OptionGroupBox* aspect_filters = 0;
    OptionGroupBox* data_filters = 0;

    QSet<int> current_search_items;
    QHash<int, SearchItemWidget*> search_items;

    QHash<ViewItem*, QString> search_key_lookups;

    int current_visible = 0;
    int filtered_match_count = 0;
    int max_visible = 0;

    int selected_id = -1;
    QString query_text;
};

#endif // SEARCHDIALOG_H
