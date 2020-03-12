#ifndef SEARCHITEMWIDGET_H
#define SEARCHITEMWIDGET_H

#include "../../Controllers/ViewController/viewitem.h"
#include "../../Controllers/ViewController/nodeviewitem.h"
#include "../../Controllers/ViewController/edgeviewitem.h"
#include "../../theme.h"

#include <QFrame>
#include <QWidget>
#include <QToolButton>
#include <QLabel>
#include <QVBoxLayout>

struct DataItem {
    QWidget* item = nullptr;
    QLabel* label_icon = nullptr;
    QLabel* label_value = nullptr;
    QLabel* label_key = nullptr;
    bool in_layout = false;
};

class SearchItemWidget : public QFrame
{
    Q_OBJECT

public:
    explicit SearchItemWidget(ViewItem* item, QWidget *parent = nullptr);
    ~SearchItemWidget() final;

    void addMatchedKeys(const QSet<QString>& keys);
    void addMatchedKey(const QString& key);
    void addPersistentKey(const QString& key);
    void clearMatchedKeys();

    void viewItemSelected(bool selected);
    void setSelected(bool selected);
    void setExpanded(bool expanded);

    VIEW_ASPECT getViewAspect();

signals:
    void searchItemClicked(int ID);
    void flashEntityItem(int ID);
    void centerEntityItem(int ID);

public slots:
    void themeChanged();

protected:
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    void updateData(const QString& data);
    void updateDataIcon(const QString& key);
    void updateIcon();
    void updateLabel();
    void updateStyleSheet();

    void setupLayout();
    void setupDataLayout();

    void setupDataKey(const QString& key);
    void updateDataKey(const QString& key, const QVariant& data);
    void removeDataKey(const QString& key);
    
    ViewItem* view_item = nullptr;
    VIEW_ASPECT view_aspect = VIEW_ASPECT::NONE;
    int ID = -1;
    
    QLabel* label_text = nullptr;
    QLabel* label_icon = nullptr;
    QToolButton* button_expand = nullptr;
    QWidget* data_widget = nullptr;

    QSize data_icon_size = QSize(24, 24);
    
    QSet<QString> matched_keys;
    QSet<QString> persistent_keys;
    QHash<QString, DataItem*> data_key_hash;

    bool viewItemSelected_ = false;
    bool selected_ = false;
};

#endif // SEARCHITEMWIDGET_H