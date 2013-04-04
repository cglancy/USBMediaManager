#ifndef OPENDIALOG_H
#define OPENDIALOG_H

#include <QDialog>

class LibraryListModel;
class Library;
class QModelIndex;
class QItemSelection;

namespace Ui {
	class OpenDialog;
}

class OpenDialog : public QDialog
{
	Q_OBJECT

public:
	OpenDialog(QWidget *parent, LibraryListModel *model);
	~OpenDialog();

	Library * selectedLibrary() const;

private slots:
	void okClicked();
	void doubleClicked(const QModelIndex &index);
	void selectionChanged(const QItemSelection&, const QItemSelection&);
    void updateSourceFields();

private:
	void updateButtonState();

private:
	Ui::OpenDialog *ui;
	LibraryListModel *_model;
	Library *_selectedLibrary;
};

#endif // OPENDIALOG_H
