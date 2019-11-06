#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    mainWidget = new GLWidget(this);
    this->resize(800, 600);
    this->setWindowTitle("Chip8Qt Emulator");
    setCentralWidget(mainWidget);

    createActions();
    createMenus();

    settings = new QSettings("cfg.ini", QSettings::IniFormat, this);
    loadSettings();

    mainWidget->setTextureColor(textureColor);
    mainWidget->setBackgroundColor(backgroundColor);

    keysMapDialog = new KeysMappingDialog(this);

    connect(keysMapDialog, &KeysMappingDialog::signalSetKeyNumber, this, &MainWindow::slotGetSignalFromKeyMap);
    connect(this, &MainWindow::signalGetKey, keysMapDialog, &KeysMappingDialog::slotGetKey);
    connect(keysMapDialog, &KeysMappingDialog::signalSendNewKeyValue, this, &MainWindow::slotSetNewKeyValue);
}
MainWindow::~MainWindow()
{
    saveSettings();
}

QVector<Qt::Key> &MainWindow::getVecKeys()
{
    return vecKeys;
}
void MainWindow::createActions()
{
    actionOpen = new QAction("Open...");
    connect(actionOpen, &QAction::triggered, this, &MainWindow::slotOpenFile);

    actionExit = new QAction("Exit");
    connect(actionExit, &QAction::triggered, &QApplication::quit);

    actionKeys = new QAction("Keys...");
    connect(actionKeys, &QAction::triggered, this, &MainWindow::slotKeysMappingDialog);

    actionColor = new QAction("Color...");
    connect(actionColor, &QAction::triggered, this, &MainWindow::slotColorDialog);
}
void MainWindow::createMenus()
{
    menuFile = menuBar()->addMenu("File");
    menuFile->addAction(actionOpen);
    menuFile->addSeparator();
    menuFile->addAction(actionExit);

    menuFile = menuBar()->addMenu("Tool");
    menuFile->addAction(actionKeys);
    menuFile->addAction(actionColor);
}
void MainWindow::loadSettings()
{
    qDebug() << "Load Settings";

    settings->beginGroup("Color Options");
    textureColor = settings->value("Textures Color").value<QColor>();
    backgroundColor = settings->value("Background Color").value<QColor>();
    settings->endGroup();

//    settings->beginGroup("Keys Mapping");
//    for(int i = 0; i < 10; ++i)
//        vecKeys.append(settings->value(QString("Key %1").arg(i)).value<Qt::Key>());

    settings->beginGroup("Keys Mapping");
    vecKeys.append(settings->value("Key 0").value<Qt::Key>());
    vecKeys.append(settings->value("Key 1").value<Qt::Key>());
    vecKeys.append(settings->value("Key 2").value<Qt::Key>());
    vecKeys.append(settings->value("Key 3").value<Qt::Key>());
    vecKeys.append(settings->value("Key 4").value<Qt::Key>());
    vecKeys.append(settings->value("Key 5").value<Qt::Key>());
    vecKeys.append(settings->value("Key 6").value<Qt::Key>());
    vecKeys.append(settings->value("Key 7").value<Qt::Key>());
    vecKeys.append(settings->value("Key 8").value<Qt::Key>());
    vecKeys.append(settings->value("Key 9").value<Qt::Key>());

    vecKeys.append(settings->value("Key A").value<Qt::Key>());
    vecKeys.append(settings->value("Key B").value<Qt::Key>());
    vecKeys.append(settings->value("Key C").value<Qt::Key>());
    vecKeys.append(settings->value("Key D").value<Qt::Key>());
    vecKeys.append(settings->value("Key E").value<Qt::Key>());
    vecKeys.append(settings->value("Key F").value<Qt::Key>());
    settings->endGroup();

    qDebug() << "Texture Color" << textureColor.red() << textureColor.green() << textureColor.blue();
    qDebug() << "Background Color" << backgroundColor.red() << backgroundColor.green() << backgroundColor.blue();

}
void MainWindow::saveSettings()
{
    qDebug() << "Save Settings";

    settings->beginGroup("Color Options");
    settings->setValue("Textures Color", textureColor);
    settings->setValue("Background Color", backgroundColor);
    settings->endGroup();

//    settings->beginGroup("Keys Mapping");
//    for(int i = 0; i < 10; ++i)
//        settings->setValue(QString("Key %1").arg(i), vecKeys.at(0));

    settings->beginGroup("Keys Mapping");
    settings->setValue("Key 0", vecKeys.at(0));
    settings->setValue("Key 1", vecKeys.at(1));
    settings->setValue("Key 2", vecKeys.at(2));
    settings->setValue("Key 3", vecKeys.at(3));
    settings->setValue("Key 4", vecKeys.at(4));
    settings->setValue("Key 5", vecKeys.at(5));
    settings->setValue("Key 6", vecKeys.at(6));
    settings->setValue("Key 7", vecKeys.at(7));
    settings->setValue("Key 8", vecKeys.at(8));
    settings->setValue("Key 9", vecKeys.at(9));

    settings->setValue("Key A", vecKeys.at(10));
    settings->setValue("Key B", vecKeys.at(11));
    settings->setValue("Key C", vecKeys.at(12));
    settings->setValue("Key D", vecKeys.at(13));
    settings->setValue("Key E", vecKeys.at(14));
    settings->setValue("Key F", vecKeys.at(15));
    settings->endGroup();
}
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    mainWidget->keyPressEvent(event);
}
void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    mainWidget->keyReleaseEvent(event);
}
void MainWindow::slotOpenFile()
{
    qDebug() << "Open File";
    QFileDialog dialog;
    QUrl url = dialog.getOpenFileUrl();
    mainWidget->loadGame(url);
}
void MainWindow::slotColorDialog()
{
    ColorOptionDialog dialog(textureColor, backgroundColor);
    if(dialog.exec() == QDialog::Accepted)
    {
        qDebug() << "Ok";
        textureColor = dialog.getTextureColor();
        backgroundColor = dialog.getBackgroundColor();

        mainWidget->setTextureColor(textureColor);
        mainWidget->setBackgroundColor(backgroundColor);

        qDebug() << "Texture Color" << textureColor.red() << textureColor.green() << textureColor.blue();
        qDebug() << "Background Color" << backgroundColor.red() << backgroundColor.green() << backgroundColor.blue();
    }
    qDebug() << "Open Color Dialog";
}

void MainWindow::slotKeysMappingDialog()
{
    if(keysMapDialog->exec() == QDialog::Accepted)
    {
        qDebug() << "Ok";
    }

    qDebug() << "Open Keys Mapping Dialog";
}

void MainWindow::slotGetSignalFromKeyMap(int keyNumber)
{
    if(keyNumber >= 0 && keyNumber < vecKeys.size())
    {
        qDebug() << "Key Number:" << keyNumber;
        emit signalGetKey(vecKeys.at(keyNumber));
    } else {
        qDebug () << "Key Number is invalid" << keyNumber;
    }
}

void MainWindow::slotSetNewKeyValue(int keyNumber, Qt::Key key)
{
    if(keyNumber >= 0 && keyNumber < vecKeys.size())
    {
        vecKeys[keyNumber] = key;
        mainWidget->getKey()[keyNumber] = key;
        for(int i = 0; i < vecKeys.size(); ++i)
        {
            qDebug() << vecKeys.at(i) << mainWidget->getKey().at(i);
        }
    } else {
        qDebug () << "Key Number is invalid" << keyNumber;
    }
}
