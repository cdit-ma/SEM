#ifndef DOCKWIDGETITEM_H
#define DOCKWIDGETITEM_H

#include <QToolButton>
#include "../theme.h"

class DockWidgetItem : public QToolButton
{
    Q_OBJECT

public:
    enum DOCKITEM_KIND{DEFAULT_ITEM, ACTION_ITEM, GROUP_ACTION_ITEM};
    explicit DockWidgetItem(QString text, QWidget* parent = 0);

    void setText(QString text);
    QString getText();
    QString getDisplayedText();

    void displayToolButtonText(bool show);
    void setIconSize(int width, int height);

    virtual DOCKITEM_KIND getItemKind();

signals:
    void textChanged();
    void displayedTextChanged(QString);
    void hoverEnter(int ID = -1);
    void hoverLeave(int ID = -1);

public slots:
    void themeChanged();

protected:
    void resizeEvent(QResizeEvent* event);

private:
    void setupLayout();
    void updateTextLabel();

    QString fullText;
    QString currentText;
    bool displayText;
    int iconWidth;
};

#endif // DOCKWIDGETITEM_H
