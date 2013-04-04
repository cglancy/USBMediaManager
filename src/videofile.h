#ifndef VIDEOFILE_H
#define VIDEOFILE_H

#include "remotefile.h"
#include <QString>
#include <QMap>

class VideoFile : public RemoteFile
{
public:
	VideoFile(Video *video);
	virtual ~VideoFile();

	bool hasMetaData() const;
	QMap<QString, QString> metaData() const;
	void setMetaData(const QMap<QString, QString> metaData);

private:
	QMap<QString, QString> _metaData;
};

#endif // VIDEOFILE_H
