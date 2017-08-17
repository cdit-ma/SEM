#include "searchdialog.h"

#include <QDateTime>
#include <QDebug>

#define DEFAULT_KEY_WIDTH 150
#define DEFAULT_DISPLAY_WIDTH 300

#define FILTER_REST_KEY -1
#define FILTER_KEY "filter_key"

/**
 * @brief SearchDialog::SearchDialog
 * @param parent
 */
SearchDialog::SearchDialog(QWidget *parent)
    : QWidget(parent)
{
    selectedSearchItemID = -1;

    loadingGif = new QMovie(this);
    loadingGif->setFileName(":/Images/Icons/loading");
    loadingGif->setScaledSize(QSize(16,16));

    setupLayout();

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    themeChanged();
}


/**
 * @brief SearchDialog::searchResults
 * @param query
 * @param results
 */
void SearchDialog::searchResults(QString query, QMap<QString, ViewItem*> results)
{
    queryText = query;

    // update displayed search string and clear previous search items
    queryLabel->setText("\"" + query + "\"");
    
    clearSearchItems();

    // clear previous data filters
    dataFilters->removeOptions();

    bool hasResults = !results.isEmpty();
    infoLabel->setVisible(!hasResults);
    if (!hasResults) {
        dataFilters->setResetButtonText("All");
        return;
    }

    dataFilters->setResetButtonText("All (" + QString::number(results.count()) + ")");

    for(auto key : results.uniqueKeys()){
        auto view_items = results.values(key);
        for(auto item : view_items){
            auto search_item = constructSearchItem(item);
            search_item->addDisplayKey(key);
            resultsLayout->addWidget(search_item);
        }
        dataFilters->addOption(key, key + " (" + QString::number(view_items.count()) + ")", "Icons", "circleHalo");
    }
    /*
    // construct a search item for each result item
    foreach (QString key, results.uniqueKeys()) {
        QList<ViewItem*> viewItems = results.values(key);
        foreach (ViewItem* item, viewItems) {
            searchItem->addDisplayKey(key);
        }
        dataFilters->addOption(key, key + " (" + QString::number(viewItems.count()) + ")", "Icons", "circleHalo");
    }*/
}


/**
 * @brief SearchDialog::themeChanged
 */
void SearchDialog::themeChanged()
{
    Theme* theme = Theme::theme();
    QString labelStyle = "QLabel {"
                         "padding: 5px 0px;"
                         "background: rgba(0,0,0,0);"
                         "color:" + theme->getTextColorHex() + ";"
                         "}";

    setStyleSheet("QFrame{ background:" + theme->getBackgroundColorHex() + "; }"
                  "QScrollArea {"
                  "background:" + theme->getBackgroundColorHex() + ";"
                  "border: 1px solid " + theme->getDisabledBackgroundColorHex() + ";"
                  "}"
                  + theme->getDialogStyleSheet());

    topToolbar->setStyleSheet(theme->getToolBarStyleSheet() + "QToolBar{ padding: 0px 4px; }");
    bottomToolbar->setStyleSheet(theme->getToolBarStyleSheet() + "QToolBar{ padding: 0px 4px; background:" + theme->getBackgroundColorHex() + ";}");
    displaySplitter->setStyleSheet(theme->getSplitterStyleSheet());
    filtersToolbar->setStyleSheet(theme->getToolBarStyleSheet() +
                               "QToolBar{ padding: 0px; spacing: 0px; }"
                               "QToolButton { border-radius:" + theme->getSharpCornerRadius() + ";}");

    centerOnButton->setIcon(theme->getIcon("Icons", "crosshair"));
    popupButton->setIcon(theme->getIcon("Icons", "popOut"));
    searchButton->setIcon(theme->getIcon("Icons", "zoom"));
    refreshButton->setIcon(theme->getIcon("Icons", "refresh"));

    infoLabel->setStyleSheet(labelStyle);
    searchLabel->setStyleSheet(labelStyle);
    queryLabel->setStyleSheet("color:" + theme->getHighlightColorHex() + ";");
}


/**
 * @brief SearchDialog::filtersChanged
 */
void SearchDialog::filtersChanged()
{
    OptionGroupBox* filterGroup = qobject_cast<OptionGroupBox*>(sender());
    if (filterGroup) {

        QVariant filterKey = filterGroup->property(FILTER_KEY);
        QList<QVariant> checkedFilters = filterGroup->getCheckedKeys();

        if (filterKey == ASPECTS_FILTER) {
            foreach (SearchItemWidget* item, searchItems) {
                int aspect = static_cast<int>(item->getViewAspect());
                item->updateVisibility(ASPECTS_FILTER, checkedFilters.contains(aspect));
            }
        } else if (filterKey == DATA_FILTER) {
            foreach (SearchItemWidget* item, searchItems) {
                bool matchedFilter = false;
                foreach (QVariant key, item->getDataKeys()) {
                    if (checkedFilters.contains(key)) {
                        matchedFilter = true;
                        break;
                    }
                }
                item->updateVisibility(DATA_FILTER, matchedFilter);
            }
        }
    }
}


