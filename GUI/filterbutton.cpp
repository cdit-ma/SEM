#include "filterbutton.h"
#include <QPaintEvent>
#include <QLayout>
#include <QPen>
#include <QLabel>
#include <QSizePolicy>
#include <QPainter>
#include <QDebug>

FilterButton::FilterButton(QString filter, QWidget *parent) :
    QWidget(parent)
{

    QHBoxLayout* hLayout = new QHBoxLayout(this);
    label = new QLabel(this);
    button = new QPushButton(this);
    button->setText("x");
    button->setFixedHeight(20);
    button->setFixedWidth(20);
    button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);


    hLayout->addWidget(label);
    hLayout->addWidget(button);



    setFilter(filter);
    connect(button, SIGNAL(clicked()),this, SLOT(buttonPressed()));
}

FilterButton::~FilterButton()
{
    //qCritical() << "Deleting FilterButton";
}

void FilterButton::setFilter(QString f)
{
    filter = f;
    label->setText("\"" + filter + "\"");
}

QString FilterButton::getFilter()
{
    return filter;
}

void FilterButton::buttonPressed()
{
    emit removeFilter(filter);
}

void FilterButton::paintEvent(QPaintEvent *pe)
{

    QPainter painter(this);

     QBrush Brush(QColor(255,0,0,50));

    QRectF rectangle(0, 4, width()-4, 28);
    painter.fillRect(rectangle, Brush);

    QWidget::paintEvent(pe);
}
