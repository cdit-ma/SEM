#ifndef LOADMOREWIDGET_H
#define LOADMOREWIDGET_H


#include <QToolBar>
#include <QToolButton>

class LoadMoreWidget : public QToolBar
{
    Q_OBJECT
public:
    explicit LoadMoreWidget();
signals:
    void LoadMore();
private:
    void themeChanged();
    void setupLayout();
private:
    QAction* load_more_action = 0;
};

#endif // LOADMOREWIDGET_H
