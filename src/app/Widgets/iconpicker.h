#ifndef ICONPICKER_H
#define ICONPICKER_H

#include <QFrame>
#include <QSet>
#include <QAction>
#include <QLabel>
#include <QLineEdit>
#include <QSize>

class IconPicker : public QFrame
{
    Q_OBJECT

public:
    IconPicker(QWidget *parent = 0);
    void clear();
    void setCurrentIcon(QString icon_prefix, QString icon_name);
    QPair<QString, QString> getCurrentIcon();
signals:
    void ApplyIcon();

    
private:
    void themeChanged();
    void setupLayout();
    void iconPressed(QAction* action);

    void updateIcon();

    
    void clearSelection();

    QSet<QAction*> all_icons;
    QSet<QAction*> selected_icons;

    QLabel* label_selected_icon = 0;
    QLabel* label_icon_prefix = 0;
    QLabel* label_icon = 0;
    QLineEdit* edit_icon_prefix = 0;
    QLineEdit* edit_icon = 0;
    QAction* apply_action = 0;
    QSize preview_icon_size = QSize(64, 64);


};


#endif //ICON_PICKER_H
