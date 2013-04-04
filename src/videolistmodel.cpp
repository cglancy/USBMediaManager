#include "videolistmodel.h"
#include "categorynode.h"
#include "video.h"
#include "utility.h"
#include "videofile.h"
#include "thumbnailfile.h"

#include <QLocale>
#include <QSet>

VideoListModel::VideoListModel(QObject *parent)
	: QAbstractListModel(parent), _filterCategory(0)
{
	_videoPixmap = QPixmap(":/pvm/images/video_16.png");
	_checkmarkPixmap = QPixmap(":/pvm/images/checkmark.png");
	_errorPixmap = QPixmap(":/pvm/images/error.png");
}

VideoListModel::~VideoListModel()
{

}

Video * VideoListModel::video(const QModelIndex &index) const
{
    if (index.isValid() && index.row() < _videos.count())
        return _videos[index.row()];
    return 0;
}

void VideoListModel::filterByCategory(CategoryNode *node)
{
    _filterCategory = node;

    if (_videos.count() > 0)
    {
        beginRemoveRows(QModelIndex(), 0, _videos.count() - 1);
        _videos.clear();
        endRemoveRows();
    }

    if (!node)
        return;

    QSet<Video*> videoSet;
	buildCategorySet(node, videoSet);

    if (videoSet.count() > 0)
    {
        beginInsertRows(QModelIndex(), 0, videoSet.count() - 1);
        _videos = QList<Video*>::fromSet(videoSet);
        endInsertRows();
    }
}

void VideoListModel::buildCategorySet(CategoryNode *node, QSet<Video*> &set)
{
	for (int i = 0; i < node->childCount(); i++)
		buildCategorySet(node->child(i), set);

	for (int i = 0; i < node->videoCount(); i++)
		set.insert(node->video(i));
}

int VideoListModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return _videos.count();
}

int VideoListModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return ColumnCount;
}

QVariant VideoListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	QVariant v;

	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		switch (section)
		{
        case NameColumn:
            v = QVariant("Name");
            break;
		case TypeColumn:
			v = QVariant("Type");
			break;
		case SizeColumn:
			v = QVariant("Size");
			break;
		case DownloadedColumn:
			v = QVariant("Downloaded");
			break;
		}
	}

	return v;
}

QVariant VideoListModel::data(const QModelIndex &index, int role) const
{
	QVariant v;
	Video *video = _videos.at(index.row());
	if (!video)
		return v;

	if (role == Qt::DisplayRole)
	{
		switch (index.column())
		{
        case OrdinalColumn:
            if (_filterCategory)
            {
                int index = _filterCategory->indexOf(video);
                if (index != -1)
                    v = QVariant(QString::number(index + 1));
            }
            break;
		case NameColumn:
		case SmallIconColumn:
        case MediumIconColumn:
		case LargeIconColumn:
			v = QVariant(video->name());
			break;
        case DownloadedColumn:
            if (video->videoFile()->state() == RemoteFile::DownloadingState)
                v = QVariant(QString::number(video->videoFile()->percentComplete(), 'f', 0) + QLocale::system().percent());
            else if (video->videoFile()->state() == RemoteFile::DownloadPausedState)
                v = QVariant(QString::number(video->videoFile()->percentComplete(), 'f', 0) + QLocale::system().percent() + tr(" (Paused)"));
            break;
		case SizeColumn:
			v = QVariant(Utility::fileSizeString(video->videoFile()->fileSize()));
			break;
		case TypeColumn:
			v = QVariant(video->videoFile()->type());
			break;
		}
	}
	else if (role == Qt::DecorationRole)
	{
		switch (index.column())
		{
		case OrdinalColumn:
			v = QVariant(_videoPixmap);
			break;
		case DownloadedColumn:
			if (video->videoFile()->state() == RemoteFile::DownloadedState)
				v = QVariant(_checkmarkPixmap);
			else if (video->videoFile()->state() == RemoteFile::DownloadErrorState)
				v = QVariant(_errorPixmap);
			break;
		case SmallIconColumn:
            v = QVariant(video->thumbnailFile()->pixmap(ThumbnailFile::SmallPixmapSize));
            break;
        case MediumIconColumn:
            v = QVariant(video->thumbnailFile()->pixmap(ThumbnailFile::MediumPixmapSize));
            break;
		case LargeIconColumn:
            v = QVariant(video->thumbnailFile()->pixmap(ThumbnailFile::LargePixmapSize));
			break;
		}
	}
	else if (role == Qt::ToolTipRole)
	{
		switch (index.column())
		{
		default:
		case NameColumn:
			v = QVariant(video->name());
			break;
		case DownloadedColumn:
			if (video->videoFile()->state() == RemoteFile::DownloadedState)
				v = QVariant(tr("Downloaded"));
			else if (video->videoFile()->state() == RemoteFile::DownloadErrorState)
				v = QVariant(video->videoFile()->errorMessage());
			break;
		}
	}
	else if (role == Qt::TextAlignmentRole)
	{
		if (index.column() == SizeColumn)
			v = QVariant(Qt::AlignRight + Qt::AlignVCenter);
	}
    else if (role == IntSortRole)
    {
        if (index.column() == OrdinalColumn && _filterCategory)
        {
            int index = _filterCategory->indexOf(video);
            if (index != -1)
                v = QVariant(index + 1);
            else
                v = QVariant(0);
        }
    }
    else if (role == LongLongSortRole)
    {
        if (index.column() == SizeColumn)
            v = QVariant(video->videoFile()->fileSize());
    }


	return v;
}

void VideoListModel::videoChanged(Video *video)
{
	int row = _videos.indexOf(video);
	if (row >= 0 && row < _videos.count())
	{
		emit dataChanged(createIndex(row, 0), createIndex(row, 0));
	}
}

void VideoListModel::libraryModelReset()
{
    beginResetModel();
    _videos.clear();
    endResetModel();
}