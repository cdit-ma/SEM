#include "optiongroupbox.h"
#include "../theme.h"
#include <QDebug>

#define OPTION_KEY "OPTION_KEY"
#define ICON_SIZE 20


/**
 * @brief OptionGroupBox::OptionGroupBox
 * @param title
 * @param parent
 */
OptionGroupBox::OptionGroupBox(const QString& title, SortOrder sort_order, QWidget* parent) : CustomGroupBox(title, parent)
{
    this->sort_order = sort_order;
    setupResetAction();
    setTitle(title);
    
    connect(Theme::theme(), &Theme::theme_Changed, this, &OptionGroupBox::themeChanged);
    themeChanged();
}


bool OptionGroupBox::isResetChecked(){
    return reset_action && reset_action->isChecked();
}



/**
 * @brief OptionGroupBox::themeChanged
 */
void OptionGroupBox::themeChanged()
{
    auto theme = Theme::theme();    
    for (auto action : actions_lookup.values()) {
        Theme::UpdateActionIcon(action, theme);
    }
    Theme::UpdateActionIcon(reset_action, theme);
}


/**
 * @brief OptionGroupBox::setExclusive
 * This sets whether this group is exclusive or not.
 * Being exclusive means that there is only ever one filter selected in this group at one time.
 * @param exclusive
 */
void OptionGroupBox::setExclusive(bool exclusive)
{
    this->exclusive = exclusive;
}


/**
 * @brief OptionGroupBox::setResetButtonVisible
 * @param visible
 */
void OptionGroupBox::setResetButtonVisible(bool visible)
{
    if (reset_action) {
        reset_action->setVisible(visible);
    }
}


/**
 * @brief OptionGroupBox::setOptionVisible
 * @param key
 * @param visible
 */
void OptionGroupBox::setOptionVisible(QVariant key, bool visible)
{
    auto option_button = actions_lookup.value(key, 0);
    if(option_button){
        option_button->setVisible(visible);
    }
}


/**
 * @brief OptionGroupBox::setOptionChecked
 * @param key
 * @param checked
 */
void OptionGroupBox::setOptionChecked(QVariant key, bool checked){
    auto option_button = actions_lookup.value(key, 0);
    if(option_button){
        if(option_button->isChecked() != checked){
            option_button->setChecked(!checked);
            emit option_button->trigger();
        }
    }
}



/**
 * @brief OptionGroupBox::setResetButtonIcon
 * @param path
 * @param name
 */
void OptionGroupBox::setResetButtonIcon(QString path, QString name)
{
    Theme::StoreActionIcon(reset_action, path, name);
}


/**
 * @brief OptionGroupBox::setResetButtonText
 * @param text
 */
void OptionGroupBox::setResetButtonText(QString text)
{
    if (reset_action) {
        reset_action->setText(text);
    }
}


/**
 * @brief OptionGroupBox::removeOption
 * @param key
 */
void OptionGroupBox::removeOption(QVariant key)
{
    auto action = actions_lookup.value(key, 0);
    if (action) {
        actions_lookup.remove(key);
        checkedKeys.removeAll(key);
        sorted_keys.removeAll(key);
        delete action;
    }
    updateTitleCount();
}

bool OptionGroupBox::gotOption(QVariant key){
    return actions_lookup.value(key, 0) != 0;
}


/**
 * @brief OptionGroupBox::removeOptions
 */
void OptionGroupBox::removeOptions()
{
    hide();
    for(auto key : actions_lookup.uniqueKeys()){
        removeOption(key);
    }
    actions_lookup.clear();
    resetOptions();
    show();
}

void OptionGroupBox::resetChecked(bool notify){
    resetOptions();
    if(notify){
        emit checkedOptionsChanged();
    }
}
void OptionGroupBox::reset(){
    resetChecked(true);
}

/**
 * @brief OptionGroupBox::uncheckOptions
 */
void OptionGroupBox::uncheckOptions()
{
    for (auto action : actions_lookup.values()) {
        action->setChecked(false);
    }
    reset_action->setChecked(false);
    checkedKeys.clear();
}


/**
 * @brief OptionGroupBox::resetOptions
 */
void OptionGroupBox::resetOptions()
{
    uncheckOptions();
    reset_action->setChecked(true);
    checkedKeys = actions_lookup.keys();
    updateTitleCount();
}


/**
 * @brief OptionGroupBox::setTitle
 * @param title
 */
