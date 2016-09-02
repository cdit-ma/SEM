#ifndef DockWidgetActionItem_H
#define DockWidgetActionItem_H

#include <QLabel>
#include <QAction>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "dockwidgetitem.h"

class DockWidgetActionItem : public DockWidgetItem
{
    Q_OBJECT

public:
    explicit DockWidgetActionItem(QAction* action, QWidget* parent = 0);

    QAction* getAction();
    DOCKITEM_KIND getItemKind();

    void setSubActionRequired(bool required);
    bool requiresSubAction();

    void setProperty(const char *name, const QVariant &value);
    QVariant getProperty(const char *name);

public slots:
    void actionChanged();
    void themeChanged();
    void updateDisplayedText(QString text);

protected:
    void enterEvent(QEvent* event);
    void leaveEvent(QEvent* event);

private:
    void setupLayout();

    QAction* dockAction;
    bool subActionRequired;

    QLabel* textLabel;
    QLabel* iconLabel;
    QLabel* arrowLabel;
    QLabel* imageLabel;

    Theme* theme;
};

#endif // DockWidgetActionItem_H
