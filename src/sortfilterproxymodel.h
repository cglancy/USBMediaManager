#ifndef SORTFILTERPROXYMODEL_H
#define SORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class SortFilterProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

public:
	SortFilterProxyModel(QObject *parent);
	~SortFilterProxyModel();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
	bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
};

#endif // SORTFILTERPROXYMODEL_H
