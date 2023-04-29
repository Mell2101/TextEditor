#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "customtextedit.h"
#include <QMenu>
#include <QFontDialog>
#include <QStyle>
#include <QAction>
#include <QLabel>
#include <QToolBar>
#include <QPrinter>
#include <QPrintDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //  Working title
    setWindowTitle(tr("Mister Note Pad 2023 XLL ULTRA"));

    m_pTextArea = new CustomTextEdit(this);
    setCentralWidget(m_pTextArea);

    menuInit();
    statusBarInit();
    toolBarInit();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Demo
void MainWindow::openFontDialog()
{
    QFontDialog fontDialog(this);
    bool accept;
    fontDialog.getFont(&accept);
}

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
    qDebug() << "\"New file\" triggered";
}

void MainWindow::openFile()
{
    qDebug() << "\"Open file\" triggered";
}

void MainWindow::saveFile()
{
    qDebug() << "\"Save file\" triggered";
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
    editMenu->addAction(tr("Fonts..."), this, &MainWindow::openFontDialog, QKeySequence(tr("Ctrl+Shift+F")));
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
    toolBarFile->addAction(QIcon("://icons/document-new.svg"), tr("New"));
    toolBarFile->addAction(QIcon("://icons/document-open.svg"), tr("Open"));
    toolBarFile->addAction(QIcon("://icons/document-save.svg"), tr("Save"));
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
    toolBarEdit->addSeparator();
    toolBarEdit->addAction
            (
                QIcon("://icons/applications-fonts.svg"),
                tr("Fonts..."),
                this,
                &MainWindow::openFontDialog
            );
}
