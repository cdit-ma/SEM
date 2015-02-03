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

    fillColor = QColor(250, 250, 250, 250);
    selectedColor = QColor(250, 160, 0, 250);
    color = fillColor;

    width = 40;
    height = 40;

    setText(label);
    setFixedSize(width + 10, height + 10);

    QPixmap pixmap = 0;
    if (label == "P") {
        //pixmap = QPixmap::fromImage(QImage(":/Resources/Icons/Workload.png"));
        pixmap = QPixmap::fromImage(QImage(":/Resources/Icons/parts.png"));
    } else if (label == "H") {
        pixmap = QPixmap::fromImage(QImage(":/Resources/Icons/HardwareNode.png"));
    } else if (label == "D") {
        pixmap = QPixmap::fromImage(QImage(":/Resources/Icons/Component.png"));
    }

    QVBoxLayout* vLayout = new QVBoxLayout();
    QLabel* imageLabel = new QLabel(this);
    QPixmap scaledPixmap = pixmap.scaled(this->size()*0.6, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    imageLabel->setStyleSheet("padding: 3px 0px 8.5px 6.5px;");
    imageLabel->setFixedSize(this->size());
    imageLabel->setPixmap(scaledPixmap);

    vLayout->setMargin(0);
    vLayout->addWidget(imageLabel);
    setLayout(vLayout);

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
 * Draw the button as an ellipse and change its color when it's selected.
 * The rect used for the path is moved to (1,1) so that the full pen width is painted.
 * @param e
 */
void DockToggleButton::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    QBrush brush(color);

    QPen pen;
    if (isEnabled()) {
        pen.setColor(QColor(120,120,120));
    } else {
        pen.setColor(QColor(150,150,150));
    }
    pen.setWidth(2);
    painter.setPen(pen);

    // fill the ellipse
    QPainterPath path1;
    QRectF rect1(1, 1, width+1, height+1);
    path1.addEllipse(rect1);
    painter.fillPath(path1, brush);

    // draw the ellipse
    QRectF rect2(1, 1, width+2, height+2);
    QPainterPath path2;
    path2.addEllipse(rect2);
    painter.drawPath(path2);

    //painter.drawText(16, 25, text());

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
 * This method returns the dock container attached to this button.
 * @return scrollArea
 */
DockScrollArea* DockToggleButton::getContainer()
{
    return scrollArea;
}


/**
 * @brief DockToggleButton::getWidth
 * Returns the width of this button.
 * @return width
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
