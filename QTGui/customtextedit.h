#pragma once

#include <QTextEdit>

class CustomTextEdit : public  QTextEdit
{
    Q_OBJECT
public:
    CustomTextEdit(QWidget* parent);
    QString getMousePosInfo();

protected:
    QMenu* contextMenu;

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

signals:
    void mouseMoved(int x, int y);
};
