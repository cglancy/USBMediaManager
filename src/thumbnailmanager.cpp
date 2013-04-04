#include "thumbnailmanager.h"
#include "remotefile.h"
#include "categorynode.h"
#include "video.h"
#include "thumbnailfile.h"

#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrentRun>
#include <QPixmap>

ThumbnailManager::ThumbnailManager(QObject *parent)
	: QObject(parent)
{
    _largeSize = QSize(400, 300);
    _mediumSize = QSize(200, 150);
    _smallSize = QSize(100, 75);
}

ThumbnailManager::~ThumbnailManager()
{

}

void ThumbnailManager::setThumbnailSizes(const QSize &largeSize, const QSize &mediumSize, const QSize &smallSize)
{
    _largeSize = largeSize;
    _mediumSize = mediumSize;
    _smallSize = smallSize;
}

void ThumbnailManager::createThumbnail(ThumbnailFile *file)
{
	QFutureWatcher<ThumbnailImages> *futureWatcher = new QFutureWatcher<ThumbnailImages>;
	connect(futureWatcher, SIGNAL(finished()), this, SLOT(thumbnailFinished()));
	_fileMap.insert(futureWatcher, file);

	QFuture<ThumbnailImages> imageFuture = QtConcurrent::run(this, &ThumbnailManager::createThumbnails, file->localPath(), _largeSize, _mediumSize, _smallSize);
	futureWatcher->setFuture(imageFuture);

	qDebug("CreateThumbnails %s", file->localPath().toLocal8Bit().data());
}

QImage ThumbnailManager::createThumbnail(const QString &filename, const QSize &size)
{
	QImage image;
	image.load(filename);
	return image.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

ThumbnailImages ThumbnailManager::createThumbnails(const QString &filename, const QSize &largeSize, const QSize &mediumSize, const QSize &smallSize)
{
    ThumbnailImages images;

    images.largeImage.load(filename);
    images.smallImage = images.largeImage.scaled(smallSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    images.mediumImage = images.largeImage.scaled(mediumSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    images.largeImage = images.largeImage.scaled(largeSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    return images;
}

void ThumbnailManager::thumbnailFinished()
{
	QFutureWatcher<ThumbnailImages> *watcher = static_cast<QFutureWatcher<ThumbnailImages>*>(sender());
	watcher->deleteLater();

	Q_ASSERT(_fileMap.contains(watcher));

	ThumbnailFile *file = _fileMap.value(watcher);
	_fileMap.remove(watcher);

	ThumbnailImages images = watcher->result();

	QPixmap smallPixmap = QPixmap::fromImage(images.smallImage);
	QPixmap mediumPixmap = QPixmap::fromImage(images.mediumImage);
	QPixmap largePixmap = QPixmap::fromImage(images.largeImage);

//	Q_ASSERT(!smallPixmap.isNull());
//	Q_ASSERT(!mediumPixmap.isNull());
//	Q_ASSERT(!largePixmap.isNull());

	file->setPixmap(smallPixmap, ThumbnailFile::SmallPixmapSize);
	file->setPixmap(mediumPixmap, ThumbnailFile::MediumPixmapSize);
	file->setPixmap(largePixmap, ThumbnailFile::LargePixmapSize);

	emit thumbnailCreated(file);

	qDebug("thumbnailFinished %s", file->localPath().toLocal8Bit().data());
}