/**
 * @brief SearchDialog::searchItemSelected
 * This is called whenever a search item is selected.
 * It deselects the previously selected search item.
 * @param ID
 */
void SearchDialog::searchItemSelected(int ID)
{
    if (selectedSearchItemID != ID) {
        if (searchItems.contains(selectedSearchItemID)) {
            SearchItemWidget* item = searchItems.value(selectedSearchItemID);
            item->setSelected(false);
        }
        selectedSearchItemID = ID;
    }
}


/**
 * @brief SearchDialog::viewItemDestructed
 */
void SearchDialog::viewItemDestructed(int ID)
{
    SearchItemWidget* widget = searchItems.value(ID, 0);
    if (widget) {
        searchItems.take(ID);
        widget->deleteLater();
    }
    if (selectedSearchItemID == ID) {
        selectedSearchItemID = -1;
    }
}


/**
 * @brief SearchDialog::centerOnSelectedItem
 */
void SearchDialog::centerOnSelectedItem()
{
    emit centerOnViewItem(selectedSearchItemID);
}


/**
 * @brief SearchDialog::popupSelectedItem
 */
void SearchDialog::popupSelectedItem()
{
    emit popupViewItem(selectedSearchItemID);
}


/**
 * @brief SearchDialog::loading
 * @param on
 */
void SearchDialog::loading(bool on)
{
    if (on) {
        loadingGif->start();
        queryLabel->setMovie(loadingGif);
    } else {
        loadingGif->stop();
        queryLabel->setPixmap(QPixmap());
        queryLabel->setText("\"" + queryText + "\"");
    }
}


/**
 * @brief SearchDialog::resetPanel
 */
void SearchDialog::resetPanel()
{
    selectedSearchItemID = -1;
    queryText = "";
    queryLabel->setText("\"" + queryText + "\"");

    // clear previous data filters
    dataFilters->removeOptions();
    dataFilters->setResetButtonText("All");

    clearSearchItems();
    infoLabel->setVisible(true);
}


/**
 * @brief SearchDialog::setupLayout
 */
void SearchDialog::setupLayout()
{
    searchLabel = new QLabel("Search: ", this);
    queryLabel = new QLabel("", this);

    QFont labelFont(font().family(), 9);
    searchLabel->setFont(labelFont);
    labelFont.setPointSize(10);
    queryLabel->setFont(labelFont);
    queryLabel->setWordWrap(true);

    int fieldHeight = 32;

    centerOnButton = new QToolButton(this);
    centerOnButton->setFixedSize(QSize(fieldHeight, fieldHeight));
    centerOnButton->setToolTip("Center View Aspect On Selected Item");

    popupButton = new QToolButton(this);
    popupButton->setFixedSize(QSize(fieldHeight, fieldHeight));
    popupButton->setToolTip("View Selected Item In New Window");

    topToolbar = new QToolBar(this);
    topToolbar->setIconSize(QSize(20, 20));

    topToolbar->addWidget(centerOnButton);
    topToolbar->addWidget(popupButton);

    searchButton = new QToolButton(this);
    searchButton->setFixedSize(QSize(fieldHeight, fieldHeight));
    searchButton->setToolTip("Search Again");

    refreshButton = new QToolButton(this);
    refreshButton->setFixedSize(QSize(fieldHeight, fieldHeight));
    refreshButton->setToolTip("Refresh Search Results");

    bottomToolbar = new QToolBar(this);
    bottomToolbar->setIconSize(QSize(20, 20));

    QWidget* stretchWidget2 = new QWidget(this);
    stretchWidget2->setStyleSheet("background: rgba(0,0,0,0);");
    stretchWidget2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    bottomToolbar->addWidget(stretchWidget2);
    bottomToolbar->addWidget(searchButton);
    bottomToolbar->addWidget(refreshButton);

    filtersToolbar = new QToolBar(this);
    filtersToolbar->setOrientation(Qt::Vertical);
    filtersToolbar->setToolButtonStyle(Qt::ToolButtonTextOnly);
    filtersToolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QScrollArea* keysArea = new QScrollArea(this);
    keysArea->setWidget(filtersToolbar);
    keysArea->setWidgetResizable(true);

    QFrame* displayWidget = new QFrame(this);
    displayWidget->setStyleSheet("QFrame{ background: rgba(0,0,0,0); }");

    resultsLayout = new QVBoxLayout(displayWidget);
    resultsLayout->setMargin(0);
    resultsLayout->setSpacing(0);
    resultsLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    resultsLayout->setSizeConstraint(QLayout::SetMinimumSize);

    infoLabel = new QLabel("No Results", this);
    infoLabel->setFont(QFont(font().family(), 10));
    infoLabel->setFixedHeight(topToolbar->sizeHint().height());
    resultsLayout->addWidget(infoLabel);

    QScrollArea* displayArea = new QScrollArea(this);
    displayArea->setWidget(displayWidget);
    displayArea->setWidgetResizable(true);

    displaySplitter = new QSplitter(this);
    displaySplitter->addWidget(keysArea);
    displaySplitter->addWidget(displayArea);
    displaySplitter->setStretchFactor(0, 0);
    displaySplitter->setStretchFactor(1, 1);
    displaySplitter->setSizes(QList<int>() << DEFAULT_KEY_WIDTH << DEFAULT_DISPLAY_WIDTH);

    QHBoxLayout* labelLayout = new QHBoxLayout();
    labelLayout->setMargin(0);
    labelLayout->setSpacing(2);
    labelLayout->addWidget(searchLabel);
    labelLayout->addWidget(queryLabel, 1);

    QHBoxLayout* topHLayout = new QHBoxLayout();
    topHLayout->setMargin(0);
    topHLayout->setSpacing(5);
    topHLayout->addLayout(labelLayout, 1);
    topHLayout->addSpacerItem(new QSpacerItem(20, 0));
    topHLayout->addWidget(topToolbar);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(1, 1, 1, 1);
    mainLayout->addLayout(topHLayout);
    mainLayout->addWidget(displaySplitter, 1);
    mainLayout->addWidget(bottomToolbar);

    int minWidth = qMax(topHLayout->sizeHint().width(), bottomToolbar->sizeHint().width()) + 20;
    int minHeight = topHLayout->sizeHint().height() + bottomToolbar->sizeHint().height() + 75;
    setMinimumSize(minWidth, minHeight);
    //setMinimumSize(DIALOG_MIN_WIDTH, DIALOG_MIN_HEIGHT);

    connect(centerOnButton, SIGNAL(clicked(bool)), this, SLOT(centerOnSelectedItem()));
    connect(popupButton, SIGNAL(clicked(bool)), this, SLOT(popupSelectedItem()));
    connect(searchButton, &QToolButton::clicked, this, &SearchDialog::searchButtonClicked);
    connect(refreshButton, &QToolButton::clicked, this, &SearchDialog::refreshButtonClicked);

    setupFilters();
}


