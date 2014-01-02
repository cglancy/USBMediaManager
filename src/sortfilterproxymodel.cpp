#include "sortfilterproxymodel.h"
#include "videolistmodel.h"
#include "video.h"
#include "videofile.h"

SortFilterProxyModel::SortFilterProxyModel(QObject *parent)
	: QSortFilterProxyModel(parent)
{

}

SortFilterProxyModel::~SortFilterProxyModel()
{

}

bool SortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
	if (left.column() == VideoListModel::SizeColumn && right.column() == VideoListModel::SizeColumn)
	{
        qlonglong leftSize = sourceModel()->data(left, VideoListModel::LongLongSortRole).toLongLong();
        qlonglong rightSize = sourceModel()->data(right, VideoListModel::LongLongSortRole).toLongLong();
        return leftSize < rightSize;
	}
    else if (left.column() == VideoListModel::OrdinalColumn && right.column() == VideoListModel::OrdinalColumn)
    {
        int leftOrdinal = sourceModel()->data(left, VideoListModel::IntSortRole).toInt();
        int rightOrdinal = sourceModel()->data(right, VideoListModel::IntSortRole).toInt();
        return leftOrdinal < rightOrdinal;
    }

	return QSortFilterProxyModel::lessThan(left, right);
}

bool SortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, VideoListModel::NameColumn, sourceParent);
    return sourceModel()->data(index, Qt::DisplayRole).toString().contains(filterRegExp());
}
