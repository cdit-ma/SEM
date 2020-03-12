#include "searchdialog.h"
#include "../../theme.h"
#include "../../../modelcontroller/entityfactory.h"

#include <QScrollBar>

#define DEFAULT_KEY_WIDTH 150
#define DEFAULT_DISPLAY_WIDTH 300

/**
 * @brief SearchDialog::SearchDialog
 * @param parent
 */
SearchDialog::SearchDialog(QWidget *parent)
	: QFrame(parent)
{
    setupLayout();

    auto v_scrollbar = results_scroll->verticalScrollBar();
    connect(v_scrollbar, &QScrollBar::valueChanged, this, &SearchDialog::scrollBarValueChanged);
    connect(load_more_button, &QToolButton::clicked, this, &SearchDialog::loadNextResults);

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    themeChanged();
}

/**
 * @brief SearchDialog::setQuery
 * @param query
 */
void SearchDialog::setQuery(const QString &query)
{
    query_text = query;
    if (query_label) {
        query_label->setText("\"" + query_text + "\"");
    }
}

/**
 * @brief SearchDialog::searchResults
 * @param query
 * @param results
 */
void SearchDialog::DisplaySearchResults(const QString &query, const QHash<QString, ViewItem *> &results)
{
    setQuery(query);
    clearSearchItems();

    data_filters->removeOptions();

    auto has_results = !results.isEmpty();
    info_label->setVisible(!has_results);

    if (has_results) {
        for (const auto& key : results.uniqueKeys()) {
            auto view_items = results.values(key);
            for (auto item : view_items) {
                search_key_lookups.insertMulti(item, key);
            }
            data_filters->addOption(key, key + " (" + QString::number(view_items.count()) + ")", "Data", key);
        }
        //Update the All Button
        data_filters->setResetButtonText("All (" + QString::number(search_key_lookups.uniqueKeys().count()) + ")");
    } else {
        data_filters->setResetButtonText("All");
    }

    // set the visibility of the search items based on the current filters
    filtersChanged();
}

/**
 * @brief SearchDialog::themeChanged
 */
void SearchDialog::themeChanged()
{
    auto theme = Theme::theme();
    
    setStyleSheet("SearchDialog {background-color: " % theme->getBackgroundColorHex() + ";border:1px solid " % theme->getDisabledBackgroundColorHex() % ";}" +
                  "QScrollArea {border: 1px solid " % theme->getAltBackgroundColorHex() % "; background: rgba(0,0,0,0); } " +
                  "QLabel {color:" + theme->getTextColorHex() + ";} " +
                  theme->getScrollBarStyleSheet() +
                  theme->getToolBarStyleSheet() +
                  theme->getSplitterStyleSheet()
                  );

    load_more_button->setStyleSheet(theme->getToolBarStyleSheet() + "QToolButton{font-size:10px;border-radius:0px;}");

    results_widget->setStyleSheet("background: rgba(0,0,0,0);");
    filters_widget->setStyleSheet("background: rgba(0,0,0,0);");

    center_action->setIcon(theme->getIcon("Icons", "crosshair"));
    popup_action->setIcon(theme->getIcon("Icons", "popOut"));
    search_action->setIcon(theme->getIcon("Icons", "zoom"));
    refresh_action->setIcon(theme->getIcon("Icons", "zoomRefresh"));
    reset_filters_action->setIcon(theme->getIcon("Icons", "cross"));

    query_label->setStyleSheet("color:" + theme->getHighlightColorHex() + ";");

    info_label->setFont(QFont(theme->getFont().family(), 12));
    info_label->setStyleSheet("background: rgba(0,0,0,0); color:" + theme->getTextColorHex(ColorRole::DISABLED) + ";");

    top_toolbar->setIconSize(theme->getIconSize());
    bottom_toolbar->setIconSize(theme->getIconSize());
}

/**
 * @brief SearchDialog::filtersChanged
 */
void SearchDialog::filtersChanged()
{
    auto checked_aspect_set = aspect_filters->getCheckedOptions<VIEW_ASPECT>().toSet();
    auto checked_key_set = data_filters->getCheckedOptions<QString>().toSet();

    current_visible = 0;
    filtered_match_count = 0;

    for (auto view_item : search_key_lookups.uniqueKeys()) {
        auto matched_keys = search_key_lookups.values(view_item).toSet();
        auto view_aspect = view_item->getViewAspect();

        //Test our cases
        auto got_matched_aspect = checked_aspect_set.contains(view_aspect);
        auto got_matched_keys = checked_key_set.intersects(matched_keys);
        auto got_match = got_matched_aspect && got_matched_keys;

        if (got_match) {
            filtered_match_count ++;
        }
        
        auto set_visible = got_match && current_visible < max_visible;

        //If we've got a match WE have to construct a search item, else if we don't need, we only need to hide the previously constructed items
        auto search_item = set_visible ? constructSearchItem(view_item) : getSearchItem(view_item);
        
        if (search_item) {
            //Show the item
            search_item->setVisible(set_visible);
            if (set_visible) {
                //Add All matching search keys
                search_item->addMatchedKeys(matched_keys);
                current_visible ++;
            }
        }
    }

    //Calculate the number of hidden items
    auto search_count = search_key_lookups.uniqueKeys().size();
    //Check if there are search results which are hidden by the filters
    auto any_filtered = search_count > filtered_match_count;
    //Check if all the matched filter results are being shown
    auto all_showing = current_visible == filtered_match_count;

    //Update the text for the status_label
    if (any_filtered) {
        status_label->setText(QString::number(search_count - filtered_match_count) + "/" + QString::number(search_count) + " entities hidden by filters");
    } 

    result_status_widget->setVisible(any_filtered);
    load_more_button->setVisible(!all_showing);
}

