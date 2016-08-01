#ifndef MULTILINEDELEGATE_H
#define MULTILINEDELEGATE_H

#include <QStyledItemDelegate>
#include "../../GUI/codeeditor.h"
#include <QPushButton>

class MultilineDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    MultilineDelegate(QWidget* parent);
    ~MultilineDelegate();

    // QAbstractItemDelegate interface
public:
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
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
public:
    void destroyEditor(QWidget *editor, const QModelIndex &index) const;
};

#endif // MULTILINEDELEGATE_H
