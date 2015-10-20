#ifndef GRAPHICSCOMBOBOX_H
#define GRAPHICSCOMBOBOX_H
#include <QComboBox>

class GraphicsComboBox : public QComboBox
{
    Q_OBJECT
public:
    GraphicsComboBox();
    void hidePopup();

signals:
    void combobox_Closed();
};

#endif // GRAPHICSCOMBOBOX_H
