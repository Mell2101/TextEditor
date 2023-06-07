#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "customtextedit.h"
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
#include <QDebug>
#include <TextEditorCore/Document.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    //  Working title
    setWindowTitle(tr("Mister Note Pad 2023 XLL ULTRA"));

    m_pTextArea = new CustomTextEdit(this);
    setCentralWidget(m_pTextArea);

    menuInit();
    statusBarInit();
    toolBarInit();

    m_pDocument = new TextEditorCore::Document(m_currentDocIndex);
    connect(m_pTextArea->document(), &QTextDocument::contentsChange, this, &MainWindow::updateTextData);
}

void MainWindow::updateTextData(int pos, int removed, int added)
{
    if (removed > 0)
        m_pDocument->modifyText(pos, -removed, "");
    else
        m_pDocument->modifyText(pos, added, m_pTextArea->toPlainText().mid(pos, added).toStdString());
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
        return;
    m_pTextArea->print(&printer);
}

void MainWindow::updateTextCursorPosInfo()
{
    int line = m_pTextArea->textCursor().blockNumber();
    int column = m_pTextArea->textCursor().positionInBlock();
    m_pTextCursorPosInfo->setText(QString("Line: %1, column: %2").arg(line).arg(column));
}

void MainWindow::newFile()
{
    m_pTextArea->clear();
    delete m_pDocument;
    m_pDocument = new TextEditorCore::Document(++m_currentDocIndex);
    m_pDocument->setFileName("untitled");
    m_pDocument->setTextData("");
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open document"), QDir::homePath(), tr("Text(*.txt);;All(*)"));
    if (!fileName.isEmpty())
    {
        m_pDocument->setFileName(fileName.toStdString());
        m_pDocument->load();
        m_pTextArea->setText(QString::fromStdString(m_pDocument->getTextData()));

        // TODO: checking for file opening via file name path, stored in fileName
//        if (true)
//        {
//            QMessageBox
//                    (
//                        QMessageBox::Critical,
//                        tr("File error"),
//                        tr("Cannot open a file"),
//                        QMessageBox::NoButton, this
//                    ).exec();
//        }
//        else
//            qDebug() << "File opened";
    }
}

void MainWindow::saveFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save document"), QDir::homePath(), tr("Text(*.txt);;All(*)"));
    if (!fileName.isEmpty())
    {
        m_pDocument->setFileName(fileName.toStdString());
        m_pDocument->save(true);

        // TODO: checking for file opening via file name path, stored in fileName
//        if (true)
//        {
//            QMessageBox
//                    (
//                        QMessageBox::Critical,
//                        tr("File error"),
//                        tr("Cannot save a file"),
//                        QMessageBox::NoButton, this
//                    ).exec();
//        }
//        else
//            qDebug() << "File saved";
    }
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
    exit(0);
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
//    editMenu->addAction(tr("Fonts..."), this, &MainWindow::openFontDialog, QKeySequence(tr("Ctrl+Shift+F")));

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
