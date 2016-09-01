#include "dockwidgetactionitem.h"

#define ICON_SIZE 45
#define ARROW_WIDTH 10
#define MIN_BUTTON_WIDTH 50
#define MIN_BUTTON_HEIGHT 75

/**
 * @brief DockWidgetActionItem::DockWidgetActionItem
 * @param action
 * @param parent
 */
DockWidgetActionItem::DockWidgetActionItem(QAction* action, QWidget *parent) :
    DockWidgetItem(action->text(), parent)
{
    dockAction = action;

    setupLayout();
    setSubActionRequired(false);
    setEnabled(true);

    displayToolButtonText(false);
    updateDisplayedText(getDisplayedText());

    actionChanged();
    themeChanged();

    connect(action, SIGNAL(changed()), this, SLOT(actionChanged()));
    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    connect(this, SIGNAL(displayedTextChanged(QString)), SLOT(updateDisplayedText(QString)));
}


/**
 * @brief DockWidgetActionItem::getAction
 * @return
 */
QAction* DockWidgetActionItem::getAction()
{
    return dockAction;
}


/**
 * @brief DockWidgetActionItem::getItemKind
 * @return
 */
DockWidgetItem::DOCKITEM_KIND DockWidgetActionItem::getItemKind()
{
    return ACTION_ITEM;
}


/**
 * @brief DockWidgetActionItem::setSubActionRequired
 * @param required
 */
void DockWidgetActionItem::setSubActionRequired(bool required)
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
 * @brief DockWidgetActionItem::requiresSubAction
 * @return
 */
bool DockWidgetActionItem::requiresSubAction()
{
    return subActionRequired;
}


/**
 * @brief DockWidgetActionItem::setProperty
 * @param name
 * @param value
 */
void DockWidgetActionItem::setProperty(const char *name, const QVariant &value)
{
    QToolButton::setProperty(name, value);
    if (dockAction) {
        dockAction->setProperty(name, value);
    }
}


/**
 * @brief DockWidgetActionItem::getProperty
 * @param name
 * @return
 */
QVariant DockWidgetActionItem::getProperty(const char *name)
{
    return QToolButton::property(name);
}


/**
 * @brief DockWidgetActionItem::actionChanged
 */
void DockWidgetActionItem::actionChanged()
{
    if (dockAction) {
        QPixmap iconPixmap = dockAction->icon().pixmap(ICON_SIZE);
        if (!iconPixmap.isNull()) {
            iconLabel->setPixmap(iconPixmap);
        } else {
            iconLabel->setPixmap(Theme::theme()->getImage("Actions", "Help", QSize(ICON_SIZE, ICON_SIZE)));
        }
        QString actionText = dockAction->text();
        if (actionText != getText()) {
            setText(actionText);
        }
        setVisible(dockAction->isVisible());
    }
}


/**
 * @brief DockWidgetActionItem::themeChanged
 */
void DockWidgetActionItem::themeChanged()
{
    theme = Theme::theme();
    textLabel->setStyleSheet("color:" + theme->getTextColorHex() + ";");
    arrowLabel->setPixmap(theme->getImage("Actions", "Arrow_Right", QSize(28,28), theme->getTextColor()));
}


/**
 * @brief DockWidgetActionItem::updateDisplayedText
 * @param text
 */
void DockWidgetActionItem::updateDisplayedText(QString text)
{
    textLabel->setText(text);
}


/**
 * @brief DockWidgetActionItem::enterEvent
 * @param event
 */
void DockWidgetActionItem::enterEvent(QEvent* event)
{
    textLabel->setStyleSheet("color:" + theme->getTextColorHex(theme->CR_SELECTED) + ";");
    arrowLabel->setPixmap(theme->getImage("Actions", "Arrow_Right", QSize(28,28), theme->getTextColor(theme->CR_SELECTED)));
    QToolButton::enterEvent(event);
}


/**
 * @brief DockWidgetActionItem::leaveEvent
 * @param event
 */
void DockWidgetActionItem::leaveEvent(QEvent* event)
{
    textLabel->setStyleSheet("color:" + theme->getTextColorHex() + ";");
    arrowLabel->setPixmap(theme->getImage("Actions", "Arrow_Right", QSize(28,28), theme->getTextColor()));
    QToolButton::leaveEvent(event);
}


/**
 * @brief DockWidgetActionItem::setupLayout
 */
void DockWidgetActionItem::setupLayout()
{
    int margin = 4;

    textLabel = new QLabel(this);
    textLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    textLabel->setFont(QFont(font().family(), 8));

    arrowLabel = new QLabel(this);
    arrowLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    arrowLabel->setFixedWidth(ARROW_WIDTH);
    arrowLabel->hide();

    iconLabel = new QLabel(this);
    iconLabel->setAlignment(Qt::AlignHCenter);
    iconLabel->setMinimumWidth(MIN_BUTTON_WIDTH - margin*2);
    iconLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->setMargin(0);
    hLayout->setSpacing(0);
    hLayout->addWidget(iconLabel, 1, Qt::AlignCenter);
    hLayout->addWidget(arrowLabel, 0, Qt::AlignCenter);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->addLayout(hLayout, 1);
    mainLayout->addWidget(textLabel, 0, Qt::AlignCenter);

    setContentsMargins(margin, margin, margin, margin + 2);
    setMinimumSize(MIN_BUTTON_WIDTH, MIN_BUTTON_HEIGHT);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
}
