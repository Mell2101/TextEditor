#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "customtextedit.h"
#include <QMenu>
#include <QFontDialog>
#include <QStyle>
#include <QAction>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //  Working title
    setWindowTitle(tr("Mister Note Pad 2023 XLL ULTRA"));

    m_pTextArea = new CustomTextEdit(this);
    setCentralWidget(m_pTextArea);

    QMenu* fileMenu = menuBar()->addMenu(tr("File"));

    // Temporarily adding actions for testing UI design
    fileMenu->addAction(tr("New"));
    fileMenu->addAction(tr("Open"));
    fileMenu->addSeparator();
    fileMenu->addAction(tr("Save"));
    fileMenu->addSeparator();
    fileMenu->addAction(tr("Exit"));

    //  This will (?) be in the final version instead
    //connect(fileMenu->addAction(tr("New")), &QAction::triggered, this, &MainWindow::NewFile);
    //connect(fileMenu->addAction(tr("Open")), &QAction::triggered, this, &MainWindow::OpenFile);
    //connect(fileMenu->addAction(tr("Save")), &QAction::triggered, this, &MainWindow::SaveFile);
    //connect(fileMenu->addAction(tr("Exit")), &QAction::triggered, this, &MainWindow::ExitProgramm);

    QMenu* editMenu = menuBar()->addMenu(tr("Edit"));
    editMenu->addAction(tr("Undo"), m_pTextArea, &CustomTextEdit::undo, QKeySequence(tr("Ctrl+Z")));
    editMenu->addAction(tr("Redo"), m_pTextArea, &CustomTextEdit::redo, QKeySequence(tr("Ctrl+Shift+Z")));
    editMenu->addAction(tr("Cut"), m_pTextArea, &CustomTextEdit::cut, QKeySequence(tr("Ctrl+X")));
    editMenu->addAction(tr("Copy"), m_pTextArea, &CustomTextEdit::copy, QKeySequence(tr("Ctrl+C")));
    editMenu->addAction(tr("Paste"), m_pTextArea, &CustomTextEdit::paste, QKeySequence(tr("Ctrl+V")));
    editMenu->addAction(tr("Fonts..."), this, &MainWindow::OpenFontDialog, QKeySequence(tr("Ctrl+Shift+F")));

    statusBar()->addWidget(m_pTextCursorPosInfo = new QLabel(this));
    connect(m_pTextArea, &CustomTextEdit::cursorPositionChanged, this, &MainWindow::updateTextCursorPosInfo);

    emit m_pTextArea->cursorPositionChanged();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Demo
void MainWindow::OpenFontDialog()
{
    QFontDialog fontDialog(this);
    bool accept;
    fontDialog.getFont(&accept);
}

void MainWindow::updateTextCursorPosInfo()
{
    int line = m_pTextArea->textCursor().blockNumber();
    int column = m_pTextArea->textCursor().positionInBlock();
    m_pTextCursorPosInfo->setText(QString("Line: %1, column: %2").arg(line).arg(column));
}
