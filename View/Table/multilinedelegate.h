#ifndef ATTRIBUTETABLEDELEGATE_H
#define ATTRIBUTETABLEDELEGATE_H

#include <QStyledItemDelegate>
#include "../../GUI/codeeditor.h"

class AttributeTableDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    AttributeTableDelegate(QWidget* parent);
    ~AttributeTableDelegate();

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

#endif // ATTRIBUTETABLEDELEGATE_H
