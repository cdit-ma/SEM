#ifndef KEYEDITWIDGET_H
#define KEYEDITWIDGET_H

#include <QObject>
#include <QHBoxLayout>

class KeyEditWidget : public QObject
{
    Q_OBJECT
public:
    explicit KeyEditWidget(QObject *parent, QString groupName, QString keyName, QVariant valueVar);
    QHBoxLayout* getLayout();
signals:
    void valueChanged(QString groupName, QString keyName, QString value);



public slots:
    void _boolChanged(bool value);
    void _valueChanged(QString value);

private:
    QString keyName;
    QString hrKeyName;

    QString groupName;
    QString newValue;

    QHBoxLayout* horizontalLayout;
};

#endif // KEYEDITWIDGET_H
