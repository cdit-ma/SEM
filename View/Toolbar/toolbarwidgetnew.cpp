#include "toolbarwidgetnew.h"
#include <QDebug>


/**
 * @brief ToolbarWidgetNew::ToolbarWidgetNew
 * @param ac
 * @param parent
 */
ToolbarWidgetNew::ToolbarWidgetNew(ActionController* ac, QWidget *parent) : QWidget(parent)
{
    // using frames, combined with the set attribute and flags, allow
    // the toolbar to have a translucent background and a mock shadow
    setAttribute(Qt::WA_TranslucentBackground);

#ifdef Q_OS_WIN32
    // toolbar with a Qt::Popup flag doesn't allow hover/icon state changes in Ubuntu
    setWindowFlags(Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::Popup);
#else
    setWindowFlags(Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::Popup | Qt::Dialog);
#endif

    actionController = ac;
    iconSize = QSize(20,20);

    setupToolbar();
    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
}


/**
 * @brief ToolbarWidgetNew::themeChanged
 */
void ToolbarWidgetNew::themeChanged()
{
    Theme* theme = Theme::theme();
    QString backgroundColor = theme->getAltBackgroundColorHex();
    QString highlightColor = theme->getHighlightColorHex();
    QString textColor = theme->getTextColorHex(Theme::CR_NORMAL);
    QString textSelectedColor = theme->getTextColorHex(Theme::CR_SELECTED);

    QColor shadowColor = theme->getBackgroundColor().darker(130);
    QString shadowColorStr = theme->QColorToHex(shadowColor);
    QColor mainColor = theme->getAltBackgroundColor().darker(130);
    QString mainBackgroundColor = theme->QColorToHex(mainColor);

    setStyleSheet("QRadioButton {"
                  "padding: 8px 10px 8px 8px;"
                  "color:" + textColor +
                  "}"
                  "QRadioButton::checked {"
                  "font-weight: bold; "
                  "color:" + highlightColor + ";"
                  "}"
                  "QMenu { "
                  "background:" + backgroundColor + ";"
                  "}"
                  "QMenu::item {"
                  "padding: 1px 20px 1px 45px;"
                  "background:" + backgroundColor + ";"
                  "color:" + textColor + ";"
                  "border: none;"
                  "}"
                  "QMenu::item:selected {"
                  "background:" + highlightColor + ";"
                  "color:" + textSelectedColor + ";"
                  "border: 1px solid gray;"
                  "}"
                  "QMenu::icon {"
                  "position: absolute;"
                  "top: 1px;"
                  "right: 3px;"
                  "bottom: 1px;"
                  "left: 4px;"
                  "}"
                  "QToolBar {"
                  "spacing: 3px;"
                  "padding: 3px;"
                  /*
                  "margin: 0px;"
                  "border-radius: 3px;"
                  "border-left: 1px inset " + altBackgroundColor + ";"
                  "border-top: 1px inset " + altBackgroundColor + ";"
                  "border-right: 2px outset " + shadowColorStr + ";"
                  "border-bottom: 2px outset " + shadowColorStr + ";"
                  */
                  "}"
                  "QToolBar::separator {"
                  "width: 5px;"
                  "background: rgba(0,0,0,0);"
                  "}"
                  "QToolButton {"
                  "padding: 4px;"
                  //"border: 1px solid " + disabledBGColor + ";"
                  "border-radius: 5px;"
                  "background:" + backgroundColor + ";"
                  "}"
                  "QToolButton:hover {"
                  "background:" + highlightColor + ";"
                  "}"
                  "QToolButton:pressed {"
                  "background:" + highlightColor + ";"
                  "}"
                  );

    mainFrame->setStyleSheet("background:" + mainBackgroundColor + "; border-radius: 6px;");
    shadowFrame->setStyleSheet("background:" + shadowColorStr + "; border-radius: 8px;");
}


/**
 * @brief ToolbarWidgetNew::setVisible
 * @param visible
 */
void ToolbarWidgetNew::setVisible(bool visible)
{
    if (visible) {
        actionController->contextToolbar->updateSpacers();
        QSize toolbarSize = toolbar->sizeHint();
        if (visible && toolbarSize.width() > iconSize.width()) {
            mainFrame->setFixedSize(toolbarSize);
            shadowFrame->setFixedSize(mainFrame->size() + QSize(3,3));
            setFixedSize(shadowFrame->size());
        } else {
            visible = false;
        }
    }

    mainFrame->setVisible(visible);
    shadowFrame->setVisible(visible);
    QWidget::setVisible(visible);
}


/**
 * @brief ToolbarWidgetNew::setupToolbar
 */
void ToolbarWidgetNew::setupToolbar()
{
    shadowFrame = new QFrame(this);
    mainFrame = new QFrame(this);

    toolbar = new QToolBar(this);
    toolbar->setIconSize(iconSize);

    if (actionController) {
        toolbar->addActions(actionController->contextToolbar->actions());
    }

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(toolbar, 0, Qt::AlignTop);
}

