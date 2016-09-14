#include "searchdialog.h"
#include "../../View/theme.h"

#define LEFT_PANEL_WIDTH 120

/**
 * @brief SearchDialog::SearchDialog
 * @param parent
 */
SearchDialog::SearchDialog(QWidget *parent) : QDialog(parent)
{
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
    constructKeyButton("All", "All (" + QString::number(results.count()) + ")", true);

    if (results.isEmpty()) {
        infoLabel->show();
        //constructSearchItem(0);
    } else {

        infoLabel->hide();
        QMap<ViewItem*, QString> resultItems;

        // store each item with the keys that match the search
        foreach (QString key, results.uniqueKeys()) {
            QList<ViewItem*> viewItems = results.values(key);
            foreach (ViewItem* item, viewItems) {
                resultItems.insertMulti(item, key);
            }
            constructKeyButton(key, key + " (" + QString::number(viewItems.count()) + ")");
        }

        foreach (ViewItem* item, resultItems.uniqueKeys()) {
            SearchItemWidget* searchItem = constructSearchItem(item);
            searchItem->setDisplayKeys(resultItems.values(item));
        }

    }

    // update the keys toolbar/buttons's size
    keysToolBar->setMinimumHeight(keysToolBar->sizeHint().height() + 10);
    updateKeysToolBarStyleSheet();
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

    setStyleSheet("QDialog{ background:" + theme->getBackgroundColorHex() + "; }"
                  "QScrollArea{ background: rgba(0,0,0,0); border: 1px solid " + theme->getDisabledBackgroundColorHex() + ";}"
                  "QComboBox {"
                  "background:" + theme->getAltBackgroundColorHex() + ";"
                  "color:" + theme->getTextColorHex() + ";"
                  "selection-background-color:" + theme->getHighlightColorHex() + ";"
                  "selection-color:" + theme->getTextColorHex(theme->CR_SELECTED) + ";"
                  "border: 1px solid " + theme->getDisabledBackgroundColorHex() + ";"
                  "padding: 0px 5px;"
                  "margin: 0px;"
                  "}"
                  "QComboBox:hover {"
                  "background:" + theme->getHighlightColorHex() + ";"
                  "color:" + theme->getTextColorHex(theme->CR_SELECTED) + ";"
                  "}"
                  "QComboBox::drop-down {"
                  "subcontrol-position: top right;"
                  "padding: 0px;"
                  "width: 20px;"
                  "}"
                  //"QComboBox::down-arrow{ image: url(Resources/Images/Actions/Arrow_Down.png); }"
                  "QComboBox QAbstractItemView {"
                  "background:" + theme->getAltBackgroundColorHex() + ";"
                  "}"
                  + labelStyle);

    //searchButton->setIcon(theme->getIcon("Actions", "Search"));
    queryLabel->setStyleSheet("color:" + theme->getHighlightColorHex() + ";");
    infoLabel->setStyleSheet(labelStyle);
    updateKeysToolBarStyleSheet();
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
 * @brief SearchDialog::setupLayout
 */
void SearchDialog::setupLayout()
{
    QLabel* searchLabel = new QLabel("Search: ", this);
    scopeLabel = new QLabel("Scope:", this);
    queryLabel = new QLabel("Searched string Searched string Searched string", this);

    QFont labelFont(font().family(), 9);
    searchLabel->setFont(labelFont);
    scopeLabel->setFont(labelFont);
    labelFont.setPointSize(10);
    queryLabel->setFont(labelFont);
    queryLabel->setWordWrap(true);

    QHBoxLayout* labelLayout = new QHBoxLayout();
    labelLayout->setMargin(0);
    labelLayout->setSpacing(2);
    labelLayout->addWidget(searchLabel);
    labelLayout->addWidget(queryLabel, 1);

    int fieldHeight = 25;

    scopeComboBox = new QComboBox(this);
    scopeComboBox->setFixedHeight(fieldHeight);
    scopeComboBox->addItem("Entire Model");
    scopeComboBox->addItem("Interfaces");
    scopeComboBox->addItem("Behaviour");
    scopeComboBox->addItem("Assemblies");
    scopeComboBox->addItem("Hardware");

    QHBoxLayout* labelLayout2 = new QHBoxLayout();
    labelLayout2->setMargin(0);
    labelLayout2->setSpacing(2);
    labelLayout2->addWidget(scopeLabel);
    labelLayout2->addWidget(scopeComboBox);

    keysActionGroup = new QActionGroup(this);
    keysActionGroup->setExclusive(true);

    keysToolBar = new QToolBar(this);
    keysToolBar->setOrientation(Qt::Vertical);
    keysToolBar->setToolButtonStyle(Qt::ToolButtonTextOnly);

    keysArea = new QScrollArea(this);
    keysArea->setWidget(keysToolBar);
    keysArea->setWidgetResizable(true);
    keysArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    keysArea->setStyleSheet("QScrollArea{ border: 0px; }");

    QWidget* displayWidget = new QWidget(this);
    displayWidget->setStyleSheet("background: rgba(0,0,0,0);");

    resultsLayout = new QVBoxLayout(displayWidget);
    resultsLayout->setMargin(0);
    resultsLayout->setSpacing(0);
    resultsLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    resultsLayout->setSizeConstraint(QLayout::SetMinimumSize);

    infoLabel = new QLabel("No Results", this);
    resultsLayout->addWidget(infoLabel);

    QScrollArea* displayArea = new QScrollArea(this);
    displayArea->setWidget(displayWidget);
    displayArea->setWidgetResizable(true);

    QHBoxLayout* bottomHLayout = new QHBoxLayout();
    bottomHLayout->setMargin(0);
    bottomHLayout->setSpacing(0);
    bottomHLayout->addWidget(keysArea);
    bottomHLayout->addWidget(displayArea, 1);

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->setMargin(0);
    hLayout->setSpacing(5);
    hLayout->addLayout(labelLayout, 1);
    hLayout->addSpacerItem(new QSpacerItem(30, 0));
    hLayout->addLayout(labelLayout2);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(10);
    mainLayout->setSpacing(5);
    mainLayout->addLayout(hLayout);
    mainLayout->addLayout(labelLayout);
    mainLayout->addSpacerItem(new QSpacerItem(0, 5));
    mainLayout->addLayout(bottomHLayout, 1);
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

    while (!searchItems.isEmpty()) {
        SearchItemWidget* widget = searchItems.takeFirst();
        resultsLayout->removeWidget(widget);
        delete widget;
    }
}


/**
 * @brief SearchDialog::constructSearchItem
 * @param item
 */
SearchItemWidget* SearchDialog::constructSearchItem(ViewItem *item)
{
    SearchItemWidget* itemWidget = new SearchItemWidget(item, this);
    resultsLayout->addWidget(itemWidget);
    searchItems.append(itemWidget);
    if (item) {
        connect(this, SIGNAL(keyButtonChecked(QString)), itemWidget, SLOT(toggleKeyWidget(QString)));
        //connect(item, SIGNAL(destructing()), itemWidget, SLOT(deleteLater()));
        connect(itemWidget, SIGNAL(centerOnViewItem(int)), this, SIGNAL(centerOnViewItem(int)));
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
    QAction* action = new QAction(text, this);
    action->setCheckable(true);
    action->setChecked(checked);
    action->setProperty("key", key);
    keysToolBar->addAction(action);
    keysActionGroup->addAction(action);
    maxToolButtonWidth = qMax(maxToolButtonWidth, fontMetrics().width(text));
    connect(action, SIGNAL(toggled(bool)), this, SLOT(keyButtonChecked(bool)));
}


/**
 * @brief SearchDialog::updateKeysToolBarStyleSheet
 */
void SearchDialog::updateKeysToolBarStyleSheet()
{
    Theme* theme = Theme::theme();
    keysToolBar->setStyleSheet("QToolBar{ padding: 0px; }"
                               "QToolButton {"
                               "padding: 5px 10px;"
                               "width:" + QString::number(maxToolButtonWidth) + "px;"
                               "border-radius: 2px;"
                               "}"
                               "QToolButton::checked {"
                               "background:" + theme->getHighlightColorHex() + ";"
                               "color:" + theme->getTextColorHex(theme->CR_SELECTED) + ";"
                               "}");
}

