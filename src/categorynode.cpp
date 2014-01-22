#include "categorynode.h"
#include "medialibrary.h"
#include "utility.h"
#include "video.h"
#include "thumbnailfile.h"
#include "videofile.h"

#include <QDir>

CategoryNodeStats::CategoryNodeStats()
{
	totalVideoCount = 0;
	downloadedVideoCount = 0;
    downloadingVideoCount = 0;
    pausedVideoCount = 0;
	uniqueVideoCount = 0;
	thumbnailCount = 0;
	downloadedThumbnailCount = 0;
	totalVideoSize = 0;
	downloadedVideoSize = 0;
	uniqueVideoSize = 0;
	thumbnailSize = 0;
	downloadedThumbnailSize = 0;
}

CategoryNodeStats::CategoryNodeStats(const CategoryNodeStats &s)
{
	totalVideoCount = s.totalVideoCount;
	downloadedVideoCount = s.downloadedVideoCount;
    downloadingVideoCount = s.downloadingVideoCount;
    pausedVideoCount = s.pausedVideoCount;
	uniqueVideoCount = s.uniqueVideoCount;
	thumbnailCount = s.thumbnailCount;
	downloadedThumbnailCount = s.downloadedThumbnailCount;
	totalVideoSize = s.totalVideoSize;
	downloadedVideoSize = s.downloadedVideoSize;
	uniqueVideoSize = s.uniqueVideoSize;
	thumbnailSize = s.thumbnailSize;
	downloadedThumbnailSize = s.downloadedThumbnailSize;
}

CategoryNodeStats & CategoryNodeStats::operator=(const CategoryNodeStats &s)
{
	totalVideoCount = s.totalVideoCount;
	downloadedVideoCount = s.downloadedVideoCount;
    downloadingVideoCount = s.downloadingVideoCount;
    pausedVideoCount = s.pausedVideoCount;
	uniqueVideoCount = s.uniqueVideoCount;
	thumbnailCount = s.thumbnailCount;
	downloadedThumbnailCount = s.downloadedThumbnailCount;
	totalVideoSize = s.totalVideoSize;
	downloadedVideoSize = s.downloadedVideoSize;
	uniqueVideoSize = s.uniqueVideoSize;
	thumbnailSize = s.thumbnailSize;
	downloadedThumbnailSize = s.downloadedThumbnailSize;
	return *this;
}


CategoryNode::CategoryNode(MediaLibrary *library, CategoryNode *parent)
    : _library(library), _parent(parent)
{
	if (parent)
		parent->addChild(this);

    _file = new ThumbnailFile(this);
}

CategoryNode::~CategoryNode()
{
    delete _file;
	qDeleteAll(_children);
	// don't delete videos, owned by MediaLibrary class
}

MediaLibrary * CategoryNode::library() const
{
	return _library;
}

CategoryNode * CategoryNode::parent() const
{
    return _parent;
}

QString CategoryNode::name() const
{
    return _name;
}

void CategoryNode::setName(const QString &name)
{
    _name = name;
}

ThumbnailFile * CategoryNode::thumbnailFile() const
{
	return _file;
}

int CategoryNode::row() const
{
    if (_parent)
        return _parent->indexOf(const_cast<CategoryNode*>(this));

    return 0;
}

int CategoryNode::childCount() const
{
	return _children.length();
}

CategoryNode * CategoryNode::child(int index) const
{
	return _children.at(index);
}

void CategoryNode::addChild(CategoryNode *child)
{
	_children.append(child);
}

int CategoryNode::indexOf(CategoryNode *child) const
{
	return _children.indexOf(child);
}

void CategoryNode::setChanged()
{
    _library->setCategoryChanged(this);
}

void CategoryNode::addVideo(Video *video)
{
	_videos.append(video);
}

int CategoryNode::videoCount() const
{
	return _videos.count();
}

Video * CategoryNode::video(int index) const
{
	return _videos.at(index);
}

int CategoryNode::indexOf(Video *video) const
{
	return _videos.indexOf(video);
}

QString CategoryNode::path(const QString &rootDir) const
{
	QString pathStr;

	if (_parent)
		pathStr = _parent->path(rootDir) + "/" + Utility::validFileSystemString(_name);
	else
		pathStr = rootDir;

	return pathStr;
}

CategoryNodeStats CategoryNode::stats() const
{
	CategoryNodeStats stats;
    QSet<Video*> videoSet;
    categoryStats(this, videoSet, stats);

    stats.uniqueVideoCount = videoSet.count();

    foreach (Video *video, videoSet)
    {
        stats.uniqueVideoSize += video->videoFile()->fileSize();
    }

	return stats;
}

void CategoryNode::categoryStats(const CategoryNode *node, QSet<Video*> &videoSet, CategoryNodeStats &stats) const
{
    if (!node)
        return;

	if (node->thumbnailFile()->state() == RemoteFile::DownloadedState)
	{
		stats.downloadedThumbnailCount++;
		stats.downloadedThumbnailSize += node->thumbnailFile()->fileSize();
	}

	stats.thumbnailCount++;
	stats.thumbnailSize = node->thumbnailFile()->fileSize();

    for (int i = 0; i < node->videoCount(); i++)
	{
		Video *video = node->video(i);
		if (video->videoFile()->state() == RemoteFile::DownloadedState)
		{
			stats.downloadedVideoSize += video->videoFile()->fileSize();
			stats.downloadedVideoCount++;

			if (video->thumbnailFile()->state() == RemoteFile::DownloadedState)
			{
				stats.downloadedThumbnailSize += video->thumbnailFile()->fileSize();
				stats.downloadedThumbnailCount++;
			}
		}
        else if (video->videoFile()->state() == RemoteFile::DownloadingState)
            stats.downloadingVideoCount++;
        else if (video->videoFile()->state() == RemoteFile::DownloadPausedState)
            stats.pausedVideoCount++;

		stats.totalVideoCount++;
		stats.totalVideoSize += video->videoFile()->fileSize();

		stats.thumbnailCount++;
		stats.thumbnailSize = video->thumbnailFile()->fileSize();

        videoSet.insert(video);
	}

    for (int i = 0; i < node->childCount(); i++)
        categoryStats(node->child(i), videoSet, stats);
}
