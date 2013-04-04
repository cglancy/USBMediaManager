#ifndef COPYDIALOG_H
#define COPYDIALOG_H

#include <QDialog>

namespace Ui {
	class CopyDialog;
}
class CopyDialog : public QDialog
{
	Q_OBJECT

public:
	CopyDialog(QWidget *parent, qint64 totalSize);
	~CopyDialog();

	QString destinationDirectory() const;
	void setDestinationDirectory(const QString &dir);

public slots:
    virtual void accept();

private slots:
	void browse();
	void updateButtonState();

private:
	Ui::CopyDialog *ui;
    qint64 _totalSize;
    QString _originalStyleSheet;
};

#endif // COPYDIALOG_H
