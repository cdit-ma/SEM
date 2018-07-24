#ifndef DATAEDITWIDGET_H
#define DATAEDITWIDGET_H

#include <QLabel>
#include <QWidget>
#include <QTextBrowser>
#include <QToolBar>

#include "../Controllers/SettingsController/settingscontroller.h"

class DataEditWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DataEditWidget(QString label, SETTING_TYPE type, QVariant data, QWidget *parent = 0);
    
    void setIcon(QString icon_path, QString icon_name);
    void setIconVisible(bool visible);

    SETTING_TYPE getType();
    void setHighlighted(bool highlighted);
    int getMinimumLabelWidth();
    void setLabelWidth(int width);
    void setValue(QVariant data);
    QVariant getValue();

private slots:

    void themeChanged();

    void dataChanged(QVariant value);
    void editFinished();

    void pickColor();
    void pickPath();
    void pickFont();
signals:
    void valueChanged(QVariant data);
private:
    QAction* getButtonAction();
    void updateIcon();

    void setupLayout();

    bool isHighlighted = false;
    QString name;
    QVariant currentData;
    QVariant newData;
    QPair<QString, QString> icon_path;

    SETTING_TYPE type;
    QWidget* editWidget_1 = 0;
    QWidget* editWidget_2 = 0;;
    
    
    
    QToolBar* toolbar = 0;
    

    QLabel* label = 0;
    QAction* icon_action = 0;
    QAction* button_action = 0;

};

#endif // DATAEDITWIDGET_H
