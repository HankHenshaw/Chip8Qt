#include "coloroptiondialog.h"
#include "ui_coloroptiondialog.h"

ColorOptionDialog::ColorOptionDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ColorOptionDialog)
{
    ui->setupUi(this);
}

ColorOptionDialog::~ColorOptionDialog()
{
    delete ui;
}
