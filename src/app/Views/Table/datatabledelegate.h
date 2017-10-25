#ifndef DATATABLEDELEGATE_H
#define DATATABLEDELEGATE_H

#include <QStyledItemDelegate>
#include "../../Widgets/CodeEditor/codeeditor.h"

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

private slots:
    void submitPressed();
    void closeDialog();
    void themeChanged();
private:
    void setupLayout();

    QWidget* parentWidget;
    QDialog* dialog;
    CodeEditor* codeEditor;
};

#endif // DATATABLEDELEGATE_H
