#ifndef FILTERBUTTON_H
#define FILTERBUTTON_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>

class FilterButton : public QWidget
{
    Q_OBJECT
public:
    FilterButton(QString filter, QWidget *parent = 0);
    ~FilterButton();
    void setFilter(QString f);
    QString getFilter();
signals:
    void removeFilter(QString);

public slots:
    void buttonPressed();

private:
    QString filter;
    QLabel* label;
    QPushButton* button;


    // QWidget interface

    // QWidget interface


    // QWidget interface
protected:
    void paintEvent(QPaintEvent *);
};

#endif // FILTERBUTTON_H
