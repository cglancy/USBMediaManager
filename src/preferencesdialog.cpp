#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include "utility.h"

#include <QFileDialog>
#include <QFile>
#include <QSettings>
#include <QDir>
#include <QStandardPaths>
#include <QUrl>

PreferencesDialog::PreferencesDialog(QWidget *parent)
	: QDialog(parent), ui(new Ui::PreferencesDialog)
{
	ui->setupUi(this);

	connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(browse()));
	connect(ui->resetButton, SIGNAL(clicked()), this, SLOT(reset()));
	connect(ui->pathLineEdit, SIGNAL(editingFinished()), this, SLOT(updateButtonState()));
    connect(ui->sourceLineEdit, SIGNAL(editingFinished()), this, SLOT(updateButtonState()));

	QSettings settings;
	if (settings.contains(Utility::VideoDirectoryKey))
		ui->pathLineEdit->setText(QDir::toNativeSeparators(settings.value(Utility::VideoDirectoryKey).toString()));
	else
		ui->pathLineEdit->setText(QStandardPaths::writableLocation(QStandardPaths::MoviesLocation));

	ui->sourceLineEdit->setText(Utility::currentLibrarySourceUrl());
	ui->sourceLineEdit->selectAll();

    if (Utility::folderThumbnailName() == Utility::PathPlusExtensionThumbnailName)
    {
        ui->folderNameButton->setChecked(true);
        ui->fixedNameButton->setChecked(false);
    }
    else
    {
        ui->folderNameButton->setChecked(false);
        ui->fixedNameButton->setChecked(true);
    }

    updateButtonState();
}

PreferencesDialog::~PreferencesDialog()
{

}

void PreferencesDialog::setVideoDirectory(const QString &path)
{
	ui->pathLineEdit->setText(QDir::toNativeSeparators(path));
    updateButtonState();
}

QString PreferencesDialog::videoDirectory() const
{
	return QDir::fromNativeSeparators(ui->pathLineEdit->text());
}

void PreferencesDialog::setLibrarySourceUrl(const QString &url)
{
    ui->sourceLineEdit->setText(url);
    updateButtonState();
}

QString PreferencesDialog::librarySourceUrl() const
{
    return ui->sourceLineEdit->text();
}

Utility::FolderThumbnailName PreferencesDialog::folderThumbnailName() const
{
    Utility::FolderThumbnailName name = Utility::StandardFolderThumbnailName;

    if (ui->folderNameButton->isChecked())
        name = Utility::PathPlusExtensionThumbnailName;

    return name;
}

void PreferencesDialog::browse()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Video Folder"),
		"", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	if (!dir.isEmpty())
	{
		ui->pathLineEdit->setText(QDir::toNativeSeparators(dir));
		updateButtonState();
	}
}

void PreferencesDialog::reset()
{
	ui->sourceLineEdit->setText(Utility::fallbackLibrarySourceUrl());
    updateButtonState();
}

void PreferencesDialog::updateButtonState()
{
    bool validVideoDir = false;
    bool validSourceUrl = false;

	if (Utility::isValidVideoDirectory(ui->pathLineEdit->text()))
    {
        validVideoDir = true;
        ui->pathLineEdit->setStyleSheet("color: black;");
    }
    else
    {
        validVideoDir = false;
        ui->pathLineEdit->setStyleSheet("color: red;");
    }

    QUrl url(ui->sourceLineEdit->text());
    if (url.scheme() != "http")
    {
        QString localFile = Utility::localPath(ui->sourceLineEdit->text());
        QFileInfo fi(localFile);
        if (fi.exists() && fi.suffix() == "xml")
        {
            validSourceUrl = true;
            ui->sourceLineEdit->setStyleSheet("color: black;");
        }
        else
        {
            validSourceUrl = false;
            ui->sourceLineEdit->setStyleSheet("color: red;");
        }
    }
    else
    {
        validSourceUrl = true;
        ui->sourceLineEdit->setStyleSheet("color: black;");
    }

    if (validVideoDir && validSourceUrl)
    {
        ui->okButton->setEnabled(true);
    }
    else
    {
        ui->okButton->setEnabled(false);
    }
}
