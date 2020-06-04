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
    void ProgressUpdated(bool show, const QString& description);
    void UpdateProgressBar(int value);

private:
    void themeChanged();
    void setupLayout(); 

    QProgressBar* progress_bar = nullptr;
    QLabel* label_text = nullptr;
};

#endif // PROGRESSPOPUP_H