#ifndef DOCKACTIONWIDGET_H
#define DOCKACTIONWIDGET_H

#include <QPushButton>
#include <QLabel>
#include <QAction>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "../theme.h"

class DockActionWidget : public QPushButton
{
    Q_OBJECT
public:
    explicit DockActionWidget(QAction* action, QWidget *parent = 0);

    QAction* getAction();

    void setSubActionRequired(bool required);
    bool requiresSubAction();

    void setProperty(const char *name, const QVariant &value);
    QVariant getProperty(const char *name);

signals:

public slots:
    void actionChanged();
    void themeChanged();

protected:
    void enterEvent(QEvent* event);
    void leaveEvent(QEvent* event);
    void resizeEvent(QResizeEvent* event);

private:
    void setupLayout();
    void updateTextLabel(QString text = "");

    QLabel* textLabel;
    QLabel* iconLabel;
    QLabel* arrowLabel;
    QLabel* imageLabel;

    QAction* dockAction;
    QString fullActionText;
    bool subActionRequired;

    Theme* theme;

};

#endif // DOCKACTIONWIDGET_H
