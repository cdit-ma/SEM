#include "dockwidgetitem.h"

#define MIN_BUTTON_WIDTH 50

/**
 * @brief DockWidgetItem::DockWidgetItem
 * @param parent
 */
DockWidgetItem::DockWidgetItem(QString text, QWidget* parent) :
    QToolButton(parent)
{
    iconWidth = 0;

    displayToolButtonText(true);
    setText(text);

    setEnabled(false);
    setMinimumWidth(MIN_BUTTON_WIDTH);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    themeChanged();
}


/**
 * @brief DockWidgetItem::setText
 * @param text
 */
void DockWidgetItem::setText(QString text)
{
    fullText = text;
    updateTextLabel();
    setToolTip(fullText);
}


/**
 * @brief DockWidgetItem::getText
 * @return
 */
QString DockWidgetItem::getText()
{
    return fullText;
}


/**
 * @brief DockWidgetItem::getDisplayedText
 * @return
 */
QString DockWidgetItem::getDisplayedText()
{
    return currentText;
}


/**
 * @brief DockWidgetItem::displayToolButtonText
 * This is the only way to hide the text of the toolbutton.
 * ToolButtonStyle Qt::ToolButtonIconOnly doesn't hide the text if a text has been set.
 * @param show
 */
void DockWidgetItem::displayToolButtonText(bool show)
{
    displayText = show;
    if (show) {
        updateTextLabel();
    } else {
        QToolButton::setText("");
    }
}


/**
 * @brief DockWidgetItem::setIconSize
 * @param width
 * @param height
 */
void DockWidgetItem::setIconSize(int width, int height)
{
    iconWidth = width + 5;
    QToolButton::setIconSize(QSize(width, height));
}


/**
 * @brief DockWidgetItem::getItemKind
 * @return
 */
DockWidgetItem::DOCKITEM_KIND DockWidgetItem::getItemKind()
{
    return DEFAULT_ITEM;
}


/**
 * @brief DockWidgetItem::themeChanged
 */
void DockWidgetItem::themeChanged()
{
    Theme* theme = Theme::theme();
    setStyleSheet(theme->getToolBarStyleSheet() +
                  "QToolButton:disabled {"
                  "border: 0px;"
                  "background: rgba(0,0,0,0);"
                  "}");
}


/**
 * @brief DockWidgetItem::resizeEvent
 * @param event
 */
void DockWidgetItem::resizeEvent(QResizeEvent* event)
{
    updateTextLabel();
    QToolButton::resizeEvent(event);
}


/**
 * @brief DockWidgetItem::updateTextLabel
 * This truncates the text to fit the width of the button with a bit of padding.
 */
void DockWidgetItem::updateTextLabel()
{
    int maxTextWidth = width() - iconWidth - 10;
    QFontMetrics fm = fontMetrics();
    QString elidedText = fm.elidedText(fullText, Qt::ElideMiddle, maxTextWidth);

    if (elidedText != currentText) {
        currentText = elidedText;
        emit displayedTextChanged(elidedText);
        if (displayText) {
            QToolButton::setText(elidedText);
        }
    }
}

