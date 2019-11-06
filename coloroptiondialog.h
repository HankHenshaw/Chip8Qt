#ifndef COLOROPTIONDIALOG_H
#define COLOROPTIONDIALOG_H

#include <QColorDialog>
#include <QDebug>

class QColorDialog;

namespace Ui {
class ColorOptionDialog;
}

class ColorOptionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ColorOptionDialog(QColor texColor, QColor backColor, QWidget *parent = nullptr);
    ~ColorOptionDialog();

    const QColor &getTextureColor() const;
    const QColor &getBackgroundColor() const;
private slots:
    void on_pushButton_textureColor_clicked();
    void on_pushButton_backgroundColor_clicked();
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::ColorOptionDialog *ui;
    QColor textureColor;
    QColor backgroundColor;
    QColorDialog *cdialog;
};

#endif // COLOROPTIONDIALOG_H
