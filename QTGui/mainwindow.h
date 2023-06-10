#pragma once

#include <QMainWindow>
#include <TextEditorCore/IDocumentListener.h>
#include <TextEditorCore/Document.h>

class MainWindow : public QMainWindow, public TextEditorCore::IDocumentListener
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow() = default;

public slots:
//    void openFontDialog();
    void openPrintDocumentDialog();
    void updateTextCursorPosInfo();
    void newFile();
    void openFile();
    void saveFile();
    void openAboutMessageBox();
    void exitProgramm();
    void updateText(int pos, int removed, int added);

private:
    class CustomTextEdit* m_pTextArea;
    class QLabel* m_pTextCursorPosInfo;
    TextEditorCore::Document m_document;
    bool m_contentsChanged = false;

private:
    void menuInit();
    void statusBarInit();
    void toolBarInit();

// IDocumentListener override
private:
    virtual void onCreated(const size_t index) override;
    virtual void onChanged(std::string& data) override;
    virtual void onModifyError() override;

    virtual void onStartLoading(const size_t index) override;
    virtual void onIOError(const size_t index, TextEditorCore::FileIOErrorsEnum) override;
    virtual void onProgress(const size_t index, const float percent) override;
    virtual void onPause(const size_t index) override;
    virtual void onResume(const size_t index) override;
    virtual void onStop(const size_t index) override;
    virtual void onLoadComplete(const size_t index, std::string& dataBuffer) override;
    virtual void onSaveComplete(const size_t index) override;
};
