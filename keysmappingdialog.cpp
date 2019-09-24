#include "keysmappingdialog.h"
#include "ui_keysmappingdialog.h"

KeysMappingDialog::KeysMappingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KeysMappingDialog)
{
    ui->setupUi(this);
}

KeysMappingDialog::~KeysMappingDialog()
{
    delete ui;
}
