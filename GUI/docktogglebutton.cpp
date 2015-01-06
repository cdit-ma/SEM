#include "docktogglebutton.h"
#include "dockscrollarea.h"
#include "../newmedeawindow.h"

#include <QPainter>
#include <QPainterPath>
#include <QBrush>


/**
 * @brief DockToggleButton::DockToggleButton
 * @param label
 * @param parent
 */
DockToggleButton::DockToggleButton(QString label, NewMedeaWindow *window, QWidget *parent) :
    QPushButton(parent)
{
    selected = false;

    fillColor = QColor(230, 230, 230, 230);
    selectedColor = QColor(230,150,150,230);
    color = fillColor;

    width = 40;
    height = 40;

    setText(label);
    setStyleSheet("font-size: 14px;");
    setMinimumSize(50, 50);

    // make connections
    connect(this, SIGNAL(pressed()), this, SLOT(on_buttonPressed()));
    connect(this, SIGNAL(dockButtonPressed(QString)), window, SLOT(on_dockButtonPressed(QString)));
}


/**
 * @brief DockToggleButton::~DockToggleButton
 */
DockToggleButton::~DockToggleButton()
{
    delete scrollArea;
}


/**
 * @brief DockToggleButton::getSelected
 * Returns this button's selected state.
 * @return
 */
bool DockToggleButton::getSelected()
{
    return selected;
}


/**
 * @brief DockToggleButton::setSelected
 * Sets this button's selected state to b.
 * @param b
 */
void DockToggleButton::setSelected(bool b)
{
    selected = b;
    repaint();
}


/**
 * @brief DockToggleButton::setContainer
 * @param area
 */
void DockToggleButton::setContainer(DockScrollArea *area)
{
    scrollArea = area;
}


/**
 * @brief DockToggleButton::paintEvent
 * Draw the button as an ellipse with and changeits color when it's selected.
 * @param e
 */
void DockToggleButton::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    QPainterPath path;
    QBrush brush(color);
    QRectF shape(0, 0, width, height);

    QVector<QLine> arrow;
    arrow.append(QLine(13,32,23,27));
    arrow.append(QLine(23,27,33,32));

    path.addEllipse(shape);
    painter.fillPath(path, brush);
    painter.drawEllipse(shape);
    painter.drawText(16, 25, text());

    QWidget::paintEvent(e);
}


/**
 * @brief DockToggleButton::hideGroupBox
 * When this button's groupbox was the last one displayed,
 * this method is called by the main window to make sure that
 * it is hidden before another groupbox is displayed.
 */
void DockToggleButton::hideContainer()
{
    // if the groupbox is still visible,
    // force press this button to hide it
    if (selected) {
        emit pressed();
    }
}


/**
 * @brief DockToggleButton::getContainer
 * @return
 */
DockScrollArea* DockToggleButton::getContainer()
{
    return scrollArea;
}


/**
 * @brief DockToggleButton::getWidth
 * @return
 */
int DockToggleButton::getWidth()
{
    return width;
}


/**
 * @brief DockToggleButton::on_buttonPressed
 * When this button is pressed, its selected state is updated
 * and its text() is sent to the main window.
 */
void DockToggleButton::on_buttonPressed()
{
    if (selected) {
        selected = false;
        color = fillColor;
    } else {
        selected = true;
        color = selectedColor;
    }

    emit dockButtonPressed(this->text());
    repaint();
}


/**
 * @brief DockToggleButton::checkEnabled
 * This method checks if this button is enabled.
 * If it's not, hide it's container.
 */
void DockToggleButton::checkEnabled()
{
    if (!isEnabled()) {
        hideContainer();
    }
}
