#include "customgroupbox.h"
#include "../theme.h"

#define TITLE_FRAME_WIDTH 4
#define TITLE_ARROW_SIZE 16
#define DEFAULT_ICON_SIZE 16

/**
 * @brief CustomGroupBox::CustomGroupBox
 * @param title
 * @param parent
 */
CustomGroupBox::CustomGroupBox(QString title, QWidget* parent)
    : QFrame(parent)
{
    groupTitle = title;
    groupTitleButton = 0;
    widgetsToolbar = 0;

    setupLayout();

    connect(Theme::theme(), &Theme::theme_Changed, this, &CustomGroupBox::themeChanged);
    
    themeChanged();
}


/**
 * @brief CustomGroupBox::setTitlte
 * @param title
 */
void CustomGroupBox::setTitle(QString title)
{
    if (groupTitleButton) {
        groupTitleButton->setText(title);
    }
    groupTitle = title;
}


/**
 * @brief CustomGroupBox::getTitle
 * @return
 */
QString CustomGroupBox::getTitle()
{
    return groupTitle;
}


/**
 * @brief CustomGroupBox::setCheckable
 * @param checkable
 */
void CustomGroupBox::setCheckable(bool checkable)
{
    if (groupTitleButton) {
        groupTitleButton->setCheckable(checkable);
        if (checkable) {
            groupTitleButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        } else {
            groupTitleButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
        }
    }
}


/**
 * @brief CustomGroupBox::setChecked
 * @param checked
 */
void CustomGroupBox::setChecked(bool checked)
{
    if (groupTitleButton) {
        groupTitleButton->setChecked(checked);
    }
}


/**
 * @brief CustomGroupBox::isChecked
 * @return
 */
bool CustomGroupBox::isChecked()
{
    if (groupTitleButton) {
        return groupTitleButton->isChecked();
    }
    return false;
}


/**
 * @brief CustomGroupBox::addWidget
 * @param widget
 * @return
 */
QAction* CustomGroupBox::addWidget(QWidget* widget)
{
    if (widgetsToolbar) {
        return widgetsToolbar->addWidget(widget);
    }
    return 0;
}


/**
 * @brief CustomGroupBox::insertWidget
 * @param beforeAction
 * @param widget
 * @return
 */
QAction* CustomGroupBox::insertWidget(QAction* beforeAction, QWidget *widget)
{
    if (widgetsToolbar) {
        return widgetsToolbar->insertWidget(beforeAction, widget);
    }
    return 0;
}


/**
 * @brief CustomGroupBox::themeChanged
 */
void CustomGroupBox::themeChanged()
{
    Theme* theme = Theme::theme();
    setStyleSheet("QFrame {"
                  "color:" + theme->getAltBackgroundColorHex() + ";"
                  "background: rgba(0,0,0,0);"
                  "}"
                  + theme->getToolBarStyleSheet() +
                  "QToolButton{ border-radius:" + theme->getSharpCornerRadius() + ";}");

    if (groupTitleButton) {
        QString checkableStyle = "";
        if (groupTitleButton->isCheckable()) {
            groupTitleButton->setIcon(theme->getIcon("ToggleIcons", "groupToggle"));
            checkableStyle = "QToolButton::checked {"
                             "background: rgba(0,0,0,0);"
                             "color:" + theme->getTextColorHex() + ";"
                             "}"
                             "QToolButton:hover {"
                             "background:" + theme->getHighlightColorHex() + ";"
                             "color:" + theme->getTextColorHex(ColorRole::SELECTED) + ";"
                             "}";
        }
        groupTitleButton->setStyleSheet("QToolButton {"
                                        "padding: 1px 1px 1px 0px;"
                                        "border: none;"
                                        "color:" + theme->getTextColorHex() + ";"
                                        "background: rgba(0,0,0,0);"
                                        "}"
                                        + checkableStyle);
    }

    widgetsToolbar->setIconSize(theme->getIconSize());
    topToolbar->setIconSize(theme->getIconSize());
}


/**
 * @brief CustomGroupBox::setupLayout
 */
void CustomGroupBox::setupLayout()
{
    widgetsToolbar = new QToolBar(this);
    widgetsToolbar->setOrientation(Qt::Vertical);
    widgetsToolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    groupTitleButton = new QToolButton();
    groupTitleButton->setText(groupTitle);
    groupTitleButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    groupTitleButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    groupTitleButton->setCheckable(true);
    groupTitleButton->setChecked(true);

    connect(groupTitleButton, &QToolButton::toggled, widgetsToolbar, &QToolBar::setVisible);

    QFrame* leftTitleFrame = new QFrame(this);
    leftTitleFrame->setFrameShape(QFrame::HLine);
    leftTitleFrame->setLineWidth(TITLE_FRAME_WIDTH);
    leftTitleFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QFrame* rightTitleFrame = new QFrame(this);
    rightTitleFrame->setFrameShape(QFrame::HLine);
    rightTitleFrame->setLineWidth(TITLE_FRAME_WIDTH);
    rightTitleFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    topToolbar = new QToolBar(this);
    topToolbar->addWidget(groupTitleButton);
    //topToolbar->setLayoutDirection(Qt::RightToLeft);

    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->setMargin(0);
    topLayout->addWidget(leftTitleFrame);
    //topLayout->addSpacerItem(new QSpacerItem(1,0));
    topLayout->addWidget(topToolbar);
    topLayout->addSpacerItem(new QSpacerItem(1,0));
    topLayout->addWidget(rightTitleFrame);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 2, 0, 2);
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(widgetsToolbar);
}
