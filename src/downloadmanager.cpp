#include "downloadmanager.h"
#include "thumbnailfile.h"
#include "video.h"
#include "categorynode.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QSettings>
#include <QUrl>

DownloadManager::DownloadManager(QObject *parent)
	: QObject(parent)
{
	_networkAccessManager = new QNetworkAccessManager(this);
    //_userAgentStr = "Media Desktop/1.0 (Digia; Qt)";
    _userAgentStr = "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.4 (KHTML, like Gecko) Chrome/22.0.1229.79 Safari/537.4";
    //_userAgentStr = "Mozilla/5.0 (Windows; U; Windows NT 6.1; sv-SE) AppleWebKit/533.19.4 (KHTML, like Gecko) Version/5.0.3 Safari/533.19.4";
}

DownloadManager::~DownloadManager()
{

}

void DownloadManager::requestVideoSize(RemoteFile *file)
{
    Q_ASSERT(file);
    if (!file)
        return;

    if (!proceedWithSizeRequest(file))
        return;

    QNetworkRequest request;
    request.setUrl(QUrl(file->url()));
    request.setRawHeader("User-Agent", _userAgentStr);

    QNetworkReply *reply = _networkAccessManager->head(request);
    connect(reply, SIGNAL(finished()), this, SLOT(sizeRequestFinished()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));

    file->setFileSize(0);
    _sizeReplyMap.insert(reply, file);

    qDebug() << "Requested size for " << file->fileName();
}

bool DownloadManager::proceedWithSizeRequest(RemoteFile *file)
{
    if (file->url().isEmpty())
    {
        file->setState(RemoteFile::DownloadErrorState);
        file->setErrorMessage(tr("Error: No URL"));
        emit videoSized(file);
        return false;
    }

    QUrl url(file->url());
    QString schemeStr = url.scheme();

    if (schemeStr == "file")
    {
        if (QFile::exists(file->localPath()))
        {
            QFileInfo fi(file->localPath());
            file->setState(RemoteFile::DownloadedState);
            file->setFileSize(fi.size());
            emit videoSized(file);
            return false;
        }
        else
        {
            file->setState(RemoteFile::DownloadErrorState);
            file->setErrorMessage(tr("Error: Local file %1 not found.").arg(file->localPath()));
            emit videoSized(file);
            return false;
        }
    }
    else if (schemeStr == "http")
    {
        if (QFile::exists(file->localPath()) && !file->alwaysDownload())
        {
            QFileInfo fi(file->localPath());
            file->setState(RemoteFile::DownloadedState);
            file->setFileSize(fi.size());
            emit videoSized(file);
            return false;
        }
    }
    else
    {
        if (QFile::exists(file->localPath()))
        {
            QFileInfo fi(file->localPath());
            file->setState(RemoteFile::DownloadedState);
            file->setFileSize(fi.size());
            emit videoSized(file);
            return false;
        }
        else
        {
            file->setState(RemoteFile::DownloadErrorState);
            file->setErrorMessage(tr("Error: Invalid URL"));
            emit videoSized(file);
            return false;
        }
    }

    return true;
}

void DownloadManager::sizeRequestFinished()
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
    reply->deleteLater();

	Q_ASSERT(_sizeReplyMap.contains(reply));
    if (!_sizeReplyMap.contains(reply))
        return;

    RemoteFile *file = _sizeReplyMap.value(reply);
    _sizeReplyMap.remove(reply);

	if (reply->error() == QNetworkReply::NoError)
	{
		qint64 size = reply->header(QNetworkRequest::ContentLengthHeader).toLongLong();
		file->setFileSize(size);
		emit videoSized(file);

        qDebug() << file->fileName() << " size = " << size;
	}
	else
	{
		file->setState(RemoteFile::DownloadErrorState);
		file->setErrorMessage(reply->errorString());
	}
}

void DownloadManager::downloadFile(RemoteFile *file)
{
	Q_ASSERT(file);
	if (!file)
		return;

    if (!proceedWithDownload(file))
        return;

    QNetworkRequest request;
    request.setUrl(QUrl(file->url()));
    request.setRawHeader("User-Agent", _userAgentStr);

	QNetworkReply *reply = _networkAccessManager->get(request);
	connect(reply, SIGNAL(finished()), this, SLOT(fileFinished()));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));

	file->setFileSize(0);
	file->setState(RemoteFile::DownloadingState);

	_videoThumbReplyMap.insert(reply, file);

	qDebug("Start download %s", file->url().toLocal8Bit().data());
}

