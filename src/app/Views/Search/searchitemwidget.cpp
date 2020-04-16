#include "searchitemwidget.h"

#include <QMouseEvent>
#include <keynames.h>

/**
 * @brief SearchItemWidget::SearchItemWidget
 * @param item
 * @param parent
 * @throws std::invalid_argument
 */
SearchItemWidget::SearchItemWidget(ViewItem* item, QWidget *parent)
    : QFrame(parent)
{
	if (item == nullptr) {
		throw std::invalid_argument("SearchItemWidget::SearchItemWidget - view item cannot be null.");
	}

    view_item = item;
	view_item->registerObject(this);

	ID = view_item->getID();

	if (view_item->isNode()) {
		auto node_view_item = (NodeViewItem*) view_item;
		view_aspect = node_view_item->getViewAspect();
	} else if (view_item->isEdge()) {
		auto edge_item = (EdgeViewItem*) view_item;
		addPersistentKey("SRC ID");
		addPersistentKey("DST ID");
		updateDataKey("SRC ID", edge_item->getSourceID());
		updateDataKey("DST ID", edge_item->getDestinationID());
	}

	connect(view_item, &ViewItem::labelChanged, this, &SearchItemWidget::updateLabel);
	connect(view_item, &ViewItem::iconChanged, this, &SearchItemWidget::updateIcon);
	connect(view_item, &ViewItem::dataChanged, this, &SearchItemWidget::updateData);

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

/**
 * @brief SearchItemWidget::addMatchedKeys
 * @param keys
 */
void SearchItemWidget::addMatchedKeys(const QSet<QString>& keys)
{
    for (const auto& key : keys) {
        addMatchedKey(key);
    }
}

/**
 * @brief SearchItemWidget::addMatchedKey
 * @param key
 */
void SearchItemWidget::addMatchedKey(const QString& key)
{
    matched_keys.insert(key);
    setupDataKey(key);
}

/**
 * @brief SearchItemWidget::addPersistentKey
 * @param key
 */
void SearchItemWidget::addPersistentKey(const QString& key)
{
    persistent_keys.insert(key);
    setupDataKey(key);
}

/**
 * @brief SearchItemWidget::clearMatchedKeys
 */
void SearchItemWidget::clearMatchedKeys()
{
    for (const auto& key : matched_keys) {
        removeDataKey(key);
    }
    matched_keys.clear();

    // hide the data widget
    setExpanded(false);
}

/**
 * @brief SearchItemWidget::viewItemSelected
 * @param selected
 */
void SearchItemWidget::viewItemSelected(bool selected)
{
    // set this item's selected state and update its stylesheet
    if (viewItemSelected_ != selected) {
        viewItemSelected_ = selected;
        updateStyleSheet();
    }
}

/**
 * @brief SearchItemWidget::setSelected
 * @param selected
 */
void SearchItemWidget::setSelected(bool selected)
{
    // set this item's selected state and update its stylesheet
    if (selected_ != selected) {
        selected_ = selected;
        updateStyleSheet();
    }
}

/**
 * @brief SearchItemWidget::setExpanded
 * @param expanded
 */
void SearchItemWidget::setExpanded(bool expanded)
{
    // update the button_expand's checked state and pixmap
    button_expand->setChecked(expanded);

    // construct the data widget when the item is first expanded
    if (!data_widget) {
        setupDataLayout();
    }
    data_widget->setVisible(expanded);
}

/**
 * @brief SearchItemWidget::getViewAspect
 * @return
 */
VIEW_ASPECT SearchItemWidget::getViewAspect()
{
    return view_aspect;
}

/**
 * @brief SearchItemWidget::updateIcon
 */
void SearchItemWidget::updateIcon()
{
    auto theme = Theme::theme();
    auto icon_path = view_item->getIcon();
    auto tint = theme->getMenuIconColor();
    auto icon_size = theme->getLargeIconSize();
    auto pixmap = theme->getImage(icon_path.first, icon_path.second, icon_size, tint);

    if (pixmap.isNull()) {
        pixmap = theme->getImage("Icons", "Help", icon_size, tint);
    }

    label_icon->setFixedSize(icon_size);
    label_icon->setPixmap(pixmap);
}

/**
 * @brief SearchItemWidget::updateDataStyleSheet
 * @param key
 */
void SearchItemWidget::updateDataStyleSheet(const QString &key)
{
    auto data = data_key_hash.value(key, nullptr);
    if (data) {
        auto theme = Theme::theme();
        auto tint = theme->getMenuIconColor();
        auto pixmap = theme->getImage("Data", key, theme->getIconSize(), tint);
        if (pixmap.isNull()) {
            pixmap = theme->getImage("Icons", "circleHalo", theme->getIconSize(), tint);
        }
        data->label_icon->setFixedSize(theme->getIconSize());
        data->label_icon->setPixmap(pixmap);

        auto label_style = "QLabel{ background: rgba(0,0,0,0); border: 0px; color:" + theme->getTextColorHex() +  ";}";
        data->label_icon->setStyleSheet(label_style);
        data->label_key->setStyleSheet(label_style + "QLabel#KEY_LABEL{ color:" + theme->getAltTextColorHex() + ";}");
        data->label_value->setStyleSheet(label_style);
    }
}

/**
 * @brief SearchItemWidget::updateLabel
 */
void SearchItemWidget::updateLabel()
{
    auto key_name = view_item->isNode() ? KeyName::Label : KeyName::Kind;
    auto label = view_item->getData(key_name).toString();
    label_text->setText(label);
}

/**
 * @brief SearchItemWidget::updateData
 * @param data
 */
void SearchItemWidget::updateData(const QString &data)
{
    if (matched_keys.contains(data)) {
        setupDataKey(data);
    }
}

/**
 * @brief SearchItemWidget::themeChanged
 */
void SearchItemWidget::themeChanged()
{
    updateStyleSheet();
    updateIcon();

    for (auto key : data_key_hash.keys()) {
        updateDataStyleSheet(key);
    }

    if (button_expand) {
        button_expand->setIconSize(theme->getIconSize());
        button_expand->setIcon(theme->getIcon("ToggleIcons", "arrowVertical"));
        button_expand->setStyleSheet("QToolButton{ background: rgba(0,0,0,0); border: 0px; }");
    }
}

/**
 * @brief SearchItemWidget::mouseReleaseEvent
 * @param event
 */
void SearchItemWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        // toggle the item's selected state
        setSelected(!selected_);
        emit searchItemClicked(ID);

        // when clicked, flash the linked entity item
        // unfortunately, this is also triggered when the item is expanded/contracted
        emit flashEntityItem(ID);

    } else if (event->button() == Qt::MiddleButton) {
        // center on the linked entity item
        emit centerEntityItem(ID);
    }
}

