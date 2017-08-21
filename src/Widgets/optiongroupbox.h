#ifndef OPTION_GROUPBOX_H
#define OPTION_GROUPBOX_H

#include "customgroupbox.h"
#include <QToolButton>

class OptionGroupBox : public CustomGroupBox
{
    Q_OBJECT

public:
    explicit OptionGroupBox(QString title, QWidget* parent = 0);

    template<class T> QList<T> getOptions()
    {
        QList<T> options;
        for(auto key : actions_lookup.keys()){
            options.append(key.value<T>());
        }
        return options;
    }

    template<class T> QList<T> getCheckedOptions()
    {
        QList<T> options;
        for(auto key : checkedKeys){
            options.append(key.value<T>());
        }
        return options;
    }

    QList<QVariant> getCheckedKeys();
    bool isAllChecked();

    bool addOption(QVariant key, QString label, QString icon_path, QString icon_name);
    void setOptionVisible(QVariant key, bool is_visible);
    void removeOption(QVariant key);
    void removeOptions();

    void setExclusive(bool exclusive);
    void setTitle(QString title);

    void setResetButtonVisible(bool visible);
    void setResetButtonIcon(QString path, QString name);
    void setResetButtonText(QString text);

signals:
    void checkedOptionsChanged();

public slots:
    void resetOptions();

private slots:
    void themeChanged();
    void optionToggled();

private:

    QAction* getNewOptionAction();

    void uncheckOptions();
    void clearFilters();

    void updateTitleCount();
    void setupResetAction();

    QAction* reset_action = 0;
    QSize icon_size = QSize(10,10);
    
    bool showResetButton = true;
    bool exclusive = false;
    QString title;

    QHash<QVariant, QAction*> actions_lookup;
    QList<QVariant> checkedKeys;
};

#endif // OPTION_GROUPBOX_H
