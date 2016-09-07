#ifndef DOCKTITLEBARWIDGET_H
#define DOCKTITLEBARWIDGET_H

#include <QToolBar>
#include <QLabel>
#include <QAction>

class DockTitleBarWidget : public QToolBar
{
    Q_OBJECT
public:
    enum DOCK_ACTION{DA_CLOSE, DA_MAXIMIZE, DA_POPOUT, DA_PROTECT, DA_HIDE, DA_ICON};

    explicit DockTitleBarWidget(QWidget *parent=0);
    ~DockTitleBarWidget();

    void setActive(bool active);
    QList<QAction*> getToolActions();

    void setToolBarIconSize(int height);
    void setIcon(QPixmap pixmap);
    void setTitle(QString title, Qt::Alignment alignment=Qt::AlignCenter);
    QString getTitle();
    QAction* getAction(DOCK_ACTION action);

private slots:
    void themeChanged();
    void updateActiveStyle();
private:
    void setupToolBar();
    bool isActive();

private:
    QLabel* iconLabel;
    QLabel* titleLabel;
    bool _isActive;

    QList<QAction*> actions;

    QAction* closeAction;
    QAction* maximizeAction;
    QAction* popOutAction;
    QAction* protectAction;
    QAction* hideAction;
    QAction* iconAction;
};
#endif // DOCKTITLEBARWIDGET_H
