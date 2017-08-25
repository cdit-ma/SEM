#ifndef PROGRESSPOPUP_H
#define PROGRESSPOPUP_H


#include <QLabel>
#include <QProgressBar>
#include "../../Widgets/Dialogs/popupwidget.h"

class ProgressPopup : public PopupWidget
{
    Q_OBJECT
public:
    explicit ProgressPopup();
public slots:
    void ProgressUpdated(bool show, QString description);
    void UpdateProgressBar(int value);
private:
    void themeChanged();
    void setupLayout(); 

    QProgressBar* progress_bar = 0;
    QLabel* label_text = 0;
};

#endif //PROGRESSPOPUP_H