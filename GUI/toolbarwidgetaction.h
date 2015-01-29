#ifndef TOOLBARWIDGETACTION_H
#define TOOLBARWIDGETACTION_H

#include <QWidgetAction>
#include <QPushButton>


class ToolbarWidgetAction : public QWidgetAction
{
    Q_OBJECT
public:
    explicit ToolbarWidgetAction(QString nodeKind, QWidget *parent = 0);

    QString getKind();

protected:
    QWidget* createWidget(QWidget *parent);

signals:

public slots:
    void actionButtonPressed();

private:
    QString kind;
    QPushButton* actionButton;

};

#endif // TOOLBARWIDGETACTION_H
