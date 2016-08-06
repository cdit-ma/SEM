#ifndef DATAEDITWIDGET_H
#define DATAEDITWIDGET_H

#include <QLabel>
#include <QWidget>
#include <QTextBrowser>

#include "../../Controller/settingscontroller.h"

class DataEditWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DataEditWidget(QString dataKey, QString label, SETTING_TYPE type, QVariant data, QWidget *parent = 0);
private slots:
    void themeChanged();
private:
    void setupLayout();

    QString dataKey;
    QString label;
    QVariant currentData;

    SETTING_TYPE type;
    QWidget* editWidget_1;
    QWidget* editWidget_2;
    QLabel* editLabel;
    QTextBrowser* description;


};

#endif // DATAEDITWIDGET_H
