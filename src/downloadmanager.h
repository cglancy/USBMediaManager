#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QMap>
#include <QNetworkReply>

class RemoteFile;
class QNetworkAccessManager;


class DownloadManager : public QObject
{
	Q_OBJECT

public:
	DownloadManager(QObject *parent);
	~DownloadManager();

	bool isDownloadingVideos() const;

    void requestVideoSize(RemoteFile *file);
	void downloadVideo(RemoteFile *file);
	void cancelVideoDownload(RemoteFile *file);
	void downloadFile(RemoteFile *file);
    
signals:
	void videoDownloaded(RemoteFile *file);
	void videoSized(RemoteFile *file);
	void fileDownloaded(RemoteFile *file);
	void allDownloadsProgress(qint64 bytesReceived, qint64 bytesTotal);
    void allDownloadsFinished();
	void downloadStatus(const QString &message);

public slots:
	void cancelAllVideoDownloads();

private slots:
	void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void sizeRequestFinished();
    void videoReadyRead();
    void videoDownloadFinished();
	void fileFinished();
	void error(QNetworkReply::NetworkError code);

private:
    void writeFile(const QString &fileName, const QByteArray &data);
    void emitAllDownloadsProgress();
    bool proceedWithDownload(RemoteFile *file);
    bool proceedWithSizeRequest(RemoteFile *file);

private:
	QNetworkAccessManager *_networkAccessManager;
	QMap<QNetworkReply*, RemoteFile*> _sizeReplyMap, _videoThumbReplyMap;
    QMap<QNetworkReply*, RemoteFile*> _videoReplyMap;
    QByteArray _userAgentStr;
};

#endif // DOWNLOADMANAGER_H
