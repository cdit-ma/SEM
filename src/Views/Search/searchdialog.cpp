#include "searchdialog.h"
#include "../../theme.h"

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
    setObjectName("SearchDialog");

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    themeChanged();
}


void SearchDialog::setQuery(QString query){
    query_text = query;
    if(query_label){
        query_label->setText("\"" + query_text + "\"");
    }
}

/**
 * @brief SearchDialog::searchResults
 * @param query
 * @param results
 */
void SearchDialog::DisplaySearchResults(QString query, QMap<QString, ViewItem*> results)
{
    setQuery(query);

    clearSearchItems();

    // clear previous data filters
    data_filters->removeOptions();

    auto has_results = !results.isEmpty();
    
    info_label->setVisible(!has_results);
    if(!has_results) {
        data_filters->setResetButtonText("All");
        return;
    }else{
        data_filters->setResetButtonText("All (" + QString::number(results.count()) + ")");
    }


    for(auto key : results.uniqueKeys()){
        auto view_items = results.values(key);
        for(auto item : view_items){
            auto search_item = constructSearchItem(item);
            search_item->addMatchedKey(key);
        }
        data_filters->addOption(key, key + " (" + QString::number(view_items.count()) + ")", "Data", key);
    }

    filtersChanged();
}

/**
 * @brief SearchDialog::themeChanged
 */
void SearchDialog::themeChanged()
{
    auto theme = Theme::theme();
    
    setStyleSheet(
                    "#SearchDialog {background-color: " % theme->getBackgroundColorHex() + ";} " +
                    "QScrollArea {border: 1px solid " % theme->getAltBackgroundColorHex() % "; background: rgba(0,0,0,0);} " +
                    "QLabel {color:" + theme->getTextColorHex() + ";} " + 
                    theme->getToolBarStyleSheet() +
                    theme->getSplitterStyleSheet()
                );

    results_widgets->setStyleSheet("background: rgba(0,0,0,0);");
    filters_widgets->setStyleSheet("background: rgba(0,0,0,0);");

    center_action->setIcon(theme->getIcon("Icons", "crosshair"));
    popup_action->setIcon(theme->getIcon("Icons", "popOut"));
    search_action->setIcon(theme->getIcon("Icons", "zoom"));
    refresh_action->setIcon(theme->getIcon("Icons", "refresh"));

    query_label->setStyleSheet("color:" + theme->getHighlightColorHex() + ";");
    info_label->setStyleSheet("color:" + theme->getAltBackgroundColorHex() + ";");
    
}

     


/**
 * @brief SearchDialog::filtersChanged
 */
void SearchDialog::filtersChanged()
{
    auto checked_aspect_set = aspect_filters->getCheckedOptions<VIEW_ASPECT>().toSet();
    auto checked_key_list = data_filters->getCheckedOptions<QString>();

    for(auto item : search_items){
        auto view_aspect = item->getViewAspect();
        //Edges are allowed
        bool matched_aspect = checked_aspect_set.contains(view_aspect) || view_aspect == VIEW_ASPECT::NONE;
        
        bool matched_key = false;
        if(matched_aspect){
            for(auto key : checked_key_list){
                if(item->gotMatchedKey(key)){
                    matched_key = true;
                    break;
                }
            }
        }
        item->setVisible(matched_aspect && matched_key);
    }
}


/**
 * @brief SearchDialog::search_itemselected
 * This is called whenever a search item is selected.
 * It deselects the previously selected search item.
 * @param ID
 */
void SearchDialog::searchItemSelected(int ID)
{
    if (selected_id != ID) {
        if (search_items.contains(selected_id)) {
            SearchItemWidget* item = search_items.value(selected_id);
            item->setSelected(false);
        }
        selected_id = ID;
    }
}


/**
 * @brief SearchDialog::viewItemDestructed
 */
void SearchDialog::viewItemDestructed(int ID)
{
    SearchItemWidget* widget = search_items.value(ID, 0);
    if (widget) {
        qCritical()  << "DELETING WIDGET:" << ID << " " << widget;
        search_items.take(ID);
        delete widget;
    }
    if (selected_id == ID) {
        selected_id = -1;
    }
}

/**
 * @brief SearchDialog::resetPanel
 */
void SearchDialog::resetPanel()
{
    selected_id = -1;
    setQuery("");

    // clear previous data filters
    data_filters->removeOptions();
    data_filters->setResetButtonText("All");

    clearSearchItems();
    info_label->setVisible(true);
}


/**
 * @brief SearchDialog::setupLayout
 */
