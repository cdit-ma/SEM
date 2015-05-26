#ifndef TEST_H
#define TEST_H

#include <QGraphicsItem>

class Test : public QGraphicsItem
{
public:
    Test(QGraphicsItem* parent = 0);

signals:

public slots:


    // QGraphicsItem interface
public:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

#endif // TEST_H
