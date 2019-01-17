#include "overlaysplitter.h"
#include "panelwidget.h"

#include "../../Views/NodeView/nodeview.h"

#include <QApplication>
#include <QPainter>
#include <QDebug>

//#define WIDGET_MIN_HEIGHT 800
#define WIDGET_MIN_HEIGHT 400
#define HANDLE_WIDTH 8

/**
 * @brief OverlaySplitter::OverlaySplitter
 * @param parent
 */
OverlaySplitter::OverlaySplitter(QWidget* parent)
    : QSplitter(parent)
{
    maximisedWidgetSize = WIDGET_MIN_HEIGHT;

    fillerWidget = new QWidget(this);
    fillerWidget->setAttribute(Qt::WA_TransparentForMouseEvents);
    addWidget(fillerWidget);

    setOrientation(Qt::Vertical);
    setHandleWidth(HANDLE_WIDTH);
    setStyleSheet("QSplitter{ background: rgba(0,0,0,0); }"
                  "QSplitter::handle:vertical{ background: rgba(0,0,0,0); }");

    // need this to pass mouse move events without having to press first
    setMouseTracking(true);
    installEventFilter(this);
}


/**
 * @brief OverlaySplitter::setWidget
 * @param widget
 * @param location
 */
void OverlaySplitter::setWidget(QWidget* widget, Qt::Alignment location)
{
    if (widget) {

        widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        widgetMinimised(false); // this sets the initial widget size constraints

        if (location == Qt::AlignTop) {
            widgetIndex = 0;
            defaultSizes = {WIDGET_MIN_HEIGHT, 0};
        } else {
            widgetIndex = 1;
            defaultSizes = {0, WIDGET_MIN_HEIGHT};
        }

        insertWidget(widgetIndex, widget);
        setCollapsible(widgetIndex, false);
        setStretchFactor(widgetIndex, 0);
        setStretchFactor(indexOf(fillerWidget), 1);
        setSizes(defaultSizes);

        PanelWidget* panel = qobject_cast<PanelWidget*>(widget);
        if (panel) {
            // adjust size & constraints based on the panel's minimised state
            widgetMinimised(panel->isMinimised());
            connect(panel, &PanelWidget::minimiseTriggered, this, &OverlaySplitter::widgetMinimised);
        }
    }
}


/**
 * @brief OverlaySplitter::widgetMinimised
 * This stops the contained widget from being resizeable when it's been minimised and restores its previous size when it's maximised.
 * @param minimised
 */
void OverlaySplitter::widgetMinimised(bool minimised)
{
    QWidget* widget = this->widget(widgetIndex);
    if (widget) {
        int widgetHeight = widget->sizeHint().height();
        if (minimised) {
            maximisedWidgetSize = sizes().at(widgetIndex);
            widget->setMinimumHeight(widgetHeight);
            widget->setMaximumHeight(widgetHeight);
        } else {
            widgetHeight = maximisedWidgetSize;
            widget->setMinimumHeight(WIDGET_MIN_HEIGHT);
            widget->setMaximumHeight(QWIDGETSIZE_MAX);
        }
        // resize the splitter accordingly when the contained widget is maximised/minimised
        defaultSizes[widgetIndex] = widgetHeight;
        setSizes(defaultSizes);
    }
}


/**
 * @brief OverlaySplitter::eventFilter
 * This filter intersects mouse move events that are contained inside the filler widget and passes it to the node view.
 * @param object
 * @param event
 * @return
 */
bool OverlaySplitter::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::MouseMove || event->type() == QEvent::MouseButtonDblClick) {
        emit mouseEventReceived((QMouseEvent*)event);
    }
    return QSplitter::eventFilter(object, event);
}
