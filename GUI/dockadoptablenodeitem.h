#ifndef DOCKADOPTABLENODEITEM_H
#define DOCKADOPTABLENODEITEM_H

#include <QPushButton>
#include <QLabel>

class DockAdoptableNodeItem : public QPushButton
{
    Q_OBJECT
public:
    explicit DockAdoptableNodeItem(QString _kind, QWidget* parent = 0);

protected:
    void paintEvent(QPaintEvent* e);

private:
    QString kind;

signals:
    void itemPressed(QString kind);

public slots:
    void buttonPressed();
    //void destructGraphML();


};

#endif // DOCKADOPTABLENODEITEM_H
