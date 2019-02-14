#ifndef DOCKTITLEBAR_H
#define DOCKTITLEBAR_H

#include <QToolBar>
#include <QLabel>
#include <QAction>
//#include <QPixmap>

class DockTitleBar : public QToolBar
{
    Q_OBJECT

public:
    enum DOCK_ACTION{DA_CLOSE, DA_MAXIMIZE, DA_POPOUT, DA_PROTECT, DA_HIDE, DA_ICON};

    explicit DockTitleBar(QWidget *parent=0);
    ~DockTitleBar();

    void setActive(bool active);
    void setIcon(QString iconPath, QString iconName);

    QAction* getAction(DOCK_ACTION action);
    QList<QAction*> getToolActions();
    void addToolAction(QAction* action, QString iconPath, QString iconName, Qt::Alignment alignment = Qt::AlignRight);

    void setTitle(QString title, Qt::Alignment alignment=Qt::AlignCenter);
    QString getTitle();

private slots:
    void themeChanged();
    void updateActiveStyle();

private:
    void updateIcon(QAction* action, QString iconPath, QString iconName);
    void setupToolBar();
    bool isActive();

private:
    QLabel* titleLabel = 0;

    QAction* closeAction;
    QAction* maximizeAction;
    QAction* popOutAction;
    QAction* protectAction;
    QAction* hideAction;
    QAction* iconAction;

    QList<QAction*> toolActions;
    bool _isActive;
};
#endif // DOCKTITLEBAR_H
