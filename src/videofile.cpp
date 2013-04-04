#include "videofile.h"

VideoFile::VideoFile(Video *video)
	: RemoteFile(video)
{

}

VideoFile::~VideoFile()
{

}

bool VideoFile::hasMetaData() const
{
	return _metaData.count() != 0;
}

QMap<QString, QString> VideoFile::metaData() const
{
	return _metaData;
}

void VideoFile::setMetaData(const QMap<QString, QString> metaData)
{
	_metaData = metaData;
	setChanged();
}