/**
 * @brief SearchDialog::viewItemDestructed
 */
void SearchDialog::viewItemDestructed(int ID, ViewItem* item)
{
    search_key_lookups.remove(item);
    if (search_items.contains(ID)) {
    	auto widget = search_items.take(ID);
    	widget->hide();
    	widget->deleteLater();
    }
    if (selected_id == ID) {
        selected_id = -1;
    }
}

/**
 * @brief SearchDialog::viewItemSelected
 * This is called whenever a view item is selected/unselected
 * @param item
 * @param selected
 */
void SearchDialog::viewItemSelected(ViewItem *item, bool selected)
{
    if (item) {
        auto search_item = search_items.value(item->getID(), nullptr);
        if (search_item) {
        	search_item->viewItemSelected(selected);
        }
    }
}

/**
 * @brief SearchDialog::searchItemClicked
 * This is called whenever a search item is clicked
 * @param ID
 */
void SearchDialog::searchItemClicked(int ID)
{
    if (selected_id != ID) {
        // deselect the previously selected search item
        auto search_item = search_items.value(selected_id, nullptr);
        if (search_item) {
        	search_item->setSelected(false);
        }
        selected_id = ID;
    } else {
        selected_id = -1;
    }
}

/**
 * @brief SearchDialog::setupLayout
 */
void SearchDialog::setupLayout()
{
    auto left_widget = new QWidget(this);
    auto right_widget = new QWidget(this);
    
    // TODO: Are these brackets necessary?
    {
        //LEFT WIDGET
        left_widget->setContentsMargins(5,5,1,5);
        auto v_layout = new QVBoxLayout(left_widget);
        v_layout->setMargin(0);

        filters_widget = new QWidget(this);
        filters_widget->setContentsMargins(5,0,5,5);
        filters_layout = new QVBoxLayout(filters_widget);
        filters_layout->setAlignment(Qt::AlignTop);
        filters_layout->setMargin(0);
        filters_layout->setSpacing(0);

        filters_scroll = new QScrollArea(this);
        filters_scroll->setWidget(filters_widget);
        filters_scroll->setWidgetResizable(true);

        v_layout->addWidget(filters_scroll, 1);
    }

    {
        //RIGHT WIDGET
        right_widget->setContentsMargins(1,5,5,5);
        auto v_layout = new QVBoxLayout(right_widget);
        v_layout->setMargin(0);
        v_layout->setSpacing(5);

        search_label = new QLabel("Search Query: ", this);
        query_label = new QLabel(this);
        query_label->setFont(QFont(font().family(), 12));

        top_toolbar = new QToolBar(this);
        top_toolbar->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding));

        search_action = top_toolbar->addAction("Search Again");
        refresh_action = top_toolbar->addAction("Refresh Search Results");
        top_toolbar->addSeparator();
        center_action = top_toolbar->addAction("Center On Selection");
        popup_action = top_toolbar->addAction("Popup On Selection");

        auto top_layout = new QHBoxLayout();
        {
            //Construct the top bar layout
            top_layout->setContentsMargins(0, 0, 0, 0);
            top_layout->addWidget(search_label);
            top_layout->addWidget(query_label, 1);
            top_layout->addWidget(top_toolbar);
        }

        results_widget = new QWidget(this);
        {
            auto v2_layout = new QVBoxLayout(results_widget);
            v2_layout->setAlignment(Qt::AlignTop);
            v2_layout->setSpacing(0);
            v2_layout->setMargin(0);
            results_layout = new QVBoxLayout();
            results_layout->setAlignment(Qt::AlignTop);
            results_layout->setSpacing(0);
            results_layout->setMargin(0);

            info_label = new QLabel("No results matching the query", this);
            info_label->setAlignment(Qt::AlignCenter);
            info_label->setFont(QFont(font().family(), 25));
            info_label->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

            //Add the No Results info label to the results layout
            v2_layout->addWidget(info_label);
            v2_layout->addLayout(results_layout);
            load_more_button = new QToolButton(this);
            load_more_button->setText("Load next 10 entities");
            load_more_button->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred));
            load_more_button->setVisible(false);
            v2_layout->addWidget(load_more_button);
        }

        results_scroll = new QScrollArea(this);
        results_scroll->setObjectName("RIGHTWIDGET");
        results_scroll->setWidget(results_widget);
        results_scroll->setWidgetResizable(true);

        result_status_widget = new QWidget(this);
        {
            status_label = new QLabel(this);
            status_label->setAlignment(Qt::AlignCenter);
            status_label->setFont(QFont(font().family(), 12));
            status_label->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred));

            bottom_toolbar = new QToolBar(this);
            bottom_toolbar->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding));
            reset_filters_action = bottom_toolbar->addAction("Reset Filters");

            result_status_widget->hide();
            auto status_layout = new QHBoxLayout(result_status_widget);
            status_layout->setContentsMargins(0, 0, 0, 0);
            status_layout->addWidget(status_label, 1);
            status_layout->addWidget(bottom_toolbar);
        }

        v_layout->addLayout(top_layout);
        v_layout->addWidget(results_scroll, 1);
        v_layout->addWidget(result_status_widget);
    }

    splitter = new QSplitter(this);
    splitter->addWidget(left_widget);
    splitter->addWidget(right_widget);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes(QList<int>() << DEFAULT_KEY_WIDTH << DEFAULT_DISPLAY_WIDTH);

    auto layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(splitter, 1);
    
    setupFilters();

    connect(center_action, &QAction::triggered, this, [=](){emit CenterOn(selected_id);});
    connect(popup_action, &QAction::triggered, this, [=](){emit Popup(selected_id);});

    connect(search_action, &QAction::triggered, this, &SearchDialog::SearchPopup);
    connect(refresh_action, &QAction::triggered, this, [=](){emit SearchQuery(query_text);});
}

