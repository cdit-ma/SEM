#include "searchdialog.h"
#include "../../View/theme.h"

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
    setStyleSheet(theme->getLineEditStyleSheet());
    searchButton->setIcon(theme->getIcon("Actions", "Search"));
}


/**
 * @brief SearchDialog::setupLayout
 */
void SearchDialog::setupLayout()
{
    QLabel* searchLabel = new QLabel("Search: ", this);
    QLineEdit* searchLineEdit = new QLineEdit(this);
    searchButton = new QToolButton(this);

    QHBoxLayout* searchLayout = new QHBoxLayout();
    searchLayout->addWidget(searchLineEdit, 1);
    searchLayout->addWidget(searchButton);

    searchLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QLabel* scopeLabel = new QLabel("Scope: ", this);
    QComboBox* scopeComboBox = new QComboBox(this);

    scopeComboBox->addItem("Entire Model");
    scopeComboBox->addItem("Interfaces");
    scopeComboBox->addItem("Behaviour");
    scopeComboBox->addItem("Assemblies");
    scopeComboBox->addItem("Hardware");

    QVBoxLayout* labelsLayout = new QVBoxLayout();
    labelsLayout->addWidget(searchLabel);
    labelsLayout->addWidget(scopeLabel);

    QVBoxLayout* fieldsLayout = new QVBoxLayout();
    fieldsLayout->addLayout(searchLayout);
    fieldsLayout->addWidget(scopeComboBox);

    QHBoxLayout* topHLayout = new QHBoxLayout();
    topHLayout->addLayout(labelsLayout);
    topHLayout->addLayout(fieldsLayout, 1);

    QWidget* keysBox = new QWidget(this);
    //keysBox->setObjectName("KEYS_BOX");
    //keysBox->setStyleSheet("QWidget#KEYS_BOX{ padding: 0px; margin: 0px; background: red; }");

    keysLayout = new QVBoxLayout(keysBox);
    keysLayout->setMargin(2);
    keysLayout->setSpacing(2);
    /*
    QPushButton* kind = new QPushButton("Kind", this);
    QPushButton* label = new QPushButton("Label", this);
    QPushButton* worker = new QPushButton("Worker", this);
    keysLayout->addWidget(kind);
    keysLayout->addWidget(label);
    keysLayout->addWidget(worker);
    */
    constructKeyButton("Kind");
    constructKeyButton("Label");
    constructKeyButton("Worker");
    keysLayout->addStretch();

    QScrollArea* displayArea = new QScrollArea(this);
    QHBoxLayout* bottomHLayout = new QHBoxLayout();
    bottomHLayout->addWidget(keysBox);
    bottomHLayout->addWidget(displayArea, 1);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
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
    button->setStyleSheet("padding: 5px;");
    keysLayout->addWidget(button);
}

