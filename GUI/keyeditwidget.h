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
    KEY_FILE,
    KEY_COLOR
};
class AppSettings;
class KeyEditWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KeyEditWidget(QString groupName, QString keyName, QString keyNameHR, QVariant valueVar, QString description="", QString customType="");
    QString getKeyName();
    QString getGroupName();
    QVariant getValue();

    void setLabelWidth(int labelWidth);
    void setValue(QVariant value);

    void setHighlighted(bool highlighted);
signals:
    void valueChanged(QString groupName, QString keyName, QVariant value);


public slots:
    void pickColor();
    void _valueChanged(QVariant value);
    void _editingFinished();


private:
    QString keyName;
    QString hrKeyName;

    QString groupName;

    QVariant newValue;
    QVariant oldValue;
    QString descriptionValue;

    bool highlighted;

    int difference;
    QVBoxLayout* oldLayout;
    QVBoxLayout* vLayout;
    QWidget *valueBox;
    QWidget *value2Box;
    QWidget *labelBox;
    QString labelStyleSheet;
    QString colorBoxStyleSheet;
    QTextBrowser* descriptionBox;

    KEY_TYPE keyType;
};

#endif // KEYEDITWIDGET_H
