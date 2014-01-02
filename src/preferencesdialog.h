#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>
#include "utility.h"

namespace Ui {
	class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
	Q_OBJECT

public:
	PreferencesDialog(QWidget *parent = 0);
	~PreferencesDialog();

	void setVideoDirectory(const QString &path);
	QString videoDirectory() const;

    void setLibrarySourceUrl(const QString &url);
    QString librarySourceUrl() const;

    Utility::FolderThumbnailName folderThumbnailName() const;

public slots:
	void browse();
	void reset();
	void updateButtonState();

private:
	Ui::PreferencesDialog *ui;
};

#endif // PREFERENCESDIALOG_H
