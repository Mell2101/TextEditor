#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "customtextedit.h"
#include "QMenu"
#include "QFontDialog"
#include "QStyle"
#include "QAction"
#include <QLabel>
#include <QMouseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//  Working title
    setWindowTitle(tr("Mister Note Pad 2023 XLL ULTRA"));

    TextArea = new CustomTextEdit(this);
    setCentralWidget(TextArea);

    QMenu* fileMenu = menuBar()->addMenu(tr("File"));

// Temporarily adding actions for testing UI design
    fileMenu->addAction(tr("New"));
    fileMenu->addAction(tr("Open"));
    fileMenu->addSeparator();
    fileMenu->addAction(tr("Save"));
    fileMenu->addSeparator();
    fileMenu->addAction(tr("Exit"));

//  This will (?) be in the final version instead
//    connect(fileMenu->addAction(tr("New")), &QAction::triggered, this, &MainWindow::NewFile);
//    connect(fileMenu->addAction(tr("Open")), &QAction::triggered, this, &MainWindow::OpenFile);
//    connect(fileMenu->addAction(tr("Save")), &QAction::triggered, this, &MainWindow::SaveFile);
//    connect(fileMenu->addAction(tr("Exit")), &QAction::triggered, this, &MainWindow::ExitProgramm);

    QMenu* editMenu = menuBar()->addMenu(tr("Edit"));

//  This will (?) be in the final version
    connect(editMenu->addAction(tr("Undo"), QKeySequence("Ctrl+Z")), &QAction::triggered, TextArea, &CustomTextEdit::undo);
    connect(editMenu->addAction(tr("Redo"), QKeySequence(tr("Ctrl+Shift+Z"))), &QAction::triggered, TextArea, &CustomTextEdit::redo);
    connect(editMenu->addAction(tr("Cut"), QKeySequence(tr("Ctrl+X"))), &QAction::triggered, TextArea, &CustomTextEdit::cut);
    connect(editMenu->addAction(tr("Copy"), QKeySequence(tr("Ctrl+C"))), &QAction::triggered, TextArea, &CustomTextEdit::copy);
    connect(editMenu->addAction(tr("Paste"), QKeySequence(tr("Ctrl+V"))), &QAction::triggered, TextArea, &CustomTextEdit::paste);
    connect(editMenu->addAction(tr("Fonts..."), QKeySequence(tr("Ctrl+Shift+F"))), &QAction::triggered, this, &MainWindow::OpenFontDialog);

    statusBar()->addWidget(MousePosInfo = new QLabel(this));
    connect(TextArea, &CustomTextEdit::mouseMoved, this, &MainWindow::updateMousePosInfo);
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

void MainWindow::updateMousePosInfo(int x, int y)
{
    MousePosInfo->setText(QString("%1; %2").arg(x).arg(y));
}
