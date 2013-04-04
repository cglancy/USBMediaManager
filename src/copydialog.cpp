#include "copydialog.h"
#include "ui_copydialog.h"
#include "utility.h"

#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QSettings>

CopyDialog::CopyDialog(QWidget *parent, qint64 totalSize)
	: QDialog(parent), ui(new Ui::CopyDialog), _totalSize(totalSize)
{
	ui->setupUi(this);

	connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(browse()));
	connect(ui->pathLineEdit, SIGNAL(editingFinished()), this, SLOT(updateButtonState()));

    ui->okButton->setEnabled(false);
	ui->totalLineEdit->setText(Utility::fileSizeString(_totalSize, false));

    QSettings settings;
    if (settings.contains(Utility::LastExportDirectoryKey))
        ui->pathLineEdit->setText(QDir::toNativeSeparators(settings.value(Utility::LastExportDirectoryKey).toString()));
    else
        ui->pathLineEdit->setText(QDir::toNativeSeparators(Utility::removableDrive()));

    updateButtonState();
}

CopyDialog::~CopyDialog()
{

}

QString CopyDialog::destinationDirectory() const
{
    return QDir::fromNativeSeparators(ui->pathLineEdit->text());
}

void CopyDialog::setDestinationDirectory(const QString &dir)
{
	ui->pathLineEdit->setText(QDir::toNativeSeparators(dir));
	updateButtonState();
}

void CopyDialog::browse()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Destination Folder"),
		ui->pathLineEdit->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	if (!dir.isEmpty())
	{
		ui->pathLineEdit->setText(dir);
		updateButtonState();
	}
}

void CopyDialog::updateButtonState()
{
	if (QFile::exists(ui->pathLineEdit->text()))
    {
        qint64 availableBytes = Utility::availableSpace(ui->pathLineEdit->text());
        ui->availableLineEdit->setText(Utility::fileSizeString(availableBytes, false));

        if (_totalSize <= availableBytes)
        {
		    ui->okButton->setEnabled(true);
            ui->availableLineEdit->setStyleSheet("color: black;");
        }
        else
        {
            ui->okButton->setEnabled(false);
            ui->availableLineEdit->setStyleSheet("color: red;");
        }
    }
    else
    {
        ui->okButton->setEnabled(false);
    }
}

void CopyDialog::accept()
{
    QDialog::accept();

    QSettings settings;
    settings.setValue(Utility::LastExportDirectoryKey, QDir::fromNativeSeparators(ui->pathLineEdit->text()));
}
