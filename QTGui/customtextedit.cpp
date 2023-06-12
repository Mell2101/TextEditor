#include "customtextedit.h"
#include "qmessagebox.h"
#include <QMenu>
#include <QContextMenuEvent>

CustomTextEdit::CustomTextEdit(QWidget* parent) : QTextEdit(parent)
{
    m_pContextMenu = new QMenu(this);
    
    connect(m_pContextMenu->addAction(tr("Undo")), &QAction::triggered, this, &CustomTextEdit::undo);
    connect(m_pContextMenu->addAction(tr("Redo")), &QAction::triggered, this, &CustomTextEdit::redo);
    connect(m_pContextMenu->addAction(tr("Cut")), &QAction::triggered, this, &CustomTextEdit::cut);
    connect(m_pContextMenu->addAction(tr("Copy")), &QAction::triggered, this, &CustomTextEdit::copy);
    connect(m_pContextMenu->addAction(tr("Paste")), &QAction::triggered, this, &CustomTextEdit::paste);
}

void CustomTextEdit::contextMenuEvent(QContextMenuEvent *event)
{
    if (event)
    {
        m_pContextMenu->exec(event->globalPos());
    }
}

void CustomTextEdit::onLoaded(const QString& data)
{
    setPlainText(data);
}

void CustomTextEdit::onSaved()
{
    QMessageBox messageBox
            (
                QMessageBox::Information,
                tr("Success"),
                tr("File saved                  "),
                QMessageBox::NoButton,
                this
            );

    messageBox.exec();
}
