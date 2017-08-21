#include "searchitemwidget.h"

/**
 * @brief SearchItemWidget::SearchItemWidget
 * @param item
 * @param parent
 */
SearchItemWidget::SearchItemWidget(ViewItem* item, QWidget *parent)
    : QFrame(parent)
{
    view_item = item;
    if(view_item){
        ID = view_item->getID();
        
        if (view_item->isNode()) {
            auto node_view_item = (NodeViewItem*) view_item;
            view_aspect = node_view_item->getViewAspect();
        }
        view_item->registerObject(this);

        connect(view_item, &ViewItem::labelChanged, this, &SearchItemWidget::updateLabel);
        connect(view_item, &ViewItem::iconChanged, this, &SearchItemWidget::updateIcon);
        connect(view_item, &ViewItem::dataChanged, this, &SearchItemWidget::updateData);
    }

    setupLayout();

    connect(Theme::theme(), &Theme::theme_Changed, this, &SearchItemWidget::themeChanged);
    themeChanged();
}


/**
 * @brief SearchItemWidget::~SearchItemWidget
 */
SearchItemWidget::~SearchItemWidget()
{
    if (view_item) {
        view_item->unregisterObject(this);
    }
}

void SearchItemWidget::addMatchedKey(QString key){
    matched_keys.insert(key);
    setupDataKey(key);
}

void SearchItemWidget::removeMatchedKey(QString key){
    if(matched_keys.remove(key)){
        removeDataKey(key);
    }
}

bool SearchItemWidget::gotMatchedKey(QString key){
    return matched_keys.contains(key);
}

void SearchItemWidget::clearMatchedKeys(){
    for(auto key : matched_keys){
        removeDataKey(key);
    }
    matched_keys.clear();

    //Triggered
    expandButtonToggled(false);
}

/**
 * @brief SearchItemWidget::setSelected
 * @param selected
 */
void SearchItemWidget::setSelected(bool selected)
{
    if (this->selected == selected) {
        return;
    }

    this->selected = selected;
    if (selected) {
        emit itemSelected(ID);
        backgroundColor =  Theme::theme()->getAltBackgroundColorHex();
    } else {
        backgroundColor =  Theme::theme()->getBackgroundColorHex();
    }

    updateStyleSheet();
}


/**
 * @brief SearchItemWidget::getViewAspect
 * @return
 */
VIEW_ASPECT SearchItemWidget::getViewAspect()
{
    return view_aspect;
}


void SearchItemWidget::updateIcon(){
    auto theme = Theme::theme();
    auto icon_path = view_item->getIcon();
    auto tint = theme->getMenuIconColor();
    auto pixmap = theme->getImage(icon_path.first, icon_path.second, icon_size, tint);
    if(pixmap.isNull()){
        pixmap = theme->getImage("Icons", "Help", icon_size, tint);
    }
    label_icon->setPixmap(pixmap);
}

void SearchItemWidget::updateDataIcon(QString key){
    auto theme = Theme::theme();
    auto data = data_key_hash.value(key, 0);
    auto tint = theme->getMenuIconColor();
    if(data){
        auto pixmap = theme->getImage("Data", key, small_icon_size, tint);
        if(pixmap.isNull()){
            pixmap = theme->getImage("Icons", "circleHalo", small_icon_size, tint);
        }
        data->label_icon->setPixmap(pixmap);
    }
}

void SearchItemWidget::updateLabel(){
    auto label = view_item->getData("label").toString();
    label_text->setText(label);
}

void SearchItemWidget::updateData(QString data){
    if(matched_keys.contains(data)){
        setupDataKey(data);
    }
}


/**
 * @brief SearchItemWidget::themeChanged
 */
void SearchItemWidget::themeChanged()
{
    Theme* theme = Theme::theme();
    if (selected) {
        backgroundColor =  theme->getAltBackgroundColorHex();
    } else {
        backgroundColor = theme->getBackgroundColorHex();
    }

    updateStyleSheet();
    updateIcon();

    if (button_expand) {
        button_expand->setIcon(theme->getIcon("Icons", "arrowHeadVerticalToggle"));
        button_expand->setStyleSheet("QToolButton{ background: rgba(0,0,0,0); border: 0px; }");
    }
}


/**
 * @brief SearchItemWidget::expandButtonToggled
 * @param checked
 */
void SearchItemWidget::expandButtonToggled(bool checked)
{
    if(!data_widget){
        setupDataLayout();
    }

    data_widget->setVisible(checked);
}


