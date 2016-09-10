#ifndef SEARCHITEMWIDGET_H
#define SEARCHITEMWIDGET_H

#include <QFrame>
#include <QWidget>
#include <QToolButton>
#include <QPushButton>
#include <QLabel>

#include "../../View/viewitem.h"
#include "../../View/theme.h"

class SearchItemWidget : public QFrame
{
    Q_OBJECT
public:
    explicit SearchItemWidget(ViewItem* item, QWidget *parent = 0);
    void setDisplayKeys(QList<QString> keys);
    
signals:
    void centerOnViewItem(int ID);

public slots:
    void themeChanged();
    void centerButtonClicked();
    void expandButtonToggled(bool checked);
    void toggleKeyWidget(QString key);
    
protected:
    void mouseReleaseEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);

private:
    void updateColor(Theme::COLOR_ROLE colorRole);

    ViewItem* viewItem;
    int viewItemID;
    
    QLabel* iconLabel;
    QSize iconSize;
    QPair<QString, QString> iconPath;

    QLabel* textLabel;
    //QPushButton* expandButton;
    QToolButton* expandButton;
    QToolButton* centerButton;
    QWidget* displayWidget;
    
    QHash<QString, QWidget*> keyWidgetHash;
    bool doubleClicked;

};

#endif // SEARCHITEMWIDGET_H
