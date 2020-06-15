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
    explicit DataEditWidget(const QString& label, SETTING_TYPE type, const QVariant& data, QWidget *parent = nullptr);
    
    void setIcon(const QString& icon_path, const QString& icon_name);
    void setIconVisible(bool visible);

    SETTING_TYPE getType();
    
    void setHighlighted(bool highlighted);
	
	void setLabelWidth(int width);
	int getMinimumLabelWidth();
    
    void setValue(const QVariant& data);
    QVariant getValue();

signals:
	void valueChanged(QVariant data);
	
private slots:
	void themeChanged();

	void dataChanged(const QVariant& value);
	void editFinished();

	void pickColor();
	void pickPath();
	void pickFont();
	
private:
    QAction* getButtonAction();
    void updateIcon();
    void setupLayout();
	
    bool isHighlighted = false;
	SETTING_TYPE type;
	
	QString name;
	QVariant currentData;
	QVariant newData;

    QPair<QString, QString> icon_path_;
    
    QWidget* editWidget_ = nullptr;
    QToolBar* toolbar = nullptr;
    QLabel* label = nullptr;
    QAction* icon_action = nullptr;
    QAction* button_action = nullptr;
};

#endif // DATAEDITWIDGET_H