/**
 * @brief SearchItemWidget::mouseDoubleClickEvent
 * @param event
 */
void SearchItemWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        if (button_expand) {
            setExpanded(!button_expand->isChecked());
        }
    }
}

/**
 * @brief SearchItemWidget::updateStyleSheet
 */
void SearchItemWidget::updateStyleSheet()
{   
    Theme* theme = Theme::theme();
    QString borderStr = "border-style: solid;";
    if (viewItemSelected_) {
        borderStr += "border-width: 2px;"
                     "border-color:" + theme->getHighlightColorHex() + ";";
    } else {
        borderStr += "border-width: 0px 0px 1px 0px;"
                     "border-color:" + theme->getDisabledBackgroundColorHex() + ";";
    }

    QString backgroundStr = theme->getBackgroundColorHex();
    if (selected_) {
		backgroundStr = theme->getAltBackgroundColorHex();
    }

    setStyleSheet("QFrame {"
				  + borderStr +
				  "background:" + backgroundStr + ";"
                  "color:" + theme->getTextColorHex() + ";"
                  "}"
                  "QFrame:hover { background:" + theme->getAltBackgroundColorHex() + ";}"
                  "QLabel{ background: rgba(0,0,0,0); border: 0px; color:" + theme->getTextColorHex() +  ";}"
                  + theme->getToolBarStyleSheet());
}

/**
 * @brief SearchItemWidget::setupLayout
 */
void SearchItemWidget::setupLayout()
{
    auto layout = new QVBoxLayout(this);
    layout->setMargin(5);
    layout->setSpacing(5);

    if (view_item) {
        label_icon = new QLabel(this);
        label_icon->setStyleSheet("padding:0px;margin:0px;");
        label_icon->setAlignment(Qt::AlignCenter);

        label_text = new QLabel(this);

        button_expand = new QToolButton(this);
        button_expand->setCheckable(true);
        button_expand->setAutoRaise(false);
        button_expand->setAttribute(Qt::WA_TransparentForMouseEvents, true);

        auto top_layout = new QHBoxLayout();
        top_layout->setMargin(0);
        top_layout->addWidget(label_icon);
        top_layout->addWidget(label_text, 1);
        top_layout->addWidget(button_expand);
        layout->addLayout(top_layout);
    }

    updateLabel();
}

/**
 * @brief SearchItemWidget::setupDataKey
 * @param key
 */
void SearchItemWidget::setupDataKey(const QString &key)
{
    auto data = data_key_hash.value(key, nullptr);
    if (data == nullptr) {
        data = new DataItem();
        data->item = new QWidget(this);
        data->item->setVisible(false);

        auto layout = new QHBoxLayout(data->item);
        layout->setMargin(0);
        layout->setSpacing(5);
        
        data->label_icon = new QLabel(data->item);
        data->label_icon->setStyleSheet("padding:0px;margin:0px;");

        data->label_value = new QLabel(data->item);
        data->label_key = new QLabel(data->item);
        data->label_key->setObjectName("KEY_LABEL");

        layout->addWidget(data->label_icon);
        layout->addWidget(data->label_key);
        layout->addWidget(data->label_value, 1);

        data_key_hash[key] = data;
        updateDataKey(key, view_item->getData(key));
        updateDataStyleSheet(key);
    }
    if (!data->in_layout && data_widget) {
        data_widget->layout()->addWidget(data->item);
        data->item->setVisible(true);
        data->in_layout = true;
    }
}

/**
 * @brief SearchItemWidget::updateDataKey
 * @param key
 * @param value
 */
void SearchItemWidget::updateDataKey(const QString &key, const QVariant &value)
{
    auto data = data_key_hash.value(key, nullptr);
    if (data) {
        data->label_key->setText(key + ":");
        data->label_value->setText(value.toString());
    }
}

/**
 * @brief SearchItemWidget::removeDataKey
 * @param key
 */
void SearchItemWidget::removeDataKey(const QString &key)
{
    auto data = data_key_hash.value(key, nullptr);
    if (data) {
        data_key_hash.remove(key);
        delete data->item;
        delete data;
    }
}

/**
 * @brief SearchItemWidget::setupDataLayout
 */
void SearchItemWidget::setupDataLayout()
{
    if (data_widget == nullptr) {
        data_widget = new QWidget(this);
        data_widget->setContentsMargins(data_icon_size.width() + 5, 5, 5, 5);
        data_widget->hide();

        auto v_layout = new QVBoxLayout(data_widget);
        v_layout->setMargin(0);
        v_layout->setSpacing(5);

        for (const auto& key : persistent_keys) {
            setupDataKey(key);
        }
        for (const auto& key : matched_keys) {
            setupDataKey(key);
        }

        // add to the main layout
        layout()->addWidget(data_widget);
    }
}