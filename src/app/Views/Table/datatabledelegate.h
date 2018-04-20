#ifndef DATATABLEDELEGATE_H
#define DATATABLEDELEGATE_H

#include <QStyledItemDelegate>
#include "../../Widgets/CodeEditor/codeeditor.h"
#include "../../Widgets/iconpicker.h"
class DataTableDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    DataTableDelegate(QWidget* parent);
    ~DataTableDelegate();

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void destroyEditor(QWidget *editor, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    QPair<QModelIndex, QModelIndex> getModelIconIndex(const QAbstractItemModel* model, int column) const;

private slots:
    void themeChanged();
private:
    void setupLayout();
    QDialog* get_editor_dialog();
    QDialog* get_icon_dialog();

    QWidget* parentWidget = 0;
    QDialog* editor_dialog = 0;
    QDialog* icon_dialog = 0;
    IconPicker* icon_picker = 0;
    CodeEditor* code_editor = 0;
};

#endif // DATATABLEDELEGATE_H
