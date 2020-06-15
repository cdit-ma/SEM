#ifndef OVERLAYSPLITTER_H
#define OVERLAYSPLITTER_H

#include <QSplitter>
#include <QMouseEvent>

class OverlaySplitter : public QSplitter
{
    Q_OBJECT

public:
    explicit OverlaySplitter(QWidget* parent = nullptr);

    void setWidget(QWidget* widget, Qt::Alignment location = Qt::AlignBottom);

signals:
    void mouseEventReceived(QMouseEvent* event);

public slots:
    void widgetMinimised(bool minimised);

protected:
    bool eventFilter(QObject* object, QEvent* event);

private:
    QWidget* fillerWidget = nullptr;
    int widgetIndex = -1;

    QList<int> defaultSizes;
    int maximisedWidgetSize;
};

#endif // OVERLAYSPLITTER_H