#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>
#include <QColorDialog>
#include <QColor>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    //OpenGLWidget
    //KeyMap Widget
    QMenu *menuFile;
    QMenu *menuTool;
    QAction *actionOpen;
    QAction *actionExit;
    QAction *actionKeys;
    QAction *actionColor;

    void createActions();
    void createMenus();
    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
public slots:
    void slotOpenFile();
    void slotColorDialog();
};
#endif // MAINWINDOW_H