bool DownloadManager::proceedWithDownload(RemoteFile *file)
{
    if (file->url().isEmpty())
    {
        file->setState(RemoteFile::DownloadErrorState);
        file->setErrorMessage(tr("Error: No URL"));
        emit fileDownloaded(file);
        return false;
    }

    QUrl url(file->url());
    QString schemeStr = url.scheme();

    if (schemeStr == "file")
    {
        if (QFile::exists(file->localPath()))
        {
            file->setState(RemoteFile::DownloadedState);
            emit fileDownloaded(file);
            return false;
        }
        else
        {
            file->setState(RemoteFile::DownloadErrorState);
            file->setErrorMessage(tr("Error: Local file %1 not found.").arg(file->localPath()));
            emit fileDownloaded(file);
            return false;
        }
    }
    else if (schemeStr == "http")
    {
        if (QFile::exists(file->localPath()) && !file->alwaysDownload())
        {
            file->setState(RemoteFile::DownloadedState);
            emit fileDownloaded(file);
            return false;
        }
    }
    else
    {
        if (QFile::exists(file->localPath()))
        {
            file->setState(RemoteFile::DownloadedState);
            emit fileDownloaded(file);
            return false;
        }
        else
        {
            file->setState(RemoteFile::DownloadErrorState);
            file->setErrorMessage(tr("Error: Invalid URL"));
            emit fileDownloaded(file);
            return false;
        }
    }

    return true;
}

void DownloadManager::fileFinished()
{
	QNetworkReply *reply = static_cast<QNetworkReply*>(sender());

	if (_videoThumbReplyMap.contains(reply))
	{
		RemoteFile *file = _videoThumbReplyMap.value(reply);
		_videoThumbReplyMap.remove(reply);

		if (!file)
			return;

		if (reply->error() == QNetworkReply::NoError)
		{
			writeFile(file->localPath(), reply->readAll());
			file->setState(RemoteFile::DownloadedState);
			emit fileDownloaded(file);

			qDebug("Downloaded %s", file->fileName().toLocal8Bit().data());
		}
		else
		{
			file->setState(RemoteFile::DownloadErrorState);
			file->setErrorMessage(reply->errorString());
			emit fileDownloaded(file);
		}
	}

	reply->deleteLater();
}

void DownloadManager::downloadVideo(RemoteFile *file)
{
	Q_ASSERT(file);
	if (!file)
		return;

	if (file->state() == RemoteFile::DownloadingState)
		return;

    if (!proceedWithDownload(file))
        return;

	QNetworkRequest request;
	request.setUrl(QUrl(file->url()));
	request.setRawHeader("User-Agent", _userAgentStr);

	QFileInfo fi(file->downloadPath());
	if (fi.exists())
	{
		QString rangeStr = QString("bytes=%1-").arg(QString::number(fi.size()));
		request.setRawHeader("Range", rangeStr.toUtf8());
		file->setBytesResumed(fi.size());
	}
	else
	{
        file->setBytesResumed(0);
	}

	QFile *downloadFile = new QFile(file->downloadPath());
	if (!downloadFile->open(QIODevice::WriteOnly | QIODevice::Append))
	{
		file->setState(RemoteFile::DownloadErrorState);
		file->setErrorMessage(tr("Error: Unable to open file %1.").arg(file->downloadPath()));
		delete downloadFile;
		return;
	}

	file->setDownloadFile(downloadFile);
	file->setState(RemoteFile::DownloadingState);
    file->setBytesReceived(file->bytesResumed());

	QNetworkReply *reply = _networkAccessManager->get(request);
	connect(reply, SIGNAL(finished()), this, SLOT(videoDownloadFinished()));
	connect(reply, SIGNAL(readyRead()), this, SLOT(videoReadyRead()));
	connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(downloadProgress(qint64, qint64)));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));

	_videoReplyMap.insert(reply, file);

	emitAllDownloadsProgress();
	emit downloadStatus(QString(tr("Downloading %1...")).arg(file->fileName()));

	qDebug("Start download %s", file->url().toLocal8Bit().data());
}

void DownloadManager::videoReadyRead()
{
	QNetworkReply *reply = static_cast<QNetworkReply*>(sender());

	if (_videoReplyMap.contains(reply))
	{
		RemoteFile *file = _videoReplyMap.value(reply);
		file->downloadFile()->write(reply->readAll());
	}
}

