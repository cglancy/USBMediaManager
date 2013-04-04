#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "utility.h"

AboutDialog::AboutDialog(QWidget *parent)
	: QDialog(parent), ui(new Ui::AboutDialog)
{
	ui->setupUi(this);
	ui->appIcon->setPixmap(QPixmap(":/pvm/images/video_folder_128.png"));

    ui->nameLabel->setText(Utility::applicationName());

    QString htmlString = 
        "<html><head/><body>"
        "<p>Version %1</p>"
        "<p><a href=\"mailto:charles@glancyfamily.net\"><span style=\" text-decoration: underline; color:#0000ff;\">charles@glancyfamily.net</span></a></p>"
        "</body></html>";
    ui->buildLabel->setText(htmlString.arg(Utility::applicationVersionString()));
}

AboutDialog::~AboutDialog()
{

}
