#include "medialibrary.h"
#include "categorynode.h"
#include "thumbnailfile.h"
#include "videofile.h"
#include "video.h"
#include "preferencesdialog.h"
#include "utility.h"

#include <QStack>
#include <QXmlStreamReader>
#include <QFile>
#include <QMessageBox>
#include <QLocale>
#include <QUrl>
#include <QDir>
#include <QApplication>
#include <QDebug>

MediaLibrary::MediaLibrary(QObject *parent)
    : QAbstractItemModel(parent), _rootNode(0)
{
    _folderPixmap = QPixmap(":/pvm/images/video_folder_16.png");
}

MediaLibrary::~MediaLibrary()
{
	clear();
}

void MediaLibrary::clear()
{
	if (_rootNode)
	{
        beginResetModel();
		delete _rootNode;
		_rootNode = 0;
        endResetModel();
	}

	QMap<QString, Video*>::const_iterator i = _videoMap.constBegin();
	while (i != _videoMap.constEnd()) 
	{
		delete i.value();
		++i;
	}
	_videoMap.clear();
}

CategoryNode * MediaLibrary::rootNode() const
{
    return _rootNode;
}

QList<Video*> MediaLibrary::videos() const
{
	return _videoMap.values();
}

void MediaLibrary::initLibrary(const QString &xmlFilename)
{
    beginResetModel();

	clear();
	_refList.clear();

    QFile* file = new QFile(xmlFilename);

    if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) 
    {
        QMessageBox::critical(QApplication::activeWindow(), Utility::applicationName(), tr("Unable to open file %1.").arg(xmlFilename), QMessageBox::Ok);
        delete file;
        return;
    }

	_videoDirectory = Utility::currentVideoDirectory();
	_mediaDirectory = _videoDirectory;
	_thumbnailDirectory = _mediaDirectory + "/thumbnails";

    QXmlStreamReader xml(file);
	QStack<CategoryNode*> nodeStack;
	Video *currentVideo = 0;

    _rootNode = new CategoryNode(this, 0);
    nodeStack.push(_rootNode);

    while (!xml.atEnd() && !xml.hasError()) 
    {
        QXmlStreamReader::TokenType token = xml.readNext();

        if (token == QXmlStreamReader::StartDocument) 
            continue;

        if (token == QXmlStreamReader::StartElement) 
        {
			if (xml.name() == "library") 
			{
				if (!parseLibrary(xml))
                    break; // abort xml parsing
			}
			else if (xml.name() == "category") 
			{
                CategoryNode *category = parseCategoryNode(xml, nodeStack.top());
                nodeStack.push(category);
			}
			else if (xml.name() == "videoref")
			{
				parseVideoRef(xml, nodeStack.top());
			}
            else if (xml.name() == "video")
            {
                currentVideo = parseVideo(xml);
            }
        }
		else if (token == QXmlStreamReader::EndElement)
		{
			if (xml.name() == "category")
				nodeStack.pop();
		}
    }

    if (xml.hasError()) 
    {
        QMessageBox::critical(QApplication::activeWindow(), Utility::applicationName(), xml.errorString(), QMessageBox::Ok);
        delete file;
		return;
    }

	xml.clear();
	file->close();
    delete file;

	for (int i = 0; i < _refList.count(); i++)
	{
		CategoryNode *node = _refList[i].first;
		QString id = _refList[i].second;

		if (_videoMap.contains(id))
		{
			Video *video = _videoMap.value(id);
			if (node && video)
				node->addVideo(video);
		}
		else
		{
			qDebug() << "Missing video " << id << " for category " << node->name();
		}
	}

	_refList.clear();

    endResetModel();
}

