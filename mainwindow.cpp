#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}
MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::createActions()
{
    actionOpen = new QAction("Open...");
    connect(actionOpen, &QAction::triggered, this, &MainWindow::slotOpenFile);

    actionExit = new QAction("Exit");
    connect(actionExit, &QAction::trigger, this, qApp->exec());

    actionColor = new QAction("Keys...");
    connect(actionColor, &QAction::triggered, this, &MainWindow::slotColorDialog);
}
void MainWindow::createMenus()
{

}
