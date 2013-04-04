#include "metadata.h"
#include "videofile.h"
#include "video.h"
#include "utility.h"

#include <QUrl>
#include <QFile>
#include <QMediaMetaData>
#include <QTimer>
#include <QMediaPlayer>
//#include <QVideoWidget>

MetaData::MetaData(QObject *parent)
	: QObject(parent), _player(0)
{
	_player = new QMediaPlayer(this);
	_player->setMuted(true);

    // don't show() the hidden video widget
    //_hiddenWidget = new QVideoWidget;
    //_player->setVideoOutput(_hiddenWidget);

	connect(_player, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64)));

    _timer = new QTimer(this);
    connect(_timer, SIGNAL(timeout()), this, SLOT(readNext()));
}

MetaData::~MetaData()
{
//    delete _hiddenWidget;
}

void MetaData::queryMetaData(VideoFile *file)
{
	if (!file)
		return;

	if (!QFile::exists(file->localPath()))
		return;

	// crashing at the moment
#if 0
	_fileQueue.enqueue(file);

    if (!_timer->isActive())
        _timer->start(0);
#endif
}

void MetaData::readNext()
{
	if (_player->state() != QMediaPlayer::StoppedState)
		return;

    if (_fileQueue.count() == 0)
    {
        _timer->stop();
        return;
    }

	VideoFile *file = _fileQueue.dequeue();
	if (!file)
		return;

	QUrl url = QUrl::fromLocalFile(file->localPath());
	_player->setMedia(url);

	QString localUrl = url.toLocalFile();
	_fileMap.insert(localUrl, file);

    qDebug() << "MetaData::readNext() playing " << localUrl;
	_player->play();

	QTimer::singleShot(5000, Qt::CoarseTimer, this, SLOT(timeout()));
}

void MetaData::durationChanged(qint64 duration)
{
	readMetaData(duration);
}

void MetaData::timeout()
{
	if (_player->state() != QMediaPlayer::StoppedState)
	{
        qDebug() << "MetaData::timeout() for " << _player->currentMedia().canonicalUrl();
		readMetaData(0);
	}
}

bool MetaData::readMetaData(qint64 duration)
{
	QString localUrl = _player->currentMedia().canonicalUrl().toLocalFile();

	if (!_fileMap.contains(localUrl))
		return false;

	VideoFile *file = _fileMap.value(localUrl);
	_fileMap.remove(localUrl);

	if (!_player->isMetaDataAvailable())
		return false;

    qDebug() << "MetaData::readMetaData() reading metadata for " << localUrl;
	QMap<QString, QString> metaData;

	metaData.insert(QMediaMetaData::Title, _player->metaData(QMediaMetaData::Title).toString());
	//metaData.insert(QMediaMetaData::Publisher, _player->metaData(QMediaMetaData::Publisher).toString());
	//metaData.insert(QMediaMetaData::Copyright, _player->metaData(QMediaMetaData::Copyright).toString());
	//metaData.insert(QMediaMetaData::MediaType, _player->metaData(QMediaMetaData::MediaType).toString());
	metaData.insert(QMediaMetaData::Genre, _player->metaData(QMediaMetaData::Genre).toString());
	metaData.insert(QMediaMetaData::Year, _player->metaData(QMediaMetaData::Year).toString());

	QStringList stringList = _player->metaData(QMediaMetaData::ContributingArtist).toStringList();
	if (stringList.count() > 0)
		metaData.insert(QMediaMetaData::ContributingArtist, stringList[0]);
	stringList = _player->metaData(QMediaMetaData::Composer).toStringList();
	if (stringList.count() > 0)
		metaData.insert(QMediaMetaData::Composer, stringList[0]);

	// video
	metaData.insert(QMediaMetaData::Duration, Utility::durationString(duration));
	metaData.insert(QMediaMetaData::Resolution, Utility::resolutionString(_player->metaData(QMediaMetaData::Resolution).toSize()));
	metaData.insert(QMediaMetaData::PixelAspectRatio, Utility::resolutionString(_player->metaData(QMediaMetaData::PixelAspectRatio).toSize()));
	metaData.insert(QMediaMetaData::VideoFrameRate, _player->metaData(QMediaMetaData::VideoFrameRate).toString());
	metaData.insert(QMediaMetaData::VideoBitRate, _player->metaData(QMediaMetaData::VideoBitRate).toString());
	metaData.insert(QMediaMetaData::VideoCodec, _player->metaData(QMediaMetaData::VideoCodec).toString());

	// audio
	metaData.insert(QMediaMetaData::AudioBitRate, _player->metaData(QMediaMetaData::AudioBitRate).toString());
	metaData.insert(QMediaMetaData::AudioCodec, _player->metaData(QMediaMetaData::AudioCodec).toString());
	metaData.insert(QMediaMetaData::ChannelCount, _player->metaData(QMediaMetaData::ChannelCount).toString());
	metaData.insert(QMediaMetaData::SampleRate, _player->metaData(QMediaMetaData::SampleRate).toString());

	file->setMetaData(metaData);

	emit metaDataFinished(file);
	_player->stop();
	//_player->setMedia(QMediaContent());


	return true;
}
