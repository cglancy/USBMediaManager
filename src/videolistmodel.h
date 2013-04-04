#ifndef VIDEOLISTMODEL_H
#define VIDEOLISTMODEL_H

#include <QAbstractListModel>
#include <QPixmap>
#include <QSet>

class CategoryNode;
class Video;

class VideoListModel : public QAbstractListModel
{
	Q_OBJECT

public:
	enum Column
	{
        OrdinalColumn    = 0,
		NameColumn		 = 1,
		TypeColumn		 = 2,
		SizeColumn		 = 3,
		DownloadedColumn = 4,
		SmallIconColumn	 = 5,
		MediumIconColumn = 6,
        LargeIconColumn	 = 7,
		ColumnCount		 = 8
	};

    enum Role
    {
        IntSortRole      = Qt::UserRole,
        LongLongSortRole = Qt::UserRole + 1
    };

public:
	VideoListModel(QObject *parent);
	~VideoListModel();

	void filterByCategory(CategoryNode *node);

    Video * video(const QModelIndex &index) const;

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	QVariant data(const QModelIndex &index, int role) const;

public slots:
	void videoChanged(Video *video);
    void libraryModelReset();

private:
	void buildCategorySet(CategoryNode *node, QSet<Video*> &set);

private:
    CategoryNode *_filterCategory;
	QList<Video*> _videos;
	QPixmap _videoPixmap, _checkmarkPixmap, _errorPixmap;
};

#endif // VIDEOLISTMODEL_H
