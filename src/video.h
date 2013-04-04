#ifndef VIDEO_H
#define VIDEO_H

#include <QString>
#include <QMap>

class MediaLibrary;
class ThumbnailFile;
class VideoFile;

class Video
{
public:
    Video(MediaLibrary *library, const QString &id);
    ~Video();

    void setRemoteFileChanged();
    void setThumbnailFileChanged();

	MediaLibrary * library() const;
	QString id() const;

	QString name() const;
	void setName(const QString &name);

	ThumbnailFile * thumbnailFile() const;
	VideoFile * videoFile() const;

    void setChanged();

private:
	MediaLibrary *_library;
	QString _id, _name;
	ThumbnailFile *_thumbnailFile;
	VideoFile *_file; 
};

#endif // VIDEO_H
