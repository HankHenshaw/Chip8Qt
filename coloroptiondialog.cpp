#include "coloroptiondialog.h"
#include "ui_coloroptiondialog.h"

ColorOptionDialog::ColorOptionDialog(QColor texColor, QColor backColor, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ColorOptionDialog), textureColor(texColor), backgroundColor(backColor)
{
    ui->setupUi(this);
    this->setWindowTitle("Color Options");

    cdialog = new QColorDialog(this);
}

ColorOptionDialog::~ColorOptionDialog()
{
    delete ui;
}

const QColor &ColorOptionDialog::getTextureColor() const
{
    return textureColor;
}

const QColor &ColorOptionDialog::getBackgroundColor() const
{
    return backgroundColor;
}

void ColorOptionDialog::on_pushButton_textureColor_clicked()
{
    if(cdialog->exec() == QDialog::Accepted)
    {
        textureColor = cdialog->currentColor();
        qDebug() << "Color Option Dialog. texture color" << textureColor.red() << textureColor.green() << textureColor.blue();
    }
}

void ColorOptionDialog::on_pushButton_backgroundColor_clicked()
{
    if(cdialog->exec() == QDialog::Accepted)
    {
        backgroundColor = cdialog->currentColor();
        qDebug() << "Color Option Dialog. background color" << backgroundColor.red() << backgroundColor.green() << backgroundColor.blue();
    }
}


void ColorOptionDialog::on_buttonBox_accepted()
{
    accept();
}

void ColorOptionDialog::on_buttonBox_rejected()
{
    reject();
}
