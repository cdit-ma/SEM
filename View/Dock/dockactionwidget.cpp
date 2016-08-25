#include "dockactionwidget.h"
#include "../theme.h"

#define ICON_SIZE 48
#define MIN_BUTTON_SIZE 75
#define ARROW_WIDTH 20

/**
 * @brief DockActionWidget::DockActionWidget
 * @param action
 * @param parent
 */
DockActionWidget::DockActionWidget(QAction *action, QWidget *parent) : QPushButton(parent)
{
    dockAction = action;

    setupLayout();
    requiresSubAction(false);
    actionChanged();

    //connect(this, SIGNAL(clicked(bool)), action, SIGNAL(triggered(bool)));
    //connect(action, SIGNAL(toggled(bool)), this, SIGNAL(toggled(bool)));
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
 * @brief DockActionWidget::getAction
 * @return
 */
QAction *DockActionWidget::getAction()
{
    return dockAction;
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
    //Theme* theme = Theme::theme();
    //setStyleSheet();
}


/**
 * @brief DockActionWidget::setupLayout
 */
void DockActionWidget::setupLayout()
{
    int margin = 4;

    iconLabel = new QLabel(this);
    textLabel = new QLabel(this);

    iconLabel->setAlignment(Qt::AlignHCenter);
    iconLabel->setMinimumWidth(MIN_BUTTON_SIZE - margin*2);
    iconLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    arrowLabel = new QLabel(this);
    arrowLabel->setAlignment(Qt::AlignRight);
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

