#ifndef SEARCHITEMWIDGET_H
#define SEARCHITEMWIDGET_H

#include "../../Controllers/ViewController/viewitem.h"
#include "../../Controllers/ViewController/nodeviewitem.h"
#include "../../theme.h"

#include <QFrame>
#include <QWidget>
#include <QToolButton>
#include <QLabel>
#include <QVBoxLayout>

struct DataItem{
    QWidget* item = 0;
    QLabel* label_icon = 0;
    QLabel* label_value = 0;
    QLabel* label_key = 0;
};

class SearchItemWidget : public QFrame
{
    Q_OBJECT
public:
    explicit SearchItemWidget(ViewItem* item, QWidget *parent = 0);
    ~SearchItemWidget();

    void addMatchedKey(QString key);
    void removeMatchedKey(QString key);
    void clearMatchedKeys();
    bool gotMatchedKey(QString key);

    void setSelected(bool selected);

    VIEW_ASPECT getViewAspect();
signals:
    void hoverEnter(int ID);
    void hoverLeave(int ID);
    void itemSelected(int ID);
public slots:
    void themeChanged();
    void expandButtonToggled(bool checked);
protected:
    void mouseReleaseEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);

private:
    void updateDataIcon(QString key);
    void updateIcon();
    void updateLabel();
    void updateData(QString data);

    void setupLayout();
    void setupDataLayout();

    void setupDataKey(QString key);
    void removeDataKey(QString key);
    
    
    
    void updateStyleSheet();
    
    void constructKeyWidgets();
    
    bool data_layout_setup = false;

    ViewItem* view_item = 0;
    VIEW_ASPECT view_aspect = VIEW_ASPECT::NONE;
    int ID = -1;
    
    QLabel* label_text = 0;
    QLabel* label_icon = 0;
    QToolButton* button_expand = 0;
    QWidget* data_widget = 0;

    QSize icon_size = QSize(24, 24);
    QSize small_icon_size = QSize(16, 16);

    
    QSet<QString> matched_keys;

    QHash<QString, DataItem*> data_key_hash;

    QString backgroundColor;

    bool doubleClicked = false;
    bool selected = false;
    bool visible = false;
};

#endif // SEARCHITEMWIDGET_H
