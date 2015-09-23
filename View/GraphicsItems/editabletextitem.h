#ifndef EDITABLETEXTITEM_H
#define EDITABLETEXTITEM_H

#include <QGraphicsTextItem>

class EditableTextItem : public QGraphicsTextItem
{
    Q_OBJECT
public:
    explicit EditableTextItem(QGraphicsItem *parent = 0, int maximumLength = 64);
    void setEditMode(bool editMode = true);
    void setPlainText(const QString &text);
    void setTextWidth(qreal width);
    void setCenterJustified();
    QString getStringValue();
    QString getFullValue();
    void setEditable(bool edit);

signals:
    void textUpdated(QString data);

    void editableItem_hasFocus(bool hasFocus);

    // QGraphicsItem interface
protected:
    void focusInEvent(QFocusEvent*);
    void focusOutEvent(QFocusEvent* event);
    void keyPressEvent(QKeyEvent *event);



private:
    QString getTruncatedText(const QString text );

    bool editable;
    bool centerJustified;
    QString currentFullValue;
    QString currentTruncValue;
    bool inEditingMode;
    qreal textWidth;
    int maxLength;
    QTextDocument* doc;


};

#endif // EDITABLETEXTITEM_H
