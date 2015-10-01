#include "aspecttogglewidget.h"
#include "../medeawindow.h"

#define HEIGHT_OFFSET 1.4
#define SHADOW_OFFSET 3

#define DEFAULT -1
#define CLICKED 0
#define DOUBLECLICKED 1
#define MIDDLECLICKED 2


/**
 * @brief AspectToggleWidget::AspectToggleWidget
 * @param text
 * @param size
 * @param parent
 */
AspectToggleWidget::AspectToggleWidget(QString text, double size, MedeaWindow* parent) :
    QWidget(parent)
{
    shadowFrame = new QFrame(this);
    mainFrame = new QFrame(this);
    aspectText = text;

    CHECKED = false;
    STATE = DEFAULT;

    setupColor();
    setupLayout(size);

    connect(this, SIGNAL(aspectToggle_clicked(bool,int)), parent, SLOT(aspectToggleClicked(bool,int)));
    connect(parent, SIGNAL(window_aspectToggleDoubleClicked(AspectToggleWidget*)), this, SLOT(aspectDoubleClicked(AspectToggleWidget*)));
}


/**
 * @brief AspectToggleWidget::getText
 * This returns the label/name of the aspect this toggle is linked to.
 * @return
 */
QString AspectToggleWidget::getText()
{
    return aspectText;
}


/**
 * @brief AspectToggleWidget::click
 * This is triggered when this toggle is clicked.
 * It checks what kind of click the event was and sets the CHECKED state
 * depeding on that. It sends a signal to the window that this was clicked.
 * @param checked - set CHECKED to this if state allows it
 * @param state - CLICKED, DOUBLECLICKED, MIDDLECLICKED
 */
void AspectToggleWidget::click(bool checked, int state)
{
    switch (state) {
    case DEFAULT:
        break;
    case CLICKED:
        CHECKED = checked;
        break;
    case DOUBLECLICKED:
        CHECKED = true;
        break;
    case MIDDLECLICKED:
        CHECKED = true;
        break;
    }

    // this tells the window to turn this aspect toggle on/off
    emit aspectToggle_clicked(CHECKED, state);
    updateStyleSheet();
}


/**
 * @brief AspectToggleWidget::isClicked
 * This returns the CHECKED state of this toggle.
 * @return
 */
bool AspectToggleWidget::isClicked()
{
    return CHECKED;
}


/**
 * @brief AspectToggleWidget::setClicked
 * This sets the CHECKED state of this toggle and updates its stylesheet.
 * @param checked - new CHECKED state
 */
void AspectToggleWidget::setClicked(bool checked)
{
    CHECKED = checked;
    updateStyleSheet();
}


/**
 * @brief AspectToggleWidget::mouseReleaseEvent
 * This triggers the click event when the mouse is released within the bounds
 * of this toggle. It sends the state of the click, and then resets it.
 * @param event
 */
void AspectToggleWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (rect().contains(event->pos())) {
        switch (event->button()) {
        case Qt::LeftButton:
            if (STATE !=  DOUBLECLICKED) {
                STATE = CLICKED;
            }
            break;
        case Qt::MiddleButton:
            STATE = MIDDLECLICKED;
            break;
        default:
            break;
        }
        click(!CHECKED, STATE);

    }

    // reset the state
    STATE = DEFAULT;
}


/**
 * @brief AspectToggleWidget::mouseDoubleClickEvent
 * This updates the current click state of this toggle to DOUBLECLICKED.
 * @param event
 */
void AspectToggleWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        STATE = DOUBLECLICKED;
    }
}


/**
 * @brief AspectToggleWidget::aspectDoubleClicked
 * This is called when an aspect toggle has been double-clicked.
 * If that toggle is not this, then un-click this toggle.
 * @param aspect - aspect that was doucble-clicked
 */
void AspectToggleWidget::aspectDoubleClicked(AspectToggleWidget *aspect)
{
    if (aspect != this) {
        setClicked(false);
    }
}


