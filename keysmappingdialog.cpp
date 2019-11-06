#include "keysmappingdialog.h"
#include "ui_keysmappingdialog.h"

KeysMappingDialog::KeysMappingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KeysMappingDialog)
{
    ui->setupUi(this);

    this->setWindowTitle("Keys Options");

    connect(ui->A_pushButton, &QPushButton::pressed, this, &KeysMappingDialog::slotKeysMapping);
    connect(ui->B_pushButton, &QPushButton::pressed, this, &KeysMappingDialog::slotKeysMapping);
    connect(ui->C_pushButton, &QPushButton::pressed, this, &KeysMappingDialog::slotKeysMapping);
    connect(ui->D_pushButton, &QPushButton::pressed, this, &KeysMappingDialog::slotKeysMapping);
    connect(ui->E_pushButton, &QPushButton::pressed, this, &KeysMappingDialog::slotKeysMapping);
    connect(ui->F_pushButton, &QPushButton::pressed, this, &KeysMappingDialog::slotKeysMapping);
    connect(ui->G_pushButton, &QPushButton::pressed, this, &KeysMappingDialog::slotKeysMapping);
    connect(ui->H_pushButton, &QPushButton::pressed, this, &KeysMappingDialog::slotKeysMapping);
    connect(ui->I_pushButton, &QPushButton::pressed, this, &KeysMappingDialog::slotKeysMapping);
    connect(ui->J_pushButton, &QPushButton::pressed, this, &KeysMappingDialog::slotKeysMapping);
    connect(ui->K_pushButton, &QPushButton::pressed, this, &KeysMappingDialog::slotKeysMapping);
    connect(ui->L_pushButton, &QPushButton::pressed, this, &KeysMappingDialog::slotKeysMapping);
    connect(ui->M_pushButton, &QPushButton::pressed, this, &KeysMappingDialog::slotKeysMapping);
    connect(ui->N_pushButton, &QPushButton::pressed, this, &KeysMappingDialog::slotKeysMapping);
    connect(ui->O_pushButton, &QPushButton::pressed, this, &KeysMappingDialog::slotKeysMapping);
    connect(ui->P_pushButton, &QPushButton::pressed, this, &KeysMappingDialog::slotKeysMapping);
}

KeysMappingDialog::~KeysMappingDialog()
{
    delete ui;
}

void KeysMappingDialog::on_buttonBox_accepted()
{
    accept();
}

void KeysMappingDialog::on_buttonBox_rejected()
{
    reject();
}

void KeysMappingDialog::slotKeysMapping()
{
    QChar keyChar =  sender()->objectName().at(0);
    keyNumber = keyChar.unicode() - 65;
    qDebug() << "Button:" << keyChar;
    emit signalSetKeyNumber(keyNumber);
}

void KeysMappingDialog::slotGetKey(Qt::Key key)
{
    KeyMappingDialog dialog(key, this);
    if(dialog.exec() == QDialog::Accepted)
    {
        qDebug() << "Ok";
        emit signalSendNewKeyValue(keyNumber, dialog.getNewKeyValue());
    }
}
