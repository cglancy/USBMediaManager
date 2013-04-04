#ifndef THUMBNAILMANAGER_H
#define THUMBNAILMANAGER_H

#include <QObject>
#include <QMap>
#include <QImage>

template <typename T>
class QFutureWatcher;
class ThumbnailFile;

struct ThumbnailImages 
{
    QImage smallImage;
    QImage mediumImage;
    QImage largeImage;
};

class ThumbnailManager : public QObject
{
	Q_OBJECT

public:
	ThumbnailManager(QObject *parent);
	~ThumbnailManager();

	void setThumbnailSizes(const QSize &largeSize, const QSize &mediumSize, const QSize &smallSize);
	void createThumbnail(ThumbnailFile *file);

signals:
	void thumbnailCreated(ThumbnailFile *file);

private slots:
	void thumbnailFinished();	

private:
	QImage createThumbnail(const QString &filename, const QSize &size);
    ThumbnailImages createThumbnails(const QString &filename, const QSize &largeSize, const QSize &mediumSize, const QSize &smallSize);

private:
	QSize _largeSize, _mediumSize, _smallSize;
	QMap<QFutureWatcher<ThumbnailImages>*, ThumbnailFile*> _fileMap;
};

#endif // THUMBNAILMANAGER_H