bool MediaLibrary::parseLibrary(QXmlStreamReader& xml)
{
	QXmlStreamAttributes attributes = xml.attributes();

	if (attributes.hasAttribute("name"))
	{
		QString name = Utility::validFileSystemString(attributes.value("name").toString());
		_mediaDirectory = _videoDirectory + "/" + name;
		_thumbnailDirectory = _mediaDirectory + "/thumbnails";

		QDir mediaDir(_mediaDirectory);
		if (!mediaDir.exists())
        {
			if (!createDirectory(_mediaDirectory))
                return false;
        }

		QDir thumbDir(_thumbnailDirectory);
		if (!thumbDir.exists())
        {
            if (!createDirectory(_thumbnailDirectory))
                return false;
        }
	}
	else
	{
		QDir thumbDir(_thumbnailDirectory);
		if (!thumbDir.exists())
        {
            if (!createDirectory(_thumbnailDirectory))
                return false;
        }
	}

    return true;
}

bool MediaLibrary::createDirectory(const QString &dirPath)
{
    QDir dir;
    bool success = dir.mkpath(dirPath);
    if (!success)
        QMessageBox::critical(QApplication::activeWindow(), Utility::applicationName(), tr("Unable to create folder %1.").arg(dirPath), QMessageBox::Ok);

    return success;
}

CategoryNode* MediaLibrary::parseCategoryNode(QXmlStreamReader& xml, CategoryNode *parent)
{
	CategoryNode *node = new CategoryNode(this, parent);

	QXmlStreamAttributes attributes = xml.attributes();

	if (attributes.hasAttribute("name")) 
		node->setName(attributes.value("name").toString());

	if (attributes.hasAttribute("thumbnail"))
	{
		QString urlStr = attributes.value("thumbnail").toString();
        initThumbnailFile(node->thumbnailFile(), urlStr, QString());
	}

	return node;
}

void MediaLibrary::initThumbnailFile(ThumbnailFile *file, const QString &urlStr, const QString &idStr)
{
    if (urlStr.isEmpty())
        return;

    file->setUrl(urlStr);

    QString filePath;

    if (urlStr.startsWith("./thumbnails"))
    {
        filePath = urlStr;
        filePath.replace("./thumbnails", _thumbnailDirectory);
    }
    else if (Utility::isLocalPath(urlStr))
    {
        filePath = Utility::localPath(urlStr);
    }
    else if (!idStr.isEmpty())
    {
        QFileInfo fileNameInfo(file->fileName());
        filePath = QString("%1/thumbnail-%3.%4").arg(_thumbnailDirectory, idStr, fileNameInfo.suffix());
    }
    else
    {
        filePath = _thumbnailDirectory + "/" + file->fileName();
    }

    file->setLocalPath(filePath);
    if (QFile::exists(filePath))
    {
        QFile localFile(filePath);
        file->setState(RemoteFile::DownloadedState);
        file->setFileSize(localFile.size());
    }
}

void MediaLibrary::parseVideoRef(QXmlStreamReader& xml, CategoryNode *node)
{
	QXmlStreamAttributes attributes = xml.attributes();

	if (attributes.hasAttribute("ref")) 
	{	
		QString ref = attributes.value("ref").toString();
		_refList.append(QPair<CategoryNode*, QString>(node, ref));
	}
}

