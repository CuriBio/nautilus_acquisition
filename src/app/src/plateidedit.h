#include <QLineEdit>
#include <QCompleter>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QInputMethodEvent>
#include <QWidget>

#ifndef PLATEIDEDIT_H
#define PLATEIDEDIT_H


class PlateIdEdit : public QLineEdit {
    Q_OBJECT

    public:
        PlateIdEdit(QWidget *parent = nullptr);
        ~PlateIdEdit() {}

    protected:
        void focusInEvent(QFocusEvent *e) override;
        void keyPressEvent(QKeyEvent *e) override;
};

#endif // PLATEIDEDIT_H
