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

    QAction* load_more_action = nullptr;
};

#endif // LOADMOREWIDGET_H