void DownloadManager::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    Q_UNUSED(bytesTotal);

	QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
	if (_videoReplyMap.contains(reply))
	{
		RemoteFile *file = _videoReplyMap.value(reply);
		file->setBytesReceived(file->bytesResumed() + bytesReceived);
        emitAllDownloadsProgress();
	}
}

void DownloadManager::error(QNetworkReply::NetworkError code)
{
    Q_UNUSED(code);

	QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
    qDebug("Network error: %s", reply->errorString().toLocal8Bit().data());
 }

void DownloadManager::videoDownloadFinished()
{
	QNetworkReply *reply = static_cast<QNetworkReply*>(sender());

	if (_videoReplyMap.contains(reply))
	{
		RemoteFile *file = _videoReplyMap.value(reply);
        _videoReplyMap.remove(reply);

		if (reply->error() == QNetworkReply::NoError)
		{
            file->downloadFile()->write(reply->readAll());
            file->downloadFile()->flush();
            file->downloadFile()->close();
            QFile::rename(file->downloadPath(), file->localPath());
            delete file->downloadFile();
			file->setDownloadFile(0);

            file->setState(RemoteFile::DownloadedState);
            emit videoDownloaded(file);
            emit downloadStatus(QString(tr("Downloaded %1")).arg(file->fileName()));

            qDebug("Downloaded %s", file->fileName().toLocal8Bit().data());
		}
		else if (reply->error() == QNetworkReply::RemoteHostClosedError && file->retryCount() < 3)
		{
			file->setRetryCount(file->retryCount() + 1);
			file->downloadFile()->close();
			delete file->downloadFile();
			file->setDownloadFile(0);
			file->setState(RemoteFile::DownloadPausedState);

			// try again
			qDebug("Retrying %s", file->fileName().toLocal8Bit().data());
			downloadVideo(file);
		}
		else if (reply->error() == QNetworkReply::OperationCanceledError)
		{
			file->downloadFile()->write(reply->readAll());
			file->downloadFile()->flush();
			if (file->downloadFile()->size() > 0)
			{
				file->setState(RemoteFile::DownloadPausedState);
				file->setBytesReceived(file->downloadFile()->size());
			}
			else
			{
				file->setState(RemoteFile::NotDownloadedState);
				file->setBytesReceived(0);
			}
			file->downloadFile()->close();
			delete file->downloadFile();
			file->setDownloadFile(0);

			emitAllDownloadsProgress();
		}
		else
		{
            file->downloadFile()->close();
            delete file->downloadFile();
			file->setDownloadFile(0);
			file->setState(RemoteFile::DownloadErrorState);
			file->setErrorMessage(reply->errorString());
			file->setBytesReceived(0);

			emitAllDownloadsProgress();
		}

        if (_videoReplyMap.count() == 0)
		{
            emit allDownloadsFinished();
		}
	}

    reply->deleteLater();
}

void DownloadManager::emitAllDownloadsProgress()
{
    if (_videoReplyMap.count() > 0)
    {
        qint64 bytesReceived = 0;
        qint64 bytesTotal = 0;

        foreach (RemoteFile *file, _videoReplyMap)
        {
            bytesReceived += file->bytesReceived();
            bytesTotal += file->fileSize();
        }

        emit allDownloadsProgress(bytesReceived, bytesTotal);
    }
    else
    {
        emit allDownloadsProgress(1, 1);
    }
}

void DownloadManager::writeFile(const QString &fileName, const QByteArray &data)
{
	// force overwrite??
	if (QFile::exists(fileName))
		QFile::remove(fileName);

	QFile localFile(fileName);
	if (!localFile.open(QIODevice::WriteOnly))
		return;

	localFile.write(data);
	localFile.close();

	return;
}

bool DownloadManager::isDownloadingVideos() const
{
	return _videoReplyMap.count() > 0;
}

void DownloadManager::cancelVideoDownload(RemoteFile *videoFile)
{
	if (!videoFile)
		return;

	QMap<QNetworkReply*, RemoteFile*>::iterator i = _videoReplyMap.begin();
	while (i != _videoReplyMap.end()) 
	{
		if (i.value() == videoFile)
		{
			i.key()->abort();
			return;
		}

		i++;
	}

	// not found, so at least change the state
	videoFile->determineFileState();
}

void DownloadManager::cancelAllVideoDownloads()
{
	QMap<QNetworkReply*, RemoteFile*>::iterator i = _videoReplyMap.begin();
	while (i != _videoReplyMap.end()) 
	{
		i.key()->abort();
		i = _videoReplyMap.begin();
	}
}
