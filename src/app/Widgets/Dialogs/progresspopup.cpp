#include "progresspopup.h"
#include "../../theme.h"
#include "../../Controllers/WindowManager/windowmanager.h"
#include <QBoxLayout>
#include <QTimer>

ProgressPopup::ProgressPopup():PopupWidget(PopupWidget::TYPE::TOOL, 0){
    setupLayout();
    connect(Theme::theme(), &Theme::theme_Changed, this, &ProgressPopup::themeChanged);
    themeChanged();
}

void ProgressPopup::ProgressUpdated(bool set_visible, QString description){
    label_text->setText(description);
    auto is_visible = isVisible();
    if(set_visible != is_visible){
        if(set_visible){
            QMetaObject::invokeMethod(this, "adjustSize", Qt::QueuedConnection);
            //Centralize it
            WindowManager::MoveWidget(this);
            show();
        }else{
            progress_bar->reset();
        }
    }
}

void ProgressPopup::UpdateProgressBar(int value){
    if(value == -1){
        progress_bar->setRange(0, 0);
    }else{
        progress_bar->setRange(0, 100);
        progress_bar->setValue(value);
        progress_bar->update();
        if (value >= 100){
            // Animation on ubuntu caused phantom window issues. Singleshot sleep timer resolves.
            //https://stackoverflow.com/questions/18037618/qt-qdialog-not-hiding-properly-when-show-hide-called-quickly
            QTimer::singleShot(100, this, &QDialog::hide);
        }
    }
}

void ProgressPopup::themeChanged(){
    auto theme = Theme::theme();
    setStyleSheet(theme->getProgressBarStyleSheet() + " QFrame{background:transparent;}  QLabel{color: " + theme->getTextColorHex() + ";}");
}

void ProgressPopup::setupLayout(){
    auto frame = new QFrame(this);
    auto layout = new QVBoxLayout(frame);
    layout->setMargin(2);
    layout->setSpacing(5);

    label_text = new QLabel(this);
    label_text->setAlignment(Qt::AlignCenter);
    label_text->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    
    progress_bar = new QProgressBar(this);
    progress_bar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    progress_bar->setRange(0,100);
    progress_bar->setTextVisible(false);
    layout->addWidget(label_text);
    layout->addWidget(progress_bar);

    setWidget(frame);
}