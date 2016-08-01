#include "multilinedelegate.h"
#include <QDebug>
#include <QBoxLayout>
#include <QDialog>
#include "View/theme.h"

MultilineDelegate::MultilineDelegate(QWidget *parent):QStyledItemDelegate(parent)
{
    parentWidget = parent;
    setupLayout();

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    themeChanged();
}

MultilineDelegate::~MultilineDelegate()
{
        dialog->deleteLater();
}

QWidget *MultilineDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.data(-2).toBool()){
        //Check for multiline gear
        //Clear the editor
        codeEditor->clear();
        return dialog;
    }else{
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

void MultilineDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(index.data(-2).toBool()){
        QString keyName = index.model()->headerData(index.row(), Qt::Vertical).toString();
        dialog->setWindowTitle("Editing <" + keyName + ">");
        codeEditor->setPlainText(index.data(Qt::EditRole).toString());
    }else{
        return QStyledItemDelegate::setEditorData(editor, index);
    }
}

void MultilineDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(index.data(-2).toBool()){
        QString keyName = index.model()->headerData(index.row(), Qt::Vertical).toString();
        dialog->setWindowTitle("Editing <" + keyName + ">");
        codeEditor->setPlainText(index.data(Qt::EditRole).toString());
        model->setData(index, codeEditor->toPlainText());
    }else{
        return QStyledItemDelegate::setModelData(editor, model, index);
    }
}

void MultilineDelegate::submitPressed()
{
    emit commitData(dialog);
    closeDialog();

}

void MultilineDelegate::closeDialog()
{
    emit closeEditor(dialog);
}

void MultilineDelegate::themeChanged()
{
    dialog->setWindowIcon(Theme::theme()->getIcon("Actions", "Popup"));
    dialog->setStyleSheet(Theme::theme()->getDialogStyleSheet());
}

void MultilineDelegate::setupLayout()
{
    dialog = new QDialog(parentWidget);
    QVBoxLayout* layout = new QVBoxLayout(dialog);
    dialog->setMinimumSize(400,700);
    dialog->setModal(true);

    dialog->setWindowFlags(dialog->windowFlags() & (~Qt::WindowContextHelpButtonHint));

    codeEditor = new CodeEditor(parentWidget);

    QPushButton* submitButton = new QPushButton("Submit", parentWidget);

    QHBoxLayout* buttonLayout = new QHBoxLayout(0);
    buttonLayout->addWidget(submitButton, 0, Qt::AlignRight);
    dialog->setLayout(layout);
    layout->addWidget(codeEditor, 1);
    layout->addLayout(buttonLayout);

    connect(dialog, SIGNAL(rejected()), this, SLOT(closeDialog()));
    connect(submitButton, SIGNAL(clicked(bool)), this, SLOT(submitPressed()));
}

void MultilineDelegate::destroyEditor(QWidget *editor, const QModelIndex &index) const
{
    if(!index.data(-2).toBool()){
        QStyledItemDelegate::destroyEditor(editor, index);
    }
}

