#include "mainwindow.h"
#include "customtextedit.h"
#include "qapplication.h"
#include "qevent.h"
#include "qstatusbar.h"
#include <QMenu>
#include <QFontDialog>
#include <QStyle>
#include <QAction>
#include <QLabel>
#include <QToolBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QPrinter>
#include <QPrintDialog>
#include <QMenuBar>
#include <qdebug.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_document(0)
{
    // Working title
    setWindowTitle(tr("Mister Note Pad 2023 XLL ULTRA"));
    resize(800, 600);
    
    m_pTextArea = new CustomTextEdit(this);
    setCentralWidget(m_pTextArea);
    
    menuInit();
    statusBarInit();
    toolBarInit();
    
    m_document.setListener(*this);
    connect(this, &MainWindow::loadComplete, m_pTextArea, &CustomTextEdit::onLoaded);
    connect(this, &MainWindow::saveComplete, m_pTextArea, &CustomTextEdit::onSaved);
    connect(m_pTextArea->document(), &QTextDocument::contentsChange, this, &MainWindow::updateText);
}

void MainWindow::updateText(int pos, int removed, int added)
{
    if (!m_contentsChanged)
        m_contentsChanged = true;

    if (removed > 0)
    {
        m_document.modifyText(pos, -removed, "");
    }
    else
    {
        m_document.modifyText(pos, added, m_pTextArea->toPlainText().mid(pos, added).toStdString());
    }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (m_contentsChanged)
    {
        QMessageBox messageBox
                (
                    QMessageBox::Warning,
                    tr("Unsaved changes"),
                    tr("Would you like to save changes?"),
                    QMessageBox::Yes | QMessageBox::Cancel | QMessageBox::No,
                    this
                );

        int result = messageBox.exec();

        switch (result)
        {
        case QMessageBox::Yes:
            saveFile();
            event->accept();
            break;
        case QMessageBox::No:
            event->accept();
            break;
        default:
            event->ignore();
        }
    }
}

// Demo
//void MainWindow::openFontDialog()
//{
//    QFontDialog fontDialog(this);
//    bool accept;
//    fontDialog.getFont(&accept);
//}

void MainWindow::openPrintDocumentDialog()
{
    QPrinter printer;
    QPrintDialog printDialog(&printer, this);
    printDialog.setWindowTitle(tr("Print Document"));
    if (printDialog.exec() != QDialog::Accepted)
    {
        return;
    }
    m_pTextArea->print(&printer);
}

void MainWindow::updateTextCursorPosInfo()
{
    int line = m_pTextArea->textCursor().blockNumber();
    int column = m_pTextArea->textCursor().positionInBlock();
    m_pTextCursorPosInfo->setText(QString("Line: %1, column: %2").arg(line).arg(column));
}

void MainWindow::onCreated(const size_t index) {}

void MainWindow::onChanged(const size_t index, std::string& data) {}

void MainWindow::onModifyError(const size_t index) {}

void MainWindow::onStartLoading(const size_t index) {}

void MainWindow::onIOError(const size_t index, TextEditorCore::FileIOErrorsEnum error)
{
    switch (error)
    {
    case TextEditorCore::FilePathIsEmpty:
        QMessageBox
                (
                    QMessageBox::Critical,
                    tr("File error"),
                    tr("Cannot open a file: file path is empty"),
                    QMessageBox::NoButton, this
                ).exec();
        break;
    case TextEditorCore::FileWriteError:
        QMessageBox
                (
                    QMessageBox::Critical,
                    tr("File error"),
                    tr("Cannot write to a file"),
                    QMessageBox::NoButton, this
                ).exec();
        break;
    case TextEditorCore::FileReadError:
        QMessageBox
                (
                    QMessageBox::Critical,
                    tr("File error"),
                    tr("Cannot read a file"),
                    QMessageBox::NoButton, this
                ).exec();
        break;
    default:;
    }
}

void MainWindow::onProgress(const size_t index, const float percent) {}

void MainWindow::onPause(const size_t index) {}

void MainWindow::onResume(const size_t index) {}

void MainWindow::onStop(const size_t index) {}

void MainWindow::onLoadComplete(const size_t index, std::string& dataBuffer)
{
    qDebug() << "Success";
    emit loadComplete(dataBuffer.c_str());
    m_contentsChanged = false;
}

void MainWindow::onSaveComplete(const size_t index)
{
    emit saveComplete();
    m_contentsChanged = false;
}

void MainWindow::newFile()
{
    if (m_contentsChanged)
    {
        QMessageBox messageBox
                (
                    QMessageBox::Warning,
                    tr("Unsaved changes"),
                    tr("Would you like to save changes?"),
                    QMessageBox::Yes | QMessageBox::Cancel | QMessageBox::No,
                    this
                );
        
        int result = messageBox.exec();

        switch (result)
        {
        case QMessageBox::Yes:
            saveFile();
            m_pTextArea->clear();
            m_document.setText("");
            m_contentsChanged = false;
            break;
        case QMessageBox::No:
            m_pTextArea->clear();
            m_document.setText("");
            m_contentsChanged = false;
            break;
        default:;
        }
    }
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open document"), QDir::homePath(), tr("Text(*.txt);;All(*)"));
    m_document.setFileName(fileName.toStdString());
    m_document.load();
}

