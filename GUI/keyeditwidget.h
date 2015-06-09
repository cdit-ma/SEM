#ifndef KEYEDITWIDGET_H
#define KEYEDITWIDGET_H

#include <QWidget>
#include <QTextBrowser>
#include <QVBoxLayout>


class AppSettings;
class KeyEditWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KeyEditWidget(QString groupName, QString keyName, QString keyNameHR, QVariant valueVar, QString description="", QString customType="");
    QString getKeyName();
    QString getGroupName();
    QString getValue();

signals:
    void valueChanged(QString groupName, QString keyName, QString value);


public slots:
    void _boolChanged(bool value);
    void _valueChanged(QString value);
    void _editingFinished();


private:
    QString keyName;
    QString hrKeyName;

    QString groupName;
    QString newValue;
    QString oldValue;
    QString descriptionValue;


    int difference;
    QVBoxLayout* oldLayout;
    QVBoxLayout* vLayout;
    QWidget *valueBox;
    QWidget *labelBox;
    QTextBrowser* descriptionBox;



};

#endif // KEYEDITWIDGET_H