/**
 * @brief AspectToggleWidget::setupColorMap
 * This sets up the colors used for the default and the gradient.
 */
void AspectToggleWidget::setupColor()
{
    QString defaultAlpha = "255";
    QString checkedAlpha = "245";

    if (aspectText == "Definitions") {
        defaultColor = "rgba(30,130,130," + defaultAlpha + ")";
        p1_Color = "rgba(190,240,240," + checkedAlpha + ")";
        p2_Color = "rgba(170,240,240," + checkedAlpha + ")";
        p3_Color = "rgba(120,210,210," + checkedAlpha + ")";
        p4_Color = "rgba(90,190,190," + checkedAlpha + ")";
    } else if (aspectText == "Workload") {
        defaultColor = "rgba(174,104,46," + defaultAlpha + ")";
        p1_Color = "rgba(255,230,206," + checkedAlpha + ")";
        p2_Color = "rgba(255,230,206," + checkedAlpha + ")";
        p3_Color = "rgba(250,185,136," + checkedAlpha + ")";
        p4_Color = "rgba(234,164,106," + checkedAlpha + ")";
    } else if (aspectText == "Assembly") {
        defaultColor = "rgba(180,80,80," + defaultAlpha + ")";
        p1_Color = "rgba(255,210,210," + checkedAlpha + ")";
        p2_Color = "rgba(255,210,210," + checkedAlpha + ")";
        p3_Color = "rgba(250,160,160," + checkedAlpha + ")";
        p4_Color = "rgba(240,140,140," + checkedAlpha + ")";
    } else if (aspectText == "Hardware") {
        defaultColor = "rgba(40,100,150," + defaultAlpha + ")";
        p1_Color = "rgba(200,220,225," + checkedAlpha + ")";
        p2_Color = "rgba(180,200,225," + checkedAlpha + ")";
        p3_Color = "rgba(140,180,220," + checkedAlpha + ")";
        p4_Color = "rgba(100,160,210," + checkedAlpha + ")";
    }
}


/**
 * @brief AspectToggleWidget::setupLayout
 * This sets up thid toggle widgets layout.
 * @param size - fixed size of this widget
 */
void AspectToggleWidget::setupLayout(double widgetSize)
{
    setFixedSize(widgetSize, widgetSize / HEIGHT_OFFSET);
    setStyleSheet("border: none;");

    shadowFrame->setFixedSize(size().width(), size().height());
    shadowFrame->setStyleSheet("background-color: rgba(10,10,10,120); border-radius: 10px;");

    mainFrame->setFixedSize(size().width() - SHADOW_OFFSET, size().height() - SHADOW_OFFSET);
    updateStyleSheet();

    QFont font = this->font();
    font.setPointSizeF(9);

    QLabel* aspectLabel = new QLabel(aspectText, this);
    aspectLabel->setFont(font);
    aspectLabel->setStyleSheet("background-color: rgba(0,0,0,0);");

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(aspectLabel);
    layout->setAlignment(aspectLabel, Qt::AlignCenter);
    mainFrame->setLayout(layout);
}


/**
 * @brief AspectToggleWidget::updateStyleSheet
 */
void AspectToggleWidget::updateStyleSheet()
{
    if (CHECKED) {
        mainFrame->move(SHADOW_OFFSET, SHADOW_OFFSET);
        mainFrame->setStyleSheet("border-radius: 8px;"
                                 "background-color:"
                                 "qlineargradient(x1: 0, y1: 0, x2: 0, y2: 0.5, x3: 0 y3: 1.0,"
                                 "stop: 0 " + p1_Color + ", stop: 0.5 " + p2_Color + ","
                                 "stop: 0.5 " + p3_Color + ", stop: 1.0 " + p4_Color + ");");
    } else {
        mainFrame->move(0, 0);
        mainFrame->setStyleSheet("border-radius: 8px; background-color:" + defaultColor + ";");
    }
}
