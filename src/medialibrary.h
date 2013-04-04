#ifndef MEDIALIBRARY_H
#define MEDIALIBRARY_H

#include <QAbstractItemModel>
#include <QMap>
#include <QList>
#include <QPixmap>

class CategoryNode;
class Video;
class QXmlStreamReader;
class ThumbnailFile;

class MediaLibrary : public QAbstractItemModel
{
    Q_OBJECT

public:
    MediaLibrary(QObject *parent);
    ~MediaLibrary();

    void initLibrary(const QString &xmlFilename);
	void clear();

    CategoryNode * rootNode() const;
	QList<Video*> videos() const;

    QModelIndex index(CategoryNode *node) const;
	CategoryNode * categoryNode(const QModelIndex &index) const;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    void setCategoryChanged(CategoryNode *node);
	void setVideoChanged(Video *video);

signals:
	void categoryChanged(CategoryNode *node);
	void videoChanged(Video *video);

private:
	CategoryNode* parseCategoryNode(QXmlStreamReader& xml, CategoryNode *parent);
	void parseVideoRef(QXmlStreamReader& xml, CategoryNode *parent);
    Video* parseVideo(QXmlStreamReader& xml);
	void parseFile(QXmlStreamReader& xml, Video *video);
	bool parseLibrary(QXmlStreamReader& xml);
    bool createDirectory(const QString &path);
    QString stripUrlQuery(const QString &url);
    void initThumbnailFile(ThumbnailFile *file, const QString &urlStr, const QString &idStr);

private:
    CategoryNode *_rootNode;
	QMap<QString, Video*> _videoMap;
	QList<QPair<CategoryNode*, QString> > _refList;
    QPixmap _folderPixmap;
	QString _videoDirectory, _mediaDirectory, _thumbnailDirectory;
};

#endif // MEDIALIBRARY_H
