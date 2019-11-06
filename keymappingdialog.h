#ifndef KEYMAPPINGDIALOG_H
#define KEYMAPPINGDIALOG_H

#include <QDialog>
#include <QKeyEvent>
#include <QDebug>

namespace Ui {
class KeyMappingDialog;
}

class KeyMappingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit KeyMappingDialog(Qt::Key key, QWidget *parent = nullptr);
    ~KeyMappingDialog();

    Qt::Key getNewKeyValue();
private:
    Ui::KeyMappingDialog *ui;
    Qt::Key keyValue;

protected:
    // QWidget interface
    void keyPressEvent(QKeyEvent *event);
private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
};

#endif // KEYMAPPINGDIALOG_H