Video* MediaLibrary::parseVideo(QXmlStreamReader& xml)
{
	QXmlStreamAttributes attributes = xml.attributes();

	if (!attributes.hasAttribute("id"))
		return 0;

	QString id = attributes.value("id").toString();
	Video *video = new Video(this, id);
	_videoMap.insert(id, video);

	if (attributes.hasAttribute("name")) 
		video->setName(attributes.value("name").toString());

	if (attributes.hasAttribute("thumbnail"))
	{
		QString urlStr = attributes.value("thumbnail").toString();
        initThumbnailFile(video->thumbnailFile(), urlStr, video->id());
	}

	if (attributes.hasAttribute("url"))
	{
		VideoFile *file = video->videoFile();

		QString urlStr = attributes.value("url").toString();
        urlStr = stripUrlQuery(urlStr);
		file->setUrl(urlStr);

        if (urlStr.isEmpty())
        {
            file->setState(RemoteFile::DownloadErrorState);
            file->setErrorMessage(tr("Error: No URL"));
        }
        else
        {
            QString filePath;
            if (Utility::isLocalPath(urlStr))
                filePath = Utility::localPath(urlStr);
            else
                filePath = _mediaDirectory + "/" + file->fileName();

            file->setLocalPath(filePath);

            QFileInfo fi(filePath);
            QString downloadPath(filePath);
            downloadPath.replace(fi.suffix(), "download");
            file->setDownloadPath(downloadPath);

            if (fi.exists())
            {
                file->setState(RemoteFile::DownloadedState);
                file->setFileSize(fi.size());
            }
            else if (QFile::exists(downloadPath))
            {
                QFileInfo dfi(downloadPath);
                file->setState(RemoteFile::DownloadPausedState);
                file->setBytesReceived(dfi.size());
            }
        }
	}

	return video;
}

QModelIndex MediaLibrary::index(CategoryNode *node) const
{
    return createIndex(node->row(), 0, node);
}

CategoryNode * MediaLibrary::categoryNode(const QModelIndex &index) const
{
	return static_cast<CategoryNode*>(index.internalPointer());
}

void MediaLibrary::setCategoryChanged(CategoryNode *node)
{
    QModelIndex i = index(node);
    emit dataChanged(i, i);
	emit categoryChanged(node);
}

void MediaLibrary::setVideoChanged(Video *video)
{
	emit videoChanged(video);
}

QModelIndex MediaLibrary::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	CategoryNode *parentNode;

	if (!parent.isValid())
		parentNode = _rootNode;
	else
		parentNode = static_cast<CategoryNode*>(parent.internalPointer());

	CategoryNode *childNode = parentNode->child(row);
	if (childNode)
		return createIndex(row, column, childNode);
	else
		return QModelIndex();
}

QModelIndex MediaLibrary::parent(const QModelIndex &index ) const
{
	if (!index.isValid())
		return QModelIndex();

	CategoryNode *childNode = static_cast<CategoryNode*>(index.internalPointer());
	CategoryNode *parentNode = childNode->parent();

	if (parentNode == _rootNode || parentNode == 0)
		return QModelIndex();

	return createIndex(parentNode->row(), 0, parentNode);
}

int MediaLibrary::rowCount(const QModelIndex &parent) const
{
	CategoryNode *parentNode;
	if (parent.column() > 0)
		return 0;

	if (!parent.isValid())
		parentNode = _rootNode;
	else
		parentNode = static_cast<CategoryNode*>(parent.internalPointer());

	if (parentNode)
		return parentNode->childCount();

	return 0;
}

int MediaLibrary::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant MediaLibrary::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        if (section == 0)
            return QVariant(tr("Categories"));
        //else if (section == 1)
        //    return QVariant("Size");
    }

    return QVariant();
}

QVariant MediaLibrary::data(const QModelIndex &index, int role) const
{
    QVariant v;

    if (!index.isValid())
        return v;

    CategoryNode *node = static_cast<CategoryNode*>(index.internalPointer());
    if (!node)
        return v;

    if (role == Qt::DisplayRole || role == Qt::ToolTipRole)
    {
        if (index.column() == 0)
            v = QVariant(node->name());
    }
    else if (index.column() == 0 && role == Qt::DecorationRole)
    {
		v = QVariant(_folderPixmap);
    }

    return v;
}

Qt::ItemFlags MediaLibrary::flags(const QModelIndex &index) const
{
	Q_UNUSED(index);

    Qt::ItemFlags f = Qt::ItemIsSelectable |  Qt::ItemIsEnabled; 
    return f;
}

QString MediaLibrary::stripUrlQuery(const QString &urlStr)
{
    QString stripStr(urlStr);
    if (stripStr.contains('?'))
        stripStr.truncate(stripStr.indexOf('?'));
    return stripStr;
}

