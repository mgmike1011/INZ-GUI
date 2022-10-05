#include "licensesdialog.h"
#include "ui_licensesdialog.h"
//
// Licenses dialog constructor
//
licensesDialog::licensesDialog(QWidget *parent) : QDialog(parent), ui(new Ui::licensesDialog)
{
    ui->setupUi(this);
    ui->textBrowser->setHtml("<div>Icons made by <a href='https://www.freepik.com' title='Freepik'>Freepik</a> from <a href='https://www.flaticon.com/' title='Flaticon'>www.flaticon.com</a></div><div>Icons made by <a href='https://www.flaticon.com/authors/smashicons' title='Smashicons'>Smashicons</a> from <a href='https://www.flaticon.com/' title='Flaticon'>www.flaticon.com</a></div><div>Icons made by <a href='https://www.flaticon.com/authors/prettycons' title='prettycons'>prettycons</a> from <a href='https://www.flaticon.com/' title='Flaticon'>www.flaticon.com</a></div><div>Icons made by <a href='https://www.flaticon.com/authors/monkik' title='monkik'>monkik</a> from <a href='https://www.flaticon.com/' title='Flaticon'>www.flaticon.com</a></div><div>Icons made by <a href='https://www.flaticon.com/authors/pixel-perfect' title='Pixel perfect'>Pixel perfect</a> from <a href='https://www.flaticon.com/' title='Flaticon'>www.flaticon.com</a></div><div>Icons made by <a href='https://www.flaticon.com/authors/nhor-phai' title='Nhor Phai'>Nhor Phai</a> from <a href='https://www.flaticon.com/' title='Flaticon'>www.flaticon.com</a></div><div><a href='https://www.flaticon.com/free-icons/start' title='start icons'>Start icons created by Freepik - Flaticon</a></div>");
    ui->textBrowser->setReadOnly(true);
    ui->textBrowser->setOpenExternalLinks(true);
}
//
// Licenses dialog destructor
//
licensesDialog::~licensesDialog()
{
    delete ui;
}
//
// Licenses dialog close
//
void licensesDialog::on_buttonBox_accepted()
{
    accept();
}

