#include "searchdialog.h"

#include <QDateTime>
#include <QDebug>

#define DEFAULT_KEY_WIDTH 135
#define DEFAULT_DISPLAY_WIDTH 215

/**
 * @brief SearchDialog::SearchDialog
 * @param parent
 */
SearchDialog::SearchDialog(QWidget *parent)
    : QWidget(parent)
{
    selectedSearchItemID = -1;

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
    queryLabel->setText("\"" + query + "\"");

    // clear previous key actions and search result items
    clear();

    constructKeyButton("All", "All (" + QString::number(results.count()) + ")");

    if (results.isEmpty()) {
        infoLabel->show();
    } else {
        infoLabel->hide();
        foreach (QString key, results.uniqueKeys()) {
            QList<ViewItem*> viewItems = results.values(key);
            foreach (ViewItem* item, viewItems) {
                SearchItemWidget* searchItem = constructSearchItem(item);
                searchItem->addDisplayKey(key);
            }
            constructKeyButton(key, key + " (" + QString::number(viewItems.count()) + ")");
        }
    }

    // update the keys toolbar/buttons's size
    keysToolbar->setMinimumHeight(keysToolbar->sizeHint().height() + 10);
    updateKeyButtonIcons();
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
                  + theme->getDialogStyleSheet()
                  + theme->getComboBoxStyleSheet());

    mainWidget->setStyleSheet("SD_MAINWIDGET{ background:" + theme->getBackgroundColorHex() + ";}");

    topToolbar->setStyleSheet(theme->getToolBarStyleSheet() + "QToolBar{ padding: 0px 4px; }");
    bottomToolbar->setStyleSheet(theme->getToolBarStyleSheet() + "QToolBar{ padding: 0px 4px; background:" + theme->getBackgroundColorHex() + ";}");
    displaySplitter->setStyleSheet(theme->getSplitterStyleSheet());
    keysToolbar->setStyleSheet(theme->getToolBarStyleSheet() +
                               "QToolBar::separator {"
                               "margin: 2px 0px;"
                               "height: 4px;"
                               "background:" + theme->getDisabledBackgroundColorHex() + ";"
                               "}"
                               "QToolButton {"
                               "padding: 5px 0px 5px 2px;"
                               "border-radius:" + theme->getSharpCornerRadius() + ";"
                               "}");

    centerOnButton->setIcon(theme->getIcon("Icons", "crosshair"));
    popupButton->setIcon(theme->getIcon("Icons", "popOut"));
    searchButton->setIcon(theme->getIcon("Icons", "zoom"));
    refreshButton->setIcon(theme->getIcon("Icons", "refresh"));

    infoLabel->setStyleSheet(labelStyle);
    searchLabel->setStyleSheet(labelStyle);
    queryLabel->setStyleSheet("color:" + theme->getHighlightColorHex() + ";");
}


/**
 * @brief SearchDialog::keyButtonChecked
 * @param checked
 */
