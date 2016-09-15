#ifndef SEARCHITEMWIDGET_H
#define SEARCHITEMWIDGET_H

#include <QFrame>
#include <QWidget>
#include <QToolButton>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

#include "../../View/viewitem.h"
#include "../../View/theme.h"

class SearchItemWidget : public QFrame
{
    Q_OBJECT
public:
    explicit SearchItemWidget(ViewItem* item, QWidget *parent = 0);
    ~SearchItemWidget();

    void addDisplayKey(QString key);
    void setDisplayKeys(QList<QString> keys);
    
signals:
    void hoverEnter(int ID);
    void hoverLeave(int ID);
    void centerOnViewItem(int ID);
    void popupViewItem(int ID);

public slots:
    void themeChanged();
    void centerButtonClicked();
    void popupButtonClicked();
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

    ViewItem* viewItem;
    int viewItemID;
    
    QLabel* iconLabel;
    QSize iconSize;
    QPair<QString, QString> iconPath;

    QLabel* textLabel;
    QToolButton* expandButton;
    QToolButton* centerButton;
    QToolButton* popupButton;
    QWidget* displayWidget;
    
    QList<QString> keys;
    QHash<QString, QWidget*> keyWidgetHash;
    QString checkedKey;

    bool keyWidgetsConstructed;
    bool doubleClicked;

};

#endif // SEARCHITEMWIDGET_H