/**
 * @brief SearchItemWidget::mouseReleaseEvent
 * Imitate clicking the expand button when any of this widget is clicked.
 */
void SearchItemWidget::mouseReleaseEvent(QMouseEvent *)
{
    
    if (doubleClicked) {
        if (button_expand) {
            bool checked = !button_expand->isChecked();
            expandButtonToggled(checked);
            button_expand->setChecked(checked);
        }
        doubleClicked = false;
    } else {
        setSelected(!selected);
    }
}


/**
 * @brief SearchItemWidget::mouseDoubleClickEvent
 */
void SearchItemWidget::mouseDoubleClickEvent(QMouseEvent *)
{
    doubleClicked = true;
}

/**
 * @brief SearchItemWidget::enterEvent
 */
void SearchItemWidget::enterEvent(QEvent *)
{
    emit hoverEnter(ID);
}

/**
 * @brief SearchItemWidget::leaveEvent
 */
void SearchItemWidget::leaveEvent(QEvent *)
{
    emit hoverLeave(ID);
}

/**
 * @brief SearchItemWidget::updateStyleSheet
 */
void SearchItemWidget::updateStyleSheet()
{
    Theme* theme = Theme::theme();
    setStyleSheet("QFrame {"
                  "border-style: solid;"
                  "border-width: 0px 0px 1px 0px;"
                  "border-color:" + theme->getDisabledBackgroundColorHex() + ";"
                  "background:" + backgroundColor + ";"
                  "color:" + theme->getTextColorHex() + ";"
                  "}"
                  "QFrame:hover { background:" + theme->getDisabledBackgroundColorHex() + ";}"
                  "QLabel{ background: rgba(0,0,0,0); border: 0px; }"
                  "QLabel#KEY_LABEL{ color:" + theme->getAltTextColorHex() + ";}"
                  /*"QWidget{border:1px solid red;}"*/
                  + theme->getToolBarStyleSheet());
}

void SearchItemWidget::setupLayout(){
    auto layout = new QVBoxLayout(this);
    layout->setMargin(5);
    layout->setSpacing(5);

    if(view_item){
        auto top_layout = new QHBoxLayout();
        label_icon = new QLabel(this);
        label_icon->setFixedSize(icon_size);

        label_text = new QLabel(this);
        label_text->setFont(QFont(font().family(), 10));

        button_expand = new QToolButton(this);
        button_expand->setCheckable(true);
        button_expand->setEnabled(false);

        top_layout->addWidget(label_icon);
        top_layout->addWidget(label_text, 1);
        top_layout->addWidget(button_expand);
        layout->addLayout(top_layout);
    }

    updateLabel();
}

void SearchItemWidget::setupDataKey(QString key){
    auto theme = Theme::theme();
    if(data_widget){
        auto data = data_key_hash.value(key, 0);
    
        if(!data){
            data = new DataItem();
            data->item = new QWidget(this);
            data->item->setObjectName("TEST");
            auto layout = new QHBoxLayout(data->item);
            layout->setMargin(0);
            layout->setSpacing(5);
            
            data->label_icon = new QLabel(data->item);
            data->label_icon->setFixedSize(small_icon_size);
            data->label_value = new QLabel(data->item);
            data->label_key = new QLabel(data->item);
            data->label_key->setObjectName("KEY_LABEL");
            layout->addWidget(data->label_icon);
            layout->addWidget(data->label_key);
            layout->addWidget(data->label_value, 1);

            data_widget->layout()->addWidget(data->item);
            data_key_hash[key] = data;
        }

        if(data){
            //Update
            data->label_key->setText(key + ":");
            data->label_value->setText(view_item->getData(key).toString());
        }
        updateDataIcon(key);
    }
}

void SearchItemWidget::removeDataKey(QString key){
    auto data = data_key_hash.value(key, 0);

    if(data){
        data_key_hash.remove(key);
        delete data->item;
        delete data;
    }
}

void SearchItemWidget::setupDataLayout(){
    if(!data_widget){
        data_widget = new QWidget(this);
        data_widget->setContentsMargins(icon_size.width() + 5, 5, 5, 5);
        data_widget->hide();

        auto v_layout = new QVBoxLayout(data_widget);
        v_layout->setMargin(0);
        v_layout->setSpacing(5);
        
        for(auto key : matched_keys){
            setupDataKey(key);
        }

        //Add to the main layout
        layout()->addWidget(data_widget);
    }
}