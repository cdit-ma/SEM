#ifndef DOCKADOPTABLENODEITEM_H
#define DOCKADOPTABLENODEITEM_H

#include <QPushButton>
#include <QLabel>

class DockAdoptableNodeItem : public QPushButton
{
    Q_OBJECT
public:
    explicit DockAdoptableNodeItem(QString _kind, QWidget* parent = 0);

    void setupLayout();
    void setPixmapSize(QSize size);

protected:
    void paintEvent(QPaintEvent* e);

signals:
    void itemPressed(QString kind);

public slots:
    void buttonPressed();

private:
    QString kind;

    QLabel* textLabel;
    QLabel* imageLabel;

    QSize initialSize;


};

#endif // DOCKADOPTABLENODEITEM_H
