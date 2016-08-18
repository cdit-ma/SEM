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

    void setToolBarIconSize(int height);
    void setLabelStyleSheet(QString style);
    void setIcon(QPixmap pixmap);
    void setTitle(QString title, Qt::Alignment alignment=Qt::AlignCenter);
    QString getTitle();
    QAction* getAction(DOCK_ACTION action);

private:
    void setupToolBar();

private:
    QLabel* iconLabel;
    QLabel* titleLabel;

    QAction* closeAction;
    QAction* maximizeAction;
    QAction* popOutAction;
    QAction* protectAction;
    QAction* hideAction;
    QAction* iconAction;
};
#endif // DOCKTITLEBARWIDGET_H
