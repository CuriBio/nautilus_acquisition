#include <spdlog/spdlog.h>
#include <QCompleter>

#include "plateidedit.h"


PlateIdEdit::PlateIdEdit(QWidget *parent) : QLineEdit(parent) { }

void PlateIdEdit::focusInEvent(QFocusEvent *e) {
    QLineEdit::focusInEvent(e);
    // force completer to show when focus is gained, by default it won't show until a key is pressed
    // TODO should wrap this in an if statement? and/or maybe need to change completion type based on the text
    this->completer()->complete();
}

void PlateIdEdit::keyPressEvent(QKeyEvent *e) {
    QLineEdit::keyPressEvent(e);
    // force completer to show whenever a key is pressed. The completer will not show by default
    // if the text box is empty, this forces it to show anyway
    // TODO should wrap this in an if statement? and/or maybe need to change completion type based on the text
    this->completer()->complete();
}
