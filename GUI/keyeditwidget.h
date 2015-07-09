#ifndef KEYEDITWIDGET_H
#define KEYEDITWIDGET_H

#include <QWidget>
#include <QTextBrowser>
#include <QVBoxLayout>


enum KEY_TYPE{
    KEY_STRING = 0,
    KEY_INT,
    KEY_DOUBLE,
    KEY_BOOL,
    KEY_FILE
};
class AppSettings;
class KeyEditWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KeyEditWidget(QString groupName, QString keyName, QString keyNameHR, QVariant valueVar, QString description="", QString customType="");
    QString getKeyName();
    QString getGroupName();
    QString getValue();

    void setLabelWidth(int labelWidth);
    void setValue(QVariant value);

    void setHighlighted(bool highlighted);
signals:
    void valueChanged(QString groupName, QString keyName, QString value);


public slots:
    void _boolChanged(bool value);
    void _valueChanged(QString value);
    void _editingFinished();


private:
    void updatePallete();
    QString keyName;
    QString hrKeyName;

    QString groupName;
    QString newValue;
    QString oldValue;
    QString descriptionValue;

    bool highlighted;

    int difference;
    QVBoxLayout* oldLayout;
    QVBoxLayout* vLayout;
    QWidget *valueBox;
    QWidget *labelBox;
    QString labelStyleSheet;
    QTextBrowser* descriptionBox;

    KEY_TYPE keyType;

    bool hover;

    QPalette normalPal;

    QPalette hoverPal;

    // QWidget interface
protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
};

#endif // KEYEDITWIDGET_H
