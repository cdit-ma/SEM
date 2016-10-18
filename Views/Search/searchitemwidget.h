#ifndef SEARCHITEMWIDGET_H
#define SEARCHITEMWIDGET_H

#include "../../Controllers/ViewController/viewitem.h"
#include "../../theme.h"

#include <QFrame>
#include <QWidget>
#include <QToolButton>
#include <QLabel>
#include <QVBoxLayout>

class SearchItemWidget : public QFrame
{
    Q_OBJECT
public:
    explicit SearchItemWidget(ViewItem* item, QWidget *parent = 0);
    ~SearchItemWidget();

    void addDisplayKey(QString key);
    void setDisplayKeys(QList<QString> keys);

    void setSelected(bool selected);
    
signals:
    void hoverEnter(int ID);
    void hoverLeave(int ID);
    void itemSelected(int ID);

public slots:
    void themeChanged();
    void expandButtonToggled(bool checked);
    void toggleKeyWidget(QString key);
    
protected:
    void mouseReleaseEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);

private:
    void setupLayout(QVBoxLayout* layout);
    void constructKeyWidgets();
    void updateStyleSheet();

    ViewItem* viewItem;
    int viewItemID;
    
    QLabel* iconLabel;
    QSize iconSize;
    QPair<QString, QString> iconPath;

    QLabel* textLabel;
    QToolButton* expandButton;
    QWidget* displayWidget;
    
    QList<QString> keys;
    QHash<QString, QWidget*> keyWidgetHash;
    QString checkedKey;

    QString backgroundColor;

    bool keyWidgetsConstructed;
    bool doubleClicked;
    bool selected;

};

#endif // SEARCHITEMWIDGET_H
