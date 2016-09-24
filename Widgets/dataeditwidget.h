#ifndef DATAEDITWIDGET_H
#define DATAEDITWIDGET_H

#include <QLabel>
#include <QWidget>
#include <QTextBrowser>

#include "../Controllers/SettingsController/settingscontroller.h"

class DataEditWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DataEditWidget(QString dataKey, QString label, SETTING_TYPE type, QVariant data, QWidget *parent = 0);

    QString getKeyName();

    SETTING_TYPE getType();
    void setHighlighted(bool highlighted);
    int getMinimumLabelWidth();
    void setLabelWidth(int width);
    void setValue(QVariant data);

private slots:

    void themeChanged();

    void dataChanged(QVariant value);
    void editFinished();

    void pickColor();
    void pickPath();
signals:
    void valueChanged(QString datakey, QVariant data);
private:

    void setupLayout();

    bool isHighlighted;
    QString dataKey;
    QString label;
    QVariant currentData;
    QVariant newData;

    SETTING_TYPE type;
    QWidget* editWidget_1;
    QWidget* editWidget_2;
    QLabel* editLabel;
    QTextBrowser* description;


};

#endif // DATAEDITWIDGET_H
