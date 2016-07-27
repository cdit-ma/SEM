#ifndef SELECTIONCONTROLLER_H
#define SELECTIONCONTROLLER_H
#include <QObject>
#include "../../Controller/selectionhandler.h"

class MedeaViewDockWidget;
class SelectionController : public QObject
{
    Q_OBJECT
public:
    SelectionController(ViewController* controller);

    SelectionHandler* constructSelectionHandler(QObject* object);
    void registerSelectionHandler(QObject* object, SelectionHandler* handler);
    void unregisterSelectionHandler(QObject* object, SelectionHandler* handler);


    QVector<ViewItem*> getSelection();
    int getSelectionCount();
    ViewItem* getFirstSelectedItem();

signals:
    void itemActiveSelectionChanged(ViewItem* item, bool isActive);
    void selectionChanged(int selected);
    void viewItemDeleted(ViewItem* item);
    void clearSelection();
    void selectAll();
private slots:
    void activeViewDockWidgetChanged(MedeaViewDockWidget* widget);
    void cycleActiveSelectionBackward();
    void cycleActiveSelectionForward();
    void removeSelectionHandler();
private:
    void cycleActiveSelectedItem(bool forward);
    void setCurrentSelectionHandler(SelectionHandler* handler);
    SelectionHandler* currentHandler;
    QHash<QObject*, int> selectionHandlerIDLookup;
    QHash<int, SelectionHandler*> selectionHandlers;
    ViewController* viewController;
};

#endif // SELECTIONCONTROLLER_H