void SearchDialog::keyButtonChecked(bool checked)
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action && checked) {
        emit keyButtonChecked(action->property("key").toString());
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
 * @brief SearchDialog::resetPanel
 */
void SearchDialog::resetPanel()
{
    clear();
    searchResults("", QMap<QString, ViewItem*>());
}


/**
 * @brief SearchDialog::updateKeyButtonIcons
 */
void SearchDialog::updateKeyButtonIcons()
{
    QPixmap pixmap = Theme::theme()->getImage("Icons", "blank", QSize(1,1));

    // added a tiny icon to left-align the tool button's text
    foreach (QAction* action, staticKeysActionGroup->actions()) {
        QToolButton* button = (QToolButton*) keysToolbar->widgetForAction(action);
        button->setIcon(pixmap);
    }
    foreach (QAction* action, dynamicKeysActionGroup->actions()) {
        QToolButton* button = (QToolButton*) keysToolbar->widgetForAction(action);
        button->setIcon(pixmap);
    }
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

    keysToolbar = new QToolBar(this);
    keysToolbar->setIconSize(QSize(18,18));
    keysToolbar->setOrientation(Qt::Vertical);
    keysToolbar->setToolButtonStyle(Qt::ToolButtonTextOnly);
    keysToolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    staticKeysActionGroup = new QActionGroup(this);
    dynamicKeysActionGroup = new QActionGroup(this);

    /*
     * TODO - Add this back when filtering by aspects is added
     *
    constructKeyButton("All", "All", true, false);
    keysToolbar->addSeparator();
    constructKeyButton("Interfaces", "Interfaces", false, false);
    constructKeyButton("Behaviour", "Behaviour", false, false);
    constructKeyButton("Assembly", "Assembly", false, false);
    constructKeyButton("Hardware", "Hardware", false, false);
    keysToolbar->addSeparator();
    */

    QScrollArea* keysArea = new QScrollArea(this);
    keysArea->setWidget(keysToolbar);
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

    mainWidget = new QWidget(this);
    mainWidget->setObjectName("SD_MAINWIDGET");
    setFocusProxy(mainWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(1, 1, 1, 1);
    mainLayout->addLayout(topHLayout);
    mainLayout->addWidget(displaySplitter, 1);
    mainLayout->addWidget(bottomToolbar);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(mainWidget);

    //setMinimumSize(DIALOG_MIN_WIDTH, DIALOG_MIN_HEIGHT);

    int minWidth = qMax(topHLayout->sizeHint().width(), bottomToolbar->sizeHint().width()) + 20;
    int minHeight = topHLayout->sizeHint().height() + bottomToolbar->sizeHint().height() + 75;
    setMinimumSize(minWidth, minHeight);

    connect(centerOnButton, SIGNAL(clicked(bool)), this, SLOT(centerOnSelectedItem()));
    connect(popupButton, SIGNAL(clicked(bool)), this, SLOT(popupSelectedItem()));
    connect(searchButton, &QToolButton::clicked, this, &SearchDialog::searchButtonClicked);
    connect(refreshButton, &QToolButton::clicked, this, &SearchDialog::refreshButtonClicked);
}


/**
 * @brief SearchDialog::clear
 * Remove all the dynamic key actions from its action group and the toolbar and the clear search result items.
 */
void SearchDialog::clear()
{
    QList<QAction*> actions = dynamicKeysActionGroup->actions();
    while (!actions.isEmpty()) {
        QAction* action = actions.takeFirst();
        dynamicKeysActionGroup->removeAction(action);
        keysToolbar->removeAction(action);
        delete action;
    }

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
    resultsLayout->addWidget(itemWidget);
    searchItems.insert(ID, itemWidget);

    if (item) {
        connect(item, &ViewItem::destructing, this, &SearchDialog::viewItemDestructed);
        connect(this, SIGNAL(keyButtonChecked(QString)), itemWidget, SLOT(toggleKeyWidget(QString)));
        connect(itemWidget, SIGNAL(itemSelected(int)), this, SLOT(searchItemSelected(int)));
        connect(itemWidget, SIGNAL(hoverEnter(int)), this, SIGNAL(itemHoverEnter(int)));
        connect(itemWidget, SIGNAL(hoverLeave(int)), this, SIGNAL(itemHoverLeave(int)));
    }

    return itemWidget;
}


/**
 * @brief SearchDialog::constructKeyButton
 * @param key
 * @param text
 * @param checked
 */
void SearchDialog::constructKeyButton(QString key, QString text, bool checked, bool addToGroup)
{
    QToolButton* button = new QToolButton(this);
    button->setText(text);
    button->setCheckable(true);
    button->setChecked(checked);
    button->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    QAction* action = keysToolbar->addWidget(button);
    action->setProperty("key", key);
    action->setCheckable(true);
    action->setChecked(checked);

    if (addToGroup) {
        dynamicKeysActionGroup->addAction(action);
    } else {
        staticKeysActionGroup->addAction(action);
    }

    connect(button, SIGNAL(clicked(bool)), action, SLOT(toggle()));
    connect(action, SIGNAL(toggled(bool)), this, SLOT(keyButtonChecked(bool)));
    connect(action, SIGNAL(toggled(bool)), button, SLOT(setChecked(bool)));
}
