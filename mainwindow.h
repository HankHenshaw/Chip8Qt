#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QMainWindow>
#include <QKeyEvent>
#include <QMenuBar>
#include <QFileDialog>
#include <QColorDialog>
#include <QDebug>
#include <QSettings>
#include "coloroptiondialog.h"
#include "keysmappingdialog.h"
#include "glwidget.h"

//TODO: Добавить иконку?
class KeysMappingDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QVector<Qt::Key> &getVecKeys();
private:
    //OpenGLWidget
    GLWidget *mainWidget;

    /*Menus & Actions*/
    QMenu *menuFile;
    QMenu *menuTool;
    QAction *actionOpen;
    QAction *actionExit;
    QAction *actionKeys;
    QAction *actionColor;

    void createActions();
    void createMenus();
    /*Keys*/
    QVector<Qt::Key> vecKeys;

    KeysMappingDialog *keysMapDialog;
    /*Colors*/
    QColor backgroundColor;
    QColor textureColor;

    /*Settings*/
    QSettings *settings;

    void loadSettings();
    void saveSettings();

    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
private slots:
    void slotOpenFile();
    void slotColorDialog();
    void slotKeysMappingDialog();

public slots:
    void slotGetSignalFromKeyMap(int keyNumber);
    void slotSetNewKeyValue(int keyNumber, Qt::Key key);
signals:
    void signalGetKey(Qt::Key key);
};
#endif // MAINWINDOW_H
