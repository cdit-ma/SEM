#include "dockactionwidget.h"
#include "../theme.h"

#define ICON_SIZE 45
#define MIN_BUTTON_SIZE 75
#define ARROW_WIDTH 10

/**
 * @brief DockActionWidget::DockActionWidget
 * @param action
 * @param parent
 */
DockActionWidget::DockActionWidget(QAction *action, QWidget *parent) : QPushButton(parent)
{
    dockAction = action;

    setupLayout();
    setSubActionRequired(false);

    actionChanged();
    themeChanged();

    connect(action, SIGNAL(changed()), this, SLOT(actionChanged()));
    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
}


/**
 * @brief DockActionWidget::getAction
 * @return
 */
QAction* DockActionWidget::getAction()
{
    return dockAction;
}


/**
 * @brief DockActionWidget::setSubActionRequired
 * @param required
 */
void DockActionWidget::setSubActionRequired(bool required)
{
    if (required != subActionRequired) {
        subActionRequired = required;
        arrowLabel->setVisible(required);
        if (required) {
            iconLabel->setStyleSheet("padding-left:" + QString::number(ARROW_WIDTH) + ";");
        }
    }
}


/**
 * @brief DockActionWidget::requiresSubAction
 * @return
 */
bool DockActionWidget::requiresSubAction()
{
    return subActionRequired;
}


/**
 * @brief DockActionWidget::setProperty
 * @param name
 * @param value
 */
void DockActionWidget::setProperty(const char *name, const QVariant &value)
{
    QPushButton::setProperty(name, value);
    if (dockAction) {
        dockAction->setProperty(name, value);
    }
}


/**
 * @brief DockActionWidget::getProperty
 * @param name
 * @return
 */
QVariant DockActionWidget::getProperty(const char *name)
{
    return QPushButton::property(name);
}


/**
 * @brief DockActionWidget::actionChanged
 */
void DockActionWidget::actionChanged()
{
    if (dockAction) {
        QPixmap iconPixmap = dockAction->icon().pixmap(ICON_SIZE);
        if (!iconPixmap.isNull()) {
            iconLabel->setPixmap(iconPixmap);
        } else {
            iconLabel->setPixmap(Theme::theme()->getImage("Actions", "Help", QSize(ICON_SIZE, ICON_SIZE)));
        }
        textLabel->setText(dockAction->text());
        setToolTip(dockAction->text());
        setVisible(dockAction->isVisible());
    }
}


/**
 * @brief DockActionWidget::themeChanged
 */
void DockActionWidget::themeChanged()
{
    // TODO - Can't get the text color to change on hover
    Theme* theme = Theme::theme();
    setStyleSheet("QLabel{ border: 0px; color:" + theme->getTextColorHex() + ";}"
                  "QPushButton{ border: 0px; background:" + theme->getAltBackgroundColorHex() + "; color:" + theme->getTextColorHex() + ";}"
                  "QPushButton:hover{ background:" + theme->getHighlightColorHex() + ";}"); // color:" + theme->getTextColorHex(theme->CR_SELECTED) + ";}");
}


/**
 * @brief DockActionWidget::setupLayout
 */
void DockActionWidget::setupLayout()
{
    int margin = 4;

    iconLabel = new QLabel(this);
    textLabel = new QLabel(this);

    textLabel->setStyleSheet("background: rgba(0,0,0,0);");
    textLabel->setFont(QFont(font().family(), 8));

    iconLabel->setStyleSheet("background: rgba(0,0,0,0);");
    iconLabel->setAlignment(Qt::AlignHCenter);
    iconLabel->setMinimumWidth(MIN_BUTTON_SIZE - margin*2);
    iconLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    arrowLabel = new QLabel(this);
    arrowLabel->setAlignment(Qt::AlignRight);
    arrowLabel->setStyleSheet("background: rgba(0,0,0,0);");
    arrowLabel->setPixmap(Theme::theme()->getImage("Actions", "Arrow_Right", QSize(28,28)));
    //arrowLabel->setStyleSheet("padding: 0px; padding-top:" + QString::number(textLabel->sizeHint().height()) + ";");
    arrowLabel->setFixedWidth(ARROW_WIDTH);
    arrowLabel->hide();

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->setMargin(0);
    hLayout->setSpacing(0);
    hLayout->addWidget(iconLabel, 1, Qt::AlignCenter);
    hLayout->addWidget(arrowLabel, 0, Qt::AlignCenter);

    mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->addLayout(hLayout, 1);
    mainLayout->addWidget(textLabel, 0, Qt::AlignCenter);

    setContentsMargins(margin, margin, margin, margin + 2);
    setMinimumSize(MIN_BUTTON_SIZE, MIN_BUTTON_SIZE);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
}

