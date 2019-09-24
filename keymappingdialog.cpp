#include "keymappingdialog.h"
#include "ui_keymappingdialog.h"

KeyMappingDialog::KeyMappingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KeyMappingDialog)
{
    ui->setupUi(this);
}

KeyMappingDialog::~KeyMappingDialog()
{
    delete ui;
}
