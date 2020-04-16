#ifndef OPTION_GROUPBOX_H
#define OPTION_GROUPBOX_H

#include "customgroupbox.h"
#include <QToolButton>

enum class SortOrder {
    ASCENDING,
    DESCENDING,
    INSERTION,
    REVERSE_INSERTION
};

class OptionGroupBox : public CustomGroupBox
{
    Q_OBJECT

public:
    explicit OptionGroupBox(const QString& title, SortOrder sort_order = SortOrder::INSERTION, QWidget* parent = nullptr);

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

    bool isResetChecked();
    int getOptionCount();
    
    bool gotOption(const QVariant& key);
    void setOptionChecked(const QVariant& key, bool checked);
	bool addOption(const QVariant& key, const QString& label, const QString& icon_path, const QString& icon_name);
    void setOptionVisible(const QVariant& key, bool is_visible);
	void updateOptionLabel(const QVariant& key, const QString& label);
	void updateOptionIcon(const QVariant& key, const QString& icon_path, const QString& icon_name);
    void removeOption(const QVariant& key);
    void removeOptions();

    void setExclusive(bool exclusive);
    void setTitle(const QString& title) override;

    void setResetButtonVisible(bool visible);
    void setResetButtonIcon(const QString& path, const QString& name);
    void setResetButtonText(const QString& text);
    void resetChecked(bool notify);

signals:
    void checkedOptionsChanged();
    void resetPressed();

public slots:
    void reset();

private slots:
    void themeChanged();
    void optionToggled();
    
private:
    void resetOptions();
	void uncheckOptions();

    QAction* getNewOptionAction(QAction* put_below = nullptr);

    void updateTitleCount();
    void setupResetAction();

    QAction* reset_action = nullptr;
    QSize icon_size = QSize(10,10);

    bool exclusive = false;

    QString title;
    SortOrder sort_order;
    
    QHash<QVariant, QAction*> actions_lookup;
    QList<QVariant> sorted_keys;
    QList<QVariant> checkedKeys;
};

#endif // OPTION_GROUPBOX_H