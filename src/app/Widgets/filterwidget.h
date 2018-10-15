#ifndef FILTERWIDGET_H
#define FILTERWIDGET_H


#include <QToolBar>
#include <QLineEdit>
#include <QToolButton>

class FilterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FilterWidget();

signals:
    void filterChanged(QString filter);

private:
    void themeChanged();
    void setupLayout();

private:
    QLineEdit* line_edit = 0;
    QToolButton* icon_button = 0;
    QToolButton* reset_button = 0;

protected:
    bool eventFilter(QObject *object, QEvent *event);
};

#endif // FILTERWIDGET_H
