#include "plateidedit.h"

PlateIdEdit::PlateIdEdit(QWidget *parent) : QLineEdit(parent) { }

// void PlateIdEdit::setCompleter(QCompleter *completer) {
//     if (c)
//         c->disconnect(this);

//     c = completer;
//     QLineEdit::setCompleter(completer);

//     if (!c)
//         return;

//     c->setWidget(this);
//     c->setCompletionMode(QCompleter::PopupCompletion);
//     c->setCaseSensitivity(Qt::CaseInsensitive);
// }

// QCompleter *PlateIdEdit::completer() const {
//     return c;
// }

void PlateIdEdit::focusInEvent(QFocusEvent *e) {
    // if (c)
    //     c->setWidget(this);
    this->completer()->complete();
    QLineEdit::focusInEvent(e);
}

// void PlateIdEdit::inputMethodEvent(QInputMethodEvent *e) {
    // QLineEdit::inputMethodEvent(e);
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
