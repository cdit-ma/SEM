#include "docktogglebutton.h"
#include "dockscrollarea.h"

#include "../../medeawindow.h"

#include <QPainter>
#include <QPainterPath>
#include <QBrush>


/**
 * @brief DockToggleButton::DockToggleButton
 * @param label
 * @param parent
 */
DockToggleButton::DockToggleButton(QString label, MedeaWindow *window, QWidget *parent) :
    QPushButton(parent)
{
    selected = false;

    defaultPenWidth = 1;
    selectedPenWidth = 2;
    penWidth = defaultPenWidth;

    defaultPenColor = QColor(120,120,120);
    disabledPenColor = QColor(150,150,150);
    selectedPenColor = Qt::blue;
    penColor = defaultPenColor;

    defaultBrushColor = QColor(235, 235, 235);
    disabledBrushColor = QColor(205, 205, 205);
    selectedBrushColor = Qt::white;
    brushColor = defaultBrushColor;

    kind = label;
    width = 40;
    height = 40;

    setText(label);
    setFixedSize(width + 10, height + 10);

    QPixmap pixmap = 0;
    if (label == "P") {
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

    connect(this, SIGNAL(dockButton_pressed(QString)), window, SLOT(dockButtonPressed(QString)));
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
    QPen pen(penColor);
    QBrush brush(brushColor);

    pen.setWidth(penWidth);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing, true);

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
    // if the groupbox is still visible, force press this button to hide it
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

QString DockToggleButton::getKind()
{
    return kind;
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
        setColor(DEFAULT);
    } else {
        selected = true;
        setColor(SELECTED);
    }
    emit dockButton_pressed(this->text());
}


/**
 * @brief DockToggleButton::enableDock
 * @param enable
 */
void DockToggleButton::enableDock(bool enable)
{
   if (!enable) {
       hideContainer();
   }
   setEnabled(enable);
}


/**
 * @brief DockToggleButton::setColor
 * This sets the colour of this toggle buttopn depending on its current state.
 */
void DockToggleButton::setColor(int state)
{
    penWidth = defaultPenWidth;
    if (state == DEFAULT) {
        penColor = defaultPenColor;
        brushColor = defaultBrushColor;
    } else if (state == DISABLED) {
        penColor = disabledPenColor;
        brushColor = disabledBrushColor;
    } else if (state = SELECTED) {
        penWidth = selectedPenWidth;
        penColor = selectedPenColor;
        brushColor = selectedBrushColor;
    }
    repaint();
}


/**
 * @brief DockToggleButton::setEnabled
 * @param enable
 */
void DockToggleButton::setEnabled(bool enable)
{
    // if there is no change to this button's enabled state, don't change its colour
    if (isEnabled() == enable) {
        return;
    }
    if (enable) {
        setColor(DEFAULT);
    } else {
        setColor(DISABLED);
    }
    QPushButton::setEnabled(enable);
}
