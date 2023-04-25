#pragma once

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void openFontDialog();
    void updateTextCursorPosInfo();
    void newFile();
    void openFile();
    void saveFile();
    void exitProgramm();

private:
    Ui::MainWindow *ui;
    class CustomTextEdit* m_pTextArea;
    class QLabel* m_pTextCursorPosInfo;

private:
    void menuInit();
    void statusBarInit();
    void toolBarInit();
};
