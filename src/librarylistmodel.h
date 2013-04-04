#ifndef LIBRARYLISTMODEL_H
#define LIBRARYLISTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QString>
#include <QPixmap>

class DownloadManager;
class RemoteFile;
class QXmlStreamReader;
class Library;

class LibraryListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    LibraryListModel(QObject *parent, DownloadManager *dm);
    ~LibraryListModel();

    QString sourceUrl() const;
    void setSourceUrl(const QString &url);
	void clear();

	QString title() const;
	QString link() const;
	QString description() const;
	QString copyright() const;

	Library * library(const QModelIndex &index);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QVariant data(const QModelIndex &index, int role) const;

signals:
    void sourceLoaded();

private slots:
    void fileDownloaded(RemoteFile *file);
	void parseLibraryList(const QString &fileName);
    void parseChannel(QXmlStreamReader &xml);
    void parseItem(QXmlStreamReader &xml);

private:
    DownloadManager *_downloadManager;
	QList<Library*> _items;
	QString _sourceUrl, _title, _link, _description, _copyright;
    RemoteFile *_sourceFile;
    QPixmap _libraryPixmap;
};

#endif // LIBRARYLISTMODEL_H
