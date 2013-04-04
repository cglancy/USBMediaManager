#ifndef METADATA_H
#define METADATA_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QQueue>

class VideoFile;
class QMediaPlayer;
class QVideoWidget;
class QTimer;

class MetaData : public QObject
{
	Q_OBJECT

public:
	MetaData(QObject *parent);
	~MetaData();

	void queryMetaData(VideoFile *file);

signals:
	void metaDataFinished(VideoFile *file);

private slots:
	void durationChanged(qint64 duration);
	void timeout();
    void readNext();

private:
	bool readMetaData(qint64 duration);

private:
	QMediaPlayer *_player;
	QQueue<VideoFile*> _fileQueue;
	QMap<QString, VideoFile*> _fileMap;
    QVideoWidget *_hiddenWidget;
    QTimer *_timer;
};

#endif // METADATA_H
