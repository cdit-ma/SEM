#include "searchdialog.h"
#include "../../View/theme.h"

#include <QSplitter>
#include <QDateTime>
#include <QDebug>

#define DEFAULT_KEY_WIDTH 120
#define DEFAULT_DISPLAY_WIDTH 200
#define MIN_HEIGHT 250

/**
 * @brief SearchDialog::SearchDialog
 * @param parent
 */
SearchDialog::SearchDialog(QWidget *parent) : QDialog(parent)
{
    setupLayout();
    setWindowTitle("Search Results");

    selectedSearchItemID = -1;

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
    constructKeyButton("All", "All (" + QString::number(results.count()) + ")", true);

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
    keysToolBar->setMinimumHeight(keysToolBar->sizeHint().height() + 10);
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

    setStyleSheet("QDialog{background:" + theme->getBackgroundColorHex() + ";}"
                  "QFrame{ background:" + theme->getBackgroundColorHex() + "; }"
                  "QScrollArea {"
                  "background: rgba(0,0,0,0);"
                  "border: 1px solid " + theme->getDisabledBackgroundColorHex() + ";"
                  "}"
                  + theme->getComboBoxStyleSheet());

    keysToolBar->setStyleSheet("QToolBar {"
                               "padding: 0px;"
                               "background:" + theme->getBackgroundColorHex() + ";"
                               "}"
                               "QToolButton {"
                               "padding: 5px 10px;"
                               "border-radius: 2px;"
                               "}"
                               "QToolButton::checked {"
                               "background:" + theme->getHighlightColorHex() + ";"
                               "color:" + theme->getTextColorHex(theme->CR_SELECTED) + ";"
                               "}");

    buttonsToolBar->setStyleSheet(theme->getToolBarStyleSheet() + "QToolButton{padding:4px;}");

    displaySplitter->setStyleSheet(theme->getSplitterStyleSheet());

    //searchButton->setIcon(theme->getIcon("Actions", "Search"));
    centerOnButton->setIcon(theme->getIcon("Actions", "Crosshair"));
    popupButton->setIcon(theme->getIcon("Actions", "Popup"));

    infoLabel->setStyleSheet(labelStyle);
    searchLabel->setStyleSheet(labelStyle);
    //scopeLabel->setStyleSheet(labelStyle);
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
 * @brief SearchDialog::setupLayout
 */
void SearchDialog::setupLayout()
{
    searchLabel = new QLabel("Search: ", this);
    //scopeLabel = new QLabel("Scope:", this);
    queryLabel = new QLabel("Searched string Searched string Searched string", this);

    QFont labelFont(font().family(), 9);
    searchLabel->setFont(labelFont);
    //scopeLabel->setFont(labelFont);
    labelFont.setPointSize(10);
    queryLabel->setFont(labelFont);
    queryLabel->setWordWrap(true);

    int fieldHeight = 32;

    /*
    searchLineEdit = new QLineEdit(this);
    searchLineEdit->setFont(QFont(font().family(), 13));
    searchLineEdit->setPlaceholderText("Search Here...");
    searchLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    searchLineEdit->setCompleter(searchCompleter);
    */

    /*
     * This is going to be used for filtering via view aspects.
     *
    scopeComboBox = new QComboBox(this);
    scopeComboBox->setFixedHeight(fieldHeight);
    scopeComboBox->addItem("Entire Model");
    scopeComboBox->addItem("Interfaces");
    scopeComboBox->addItem("Behaviour");
    scopeComboBox->addItem("Assemblies");
    scopeComboBox->addItem("Hardware");
    */

    centerOnButton = new QToolButton(this);
    centerOnButton->setFixedSize(QSize(fieldHeight, fieldHeight));
    centerOnButton->setToolTip("Center View Aspect On Selected Item");

    popupButton = new QToolButton(this);
    popupButton->setFixedSize(QSize(fieldHeight, fieldHeight));
    popupButton->setToolTip("View Selected Item In New Window");

    buttonsToolBar = new QToolBar(this);

    buttonsToolBar->setIconSize(QSize(20, 20));
    //buttonsToolbar->setFixedHeight(fieldHeight);
    buttonsToolBar->addWidget(centerOnButton);
    buttonsToolBar->addWidget(popupButton);

    keysToolBar = new QToolBar(this);
    keysToolBar->setOrientation(Qt::Vertical);
    keysToolBar->setToolButtonStyle(Qt::ToolButtonTextOnly);
    keysToolBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QScrollArea* keysArea = new QScrollArea(this);
    keysArea->setWidget(keysToolBar);
    keysArea->setWidgetResizable(true);

    QFrame* displayWidget = new QFrame(this);
    resultsLayout = new QVBoxLayout(displayWidget);
    resultsLayout->setMargin(0);
    resultsLayout->setSpacing(0);
    resultsLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    resultsLayout->setSizeConstraint(QLayout::SetMinimumSize);

    infoLabel = new QLabel("No Results", this);
    infoLabel->setFont(QFont(font().family(), 10));
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

    /*
    QHBoxLayout* labelLayout2 = new QHBoxLayout();
    labelLayout2->setMargin(0);
    labelLayout2->setSpacing(2);
    labelLayout2->addWidget(scopeLabel);
    labelLayout2->addWidget(scopeComboBox);
    */

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->setMargin(0);
    hLayout->setSpacing(5);
    hLayout->addLayout(labelLayout, 1);
    hLayout->addSpacerItem(new QSpacerItem(30, 0));
    //hLayout->addLayout(labelLayout2);
    hLayout->addWidget(buttonsToolBar);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(10);
    mainLayout->setSpacing(5);
    mainLayout->addLayout(hLayout);
    //mainLayout->addSpacerItem(new QSpacerItem(0, 5));
    //mainLayout->addWidget(buttonsToolbar, 1, Qt::AlignRight);
    mainLayout->addSpacerItem(new QSpacerItem(0, 5));
    mainLayout->addWidget(displaySplitter, 1);

    setMinimumSize(mainLayout->sizeHint().width() + 50, MIN_HEIGHT);

    keysActionGroup = new QActionGroup(this);
    keysActionGroup->setExclusive(true);

    connect(centerOnButton, SIGNAL(clicked(bool)), this, SLOT(centerOnSelectedItem()));
    connect(popupButton, SIGNAL(clicked(bool)), this, SLOT(popupSelectedItem()));
}


/**
 * @brief SearchDialog::clear
 * Remove all the actions from the keys action group and the keys toolbar and clear search result items.
 */
void SearchDialog::clear()
{
    QList<QAction*> actions = keysActionGroup->actions();
    while (!actions.isEmpty()) {
        QAction* action = actions.takeFirst();
        keysActionGroup->removeAction(action);
        keysToolBar->removeAction(action);
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
void SearchDialog::constructKeyButton(QString key, QString text, bool checked)
{
    QToolButton* button = new QToolButton(this);
    button->setText(text);
    button->setCheckable(true);
    button->setChecked(checked);
    button->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    QAction* action = keysToolBar->addWidget(button);
    action->setProperty("key", key);
    action->setCheckable(true);
    action->setChecked(checked);
    keysActionGroup->addAction(action);

    connect(button, SIGNAL(clicked(bool)), action, SLOT(toggle()));
    connect(action, SIGNAL(toggled(bool)), this, SLOT(keyButtonChecked(bool)));
    connect(action, SIGNAL(toggled(bool)), button, SLOT(setChecked(bool)));
}
