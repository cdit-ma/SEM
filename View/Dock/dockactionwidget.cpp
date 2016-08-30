#include "dockactionwidget.h"

#include <QToolBar>

#define ICON_SIZE 45
#define MIN_BUTTON_WIDTH 50
#define MIN_BUTTON_HEIGHT 75
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
        QString actionText = dockAction->text();
        if (actionText != fullActionText) {
            updateTextLabel(actionText);
            setToolTip(actionText);
        }
        setVisible(dockAction->isVisible());
    }
}


/**
 * @brief DockActionWidget::themeChanged
 */
void DockActionWidget::themeChanged()
{
    theme = Theme::theme();
    setStyleSheet("QPushButton, QLabel {"
                  "border: 0px;"
                  "background: rgba(0,0,0,0);"
                  "}"
                  "QPushButton:hover {"
                  "background:" + theme->getHighlightColorHex() + ";"
                  "}");

    textLabel->setStyleSheet("color:" + theme->getTextColorHex() + ";");
    arrowLabel->setPixmap(theme->getImage("Actions", "Arrow_Right", QSize(28,28), theme->getTextColor()));
}


/**
 * @brief DockActionWidget::enterEvent
 * @param event
 */
void DockActionWidget::enterEvent(QEvent* event)
{
    textLabel->setStyleSheet("color:" + theme->getTextColorHex(theme->CR_SELECTED) + ";");
    arrowLabel->setPixmap(theme->getImage("Actions", "Arrow_Right", QSize(28,28), theme->getTextColor(theme->CR_SELECTED)));
    QPushButton::enterEvent(event);
}


/**
 * @brief DockActionWidget::leaveEvent
 * @param event
 */
void DockActionWidget::leaveEvent(QEvent* event)
{
    textLabel->setStyleSheet("color:" + theme->getTextColorHex() + ";");
    arrowLabel->setPixmap(theme->getImage("Actions", "Arrow_Right", QSize(28,28), theme->getTextColor()));
    QPushButton::leaveEvent(event);
}


/**
 * @brief DockActionWidget::resizeEvent
 */
void DockActionWidget::resizeEvent(QResizeEvent* event)
{
    updateTextLabel();
    QPushButton::resizeEvent(event);
}


/**
 * @brief DockActionWidget::setupLayout
 */
void DockActionWidget::setupLayout()
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


/**
 * @brief DockActionWidget::updateTextLabel
 * @param text
 */
void DockActionWidget::updateTextLabel(QString text)
{
    if (text.isEmpty()) {
        text = fullActionText;
    } else {
        fullActionText = text;
    }
    QFontMetrics fm = textLabel->fontMetrics();
    QString elidedText = fm.elidedText(text, Qt::ElideMiddle, width() - ARROW_WIDTH);
    textLabel->setText(elidedText);
}

