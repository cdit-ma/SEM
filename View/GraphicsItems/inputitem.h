#ifndef INPUTITEM_H
#define INPUTITEM_H
#include <QGraphicsObject>
#include <QGraphicsProxyWidget>
#include "editabletextitem.h"
#include "graphmlitem.h"
#include "graphicscombobox.h"
#include <QComboBox>
#include <QTime>

class InputItem : public QGraphicsObject
{
    Q_OBJECT
public:
    enum INPUT_ENUM {IE_TEXTBOX, IE_COMBOBOX, IE_NUMBERBOX};
    InputItem(GraphMLItem* parent, QStringList defaultOptions, QString initialValue);
    InputItem(GraphMLItem* parent, bool test, QString initialValue);

    void setCenterAligned(bool center);

    void setHandleMouse(bool on);
    void setFont(QFont font);
    //void setTextAlignment(Qt::AlignmentFlag alignment);

    QString getValue();



    QRectF boundingRect() const;
    QRectF arrowRect() const;
    bool isInEditMode();
    void setWidth(qreal width);
    void setHeight(qreal width);
    qreal getWidth();
    qreal getHeight();

private slots:
    void textValueChanged(QString newValue);
public slots:

    void setValue(QString value="");
    void setEditMode(bool editable =false);

signals:
    void InputItem_ValueChanged(QString newValue);
    void InputItem_EditModeRequested();
    void InputItem_HasFocus(bool focus);

protected:

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

private:
    void setupLayout();
    void updateTextSize();
    QString currentValue;
    bool inEditMode;
    bool handleMouse;
    qreal width;
    qreal height;
    qreal inputWidth;
    qreal inputHeight;
    bool wasDoubleClicked;

    QGraphicsProxyWidget* comboBoxProxy;


    GraphicsComboBox* comboBox;
    EditableTextItem* edititem;

    QTime lastPressed;


    INPUT_ENUM type;

    // QGraphicsItem interface
public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

#endif // INPUTITEM_H
