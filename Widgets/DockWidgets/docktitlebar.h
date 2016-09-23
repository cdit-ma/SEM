#ifndef DOCKTITLEBAR_H
#define DOCKTITLEBAR_H

#include <QToolBar>
#include <QLabel>
#include <QAction>
#include <QPixmap>
class DockTitleBar : public QToolBar
{
    Q_OBJECT
public:
    enum DOCK_ACTION{DA_CLOSE, DA_MAXIMIZE, DA_POPOUT, DA_PROTECT, DA_HIDE, DA_ICON};

    explicit DockTitleBar(QWidget *parent=0);
    ~DockTitleBar();

    void setActive(bool active);
    QList<QAction*> getToolActions();

    void setToolBarIconSize(int height);
    void setIcon(QPixmap pixmap);
    QPixmap getIcon();
    void setTitle(QString title, Qt::Alignment alignment=Qt::AlignCenter);
    QString getTitle();
    QAction* getAction(DOCK_ACTION action);

    void addToolAction(QAction* action, Qt::Alignment alignment);

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
#endif // DOCKTITLEBAR_H