void MainWindow::saveFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save document"), QDir::homePath(), tr("Text(*.txt);;All(*)"));
    m_document.setFileName(fileName.toStdString());
    m_document.save(true);
}

void MainWindow::openAboutMessageBox()
{
    QMessageBox::about
            (
                this,
                tr("About \"Mister Note Pad 2023 XLL ULTRA\""),
                tr("\"Mister Note Pad 2023 XLL ULTRA\"\n\n"
                   "Development team:\n"
                   "Alexeychiv\n"
                   "Timofey Kagochkin\n"
                   "Алексей (Riki)\n"
                   "Prohor\n"
                   "Nikita Labur-Strel\n"
                   "Максим Строганов\n"
                   "Макдудоль Сержант\n\n"
                   "2023")
            );
}

void MainWindow::exitProgramm()
{
    close();
}

inline void MainWindow::menuInit()
{
    QMenu* fileMenu = menuBar()->addMenu(tr("File"));
    
    fileMenu->addAction(tr("New"), this, &MainWindow::newFile, QKeySequence(tr("Ctrl+N")));
    fileMenu->addAction(tr("Open"), this, &MainWindow::openFile, QKeySequence(tr("Ctrl+O")));
    fileMenu->addSeparator();
    fileMenu->addAction(tr("Save"), this, &MainWindow::saveFile, QKeySequence(tr("Ctrl+S")));
    fileMenu->addAction(tr("Print..."), this, &MainWindow::openPrintDocumentDialog, QKeySequence(tr("Ctrl+P")));
    fileMenu->addSeparator();
    fileMenu->addAction(tr("Exit"), this, &MainWindow::exitProgramm);
    
    QMenu* editMenu = menuBar()->addMenu(tr("Edit"));
    editMenu->addAction(tr("Undo"), m_pTextArea, &CustomTextEdit::undo, QKeySequence(tr("Ctrl+Z")));
    editMenu->addAction(tr("Redo"), m_pTextArea, &CustomTextEdit::redo, QKeySequence(tr("Ctrl+Shift+Z")));
    editMenu->addAction(tr("Cut"), m_pTextArea, &CustomTextEdit::cut, QKeySequence(tr("Ctrl+X")));
    editMenu->addAction(tr("Copy"), m_pTextArea, &CustomTextEdit::copy, QKeySequence(tr("Ctrl+C")));
    editMenu->addAction(tr("Paste"), m_pTextArea, &CustomTextEdit::paste, QKeySequence(tr("Ctrl+V")));
    
    QMenu* helpMenu = menuBar()->addMenu(tr("Help"));
    helpMenu->addAction(tr("About..."), this, &MainWindow::openAboutMessageBox);
    helpMenu->addAction(tr("About Qt..."), this, &QApplication::aboutQt);
}

inline void MainWindow::statusBarInit()
{
    statusBar()->addWidget(m_pTextCursorPosInfo = new QLabel(this));
    connect(m_pTextArea, &CustomTextEdit::cursorPositionChanged, this, &MainWindow::updateTextCursorPosInfo);
    emit m_pTextArea->cursorPositionChanged();
}

inline void MainWindow::toolBarInit()
{
    QToolBar* toolBarFile = addToolBar(tr("File"));
    toolBarFile->addAction
            (
                QIcon("://icons/document-new.svg"),
                tr("New"),
                this,
                &MainWindow::newFile
            );
    toolBarFile->addAction
            (
                QIcon("://icons/document-open.svg"),
                tr("Open"),
                this,
                &MainWindow::openFile
            );
    toolBarFile->addAction
            (
                QIcon("://icons/document-save.svg"),
                tr("Save"),
                this,
                &MainWindow::saveFile
            );
    toolBarFile->addAction
            (
                QIcon("://icons/document-print.svg"),
                tr("Print..."),
                this,
                &MainWindow::openPrintDocumentDialog
            );
    
    QToolBar* toolBarEdit = addToolBar(tr("Edit"));
    toolBarEdit->addAction
            (
                QIcon("://icons/edit-undo.svg"),
                tr("Undo"),
                m_pTextArea,
                &CustomTextEdit::undo
            );
    toolBarEdit->addAction
            (
                QIcon("://icons/edit-redo.svg"),
                tr("Redo"),
                m_pTextArea,
                &CustomTextEdit::redo
            );
    toolBarEdit->addAction
            (
                QIcon("://icons/edit-cut.svg"),
                tr("Cut"),
                m_pTextArea,
                &CustomTextEdit::cut
            );
    toolBarEdit->addAction
            (
                QIcon("://icons/edit-copy.svg"),
                tr("Copy"),
                m_pTextArea,
                &CustomTextEdit::copy
            );
    toolBarEdit->addAction
            (
                QIcon("://icons/edit-paste.svg"),
                tr("Paste"),
                m_pTextArea,
                &CustomTextEdit::paste
            );
//    toolBarEdit->addSeparator();
//    toolBarEdit->addAction
//            (
//                QIcon("://icons/applications-fonts.svg"),
//                tr("Fonts..."),
//                this,
//                &MainWindow::openFontDialog
//            );
}
