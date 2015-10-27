#ifndef INPUTITEM_H
#define INPUTITEM_H
#include <QGraphicsObject>
#include <QGraphicsProxyWidget>
#include "editabletextitem.h"
#include "graphmlitem.h"
#include <QComboBox>
#include <QTime>
#include <QListWidget>

class InputItem : public QGraphicsObject
{
    Q_OBJECT
public:
    InputItem(GraphMLItem* parent, QString initialValue, bool isCombo);
    void setRightAligned(bool right);

    void setAlignment(Qt::Alignment align);
    void setBrush(QBrush brush);

    void setToolTipString(QString tooltip);


    void setDropDown(bool isCombo);
    void setHandleMouse(bool on);
    void setFont(QFont font);

    void updatePosSize(QRectF size);
    //void setTextAlignment(Qt::AlignmentFlag alignment);

    QString getValue();



    QRectF boundingRect() const;
    QRectF arrowRect() const;
    bool isInEditMode();
    void setWidth(qreal width);
    void setHeight(qreal width);
    qreal getWidth();
    qreal getHeight();

public slots:
    void setValue(QString value="");
    void setEditMode(bool editable);

signals:
    void InputItem_ValueChanged(QString newValue);
    void InputItem_EditModeRequested();

    void InputItem_HasFocus(bool focus);

protected:

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

private:
    void setupLayout(QString initialValue);
    void updateTextSize();

    bool handleMouse;
    bool isComboBox;

    qreal width;
    qreal height;

    bool wasDoubleClicked;
    QString _tooltip;

    EditableTextItem* textItem;

    QTime lastPressed;
    QBrush backgroundBrush;


    // QGraphicsItem interface
public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

#endif // INPUTITEM_H