/**
 * @brief SearchDialog::setupFilters
 */
void SearchDialog::setupFilters()
{
    aspect_filters = new OptionGroupBox("ASPECT", SortOrder::INSERTION, this);
    filters_layout->addWidget(aspect_filters);

    for (auto view_aspect : EntityFactory::getViewAspects()) {
        auto node_kind = EntityFactory::getViewAspectKind(view_aspect);
        aspect_filters->addOption(QVariant::fromValue(view_aspect), getViewAspectName(view_aspect), "EntityIcons", EntityFactory::getNodeKindString(node_kind));
    }
    aspect_filters->addOption(QVariant::fromValue(VIEW_ASPECT::NONE), "Edges", "Icons", "connect");

    data_filters = new OptionGroupBox("DATA", SortOrder::INSERTION, this);
    filters_layout->addWidget(data_filters);

    connect(aspect_filters, &OptionGroupBox::checkedOptionsChanged, this, &SearchDialog::filtersChanged);
    connect(data_filters, &OptionGroupBox::checkedOptionsChanged, this, &SearchDialog::filtersChanged);

    connect(reset_filters_action, &QAction::triggered, aspect_filters, &OptionGroupBox::reset);
    connect(reset_filters_action, &QAction::triggered, data_filters, &OptionGroupBox::reset);
}

/**
 * @brief SearchDialog::clearsearch_items
 */
void SearchDialog::clearSearchItems()
{   
    for (auto i : search_items) {
        i->clearMatchedKeys();
        i->hide();
    }
    search_key_lookups.clear();
    searchItemClicked(-1);
    max_visible = 30;
}

/**
 * @brief SearchDialog::scrollBarValueChanged
 */
void SearchDialog::scrollBarValueChanged()
{
    auto v_scroll = results_scroll->verticalScrollBar();
    if (v_scroll && v_scroll->value() > 0 && v_scroll->value() == v_scroll->maximum()) {
        loadNextResults();
    }
}

/**
 * @brief SearchDialog::loadNextResults
 */
void SearchDialog::loadNextResults()
{
    if (current_visible < filtered_match_count) {
        //Load more!
        max_visible += 10;
        filtersChanged();
    }
}

/**
 * @brief SearchDialog::constructSearchItem
 * @param item
 */
SearchItemWidget* SearchDialog::constructSearchItem(ViewItem *item)
{
    if (!item) {
        return nullptr;
    }

    auto ID = item->getID();
    if (search_items.contains(ID)) {
        return search_items.value(ID);
    }

    auto search_item = new SearchItemWidget(item, this);
    connect(search_item, &SearchItemWidget::searchItemClicked, this, &SearchDialog::searchItemClicked);
    connect(search_item, &SearchItemWidget::flashEntityItem, this, &SearchDialog::FlashEntity);
    connect(search_item, &SearchItemWidget::centerEntityItem, this, &SearchDialog::CenterOn);

    search_items.insert(ID, search_item);
    results_layout->addWidget(search_item);
    return search_item;
}

/**
 * @brief SearchDialog::getSearchItem
 * @param item
 * @return
 */
SearchItemWidget* SearchDialog::getSearchItem(ViewItem* item)
{
    if (item) {
        return search_items.value(item->getID(), nullptr);
    }
    return nullptr;
}
