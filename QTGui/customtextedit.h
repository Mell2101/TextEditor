#pragma once

#include <QTextEdit>

class CustomTextEdit : public  QTextEdit
{
    Q_OBJECT
public:
    CustomTextEdit(QWidget* parent);

protected:
    QMenu* contextMenu;

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;
};
