#include "opendialog.h"
#include "ui_opendialog.h"
#include "librarylistmodel.h"
#include "utility.h"

#include <QInputDialog>
#include <QSettings>

OpenDialog::OpenDialog(QWidget *parent, LibraryListModel *model)
	: QDialog(parent), ui(new Ui::OpenDialog), _model(model), _selectedLibrary(0)
{
	ui->setupUi(this);

	ui->treeView->setModel(_model);
	ui->treeView->setRootIsDecorated(false);
	ui->treeView->setColumnWidth(0, 340);

	connect(ui->okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
	connect(ui->treeView, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(doubleClicked(const QModelIndex&)));
	connect(ui->treeView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(selectionChanged(const QItemSelection&, const QItemSelection&)));
    connect(_model, SIGNAL(sourceLoaded()), this, SLOT(updateSourceFields()));

    updateSourceFields();
	updateButtonState();
}

OpenDialog::~OpenDialog()
{

}

Library* OpenDialog::selectedLibrary() const
{
	return _selectedLibrary;
}

void OpenDialog::okClicked()
{
	QModelIndex index = ui->treeView->currentIndex();
	if (index.isValid())
	{
		_selectedLibrary = _model->library(index);
	}
}

void OpenDialog::doubleClicked(const QModelIndex &index)
{
	if (index.isValid())
	{
		_selectedLibrary = _model->library(index);
		accept();
	}
}

void OpenDialog::selectionChanged(const QItemSelection&, const QItemSelection&)
{
	updateButtonState();
}

void OpenDialog::updateButtonState()
{
	QModelIndex index = ui->treeView->currentIndex();
	if (index.isValid())
		ui->okButton->setEnabled(true);
	else
		ui->okButton->setEnabled(false);
}

void OpenDialog::updateSourceFields()
{
    ui->sourceLineEdit->setText(_model->title());
    ui->descriptionLineEdit->setText(_model->description());
    ui->copyrightLineEdit->setText(_model->copyright());
}
