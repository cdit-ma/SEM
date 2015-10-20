#include "graphicscombobox.h"
#include <QDebug>


GraphicsComboBox::GraphicsComboBox():QComboBox()
{
}

void GraphicsComboBox::hidePopup()
{
    QComboBox::hidePopup();

    emit combobox_Closed();
    //emit currentTextChanged(currentText());
}


