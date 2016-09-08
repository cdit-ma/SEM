#include "searchdialog.h"
#include "../../View/theme.h"

#include <QScrollArea>

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
 * @brief SearchDialog::themeChanged
 */
void SearchDialog::themeChanged()
{
    Theme* theme = Theme::theme();

    setStyleSheet(theme->getLineEditStyleSheet() + "QDialog {"
                  "background:" + theme->getBackgroundColorHex() + ";"
                  "}"
                  "QLabel {"
                  "padding: 5px 0px;"
                  "background: rgba(0,0,0,0);"
                  "color:" + theme->getTextColorHex() + ";"
                  "}"
                  "QComboBox {"
                  "background:" + theme->getAltBackgroundColorHex() + ";"
                  "color:" + theme->getTextColorHex() + ";"
                  //"selection-background-color:" + theme->getHighlightColorHex() + ";"
                  //"selection-color:" + theme->getTextColorHex(theme->CR_SELECTED) + ";"
                  "border: 1px solid " + theme->getDisabledBackgroundColorHex() + ";"
                  "padding: 0px 5px;"
                  "margin: 0px;"
                  "}"
                  "QComboBox::drop-down {"
                  "subcontrol-position: top right;"
                  "padding: 0px;"
                  "width: 20px;"
                  "}"
                  //"QComboBox::down-arrow{ image: url(Resources/Images/Actions/Arrow_Down.png); }"
                  "QComboBox QAbstractItemView::item {"
                  "background:" + theme->getAltBackgroundColorHex() + ";"
                  "}"
                  "QPushButton {"
                  "padding: 5px 10px;"
                  "}"
                  + theme->getLineEditStyleSheet());

    keysToolBar->setStyleSheet("QToolBar{ padding: 0px; }"
                               "QToolButton {"
                               "padding: 5px 10px;"
                               //"width:" + QString::number(LEFT_PANEL_WIDTH - 25) + "px;"
                               "width:" + QString::number(maxToolButtonWidth) + "px;"
                               "border-radius: 2px;"
                               "}"
                               "QToolButton::checked {"
                               "background:" + theme->getHighlightColorHex() + ";"
                               "color:" + theme->getTextColorHex(theme->CR_SELECTED) + ";"
                               "}");

    //searchButton->setIcon(theme->getIcon("Actions", "Search"));
    queryLabel->setStyleSheet("color:" + theme->getHighlightColorHex() + ";");
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

    constructKeyButton("All");
    constructKeyButton("Description");
    constructKeyButton("Kind");
    constructKeyButton("Label", true);
    constructKeyButton("Worker");
    constructKeyButton("ID");
    keysToolBar->setMinimumHeight(keysToolBar->sizeHint().height() + 10);

    QScrollArea* displayArea = new QScrollArea(this);
    QHBoxLayout* bottomHLayout = new QHBoxLayout();
    bottomHLayout->setMargin(0);
    bottomHLayout->setSpacing(5);
    bottomHLayout->addWidget(keysToolBar);
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
 * @brief SearchDialog::constructKeyButton
 * @param key
 * @param checked
 */
void SearchDialog::constructKeyButton(QString key, bool checked)
{
    QAction* action = new QAction(key, this);
    action->setCheckable(true);
    action->setChecked(checked);
    keysToolBar->addAction(action);
    keysActionGroup->addAction(action);
    maxToolButtonWidth = qMax(maxToolButtonWidth, fontMetrics().width(key));
}

