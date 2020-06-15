#ifndef DATATABLEDELEGATE_H
#define DATATABLEDELEGATE_H

#include <QStyledItemDelegate>

#include "../../Widgets/CodeEditor/codeeditor.h"
#include "../../Widgets/iconpicker.h"

class DataTableDelegate : public QStyledItemDelegate
{
    Q_OBJECT
    
public:
    explicit DataTableDelegate(QWidget* parent);
    ~DataTableDelegate() final;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void destroyEditor(QWidget *editor, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    QPair<QModelIndex, QModelIndex> getModelIconIndex(const QAbstractItemModel* model, int column) const;

private slots:
    void themeChanged();

private:    
    QDialog* get_editor_dialog();
    QDialog* get_icon_dialog();

    QDialog* editor_dialog = nullptr;
    QDialog* icon_dialog = nullptr;
    IconPicker* icon_picker = nullptr;
    CodeEditor* code_editor = nullptr;
};

#endif // DATATABLEDELEGATE_H