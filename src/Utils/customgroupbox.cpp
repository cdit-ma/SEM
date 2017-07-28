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
void CustomGroupBox::setTitlte(QString title)
{
    if (groupTitleButton) {
        groupTitleButton->setText(title);
    }
    groupTitle = title;
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
        /*
        QIcon icon = Theme::theme()->getIcon("Icons", "arrowHeadVerticalToggle");
        icon.addPixmap(theme->getImage("Icon", "arrowHeadDown", QSize(), QColor(250,0,0)), QIcon::Normal, QIcon::On);
        icon.addPixmap(theme->getImage("Icon", "arrowHeadUp", QSize(), QColor(250,0,0)), QIcon::Normal, QIcon::Off);
        icon.addPixmap(theme->getImage("Icon", "arrowHeadDown", QSize(), theme->getHighlightColor()), QIcon::Active, QIcon::On);
        icon.addPixmap(theme->getImage("Icon", "arrowHeadUp", QSize(), theme->getHighlightColor()), QIcon::Active, QIcon::Off);
        groupTitleButton->setIcon(icon);
        */
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

    foreach (QAction* action, widgetsToolbar->actions()) {
        QToolButton* button = qobject_cast<QToolButton*>(widgetsToolbar->widgetForAction(action));
        QString iconPath = button->property("iconPath").toString();
        QString iconName = button->property("iconName").toString();
        button->setIcon(theme->getIcon(iconPath, iconName));
    }
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
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(widgetsToolbar);
}
