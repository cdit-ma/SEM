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

protected:
	bool eventFilter(QObject *object, QEvent *event) override;
	
private:
	void themeChanged();
    void setupLayout();

    QLineEdit* line_edit = nullptr;
};

#endif // FILTERWIDGET_H