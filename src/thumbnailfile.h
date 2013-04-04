#ifndef THUMBNAILFILE_H
#define THUMBNAILFILE_H

#include "remotefile.h"
#include <QPixmap>
#include <QList>

class ThumbnailFile : public RemoteFile
{
public:
    enum PixmapSize
    {
        SmallPixmapSize = 0,
        MediumPixmapSize = 1,
        LargePixmapSize = 2
    };

public:
    ThumbnailFile(CategoryNode *category);
	ThumbnailFile(Video *video);
    virtual ~ThumbnailFile();

    QPixmap pixmap(PixmapSize size) const;
    void setPixmap(const QPixmap &pixmap, PixmapSize size);

private:
    QList<QPixmap> _pixmaps;    
};

#endif // THUMBNAILFILE_H
