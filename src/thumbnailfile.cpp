#include "thumbnailfile.h"


ThumbnailFile::ThumbnailFile(CategoryNode *category)
    : RemoteFile(category)
{
    _pixmaps.append(QPixmap());
    _pixmaps.append(QPixmap());
    _pixmaps.append(QPixmap());
}

ThumbnailFile::ThumbnailFile(Video *video)
	: RemoteFile(video)
{
	_pixmaps.append(QPixmap());
	_pixmaps.append(QPixmap());
	_pixmaps.append(QPixmap());
}

ThumbnailFile::~ThumbnailFile()
{

}

QPixmap ThumbnailFile::pixmap(ThumbnailFile::PixmapSize size) const
{
    return _pixmaps.at(size);
}

void ThumbnailFile::setPixmap(const QPixmap &p, ThumbnailFile::PixmapSize size)
{
    _pixmaps.replace(size, p);
	setChanged();
}

