#include "searchdialog.h"
#include "../../View/theme.h"

#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QScrollArea>
#include <QPushButton>

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
    //setStyleSheet(theme->getLineEditStyleSheet());
    searchButton->setIcon(theme->getIcon("Actions", "Search"));

    setStyleSheet("QDialog {"
                  "background:" + theme->getBackgroundColorHex() + ";"
                  "}"
                  "QLabel {"
                  "background: rgba(250,0,0,250);"
                  "color:" + theme->getTextColorHex() + ";"
                  "}"
                  "QGroupBox { padding:0px; margin:0px;border:0px;background: rgba(0,250,0,250); }"
                  "QComboBox {"
                  "background:" + theme->getAltBackgroundColorHex() + ";"
                  "color:" + theme->getTextColorHex() + ";"
                  "selection-background-color:" + theme->getHighlightColorHex() + ";"
                  "selection-color:" + theme->getTextColorHex(theme->CR_SELECTED) + ";"
                  "border: 1px solid " + theme->getDisabledBackgroundColorHex() + ";"
                  "padding: 0px 5px;"
                  "margin: 0px;"
                  "}"
                  "QComboBox::drop-down {"
                  "subcontrol-position: top right;"
                  "padding: 0px;"
                  "width: 25px;"
                  "}"
                  "QComboBox::down-arrow{ image: url(Resources/Images/Actions/Arrow_Down.png);}"
                  "QComboBox QAbstractItemView {"
                  "background: yellow;"// + theme->getAltBackgroundColorHex() + ";"
                  "}"
                  "QPushButton {"
                  "padding: 5px 10px;"
                  "}"
                  + theme->getLineEditStyleSheet());
}


/**
 * @brief SearchDialog::setupLayout
 */
void SearchDialog::setupLayout()
{
    QLabel* searchLabel = new QLabel("Search:", this);
    QLineEdit* searchLineEdit = new QLineEdit(this);
    searchButton = new QToolButton(this);

    int fieldHeight = 35;
    searchLineEdit->setStyleSheet("padding:0px; margin:0px;border:0px;");
    searchLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QLabel* scopeLabel = new QLabel("Scope:", this);
    QComboBox* scopeComboBox = new QComboBox(this);

    scopeComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scopeComboBox->addItem("Entire Model");
    scopeComboBox->addItem("Interfaces");
    scopeComboBox->addItem("Behaviour");
    scopeComboBox->addItem("Assemblies");
    scopeComboBox->addItem("Hardware");

    /*
    QHBoxLayout* searchLayout = new QHBoxLayout();
    searchLayout->setMargin(0);
    searchLayout->setSpacing(5);
    searchLayout->addWidget(searchLineEdit, 1);
    searchLayout->addWidget(searchButton);

    QVBoxLayout* labelsLayout = new QVBoxLayout();
    labelsLayout->setMargin(0);
    labelsLayout->setSpacing(5);
    labelsLayout->addWidget(searchLabel);
    labelsLayout->addWidget(scopeLabel);

    QVBoxLayout* fieldsLayout = new QVBoxLayout();
    fieldsLayout->setMargin(0);
    fieldsLayout->setSpacing(5);
    fieldsLayout->addWidget(searchLineEdit);
    fieldsLayout->addWidget(scopeComboBox);

    QHBoxLayout* topHLayout = new QHBoxLayout();
    topHLayout->setMargin(0);
    topHLayout->setSpacing(5);
    topHLayout->addLayout(labelsLayout);
    topHLayout->addLayout(fieldsLayout, 1);
    */

    /*
    QHBoxLayout* topHLayout = new QHBoxLayout();
    topHLayout->setMargin(0);
    topHLayout->setSpacing(2);
    topHLayout->addWidget(searchLabel);
    topHLayout->addWidget(searchLineEdit, 1);
    topHLayout->addWidget(searchButton);
    topHLayout->addSpacerItem(new QSpacerItem(10, 0));
    topHLayout->addWidget(scopeLabel);
    topHLayout->addWidget(scopeComboBox, 1);
    */

    QGroupBox* searchBox = new QGroupBox(this);
    QGroupBox* scopeBox = new QGroupBox(this);
    QHBoxLayout* searchLayout = new QHBoxLayout(searchBox);
    QHBoxLayout* scopeLayout = new QHBoxLayout(scopeBox);

    searchLayout->setMargin(0);
    searchLayout->setSpacing(2);
    searchLayout->addWidget(searchLabel);
    searchLayout->addWidget(searchLineEdit, 1);
    searchLayout->addWidget(searchButton);

    scopeLayout->setMargin(0);
    scopeLayout->setSpacing(2);
    scopeLayout->addWidget(scopeLabel);
    scopeLayout->addWidget(scopeComboBox, 1);

    //scopeLabel->setFixedHeight(searchLabel->sizeHint().height());

    QHBoxLayout* topHLayout = new QHBoxLayout();
    topHLayout->setMargin(0);
    topHLayout->setSpacing(5);
    topHLayout->addWidget(searchBox);
    topHLayout->addWidget(scopeBox);

    searchBox->setFixedHeight(fieldHeight);
    scopeBox->setFixedHeight(fieldHeight);

    QWidget* keysBox = new QWidget(this);
    keysLayout = new QVBoxLayout(keysBox);
    keysLayout->setMargin(0);
    keysLayout->setSpacing(2);

    constructKeyButton("Kind");
    constructKeyButton("Label");
    constructKeyButton("Worker");
    keysLayout->addStretch();

    QScrollArea* displayArea = new QScrollArea(this);
    QHBoxLayout* bottomHLayout = new QHBoxLayout();
    bottomHLayout->setMargin(0);
    bottomHLayout->setSpacing(5);
    bottomHLayout->addWidget(keysBox);
    bottomHLayout->addWidget(displayArea, 1);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(10);
    mainLayout->setSpacing(5);
    mainLayout->addLayout(topHLayout);
    mainLayout->addLayout(bottomHLayout, 1);
}


/**
 * @brief SearchDialog::constructKeyButton
 * @param key
 */
void SearchDialog::constructKeyButton(QString key)
{
    QPushButton* button = new QPushButton(key, this);
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    button->setFixedHeight(25);
    button->setStyleSheet("padding: 5px 10px;");
    keysLayout->addWidget(button);
}

