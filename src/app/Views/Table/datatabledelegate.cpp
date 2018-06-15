#include "datatabledelegate.h"
#include "datatablemodel.h"
#include "../../theme.h"

#include <QPushButton>
#include <QComboBox>
#include <QBoxLayout>
#include <QDialog>
#include <QStringBuilder>
#include <QDebug>


DataTableDelegate::DataTableDelegate(QWidget *parent):QStyledItemDelegate(parent)
{
    parentWidget = parent;

    get_editor_dialog();
    get_icon_dialog();


    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    themeChanged();
}

DataTableDelegate::~DataTableDelegate()
{
    if(editor_dialog){
        editor_dialog->deleteLater();
    }

    if(icon_dialog){
        icon_dialog->deleteLater();
    }
}

QWidget *DataTableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //If we have a Multiline editor we should popup a code editor.
    if(index.data(DataTableModel::MULTILINE_ROLE).toBool()){
        return editor_dialog;
    }else if(index.data(DataTableModel::ICON_ROLE).toBool()){
        return icon_dialog;
    }else{
        QStringList validValues = index.data(DataTableModel::VALID_VALUES_ROLE).toStringList();
        if(!validValues.isEmpty()){
            QComboBox *editor = new QComboBox(parent);
            editor->addItems(validValues);
            return editor;
        }else{
            return QStyledItemDelegate::createEditor(parent, option, index);
        }
    }
}

void DataTableDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(index.data(DataTableModel::MULTILINE_ROLE).toBool()){
        if(icon_dialog){
            QString keyName = index.model()->headerData(index.row(), Qt::Vertical).toString();
            icon_dialog->setWindowTitle("Editing <" + keyName + ">");
            code_editor->setPlainText(index.data(Qt::EditRole).toString());
        }
    }else if(index.data(DataTableModel::ICON_ROLE).toBool()){
        auto model_icon = getModelIconIndex(index.model(), index.column());

        if(icon_picker){
            auto icon_prefix = model_icon.first.data(Qt::EditRole).toString();
            auto icon_name = model_icon.second.data(Qt::EditRole).toString();
            icon_picker->setCurrentIcon(icon_prefix, icon_name);
        }
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

QPair<QModelIndex, QModelIndex> DataTableDelegate::getModelIconIndex(const QAbstractItemModel* model, int column) const{
    QPair<QModelIndex, QModelIndex> icon;
    for(int row = 0; row < model->rowCount(); row ++){
        QString key_name = model->headerData(row, Qt::Vertical).toString();
        if(key_name == "icon"){
            icon.second = model->index(row, column);
        }else if(key_name == "icon_prefix"){
            icon.first = model->index(row, column);
        }
    }
    return icon;
}

void DataTableDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(index.data(DataTableModel::MULTILINE_ROLE).toBool()){
        if(code_editor){
            model->setData(index, code_editor->toPlainText());
        }
    }else if(index.data(DataTableModel::ICON_ROLE).toBool()){
        if(icon_picker){
            auto model_icon = getModelIconIndex(model, index.column());
            auto icon = icon_picker->getCurrentIcon();

            if(model_icon.first.isValid() && model_icon.second.isValid()){
                model->setData(model_icon.first, icon.first);
                model->setData(model_icon.second, icon.second);
            }
        }
    }else{
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}



void DataTableDelegate::themeChanged()
{
    Theme* t = Theme::theme();

    if(editor_dialog){
        editor_dialog->setWindowIcon(t->getIcon("Icons", "popOut"));
        editor_dialog->setStyleSheet(t->getDialogStyleSheet() % t->getPushButtonStyleSheet() );
    }

    if(icon_dialog){
        icon_dialog->setStyleSheet(t->getDialogStyleSheet());;
    }

    
}

QDialog* DataTableDelegate::get_editor_dialog(){
    if(!editor_dialog){

        editor_dialog = new QDialog();
        editor_dialog->setMinimumSize(400,700);
        editor_dialog->setModal(true);
    
        //Get rid of the ? button.
        editor_dialog->setWindowFlags(editor_dialog->windowFlags() & (~Qt::WindowContextHelpButtonHint));

        code_editor = new CodeEditor(editor_dialog);
        auto submit_button = new QPushButton("Submit", editor_dialog);

        auto layout = new QVBoxLayout(editor_dialog);

        layout->addWidget(code_editor, 1);
        layout->addWidget(submit_button, 0, Qt::AlignRight);

        connect(editor_dialog, &QDialog::rejected, [=](){emit closeEditor(editor_dialog);});
        connect(submit_button, &QPushButton::clicked, [=](){
            emit commitData(editor_dialog);
            emit closeEditor(editor_dialog);
            });
        
    }
    code_editor->clear();
    return editor_dialog;
}

QDialog* DataTableDelegate::get_icon_dialog(){
    if(!icon_dialog){

        icon_dialog = new QDialog();
        icon_dialog->setModal(true);
    
        //Get rid of the ? button.
        icon_dialog->setWindowFlags(icon_dialog->windowFlags() & (~Qt::WindowContextHelpButtonHint));

        icon_picker = new IconPicker(icon_dialog);
        auto layout = new QVBoxLayout(icon_dialog);

        layout->addWidget(icon_picker, 1);


        connect(icon_dialog, &QDialog::rejected, [=](){emit closeEditor(editor_dialog);});
        connect(icon_picker, &IconPicker::ApplyIcon, [=](){
            emit commitData(icon_dialog);
            emit closeEditor(icon_dialog);
            });
    }
    icon_picker->clear();
    return icon_dialog;
}


void DataTableDelegate::destroyEditor(QWidget *editor, const QModelIndex &index) const
{
    if(index.data(DataTableModel::MULTILINE_ROLE).toBool()){
    
    }else if(index.data(DataTableModel::ICON_ROLE).toBool()){

    }else{
        QStyledItemDelegate::destroyEditor(editor, index);
    }
}

