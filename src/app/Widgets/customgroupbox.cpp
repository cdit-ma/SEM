#include "customgroupbox.h"
#include "../theme.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#define TITLE_FRAME_WIDTH 4

/**
 * @brief CustomGroupBox::CustomGroupBox
 * @param title
 * @param parent
 */
CustomGroupBox::CustomGroupBox(const QString& title, QWidget* parent)
    : QFrame(parent)
{
    setupLayout();

	if (groupTitleButton) {
		groupTitleButton->setText(title);
	}

    connect(Theme::theme(), &Theme::theme_Changed, this, &CustomGroupBox::themeChanged);
    themeChanged();
}

/**
 * @brief CustomGroupBox::setTitle
 * @param title
 */
void CustomGroupBox::setTitle(const QString& title)
{
    if (groupTitleButton) {
        groupTitleButton->setText(title);
    }
}

/**
 * @brief CustomGroupBox::getTitle
 * @return
 */
QString CustomGroupBox::getTitle()
{
    if (groupTitleButton) {
        return groupTitleButton->text();
    }
    return "";
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
    return nullptr;
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
    return nullptr;
}

/**
 * @brief CustomGroupBox::themeChanged
 */
void CustomGroupBox::themeChanged()
{
    Theme* theme = Theme::theme();
    auto frame_color = theme->getAltTextColor();
    frame_color.setAlphaF(0.5);

    setStyleSheet("QFrame {"
                  "color:" + Theme::QColorToHex(frame_color) + ";"
                  "background: rgba(0,0,0,0);"
                  "}");

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

    topToolbar->setIconSize(theme->getIconSize());
    widgetsToolbar->setIconSize(theme->getIconSize());
    widgetsToolbar->setStyleSheet(theme->getToolBarStyleSheet() +
                                  "QToolButton{ border-radius:" + theme->getSharpCornerRadius() + ";}");
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
    groupTitleButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    groupTitleButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    groupTitleButton->setCheckable(true);
    groupTitleButton->setChecked(true);
    connect(groupTitleButton, &QToolButton::toggled, widgetsToolbar, &QToolBar::setVisible);

    auto leftTitleFrame = new QFrame(this);
    leftTitleFrame->setFrameShape(QFrame::HLine);
    leftTitleFrame->setLineWidth(TITLE_FRAME_WIDTH);

	auto rightTitleFrame = new QFrame(this);
    rightTitleFrame->setFrameShape(QFrame::HLine);
    rightTitleFrame->setLineWidth(TITLE_FRAME_WIDTH);

    topToolbar = new QToolBar(this);
    topToolbar->addWidget(groupTitleButton);
    topToolbar->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

	auto topLayout = new QHBoxLayout();
    topLayout->setMargin(0);
    topLayout->addWidget(leftTitleFrame, 1);
    topLayout->addSpacerItem(new QSpacerItem(1,0));
    topLayout->addWidget(topToolbar, 0);
    topLayout->addSpacerItem(new QSpacerItem(1,0));
    topLayout->addWidget(rightTitleFrame, 1);

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 2, 0, 2);
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(widgetsToolbar);
}