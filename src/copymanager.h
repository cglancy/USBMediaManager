#ifndef COPYMANAGER_H
#define COPYMANAGER_H

#include <QObject>
#include <QList>
#include <QPair>
#include <QFutureWatcher>

class QMainWindow;
class QProgressDialog;

typedef QPair<bool, QString> FileCopyFuture;
typedef QList<QPair<QString, QString> > FileCopyPair;

class CopyManager : public QObject
{
    Q_OBJECT

public:
    CopyManager(QMainWindow *mainWindow);
    ~CopyManager();

    bool isCopying() const;
    void startCopying(const QList<QString> &dirList, const FileCopyPair &fileList);

signals:
    void copyingFinished();

public slots:
	void cancel();

private slots:
    void copyFinished();
    void copyNextFile();
    void finish();

private:
    bool createDirectory(const QString &path);
    void copyFile(const QString &fileName, const QString &newName);
    static FileCopyFuture futureCopy(const QString &fileName, const QString &newName);

private:
    bool _overwriteAll;
    QList<QPair<QString, QString> > _fileList;
    QProgressDialog *_progressDialog;
    int _progressValue;
    QFutureWatcher<FileCopyFuture> *_futureWatcher;
};

#endif // COPYMANAGER_H
