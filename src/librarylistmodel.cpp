#include "librarylistmodel.h"
#include "downloadmanager.h"
#include "remotefile.h"
#include "utility.h"
#include "library.h"

#include <QFile>
#include <QFileInfo>
#include <QXmlStreamReader>
#include <QMessageBox>
#include <QApplication>

LibraryListModel::LibraryListModel(QObject *parent, DownloadManager *dm)
    : QAbstractListModel(parent), _downloadManager(dm), _sourceFile(0)
{
    _libraryPixmap = QPixmap(":/pvm/images/video_folder_16.png");
    connect(_downloadManager, SIGNAL(fileDownloaded(RemoteFile*)), this, SLOT(fileDownloaded(RemoteFile*)));
}

LibraryListModel::~LibraryListModel()
{
    if (_sourceFile)
        delete _sourceFile;

	clear();
}

QString LibraryListModel::sourceUrl() const
{
    return _sourceUrl;
}

void LibraryListModel::setSourceUrl(const QString &urlStr)
{
	if (urlStr.isEmpty())
		return;

    _sourceUrl = urlStr;

	if (_items.count() > 0)
	{
		beginRemoveRows(QModelIndex(), 0, _items.count() - 1);
		clear();
		endRemoveRows();
	}

	if (!_sourceFile || (_sourceFile && _sourceFile->url() != _sourceUrl))
	{
		if (_sourceFile)
			delete _sourceFile;

        QString localPath;
        if (Utility::isLocalPath(_sourceUrl))
        {
            localPath = Utility::localPath(_sourceUrl);
        }
        else
        {
            QFileInfo fi(_sourceUrl);
            QString videoDir = Utility::currentVideoDirectory();
            localPath = videoDir + "/" + fi.fileName();
        }

		_sourceFile = new RemoteFile();
		_sourceFile->setUrl(_sourceUrl);
		_sourceFile->setLocalPath(localPath);
		_sourceFile->setAlwaysDownload(true);
	}

	_downloadManager->downloadFile(_sourceFile);
}

void LibraryListModel::clear()
{
	qDeleteAll(_items);
	_items.clear();
}

void LibraryListModel::fileDownloaded(RemoteFile *file)
{
	if (file != _sourceFile)
		return;

	if (_sourceFile->state() != RemoteFile::DownloadedState)
	{
        QMessageBox::critical(QApplication::activeWindow(), Utility::applicationName(), tr("Open library failed. Unable to download %1.").arg(_sourceFile->url()), 
            QMessageBox::Ok);
		return;
	}

	parseLibraryList(_sourceFile->localPath());

    if (_items.count() > 0)
    {
        beginInsertRows(QModelIndex(), 0, _items.count() - 1);
        endInsertRows();
    }
}

void LibraryListModel::parseLibraryList(const QString &fileName)
{
	QFile localFile(fileName);

	if (!localFile.open(QIODevice::ReadOnly | QIODevice::Text)) 
	{
        QMessageBox::critical(QApplication::activeWindow(), Utility::applicationName(), tr("Open library failed. Unable to open %1.").arg(fileName), 
		    QMessageBox::Ok);
		return;
	}

	QXmlStreamReader xml(&localFile);

	while (!xml.atEnd() && !xml.hasError()) 
	{
		QXmlStreamReader::TokenType token = xml.readNext();

		if (token == QXmlStreamReader::StartDocument) 
			continue;

		if (token == QXmlStreamReader::StartElement) 
		{
			if (xml.name() == "channel")
				parseChannel(xml);
		}
	}

	localFile.close();
    emit sourceLoaded();
}

void LibraryListModel::parseChannel(QXmlStreamReader &xml)
{
	xml.readNext();

	while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "channel")) 
	{
		if (xml.tokenType() == QXmlStreamReader::StartElement) 
		{
			if (xml.name() == "title") 
			{
				xml.readNext();
				_title = xml.text().toString();
			}

			if (xml.name() == "link") 
			{
				xml.readNext();
				_link = xml.text().toString();
			}

			if (xml.name() == "description") 
			{
				xml.readNext();
				_description = xml.text().toString();
			}

			if (xml.name() == "copyright") 
			{
				xml.readNext();
				_copyright = xml.text().toString();
			}

			if (xml.name() == "item") 
			{
				parseItem(xml);
			}
		}

		xml.readNext();
	}
}

void LibraryListModel::parseItem(QXmlStreamReader &xml)
{
	Library *item = new Library();
	_items.append(item);

	xml.readNext();

	while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "item")) 
	{
        if (xml.tokenType() == QXmlStreamReader::StartElement) 
        {
            if (xml.name() == "title") 
            {
                xml.readNext();
                item->setName(xml.text().toString());
            }

            if (xml.name() == "link") 
            {
                xml.readNext();
                QString urlStr = xml.text().toString();
                item->setUrl(urlStr);

                if (Utility::isLocalPath(urlStr))
                {
                    item->setLocalPath(Utility::localPath(urlStr));
                }
                else
                {
                    QFileInfo fi(urlStr);
                    QString videoDir = Utility::currentVideoDirectory();
                    QString localPath = videoDir + "/" + fi.fileName();
                    item->setLocalPath(localPath);
                }
            }

            if (xml.name() == "description") 
            {
                xml.readNext();
                item->setDescription(xml.text().toString());
            }
        }

        xml.readNext();
	}
}

Library * LibraryListModel::library(const QModelIndex &index)
{
	Library *library = 0;

	if (!index.isValid())
		return library;

	if (index.row() >= 0 && index.row() < _items.count())
		library = _items.at(index.row());

	return library;
}

QString LibraryListModel::title() const
{
	return _title;
}

QString LibraryListModel::link() const
{
	return _link;
}

QString LibraryListModel::description() const
{
	return _description;
}

QString LibraryListModel::copyright() const
{
	return _copyright;
}

int LibraryListModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
    return _items.count();
}

int LibraryListModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
    return 2;
}

QVariant LibraryListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	QVariant v;

	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		if (section == 0)
			v = QVariant(tr("Library"));
		else if (section == 1)
			v = QVariant(tr("Size"));
	}

	return v;
}

QVariant LibraryListModel::data(const QModelIndex &index, int role) const
{
	QVariant v;

	Library *item = _items.at(index.row());
	if (!item)
		return v;

	if (role == Qt::DisplayRole)
	{
		if (index.column() == 0)
			v = QVariant(item->name());
		else if (index.column() == 1)
			v = QVariant(item->description());
	}
	else if (role == Qt::DecorationRole)
	{
        if (index.column() == 0)
            v = QVariant(_libraryPixmap);
	}

	return v;
}
