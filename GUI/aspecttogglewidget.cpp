#include "aspecttogglewidget.h"
#include "../medeawindow.h"

#define DEFAULT -1
#define CLICKED 0
#define DOUBLECLICKED 1
#define MIDDLECLICKED 2

#define SHADOW_OFFSET 3


/**
 * @brief AspectToggleWidget::AspectToggleWidget
 * @param text
 * @param color
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
 * @return
 */
QString AspectToggleWidget::getText()
{
    return aspectText;
}


/**
 * @brief AspectToggleWidget::click
 * @param checked
 * @param doubleClick
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

    emit aspectToggle_clicked(CHECKED, state);
    updateStyleSheet();
}


/**
 * @brief AspectToggleWidget::clicked
 * @return
 */
bool AspectToggleWidget::clicked()
{
    return CHECKED;
}


/**
 * @brief AspectToggleWidget::setClicked
 * @param checked
 */
void AspectToggleWidget::setClicked(bool checked)
{
    CHECKED = checked;
    updateStyleSheet();
}


/**
 * @brief AspectToggleWidget::mouseReleaseEvent
 * @param event
 */
void AspectToggleWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        if (STATE !=  DOUBLECLICKED) {
            STATE = CLICKED;
        }
    } else if (event->button() == Qt::MiddleButton) {
        STATE = MIDDLECLICKED;
    }

    click(!CHECKED, STATE);
    STATE = DEFAULT;
}


/**
 * @brief AspectToggleWidget::mouseDoubleClickEvent
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
 * @param aspect
 */
void AspectToggleWidget::aspectDoubleClicked(AspectToggleWidget *aspect)
{
    if (aspect != this) {
        setClicked(false);
    }
}


/**
 * @brief AspectToggleWidget::setupColorMap
 */
void AspectToggleWidget::setupColor()
{
    QString defaultAlpha = "255";
    QString checkedAlpha = "220";

    if (aspectText == "Definitions") {
        defaultColor = "rgba(80,180,180," + defaultAlpha + ")";
        p1_Color = "rgba(190,240,240," + checkedAlpha + ")";
        p2_Color = "rgba(170,240,240," + checkedAlpha + ")";
        p3_Color = "rgba(120,210,210," + checkedAlpha + ")";
        p4_Color = "rgba(90,190,190," + checkedAlpha + ")";
    } else if (aspectText == "Behaviour") {
        defaultColor = "rgba(224,154,96," + defaultAlpha + ")";
        p1_Color = "rgba(255,230,206," + checkedAlpha + ")";
        p2_Color = "rgba(255,230,206," + checkedAlpha + ")";
        p3_Color = "rgba(250,185,136," + checkedAlpha + ")";
        p4_Color = "rgba(234,164,106," + checkedAlpha + ")";
    } else if (aspectText == "Assembly") {
        defaultColor = "rgba(230,130,130," + defaultAlpha + ")";
        p1_Color = "rgba(255,210,210," + checkedAlpha + ")";
        p2_Color = "rgba(255,210,210," + checkedAlpha + ")";
        p3_Color = "rgba(250,160,160," + checkedAlpha + ")";
        p4_Color = "rgba(240,140,140," + checkedAlpha + ")";
    } else if (aspectText == "Hardware") {
        defaultColor = "rgba(80,140,190," + defaultAlpha + ")";
        p1_Color = "rgba(190,210,215," + checkedAlpha + ")";
        p2_Color = "rgba(170,190,215," + checkedAlpha + ")";
        p3_Color = "rgba(130,170,210," + checkedAlpha + ")";
        p4_Color = "rgba(90,150,200," + checkedAlpha + ")";
    }
}


/**
 * @brief AspectToggleWidget::setupLayout
 * @param color
 * @param size
 */
void AspectToggleWidget::setupLayout(double size)
{
    setFixedSize(size + SHADOW_OFFSET, size + SHADOW_OFFSET);
    setStyleSheet("border: none;");

    shadowFrame->setFixedSize(size + SHADOW_OFFSET, size);
    shadowFrame->setStyleSheet("background-color: rgba(10,10,10,100); border-radius: 10px;");

    mainFrame->setFixedSize(size, size - SHADOW_OFFSET);
    updateStyleSheet();

    QFont font = this->font();
    font.setPointSize(10);

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
