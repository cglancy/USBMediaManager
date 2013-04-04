#include "remotefile.h"
#include "video.h"
#include "categorynode.h"

#include <QFileInfo>
#include <QLocale>
#include <QFile>

RemoteFile::RemoteFile(CategoryNode *category)
:	_video(0),
    _category(category)
{
    init();
}

RemoteFile::RemoteFile(Video *video)
:	_video(video),
    _category(0)
{
    init();
}

RemoteFile::RemoteFile()
:	_video(0),
    _category(0)
{
    init();
}

RemoteFile::~RemoteFile()
{

}

void RemoteFile::init()
{
    _state = NotDownloadedState;
    _size = 0; 
    _bytesReceived = 0; 
    _downloadFile = 0;
    _retryCount = 0; 
    _bytesResumed = 0;
	_alwaysDownload = false;
}

void RemoteFile::setState(State state)
{
	_state = state;
    setChanged();
}

RemoteFile::State RemoteFile::state() const
{
	return _state;
}

QString RemoteFile::url() const
{
	return _url;
}

void RemoteFile::setUrl(QString url)
{
	_url = url;
}

QString RemoteFile::fileName() const
{
	QFileInfo fi(_url);
	return fi.fileName();
}

QString RemoteFile::localPath() const
{
	return _localPath;
}

void RemoteFile::setLocalPath(const QString &path)
{
	_localPath = path;
}

qint64 RemoteFile::fileSize() const
{
	return _size;
}

void RemoteFile::setFileSize(qint64 sizeBytes)
{
	_size = sizeBytes;
    setChanged();
}

qint64 RemoteFile::bytesReceived() const
{
    return _bytesReceived;
}

void RemoteFile::setBytesReceived(qint64 bytesReceived)
{
	_bytesReceived = bytesReceived;
    setChanged();
}

double RemoteFile::percentComplete() const
{
    double percent = 0.;
    if (_size > 0)
        percent = _bytesReceived * 100. / _size;
    return percent;
}

QString RemoteFile::errorMessage() const
{
	return _errorMessage;
}

void RemoteFile::setErrorMessage(const QString &error)
{
	_errorMessage = error;
}

QString RemoteFile::type() const
{
    QString typeStr;

    QFileInfo fi(_localPath);
    QString extension = fi.suffix();

    if (extension == "mp4")
        typeStr = "MP4 Video";
    else if (extension == "mov")
        typeStr = "QuickTime Movie";
    else if (extension == "jpg")
        typeStr = "JPEG Image";
    else
        typeStr = "Unknown File Type";

    return typeStr;
}

int RemoteFile::retryCount() const
{
    return _retryCount;
}

void RemoteFile::setRetryCount(int count)
{
    _retryCount = count;
}

QString RemoteFile::downloadPath() const
{
    return _downloadPath;
}

void RemoteFile::setDownloadPath(const QString &path)
{
    _downloadPath = path;
}

QFile * RemoteFile::downloadFile() const
{
    return _downloadFile;
}

void RemoteFile::setDownloadFile(QFile *file)
{
    _downloadFile = file;
}

qint64 RemoteFile::bytesResumed() const
{
    return _bytesResumed;
}

void RemoteFile::setBytesResumed(qint64 bytesResumed)
{
    _bytesResumed = bytesResumed;
}

void RemoteFile::setChanged()
{
    if (_video)
        _video->setChanged();
	else if (_category)
		_category->setChanged();
}

void RemoteFile::determineFileState()
{
	if (QFile::exists(_localPath))
		_state = DownloadedState;
	else if (QFile::exists(_downloadPath))
		_state = DownloadPausedState;
	else
		_state = NotDownloadedState;
}

bool RemoteFile::alwaysDownload() const
{
	return _alwaysDownload;
}

void RemoteFile::setAlwaysDownload(bool download)
{
	_alwaysDownload = download;
}