#ifndef BASEDOCKWIDGET_H
#define BASEDOCKWIDGET_H

#include "docktitlebar.h"
#include <QDockWidget>

class BaseWindow;
enum class BaseDockType{DOCK, TOOL, INVISIBLE};

class BaseDockWidget : public QDockWidget
{
    Q_OBJECT
    friend class WindowManager;

public:
    int getID() const;
    BaseDockType getBaseDockType() const;

    void setDockWidgetArea(Qt::DockWidgetArea area);
    Qt::DockWidgetArea getDockWidgetArea();

    void setSourceWindow(BaseWindow* window);
    BaseWindow* getSourceWindow();

    virtual void themeChanged();

    // NOTE: This is a non-virtual function - QDockWidget's function is called within
    virtual void setWidget(QWidget* widget);

    DockTitleBar* getTitleBar();
    void removeTitleBar();

    bool isProtected() const;
    void setProtected(bool protect);

    void setCurrentWindow(BaseWindow* window);
    BaseWindow* getCurrentWindow();

    void setIcon(const QString& prefix, const QString& alias);
    void setIcon(const QPair<QString, QString>& pair);
    QPair<QString, QString> getIcon();

    void setTitle(const QString& title, Qt::Alignment alignment = Qt::AlignLeft);
    QString getTitle() const;

    virtual void setActive(bool active);
    bool isActive();

    void setIconVisible(bool visible);
    void setCloseVisible(bool visible);
    void setHideVisible(bool visible);
    void setMaximizeVisible(bool visible);
    void setPopOutVisible(bool visible);

    void setMaximizeToggled(bool toggled);

    // NOTE: This is a non-virtual function
    //  It is being overridden so that closing of the dockwidget has to be requested and approved
    //  It is used to reparent the dockwidget on close if need be instead of destructing it
    void close();

signals:
    void iconChanged();
    void titleChanged();
    void dockSetActive(bool);
    void req_Maximize(int ID, bool maximize);
    void req_Visible(int ID, bool visible);
    void req_PopOut(int ID);
    void req_Close(int ID);

private slots:
    void title_Maximize(bool maximize);
    void title_Visible(bool visible);
    void title_PopOut(bool);
    void title_Close(bool);

    void _visibilityChanged(bool visible);

    void destruct();
    void showContextMenu(const QPoint &point);

protected:
    explicit BaseDockWidget(BaseDockType type, QWidget* parent = nullptr);

    bool eventFilter(QObject *object, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void closeOrHide();

    void setActionVisible(DockTitleBar::DOCK_ACTION action, bool visible);
    void setActionToggled(DockTitleBar::DOCK_ACTION action, bool toggled);

    QAction* getAction(DockTitleBar::DOCK_ACTION action);
    QAction* addAction(const QString& text, const QString& iconPath = "", const QString& iconName = "", Qt::Alignment a = Qt::AlignRight);

    QString title;
    DockTitleBar* titleBar = nullptr;
    QPair<QString, QString> titleIcon;

    BaseWindow* sourceWindow = nullptr;
    BaseWindow* currentWindow = nullptr;

    QFrame* borderFrame = nullptr;

    Qt::DockWidgetArea initialArea = Qt::TopDockWidgetArea;
    BaseDockType type;

    bool _isProtected = false;
    bool _isActive = false;

    int ID;
    static int _DockWidgetID;
};

#endif // BASEDOCKWIDGET_H