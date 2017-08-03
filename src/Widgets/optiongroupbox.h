#ifndef OPTION_GROUPBOX_H
#define OPTION_GROUPBOX_H

#include "customgroupbox.h"
#include <QHash>
#include <QToolButton>

class OptionGroupBox : public CustomGroupBox
{
    Q_OBJECT
public:
    explicit OptionGroupBox(QString title, QWidget *parent = 0);

    void setExclusive(bool exclusive);

    template<class T> QList<T> getOptions(){
        QList<T> options;
        
        for(auto key : optionsLookup.keys()){
            options.append(key.value<T>());
        }
        return options;
    };

    template<class T> QList<T> getCheckedOptions(){
        QList<T> options;
        
        for(auto key : checkedKeys){
            options.append(key.value<T>());
        }
        return options;
    };

    bool addOption(QVariant key, QString label, QString icon_path, QString icon_name);
    void removeOption(QVariant key);
    void removeOptions();

    void setIconSize(QSize iconSize);
    void setTitle(QString title);

    void setResetButtonVisible(bool visible);
    void setResetButtonIcon(QString path, QString name);
    void setResetButtonText(QString text);
    void setResetButtonKey(QVariant key);
signals:
    void checkedOptionsChanged();
public slots:
    void resetOptions();
private slots:
    void optionToggled();
    void themeChanged();

private:
    void uncheckOptions();
    void setupResetButton();
    QAction* addOptionButton(QToolButton* button, QVariant key = QVariant());
    void clearFilters();
    void updateTitleCount();


    QToolButton* resetButton = 0;
    QAction* resetAction = 0;
    QSize icon_size = QSize(10,10);
    
    bool showResetButton = true;
    bool exclusive = false;
    QString title;


    QHash<QVariant, QToolButton*> optionsLookup;
    QList<QVariant> checkedKeys;
};


#endif // OPTION_GROUPBOX_H
