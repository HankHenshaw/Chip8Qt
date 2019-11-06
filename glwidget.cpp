#include "glwidget.h"

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    textureColor = QColor(255, 255, 255);
    backgroundColor = QColor(0, 0, 0);
    emuSpeedDenominator = 300;
    soundDecrementCounter = 0;
    soundDecrementValue = emuSpeedDenominator/60;
    isSound = false;
    isPaused = false;
    keysInit();
    textureInit();
    connect(&cycleTimer, &QTimer::timeout, this, &GLWidget::slotTimeout);
}
void GLWidget::loadGame(const QUrl &url)
{
    bool isGameLoaded = false;
    QFile game(url.path().remove(0, 1));
    if(game.open(QIODevice::ReadOnly))
    {
        qint64 fileSize = game.size();
        qDebug() << "File Size =" << fileSize;
        if(fileSize < 4096 - 512)
        {
            chip.chipInit();

            QByteArray ba = game.readAll();
            uint offset = 512;
            for(int i = 0; i < fileSize; ++i, ++offset)
                chip.getMemory()[offset] = static_cast<uchar>(ba.data()[i]);

            qDebug() << "Game" << url.fileName() << "loaded";
            isGameLoaded = true;
            ba.clear();
        } else {
            qDebug() << "File is too big.";
        }
    } else {
        qDebug() << "Can't open" << url.fileName();
    }
    if(isGameLoaded)
    {
        isGameLoaded = false;
        cycleTimer.start(1000/emuSpeedDenominator);
    }
}
void GLWidget::keyPressEvent(QKeyEvent *event)
{//TODO: Добавить возможность редактировать не игровые клавиши, а также не возможность забиндить игровые клавиши на не игровые
    qDebug() << event->key();
    for(int i = 0; i < keysMapping.size(); ++i)
    {
        if(event->key() == keysMapping.at(i))
        {
            chip.getKeyboard()[i] = 1;
        }
    }

    if(event->key() == Qt::Key_Pause)
    {
        if(isPaused)
        {
            cycleTimer.start(1000/emuSpeedDenominator);
            isPaused = false;
        } else {
            cycleTimer.stop();
            isPaused = true;
        }
    } else if(event->key() == Qt::Key_Plus) {
        if(emuSpeedDenominator < 600)
        {
            emuSpeedDenominator += 60;
            soundDecrementValue = emuSpeedDenominator/60;
            if(!isPaused)
            {
                cycleTimer.start(1000/emuSpeedDenominator);
                soundDecrementCounter = 0;
            }
        }
    } else if(event->key() == Qt::Key_Minus) {
        if(emuSpeedDenominator > 60)
        {
            emuSpeedDenominator -= 60;
            soundDecrementValue = emuSpeedDenominator/60;
            if(!isPaused)
            {
                cycleTimer.start(1000/emuSpeedDenominator);
                soundDecrementCounter = 0;
            }
        }
    } else if(event->key() == Qt::Key_Equal) {
        emuSpeedDenominator = 300;
        soundDecrementValue = 5;
        if(!isPaused)
        {
            cycleTimer.start(1000/emuSpeedDenominator);
            soundDecrementCounter = 0;
        }
    }
    chip.opcodeDecode();
}
void GLWidget::keyReleaseEvent(QKeyEvent *event) // WARNING: Могут быть проблемы
{
    for(int i = 0; i < keysMapping.size(); ++i)
        if(event->key() == keysMapping.at(i))
            chip.getKeyboard()[i] = 0;
}

void GLWidget::setTextureColor(const QColor &color)
{
    textureColor = color;
}

void GLWidget::setBackgroundColor(const QColor &color)
{
    backgroundColor = color;
}

QVector<Qt::Key> &GLWidget::getKey()
{
    return keysMapping;
}
void GLWidget::initializeGL()
{
    QOpenGLFunctions *func = QOpenGLContext().currentContext()->functions();
    func->glClearColor(0.0, 0.0, 0.0, 1.0);
    func->glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    func->glGenTextures(1, &texName);
    func->glBindTexture(GL_TEXTURE_2D, texName);

    func->glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    func->glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    func->glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    func->glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

    func->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widgetWidth, widgetHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);
}
void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
void GLWidget::paintGL()
{
    int w = 64;
    int h = 32;
    if(chip.isExtendedMod())
    {
        w = 0;
        h = 0;
    }
    for(int i = 0; i < widgetHeight; ++i)
    {
        for(int j = 0; j < widgetWidth - w; ++j)
        {
            if(chip.getScreen()[j*64 + i] == 0)
            {
                texture[j][i][0] = static_cast<GLubyte>(backgroundColor.red());
                texture[j][i][1] = static_cast<GLubyte>(backgroundColor.green());
                texture[j][i][2] = static_cast<GLubyte>(backgroundColor.blue());
            } else {
                texture[j][i][0] = static_cast<GLubyte>(textureColor.red());
                texture[j][i][1] = static_cast<GLubyte>(textureColor.green());
                texture[j][i][2] = static_cast<GLubyte>(textureColor.blue());
            }
        }
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widgetWidth - w, widgetHeight - h, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, texName);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, 1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 0.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}
void GLWidget::textureInit()
{
    for(int i = 0; i < widgetWidth; ++i)
        for(int j = 0; j < widgetHeight; ++j)
            texture[i][j][0] = texture[i][j][1] = texture[i][j][2] = 0;
}

void GLWidget::keysInit()
{
    for(int i = 0; i < 10; ++i)
        keysMapping.append(static_cast<Qt::Key>(0x30 + i));

    for(int i = 0; i < 6; ++i)
        keysMapping.append(static_cast<Qt::Key>(0x41 + i));
}
void GLWidget::slotTimeout()
{
    soundDecrementCounter++;
    if(soundDecrementCounter == soundDecrementValue)
    {
        soundDecrementCounter = 0;
        if(chip.getDelayTimer())
            chip.decrementTimer(1);
        if(chip.getSoundTimer())
        {
            chip.decrementTimer(0);
            isSound = true;
        }
        if(isSound)
        {
            //QSound::play("sound.wav"); // TODO: Добавить звуковой файл
            isSound = false;
        }
    }
    chip.opcodeDecode();

    ushort opcode = chip.getOpcode();
    if((opcode & 0xF000) == 0xD000)
        update();
    else if((opcode & 0x00F0) == 0x00C0)
        update();
    else if((opcode & 0x00FF) == 0x00FB)
        update();
    else if((opcode & 0x00FF) == 0x00FC)
        update();
    else if((opcode & 0x00FF) == 0x00FD)
        cycleTimer.stop();
}
