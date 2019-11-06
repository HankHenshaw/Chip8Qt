#ifndef KEYSMAPPINGDIALOG_H
#define KEYSMAPPINGDIALOG_H

#include <QDebug>
#include "keymappingdialog.h"

namespace Ui {
class KeysMappingDialog;
}

class KeysMappingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit KeysMappingDialog(QWidget *parent = nullptr);
    ~KeysMappingDialog();

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void slotKeysMapping();
public slots:
    void slotGetKey(Qt::Key key);
signals:
    void signalSetKeyNumber(int keyNumber);
    void signalSendNewKeyValue(int keyNumber, Qt::Key key);
private:
    Ui::KeysMappingDialog *ui;
    int keyNumber;
};

#endif // KEYSMAPPINGDIALOG_H
