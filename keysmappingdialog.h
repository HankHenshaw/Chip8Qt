#ifndef KEYSMAPPINGDIALOG_H
#define KEYSMAPPINGDIALOG_H

#include <QDialog>

namespace Ui {
class KeysMappingDialog;
}

class KeysMappingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit KeysMappingDialog(QWidget *parent = nullptr);
    ~KeysMappingDialog();

private:
    Ui::KeysMappingDialog *ui;
};

#endif // KEYSMAPPINGDIALOG_H
