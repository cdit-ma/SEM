#ifndef DOCKTITLEBAR_H
#define DOCKTITLEBAR_H

#include <QToolBar>
#include <QLabel>
#include <QAction>

class DockTitleBar : public QToolBar
{
    Q_OBJECT

public:
    enum DOCK_ACTION{DA_CLOSE, DA_MAXIMIZE, DA_POPOUT, DA_PROTECT, DA_HIDE, DA_ICON};

    explicit DockTitleBar(QWidget* parent = nullptr);

    void setActive(bool active);
    void setIcon(const QString& iconPath, const QString& iconName);

    QAction* getAction(DOCK_ACTION action);
    QList<QAction*> getToolActions() const ;
    void addToolAction(QAction* action, const QString& iconPath, const QString& iconName, Qt::Alignment alignment = Qt::AlignRight);

    void setTitle(const QString& title, Qt::Alignment alignment = Qt::AlignCenter);
    QString getTitle() const;

private slots:
    void themeChanged();
    void updateActiveStyle();

private:
    void updateIcon(QAction* action, const QString& iconPath, const QString& iconName);
    void setupToolBar();

    QLabel* titleLabel = nullptr;

    QAction* closeAction = nullptr;
    QAction* maximizeAction = nullptr;
    QAction* popOutAction = nullptr;
    QAction* protectAction = nullptr;
    QAction* hideAction = nullptr;
    QAction* iconAction = nullptr;

    QList<QAction*> toolActions;
    bool _isActive = false;
};

#endif // DOCKTITLEBAR_H