/**
 * @brief SearchDialog::setupFilters
 */
void SearchDialog::setupFilters()
{
    aspectsFilters = new OptionGroupBox("ASPECT", this);
    aspectsFilters->setProperty(FILTER_KEY, ASPECTS_FILTER);
    filtersToolbar->addWidget(aspectsFilters);
    foreach (VIEW_ASPECT aspect, GET_VIEW_ASPECTS()) {
        aspectsFilters->addOption(static_cast<int>(aspect), GET_ASPECT_NAME(aspect), "EntityIcons", GET_ASPECT_ICON(aspect));
    }

    dataFilters = new OptionGroupBox("DATA", this);
    dataFilters->setProperty(FILTER_KEY, DATA_FILTER);
    dataGroupAction = filtersToolbar->addWidget(dataFilters);

    connect(aspectsFilters, &OptionGroupBox::checkedOptionsChanged, this, &SearchDialog::filtersChanged);
    connect(dataFilters, &OptionGroupBox::checkedOptionsChanged, this, &SearchDialog::filtersChanged);
}


/**
 * @brief SearchDialog::clearSearchItems
 */
void SearchDialog::clearSearchItems()
{
    QList<SearchItemWidget*> widgets = searchItems.values();
    while (!widgets.isEmpty()) {
        SearchItemWidget* widget = widgets.takeFirst();
        resultsLayout->removeWidget(widget);
        delete widget;
    }
    searchItems.clear();
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

    int ID = item->getID();
    if (searchItems.contains(ID)) {
        return searchItems.value(ID);
    }

    SearchItemWidget* itemWidget = new SearchItemWidget(item, this);
    itemWidget->setAspectFilterKey(ASPECTS_FILTER);
    itemWidget->setDataFilterKey(DATA_FILTER);
    resultsLayout->addWidget(itemWidget);
    searchItems.insert(ID, itemWidget);

    if (item) {
        connect(item, &ViewItem::destructing, this, &SearchDialog::viewItemDestructed);
        connect(itemWidget, SIGNAL(itemSelected(int)), this, SLOT(searchItemSelected(int)));
        connect(itemWidget, SIGNAL(hoverEnter(int)), this, SIGNAL(itemHoverEnter(int)));
        connect(itemWidget, SIGNAL(hoverLeave(int)), this, SIGNAL(itemHoverLeave(int)));
    }


    // update the search item's visibility based on the currently checked filters
    bool showItem = true;
    if (!aspectsFilters->isAllChecked()) {
        int aspect = static_cast<int>(itemWidget->getViewAspect());
        showItem = aspectsFilters->getCheckedKeys().contains(aspect);
    }
    if (showItem && !dataFilters->isAllChecked()) {
        showItem = false;
        foreach (QVariant key, itemWidget->getDataKeys()) {
            if (dataFilters->getCheckedKeys().contains(key)) {
                showItem = true;
                break;
            }
        }
    }

    itemWidget->setVisible(showItem);

    return itemWidget;
}
