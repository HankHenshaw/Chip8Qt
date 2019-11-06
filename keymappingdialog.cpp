#include "keymappingdialog.h"
#include "ui_keymappingdialog.h"

KeyMappingDialog::KeyMappingDialog(Qt::Key key, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KeyMappingDialog), keyValue(key)
{
    ui->setupUi(this);
    ui->label_2->setText(static_cast<QChar>(keyValue));
    this->setWindowTitle("Key Mapping Dialog");
}

KeyMappingDialog::~KeyMappingDialog()
{
    delete ui;
}

Qt::Key KeyMappingDialog::getNewKeyValue()
{
    return keyValue;
}

void KeyMappingDialog::keyPressEvent(QKeyEvent *event)
{
    keyValue = static_cast<Qt::Key>(event->key());
    ui->label_2->setText(static_cast<QChar>(keyValue));
}

void KeyMappingDialog::on_buttonBox_accepted()
{
    accept();
}

void KeyMappingDialog::on_buttonBox_rejected()
{
    reject();
}