void OptionGroupBox::setTitle(const QString& title)
{
    this->title = title;
    updateTitleCount();
}


/**
 * @brief OptionGroupBox::updateTitleCount
 */
void OptionGroupBox::updateTitleCount()
{
    auto new_title = title;
    if (!exclusive) {
        if (reset_action && !reset_action->isChecked()) {
            new_title += QString(" (%1)").arg(QString::number(checkedKeys.count()));
        }
    }
    CustomGroupBox::setTitle(new_title);
}


/**
 * @brief OptionGroupBox::filterTriggered
 */
void OptionGroupBox::optionToggled()
{
    auto action = qobject_cast<QAction*>(sender());

    // handle "All" and exclusive case
    if (action == reset_action) {
        resetOptions();
    } else {
        bool checked = action->isChecked();
        auto key = action->property(OPTION_KEY);
        if (!checked) {
            checkedKeys.removeAll(key);
        } else {
            if (exclusive) {
                //Uncheck everything
                uncheckOptions();
                action->setChecked(true);
            } else {
                if (reset_action->isChecked()) {
                    uncheckOptions();
                }
                action->setChecked(true);
            }
            checkedKeys.append(key);
        }
        if (checkedKeys.isEmpty()) {
            resetOptions();
        }
    }

    updateTitleCount();
    emit checkedOptionsChanged();
}


/**
 * @brief OptionGroupBox::setupResetButton
 */
void OptionGroupBox::setupResetAction()
{
    if(!reset_action){
        reset_action = getNewOptionAction();
        reset_action->setText("All");
        reset_action->setChecked(true);
        setResetButtonIcon("Icons", "list");
        connect(reset_action, &QAction::triggered, this, &OptionGroupBox::resetPressed);
    }
}

int OptionGroupBox::getOptionCount(){
    return actions_lookup.size();
}
/**
 * @brief OptionGroupBox::addOption
 * @param key
 * @param label
 * @param icon_path
 * @param icon_name
 * @return
 */
bool OptionGroupBox::addOption(QVariant key, QString label, QString icon_path, QString icon_name, bool put_on_top)
{
    if (key.isNull() || actions_lookup.count(key)) {
        qWarning("OptionGroupBox::addOption - The key is null.");
        return false;
    }

    QAction* put_below = 0;
    if(sort_order == SortOrder::REVERSE_INSERTION){
        sorted_keys.insert(0, key);
    }else{
        sorted_keys.append(key);
    }

    if(sort_order == SortOrder::DESCENDING || sort_order == SortOrder::ASCENDING){
        std::sort(sorted_keys.begin(), sorted_keys.end());

        if(sort_order == SortOrder::DESCENDING){
            std::reverse(sorted_keys.begin(), sorted_keys.end());
        }
    }

    auto index_of = sorted_keys.indexOf(key) + 1;

    if(index_of < sorted_keys.size()){
        auto before_key = sorted_keys.at(index_of);
        put_below = actions_lookup.value(before_key, 0);
    }

    auto option_action = getNewOptionAction(put_below);
    option_action->setText(label);
    Theme::StoreActionIcon(option_action, icon_path, icon_name);
    option_action->setProperty(OPTION_KEY, key);

    actions_lookup.insert(key, option_action);

    resetOptions();
    return true;
}

void OptionGroupBox::updateOptionIcon(QVariant key, QString icon_path, QString icon_name){
    auto option_action = actions_lookup.value(key, 0);
    if(option_action){
        Theme::StoreActionIcon(option_action, icon_path, icon_name);
        Theme::UpdateActionIcon(option_action);
    }
}

void OptionGroupBox::updateOptionLabel(QVariant key, QString label){
    auto option_action = actions_lookup.value(key, 0);
    if(option_action){
        option_action->setText(label);
    }
}


QAction* OptionGroupBox::getNewOptionAction(QAction* put_below){
    auto button = new QToolButton(this);
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    button->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    auto action = insertWidget(put_below, button);

    
    //Connect the button to it's action so we don't need to worry about QToolButton stuff
    button->setDefaultAction(action);
    action->setCheckable(true);
    action->setChecked(false);
    connect(action, &QAction::triggered, this, &OptionGroupBox::optionToggled);
    return action;
}


inline uint qHash(QVariant key, uint seed)
{
    return ::qHash(key.toUInt(), seed);
}
