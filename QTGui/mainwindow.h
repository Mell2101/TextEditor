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
    void OpenFontDialog();
//    void NewFile();
//    void OpenFile();
//    void SaveFile();
//    void ExitProgramm();

private:
    Ui::MainWindow *ui;
    class CustomTextEdit* TextArea;
};
