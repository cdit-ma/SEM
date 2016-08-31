#include "dockwidgetitem.h"

#define MIN_BUTTON_WIDTH 50


/**
 * @brief DockWidgetItem::DockWidgetItem
 * @param parent
 */
DockWidgetItem::DockWidgetItem(QString text, QWidget* parent) :
    QToolButton(parent)
{
    displayToolButtonText(false);
    setText(text);

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
 * @param show
 */
void DockWidgetItem::displayToolButtonText(bool show)
{
    displayText = show;
}


/**
 * @brief DockWidgetItem::themeChanged
 */
void DockWidgetItem::themeChanged()
{
    Theme* theme = Theme::theme();
    setStyleSheet(theme->getToolBarStyleSheet());
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
 */
void DockWidgetItem::updateTextLabel()
{
    QFontMetrics fm = fontMetrics();
    QString elidedText = fm.elidedText(fullText, Qt::ElideMiddle, width() - 10);
    if (elidedText != currentText) {
        currentText = elidedText;
        emit displayedTextChanged(elidedText);
        if (displayText) {
            QToolButton::setText(elidedText);
        }
    }
}

