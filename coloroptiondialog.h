#ifndef COLOROPTIONDIALOG_H
#define COLOROPTIONDIALOG_H

#include <QWidget>

namespace Ui {
class ColorOptionDialog;
}

class ColorOptionDialog : public QWidget
{
    Q_OBJECT

public:
    explicit ColorOptionDialog(QWidget *parent = nullptr);
    ~ColorOptionDialog();

private:
    Ui::ColorOptionDialog *ui;
};

#endif // COLOROPTIONDIALOG_H
