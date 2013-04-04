#ifndef CATEGORYNODE_H
#define CATEGORYNODE_H

#include <QString>
#include <QList>
#include <QSet>

class CategoryFile;
class Video;
class MediaLibrary;
class ThumbnailFile;

class CategoryNodeStats
{
public:
	CategoryNodeStats();
	CategoryNodeStats(const CategoryNodeStats &s);
	CategoryNodeStats & operator=(const CategoryNodeStats &s);

	int totalVideoCount;
	int downloadedVideoCount;
    int downloadingVideoCount;
    int pausedVideoCount;
	int uniqueVideoCount;
	int thumbnailCount;
	int downloadedThumbnailCount;
	qint64 totalVideoSize;
	qint64 downloadedVideoSize;
	qint64 uniqueVideoSize;
	qint64 thumbnailSize;
	qint64 downloadedThumbnailSize;
};

class CategoryNode
{
public:
    CategoryNode(MediaLibrary *library, CategoryNode *parent = 0);
    virtual ~CategoryNode();

	MediaLibrary * library() const;
    CategoryNode * parent() const;

    QString name() const;
    void setName(const QString &name);

    ThumbnailFile * thumbnailFile() const;

    int row() const;

    virtual void addChild(CategoryNode *child);
    virtual int childCount() const;
    virtual CategoryNode * child(int index) const;
    virtual int indexOf(CategoryNode *child) const;

	void addVideo(Video *video);
	int videoCount() const;
	Video * video(int index) const;
	int indexOf(Video *video) const;

	QString path(const QString &rootDir) const;

    CategoryNodeStats stats() const;

	void setChanged();

private:
    void categoryStats(const CategoryNode *node, QSet<Video*> &videoSet, CategoryNodeStats &stats) const;

private:
	MediaLibrary *_library;
    CategoryNode *_parent;
	QList<CategoryNode*> _children;
	QList<Video*> _videos;
    QString _name;
    ThumbnailFile *_file;
};

#endif // CATEGORYNODE_H
