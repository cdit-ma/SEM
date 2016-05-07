#ifndef EDITABLETEXTITEM_H
#define EDITABLETEXTITEM_H

#include <QGraphicsTextItem>
#include <QMimeData>

class EditableTextItem : public QGraphicsTextItem
{
    Q_OBJECT
public:
    explicit EditableTextItem(QGraphicsItem *parent = 0, int maximumLength = 64);
    QRectF boundingRect() const;

    void setAlignment(Qt::Alignment align);
    void setEditMode(bool editMode = true);
    bool isInEditMode();

    bool isTruncated();
    void setFontSize(qreal fontSize);
    void setPlainText(const QString &text);
    void setTextWidth(qreal width);
    QString getStringValue();
    QString getFullValue() const;
    void setEditable(bool edit);


signals:
    void textUpdated(QString data);
    void editableItem_EditModeRequested();
    void editableItem_hasFocus(bool hasFocus);



    // QGraphicsItem interface
protected:
    void focusInEvent(QFocusEvent*);
    void focusOutEvent(QFocusEvent* event);
    void keyPressEvent(QKeyEvent *event);
    void paste();
private:
    QString getTruncatedText(const QString text );

    Qt::Alignment alignment;

    bool editable;
    QString currentFullValue;
    QString currentTruncValue;
    bool inEditingMode;
    qreal textWidth;
    int maxLength;
    QTextDocument* doc;

    // QGraphicsItem interface

    // QGraphicsItem interface
public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

#endif // EDITABLETEXTITEM_H
