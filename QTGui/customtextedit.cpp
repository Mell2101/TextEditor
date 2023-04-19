#include "customtextedit.h"
#include <QMenu>
#include <QContextMenuEvent>

CustomTextEdit::CustomTextEdit(QWidget* parent) : QTextEdit(parent)
{
    contextMenu = new QMenu(this);

    connect(contextMenu->addAction(tr("Undo")), &QAction::triggered, this, &CustomTextEdit::undo);
    connect(contextMenu->addAction(tr("Redo")), &QAction::triggered, this, &CustomTextEdit::redo);
    connect(contextMenu->addAction(tr("Cut")), &QAction::triggered, this, &CustomTextEdit::cut);
    connect(contextMenu->addAction(tr("Copy")), &QAction::triggered, this, &CustomTextEdit::copy);
    connect(contextMenu->addAction(tr("Paste")), &QAction::triggered, this, &CustomTextEdit::paste);
}

void CustomTextEdit::contextMenuEvent(QContextMenuEvent *event)
{
    contextMenu->exec(event->globalPos());
}
