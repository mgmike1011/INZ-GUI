#ifndef LICENSESDIALOG_H
#define LICENSESDIALOG_H

#include <QDialog>

namespace Ui {
class licensesDialog;
}

class licensesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit licensesDialog(QWidget *parent = nullptr);
    ~licensesDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::licensesDialog *ui;
};

#endif // LICENSESDIALOG_H
