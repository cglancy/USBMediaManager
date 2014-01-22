#include "video.h"
#include "medialibrary.h"
#include "thumbnailfile.h"
#include "videofile.h"

Video::Video(MediaLibrary *library, const QString &id)
	: _library(library), _id(id)
{
	_thumbnailFile = new ThumbnailFile(this);
	_file = new VideoFile(this);
}

Video::~Video()
{
	delete _thumbnailFile;
	delete _file;
}

MediaLibrary * Video::library() const
{
	return _library;
}

QString Video::id() const
{
	return _id;
}

QString Video::name() const
{
	return _name;
}

void Video::setName(const QString &name)
{
	_name = name;
}

ThumbnailFile * Video::thumbnailFile() const
{
	return _thumbnailFile;
}

VideoFile * Video::videoFile() const
{
	return _file;
}

void Video::setChanged()
{
    _library->setVideoChanged(this);
}
