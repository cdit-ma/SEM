#include "multilinedelegate.h"
#include "attributetablemodel.h"

#include <QPushButton>
#include <QComboBox>
#include <QBoxLayout>
#include <QDialog>
#include "View/theme.h"
#include <QStringBuilder>
#include <QDebug>


AttributeTableDelegate::AttributeTableDelegate(QWidget *parent):QStyledItemDelegate(parent)
{
    parentWidget = parent;

    setupLayout();

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    themeChanged();
}

AttributeTableDelegate::~AttributeTableDelegate()
{
    dialog->deleteLater();
}

QWidget *AttributeTableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //If we have a Multiline editor we should popup a code editor.
    if(index.data(AttributeTableModel::MULTILINE_ROLE).toBool()){
        codeEditor->clear();
        return dialog;
    }else{
        QStringList validValues = index.data(AttributeTableModel::VALID_VALUES_ROLE).toStringList();
        if(!validValues.isEmpty()){
            QComboBox *editor = new QComboBox(parent);
            editor->addItems(validValues);
            return editor;
        }else{
            return QStyledItemDelegate::createEditor(parent, option, index);
        }
    }
}

void AttributeTableDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(index.data(AttributeTableModel::MULTILINE_ROLE).toBool()){
        QString keyName = index.model()->headerData(index.row(), Qt::Vertical).toString();
        dialog->setWindowTitle("Editing <" + keyName + ">");
        codeEditor->setPlainText(index.data(Qt::EditRole).toString());
    }else{
        QComboBox *spinBox = qobject_cast<QComboBox*>(editor);
        if(spinBox){
            QString currentValue = index.model()->data(index, Qt::EditRole).toString();
            spinBox->setCurrentText(currentValue);
        }else{
            QStyledItemDelegate::setEditorData(editor, index);
        }
    }
}

void AttributeTableDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(index.data(AttributeTableModel::MULTILINE_ROLE).toBool()){
        model->setData(index, codeEditor->toPlainText());
    }else{
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

void AttributeTableDelegate::submitPressed()
{
    emit commitData(dialog);
    closeDialog();
}

void AttributeTableDelegate::closeDialog()
{
    emit closeEditor(dialog);
}

void AttributeTableDelegate::themeChanged()
{

    Theme* t = Theme::theme();
    dialog->setWindowIcon(t->getIcon("Actions", "Popup"));
    dialog->setStyleSheet(t->getDialogStyleSheet() % t->getPushButtonStyleSheet() );
}

void AttributeTableDelegate::setupLayout()
{
    dialog = new QDialog();
    dialog->setMinimumSize(400,700);
    dialog->setModal(true);
    //Get rid of the ? button.
    dialog->setWindowFlags(dialog->windowFlags() & (~Qt::WindowContextHelpButtonHint));

    codeEditor = new CodeEditor(parentWidget);
    QPushButton* submitButton = new QPushButton("Submit", parentWidget);

    QVBoxLayout* layout = new QVBoxLayout(dialog);

    layout->addWidget(codeEditor, 1);
    layout->addWidget(submitButton, 0, Qt::AlignRight);

    connect(dialog, SIGNAL(rejected()), this, SLOT(closeDialog()));
    connect(submitButton, SIGNAL(clicked(bool)), this, SLOT(submitPressed()));
}

void AttributeTableDelegate::destroyEditor(QWidget *editor, const QModelIndex &index) const
{
    if(!index.data(AttributeTableModel::MULTILINE_ROLE).toBool()){
        QStyledItemDelegate::destroyEditor(editor, index);
    }
}

