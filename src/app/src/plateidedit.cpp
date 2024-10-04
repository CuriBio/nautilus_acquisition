#include "plateidedit.h"

PlateIdEdit::PlateIdEdit(QWidget *parent) : QLineEdit(parent) { }

void PlateIdEdit::focusInEvent(QFocusEvent *e) {
    QLineEdit::focusInEvent(e);
    this->completer()->complete();
}

// void PlateIdEdit::inputMethodEvent(QInputMethodEvent *e) {
//     QLineEdit::inputMethodEvent(e);
// #ifndef QT_NO_COMPLETER
//     if (e->commitString().isEmpty()) {
//         c->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
//         c->setCompletionPrefix(" ");
//     } else {
//         c->setCompletionMode(QCompleter::PopupCompletion);
//         c->setCompletionPrefix(e->commitString());
//     }
//     c->complete();
// #endif
// }
