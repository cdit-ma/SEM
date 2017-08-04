#include "customgroupbox.h"
#include "../theme.h"

#define TITLE_FRAME_WIDTH 4
#define TITLE_ARROW_SIZE 20

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
 * @brief CustomGroupBox::setIconSize
 * @param size
 */
void CustomGroupBox::setIconSize(QSize size)
{
    if (groupTitleButton) {
        groupTitleButton->setIconSize(size);
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
 * @brief CustomGroupBox::getTopAction
 * @return
 */
QAction* CustomGroupBox::getTopAction()
{
    if (widgetsToolbar) {
        if (!widgetsToolbar->actions().isEmpty()) {
            return widgetsToolbar->actions().at(0);
        }
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
        groupTitleButton->setIcon(theme->getIcon("Icons", "folderToggle"));
        groupTitleButton->setStyleSheet("QToolButton {"
                                        "padding: 1px;"
                                        //"padding-left: 4px;"
                                        "border: none;"
                                        "background: rgba(0,0,0,0);"
                                        "}"
                                        "QToolButton::checked {"
                                        "background: rgba(0,0,0,0);"
                                        "color:" + theme->getTextColorHex() + ";"
                                        "}"
                                        "QToolButton:hover {"
                                        "background:" + theme->getHighlightColorHex() + ";"
                                        "color:" + theme->getTextColorHex(Theme::CR_SELECTED) + ";"
                                        "}");
    }
    /*
    foreach (QAction* action, widgetsToolbar->actions()) {
        QToolButton* button = qobject_cast<QToolButton*>(widgetsToolbar->widgetForAction(action));
        QString iconPath = button->property("iconPath").toString();
        QString iconName = button->property("iconName").toString();
        button->setIcon(theme->getIcon(iconPath, iconName));
    }*/
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
    leftTitleFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QFrame* rightTitleFrame = new QFrame(this);
    rightTitleFrame->setFrameShape(QFrame::HLine);
    rightTitleFrame->setLineWidth(TITLE_FRAME_WIDTH);
    rightTitleFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QToolBar* topToolbar = new QToolBar(this);
    topToolbar->setIconSize(QSize(TITLE_ARROW_SIZE, TITLE_ARROW_SIZE));
    topToolbar->addWidget(groupTitleButton);
    //topToolbar->setLayoutDirection(Qt::RightToLeft);

    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->setMargin(0);
    topLayout->addWidget(leftTitleFrame);
    topLayout->addSpacerItem(new QSpacerItem(1,0));
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
