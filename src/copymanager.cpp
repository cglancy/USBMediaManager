#include "copymanager.h"
#include "utility.h"

#include <QMainWindow>
#include <QDir>
#include <QMessageBox>
#include <QApplication>
#include <QFile>
#include <QFutureWatcher>
#include <QFuture>
#include <QtConcurrentRun>
#include <QProgressDialog>
#include <QTimer>

CopyManager::CopyManager(QMainWindow *mainWindow)
    : QObject(mainWindow), _overwriteAll(false), _futureWatcher(0)
{
    _progressDialog = new QProgressDialog(mainWindow);
    _progressDialog->setMinimum(0);
    _progressDialog->setWindowTitle(Utility::applicationName());
    _progressDialog->setLabelText(tr("Exporting videos..."));

    _futureWatcher = new QFutureWatcher<FileCopyFuture>(this);
    connect(_futureWatcher, SIGNAL(finished()), this, SLOT(copyFinished()));
}

CopyManager::~CopyManager()
{

}

bool CopyManager::isCopying() const
{
    return _fileList.count() > 0;
}

void CopyManager::startCopying(const QList<QString> &dirList, const QList<QPair<QString, QString> > &fileList)
{
    _progressDialog->setMaximum(fileList.count() + 1);
    _progressValue = 0;
    _overwriteAll = false;

    foreach (QString dir, dirList)
    {
        if (!createDirectory(dir))
        {
            _progressDialog->cancel();
            return;
        }
    }

    _progressDialog->setValue(_progressValue++);

    _fileList = fileList;
    copyNextFile();
}

bool CopyManager::createDirectory(const QString &path)
{
    QDir dir(path);
    if (!dir.exists())
    {
        if (!dir.mkpath(path))
        {
            QMessageBox::critical(_progressDialog, Utility::applicationName(), tr("Unable to create folder <b>%1</b>.").arg(path));
            return false;
        }
    }

    return true;
}

void CopyManager::copyNextFile()
{
    if (_progressDialog->wasCanceled())
    {
        cancel();
        return;
    }

    if (_fileList.isEmpty())
    {
        finish();
        return;
    }

    QPair<QString, QString> filePair = _fileList.takeFirst();
    copyFile(filePair.first, filePair.second);
}

void CopyManager::copyFile(const QString &fileName, const QString &newName)
{
    if (QFile::exists(newName))
    {
        bool overwrite = false;

        if (!_overwriteAll)
        {
            QMessageBox box(_progressDialog);
            box.setModal(true);
            box.setWindowTitle(Utility::applicationName());
            box.setText(tr("File <b>%1</b> already exists.").arg(newName));
            box.setInformativeText(tr("Do you want to overwrite the file?"));
            box.setIcon(QMessageBox::Question);
            box.setStandardButtons(QMessageBox::YesToAll | QMessageBox::Yes | QMessageBox::No);

            switch (box.exec())
            {
            default:
            case QMessageBox::No:
                cancel();
                return;
                break;
            case QMessageBox::YesToAll:
                _overwriteAll = true;
            case QMessageBox::Yes:
                overwrite = true;
                break;
            }
        }

        if (overwrite || _overwriteAll)
        {
            if (!QFile::remove(newName))
            {
                QMessageBox::critical(_progressDialog, Utility::applicationName(), tr("Unable to delete <b>%1</b>.").arg(newName));
                cancel();
                return;
            }
        }
    }

    QFuture<FileCopyFuture> future = QtConcurrent::run(&CopyManager::futureCopy, fileName, newName);
    _futureWatcher->setFuture(future);
}

FileCopyFuture CopyManager::futureCopy(const QString &fileName, const QString &newName)
{
    bool success = QFile::copy(fileName, newName);
    return FileCopyFuture(success, newName);
}

void CopyManager::copyFinished()
{
    FileCopyFuture future = _futureWatcher->result();

    if (future.first)
    {
        qDebug() << "Copied " << future.second;

        if (!_progressDialog->wasCanceled())
            _progressDialog->setValue(_progressValue++);
        
        copyNextFile();
    }
    else
    {
        QMessageBox::critical(_progressDialog, Utility::applicationName(), tr("Unable to copy file <b>%1</b>.").arg(future.second));
        cancel();
    }
}

void CopyManager::cancel()
{
    _progressDialog->reset();
    _fileList.clear();
    emit copyingFinished();
}

void CopyManager::finish()
{
    _progressDialog->setValue(_progressDialog->maximum());
    _progressDialog->reset();
    emit copyingFinished();
}
