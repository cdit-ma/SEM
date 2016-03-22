#include "shortcutdialog.h"
#include <QHeaderView>
#include <QAction>
#include <QPalette>
#include <QBrush>
#include <QDebug>
#include "../theme.h"
#define WIDTH 550
#define HEIGHT 650

ShortcutDialog::ShortcutDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle("MEDEA - Shortcuts");
    setWindowFlags(windowFlags() & (~Qt::WindowContextHelpButtonHint));
    setWindowIcon(Theme::theme()->getImage("Actions", "Keyboard"));
    setModal(false);

    resize(WIDTH, HEIGHT);

    setupLayout();
    addTitle("Global", Theme::theme()->getImage("Actions", "Global"));

    addShortcut("F1", "Opens the Shortcut window.");
    addShortcut("F3", "Sets the focus into the search box.");
    addShortcut("F10", "Opens the settings window.");
    addShortcut("F11", "Toggles fullscreen mode.");
    addShortcut("F12", "Takes a screenshot.");
    addShortcut("CTRL + SHIFT + M", "Toggles minimap visibility.");
    addShortcut("ARROW KEYS", "Nudges the viewport by a small amount.");
    addShortcut("SHIFT + ARROW KEYS", "Nudges the viewport by a larger amount.");

    addTitle("Project", Theme::theme()->getImage("Actions", "New"));
    addShortcut("CTRL + N", "Constructs a new project.");
    addShortcut("CTRL + O", "Opens an existing project.");
    addShortcut("CTRL + S", "Saves the current project.");
    addShortcut("CTRL + SHIFT + S", "Saves the current project as another file.");
    addShortcut("CTRL + I", "Imports a Graphml document into current project.");
    addShortcut("CTRL + SHIFT + V", "Validates the current project.");
    addShortcut("CTRL + Z", "Undoes the last change in the model.");
    addShortcut("CTRL + Y", "Redoes the last Undo.");
    addShortcut("CTRL + Space", "Fits the entire model into the view.");
    addShortcut("CTRL + J", "Imports the Nodes from the Jenkins Server.");





    addTitle("Selection", Theme::theme()->getImage("Actions", "SelectAll"));
    addShortcut("ESC", "Clears the current selection / Closes current dialog window.");
    addShortcut("F2", "Renames the current (singular) selection.");
    addShortcut("CTRL + A", "Selects all children of currently selected entity.");
    addShortcut("CTRL + C", "Copies the currently selected entities.");
    addShortcut("CTRL + D", "Replicates the currently selected entities.");
    addShortcut("CTRL + X", "Cuts the currently selected entities.");
    addShortcut("CTRL + V", "Pastes the data from the clipboard into the current selected entity.");
    addShortcut("CTRL", "Holding control allows multiple selection.");
    addShortcut("DELETE", "Deletes all selected entities.");
    addShortcut("SHIFT + D", "Centers on the (singular) selected entity's definition.");
    addShortcut("SHIFT + I", "Centers on the (singular) selected entity's implementation.");

    addTitle("Mouse", Theme::theme()->getImage("Actions", "Mouse"));
    addShortcut("L MOUSE", "Selects the entity under the cursor.");
    addShortcut("R MOUSE", "Opens the toolbar for the selection.");
    addShortcut("M MOUSE", "Centers the entity under the cursor.");

    addShortcut("SCROLL UP", "Zooms in view.");
    addShortcut("SCROLL DOWN", "Zooms out view.");

    addShortcut("L MOUSE + DRAG", "Moves the selected entities.");
    addShortcut("R MOUSE + DRAG", "Pans the view.");
    //addShortcut("CTRL + M MOUSE", "Sorts the children of the entity under the cursor.");
    addShortcut("CTRL + SHIFT", "Turns on rubberband selection mode.");

    tableWidget->resizeColumnsToContents();
}

void ShortcutDialog::addShortcut(QString shortcut, QString description)
{
    QFont boldFont;
    QFont italicFont;
    boldFont.setBold(true);
    italicFont.setItalic(true);
    boldFont.setPointSize(boldFont.pointSize() - 2);
    italicFont.setPointSize(italicFont.pointSize() - 1);


    #ifdef Q_OS_DARWIN
        //Replace shortcut with Command.
        shortcut.replace("CTRL", "COMMAND");
    #endif

    QTableWidgetItem * shortcutR = new QTableWidgetItem(shortcut);
    QTableWidgetItem * descriptionR = new QTableWidgetItem(description);

    shortcutR->setTextAlignment(Qt::AlignCenter);
    shortcutR->setFont(boldFont);
    descriptionR->setFont(italicFont);

    shortcutR->setForeground(QColor(90,90,90));
    descriptionR->setForeground(QColor(90,90,90));

    descriptionR->setFlags(descriptionR->flags() ^ Qt::ItemIsEditable);
    shortcutR->setFlags(shortcutR->flags() ^ Qt::ItemIsEditable);


    if(tableWidget){
        int insertLocation = tableWidget->rowCount();
        tableWidget->insertRow(insertLocation);
        tableWidget->setItem(insertLocation, 0, shortcutR);
        tableWidget->setItem(insertLocation, 1, descriptionR);
    }
}

void ShortcutDialog::addTitle(QString label, QIcon icon)
{
    if(tableWidget){
        QFont boldFont;
        boldFont.setBold(true);
        boldFont.setPointSize(boldFont.pointSize() + 1);

        QTableWidgetItem * titleRow = new QTableWidgetItem(label);
        titleRow->setFlags(titleRow->flags() ^ Qt::ItemIsEditable);
        titleRow->setIcon(icon);

        titleRow->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        titleRow->setFont(boldFont);
        titleRow->setBackgroundColor(palette().color(QPalette::Base));


        int insertLocation = tableWidget->rowCount();
        tableWidget->insertRow(insertLocation);
        tableWidget->setItem(insertLocation, 0, titleRow);
        tableWidget->setSpan(insertLocation, 0, 1, 2);
    }
}



void ShortcutDialog::setupLayout()
{
    layout = new QVBoxLayout();
    this->setLayout(layout);
    tableWidget = new QTableWidget(0, 2, this);
    tableWidget->verticalHeader()->setVisible(false);
    tableWidget->horizontalHeader()->setVisible(false);

    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->setShowGrid(false);
    tableWidget->setAlternatingRowColors(true);
    tableWidget->setEnabled(true);

    /*
    QTableWidgetItem * shortcutTitle = new QTableWidgetItem(QIcon(":/Actions/Keyboard.png"), "Shortcut");
    shortcutTitle->setTextAlignment(Qt::AlignHCenter);
    QTableWidgetItem * descriptionTitle = new QTableWidgetItem(QIcon(":/Actions/Notes.png"), "Description");
    descriptionTitle->setTextAlignment(Qt::AlignHCenter);

    tableWidget->setHorizontalHeaderItem(0, shortcutTitle);
    tableWidget->setHorizontalHeaderItem(1, descriptionTitle);
    */


    layout->addWidget(tableWidget);
}