void SearchDialog::setupLayout()
{
    auto right_widget = new QWidget(this);
    auto v_layout = new QVBoxLayout(right_widget);
    //Add Padding to the top
    v_layout->setContentsMargins(0, 5, 0, 0);
    v_layout->setSpacing(5);

    
    search_label = new QLabel("Search Query: ", this);
    query_label = new QLabel(this);
    query_label->setFont(QFont(font().family(), 12));
    
    info_label = new QLabel("No results matching the query.", this);
    info_label->setAlignment(Qt::AlignCenter);
    info_label->setFont(QFont(font().family(), 25));
    
    
    top_toolbar = new QToolBar(this);
    top_toolbar->setIconSize(QSize(16, 16));
    
    //Construct the top layout
    auto top_layout = new QHBoxLayout();
    top_layout->addWidget(search_label);
    top_layout->addWidget(query_label, 1);
    top_layout->addWidget(top_toolbar);

    search_action = top_toolbar->addAction("Search Again");
    refresh_action = top_toolbar->addAction("Refresh Search Results");
    top_toolbar->addSeparator();
    center_action = top_toolbar->addAction("Center On Selection");
    popup_action = top_toolbar->addAction("Popup On Selection");


    filters_widgets = new QWidget(this);
    filters_layout = new QVBoxLayout(filters_widgets);
    filters_layout->setAlignment(Qt::AlignTop);
    filters_layout->setMargin(5);
    filters_layout->setSpacing(0);
    filters_layout->setSizeConstraint(QLayout::SetMinimumSize);

    auto filter_scroll = new QScrollArea(this);
    filter_scroll->setWidget(filters_widgets);
    filter_scroll->setWidgetResizable(true);


    
    results_widgets = new QWidget(this);


    results_layout = new QVBoxLayout(results_widgets);
    results_layout->setAlignment(Qt::AlignTop);
    results_layout->setSpacing(0);
    results_layout->setMargin(0);
    //Add the No Results info label to the results layout
    results_layout->addWidget(info_label);

    auto results_scroll = new QScrollArea(this);
    results_scroll->setWidget(results_widgets);
    results_scroll->setWidgetResizable(true);

    v_layout->addLayout(top_layout);
    v_layout->addWidget(results_scroll, 1);


    splitter = new QSplitter(this);
    splitter->addWidget(filter_scroll);
    splitter->addWidget(right_widget);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes(QList<int>() << DEFAULT_KEY_WIDTH << DEFAULT_DISPLAY_WIDTH);

    auto layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->setContentsMargins(1, 1, 1, 1);
    layout->addWidget(splitter, 1);
    


    connect(center_action, &QAction::triggered, this, [=](){emit CenterOn(selected_id);});
    connect(popup_action, &QAction::triggered, this, [=](){emit Popup(selected_id);});

    connect(search_action, &QAction::triggered, this, &SearchDialog::SearchPopup);
    connect(refresh_action, &QAction::triggered, this, [=](){emit SearchQuery(query_text);});

    setupFilters();
}


/**
 * @brief SearchDialog::setupFilters
 */
void SearchDialog::setupFilters()
{
    aspect_filters = new OptionGroupBox("ASPECT", this);
    filters_widgets->layout()->addWidget(aspect_filters);

    for(auto view_aspect : GET_VIEW_ASPECTS()){
        aspect_filters->addOption(QVariant::fromValue(view_aspect), GET_ASPECT_NAME(view_aspect), "EntityIcons", GET_ASPECT_ICON(view_aspect));
    }

    data_filters = new OptionGroupBox("DATA", this);
    filters_widgets->layout()->addWidget(data_filters);

    connect(aspect_filters, &OptionGroupBox::checkedOptionsChanged, this, &SearchDialog::filtersChanged);
    connect(data_filters, &OptionGroupBox::checkedOptionsChanged, this, &SearchDialog::filtersChanged);
}


/**
 * @brief SearchDialog::clearsearch_items
 */
void SearchDialog::clearSearchItems()
{   
    for(auto i : search_items){
        i->clearMatchedKeys();
        i->hide();
    }
}


/**
 * @brief SearchDialog::constructSearchItem
 * @param item
 */
SearchItemWidget* SearchDialog::constructSearchItem(ViewItem *item)
{
    if (!item) {
        return 0;
    }

    auto ID = item->getID();
    if (search_items.contains(ID)) {
        return search_items.value(ID);
    }

    auto search_item = new SearchItemWidget(item, this);
    results_layout->addWidget(search_item);
    search_items.insert(ID, search_item);

    if (item) {
        connect(search_item, &SearchItemWidget::itemSelected, this, &SearchDialog::searchItemSelected);
        connect(search_item, &SearchItemWidget::hoverEnter, this, &SearchDialog::itemHoverEnter);
        connect(search_item, &SearchItemWidget::hoverLeave, this, &SearchDialog::itemHoverLeave);
    }
    return search_item;
}
