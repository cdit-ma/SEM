#ifndef KEYEDITWIDGET_H
#define KEYEDITWIDGET_H

#include <QWidget>


class AppSettings;
class KeyEditWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KeyEditWidget(AppSettings* parent, QString groupName, QString keyName, QVariant valueVar);
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

    QWidget *valueBox;
    QWidget *labelBox;


};

#endif // KEYEDITWIDGET_H
