#include "dockactionwidget.h"
#include "../theme.h"

#define ICON_SIZE 48

/**
 * @brief DockActionWidget::DockActionWidget
 * @param action
 * @param parent
 */
DockActionWidget::DockActionWidget(QAction *action, QWidget *parent) : QPushButton(parent)
{
    dockAction = action;
    subActionRequired = false;

    setFixedSize(75,75);
    setupLayout();
    actionChanged();

    connect(action, SIGNAL(toggled(bool)), this, SIGNAL(toggled(bool)));
    connect(action, SIGNAL(changed()), this, SLOT(actionChanged()));
    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
}


/**
 * @brief DockActionWidget::requiresSubAction
 * @param required
 */
void DockActionWidget::requiresSubAction(bool required)
{
    if (required != subActionRequired) {
        subActionRequired = required;
        arrowLabel->setVisible(required);
    }
}


/**
 * @brief DockActionWidget::actionChanged
 */
void DockActionWidget::actionChanged()
{
    if (dockAction) {
        iconLabel->setPixmap(dockAction->icon().pixmap(ICON_SIZE));
        textLabel->setText(dockAction->text());
    }
}


/**
 * @brief DockActionWidget::themeChanged
 */
void DockActionWidget::themeChanged()
{

}


/**
 * @brief DockActionWidget::setupLayout
 */
void DockActionWidget::setupLayout()
{
    iconLabel = new QLabel(this);
    textLabel = new QLabel(this);

    //iconLabel->setStyleSheet("background:red;");
    //textLabel->setStyleSheet("background:green;");

    iconLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    textLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->setMargin(0);
    vLayout->setSpacing(0);
    vLayout->addWidget(iconLabel, 3, Qt::AlignCenter);
    vLayout->addWidget(textLabel, 1, Qt::AlignCenter);

    arrowLabel = new QLabel(this);
    arrowLabel->setPixmap(Theme::theme()->getImage("Actions", "Arrow_Right"));
    arrowLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);
    arrowLabel->hide();

    mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->addLayout(vLayout);
    mainLayout->addWidget(arrowLabel);

    setContentsMargins(2,2,2,5);
}

