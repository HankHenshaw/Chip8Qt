#ifndef KEYMAPPINGDIALOG_H
#define KEYMAPPINGDIALOG_H

#include <QDialog>

namespace Ui {
class KeyMappingDialog;
}

class KeyMappingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit KeyMappingDialog(QWidget *parent = nullptr);
    ~KeyMappingDialog();

private:
    Ui::KeyMappingDialog *ui;
};

#endif // KEYMAPPINGDIALOG_H
