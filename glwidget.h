#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QKeyEvent>
#include <QTimer>
#include <QSound>
#include <QDebug>
#include "chip8.h"

class GLWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget *parent = nullptr);
    void loadGame(const QUrl& url);

    // QWidget interface
    void keyPressEvent(QKeyEvent *event); // TODO: Сделать protected?
    void keyReleaseEvent(QKeyEvent *event);//TODO: Сделать protected?

    void setTextureColor(const QColor &color);
    void setBackgroundColor(const QColor &color);

    QVector<Qt::Key> &getKey();
protected:
    // QOpenGLWidget interface
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
private:
    static const int widgetWidth = 128;
    static const int widgetHeight = 64;
    GLuint texName;
    GLubyte texture[widgetWidth][widgetHeight][3];

    QColor textureColor;
    QColor backgroundColor;
    void textureInit();

    Chip8 chip;

    QVector<Qt::Key> keysMapping;
    void keysInit();

    uint emuSpeedDenominator;
    uint soundDecrementCounter;
    uint soundDecrementValue;
    bool isSound;

    QTimer cycleTimer;

    bool isPaused;
private slots:
    void slotTimeout();
};

#endif // GLWIDGET_H
