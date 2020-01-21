#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QScrollArea>
#include <QLabel>
#include <QToolBar>
#include <QVBoxLayout>
#include <QSplitter>
#include <QSet>

#include "../../Controllers/ViewController/viewitem.h"
#include "searchitemwidget.h"
#include "../../Widgets/optiongroupbox.h"

class SearchDialog : public QFrame
{
    Q_OBJECT
public:
    explicit SearchDialog(QWidget *parent = nullptr);

    void DisplaySearchResults(const QString& query, const QHash<QString, ViewItem*>& results);

signals:
    void SearchQuery(QString query);
    void SearchPopup();
    void FlashEntity(int ID);
    void CenterOn(int ID);
    void Popup(int ID);

private slots:
    void themeChanged();
    void filtersChanged();

public slots:
    void viewItemDestructed(int ID, ViewItem* item);
    void viewItemSelected(ViewItem* item, bool selected);

    void searchItemClicked(int ID);

private:
    void scrollBarValueChanged();
    void loadNextResults();

    void setQuery(const QString& query);

    void setupLayout();
    void setupFilters();

    void clearSearchItems();

    SearchItemWidget* constructSearchItem(ViewItem* item);
    SearchItemWidget* getSearchItem(ViewItem* item);

    QLabel* search_label = nullptr;
    QLabel* query_label = nullptr;
    
    QLabel* info_label = nullptr;
    QLabel* status_label = nullptr;

    QWidget* filters_widget = nullptr;
    QWidget* results_widget = nullptr;
    QWidget* result_status_widget = nullptr;
    QVBoxLayout* results_layout = nullptr;
    QVBoxLayout* filters_layout = nullptr;

    QToolBar* top_toolbar = nullptr;
    QToolBar* bottom_toolbar = nullptr;

    QToolButton* load_more_button = nullptr;
    QAction* center_action = nullptr;
    QAction* popup_action = nullptr;
    QAction* search_action = nullptr;
    QAction* refresh_action = nullptr;
    QAction* reset_filters_action = nullptr;
    
    QSplitter* splitter = nullptr;

    QScrollArea* filters_scroll = nullptr;
    QScrollArea* results_scroll = nullptr;

    OptionGroupBox* aspect_filters = nullptr;
    OptionGroupBox* data_filters = nullptr;

    QHash<int, SearchItemWidget*> search_items;
    QHash<ViewItem*, QString> search_key_lookups;

    int current_visible = 0;
    int filtered_match_count = 0;
    int max_visible = 0;

    int selected_id = -1;
    QString query_text;
};

#endif // SEARCHDIALOG_H
