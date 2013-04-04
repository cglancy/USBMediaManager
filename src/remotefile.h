#ifndef REMOTEFILE_H
#define REMOTEFILE_H

#include <QString>

class Video;
class CategoryNode;
class QFile;

class RemoteFile
{
public:
	enum State 
	{ 
		NotDownloadedState, 
		DownloadingState, 
		DownloadPausedState, 
		DownloadedState, 
		DownloadErrorState 
	};

public:
    RemoteFile();
	RemoteFile(Video *video);
	RemoteFile(CategoryNode *category);
	virtual ~RemoteFile();

	void setState(State state);
	State state() const;

	QString url() const;
	void setUrl(QString url);
	QString fileName() const;

    QString type() const;

	qint64 fileSize() const;
	void setFileSize(qint64 sizeBytes);

	QString localPath() const;
	void setLocalPath(const QString &path);

	bool alwaysDownload() const;
	void setAlwaysDownload(bool download);

    qint64 bytesReceived() const;
	void setBytesReceived(qint64 bytesReceived);

	double percentComplete() const;

	QString errorMessage() const;
	void setErrorMessage(const QString &error);

    int retryCount() const;
    void setRetryCount(int count);

    QString downloadPath() const;
    void setDownloadPath(const QString &path);

    QFile * downloadFile() const;
    void setDownloadFile(QFile *file);

    qint64 bytesResumed() const;
    void setBytesResumed(qint64 bytesResumed);

	void determineFileState();

protected:
    Video *_video;
	CategoryNode *_category;
	void setChanged();
    void init();

private:
	State _state;
	QString _url, _localPath, _errorMessage;
	qint64 _size, _bytesReceived;
    QString _downloadPath;
    QFile *_downloadFile;
    int _retryCount;
    qint64 _bytesResumed;
	bool _alwaysDownload;
};

#endif // REMOTEFILE_H
