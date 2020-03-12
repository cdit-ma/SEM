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
    explicit IconPicker(QWidget *parent = nullptr);
    
    void clear();
    
    void setCurrentIcon(const QString& icon_prefix, const QString& icon_name);
    QPair<QString, QString> getCurrentIcon();

signals:
    void ApplyIcon();

protected:
    void showEvent(QShowEvent *event) override;

private:
    void themeChanged();
    void setupLayout();
    void iconPressed(QAction* action);
    void updateIcon();
    void clearSelection();

    QSet<QAction*> all_icons;
    QSet<QAction*> selected_icons;

    QLabel* label_selected_icon = nullptr;
    QLabel* label_icon_prefix = nullptr;
    QLabel* label_icon = nullptr;
    QLineEdit* edit_icon_prefix = nullptr;
    QLineEdit* edit_icon = nullptr;
    QAction* apply_action = nullptr;
    
    QSize preview_icon_size = QSize(64, 64);
    bool setup_layout = false;
};

#endif // ICON_PICKER_H