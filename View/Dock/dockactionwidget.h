#ifndef DOCKACTIONWIDGET_H
#define DOCKACTIONWIDGET_H

#include <QPushButton>
#include <QLabel>
#include <QAction>
#include <QVBoxLayout>
#include <QHBoxLayout>

class DockActionWidget : public QPushButton
{
    Q_OBJECT
public:
    explicit DockActionWidget(QAction* action, QWidget *parent = 0);
    void requiresSubAction(bool required);

signals:

public slots:
    void actionChanged();
    void themeChanged();

private:
    void setupLayout();

    QHBoxLayout* mainLayout;
    QLabel* textLabel;
    QLabel* iconLabel;
    QLabel* arrowLabel;

    QAction* dockAction;
    bool subActionRequired;

};

#endif // DOCKACTIONWIDGET_H
