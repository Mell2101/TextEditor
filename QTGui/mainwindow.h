#pragma once

#include <QMainWindow>

namespace TextEditorCore {
    class Document;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;

public slots:
//    void openFontDialog();
    void openPrintDocumentDialog();
    void updateTextCursorPosInfo();
    void newFile();
    void openFile();
    void saveFile();
    void openAboutMessageBox();
    void exitProgramm();
    void updateTextData(int pos, int removed, int added);

private:
    class CustomTextEdit* m_pTextArea;
    class QLabel* m_pTextCursorPosInfo;
    TextEditorCore::Document* m_pDocument;
    size_t m_currentDocIndex = 0;

private:
    void menuInit();
    void statusBarInit();
    void toolBarInit();
};
