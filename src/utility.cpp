#include "utility.h"

#include <QLocale>
#include <QTime>
#include <QRegExp>
#include <QSettings>
#include <QStandardPaths>
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QUrl>

#ifdef Q_OS_WIN
#include <windows.h>
#endif
#ifdef Q_OS_MAC
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/mount.h>
#endif

Utility::Utility()
{

}

Utility::~Utility()
{

}

QString Utility::VideoDirectoryKey          = "videoDirectory";
QString Utility::LibrarySourceUrlKey        = "librarySourceUrl";
QString Utility::LastExportDirectoryKey     = "lastExportDirectory";
QString Utility::RecentLibraryListKey       = "recentLibraryList";
QString Utility::GeometryKey                = "geometry";
QString Utility::WindowStateKey             = "windowState";
QString Utility::FolderThumbnailNameKey     = "folderThumbnailName";


QString Utility::applicationName()
{
    return "USB Video Manager";
}

QString Utility::applicationVersionString()
{
    return "20140121 Beta";
}

QString Utility::fileSizeString(qint64 size, bool kb)
{
	static const qint64 bytesPerGB = 1024 * 1024 * 1024;
	static const qint64 bytesPerMB = 1024 * 1024;

	QString sizeStr;
	
	if (kb)
	{
		sizeStr = QLocale::system().toString(size / 1024) + " KB";
	}
	else
	{
		if (size > bytesPerGB)
			sizeStr = QString::number(size / (double) bytesPerGB, 'f', 1) + " GB";
		else if (size > bytesPerMB)
			sizeStr = QString::number(size / (double) bytesPerMB, 'f', 1) + " MB";
		else
			sizeStr = QString::number(size / 1024.0, 'f', 1) + " KB";
	}

	return sizeStr;
}

QString Utility::resolutionString(const QSize &size)
{
	QString resStr;
	if (size.isValid())
		resStr = QString("%1x%2").arg(QString::number(size.width()), QString::number(size.height()));
	return resStr;
}

QString Utility::durationString(qint64 milliseconds)
{
	qint64 x = milliseconds / 1000;
	int	seconds = x % 60;
	x /= 60;
	int minutes = x % 60;
	x /= 60;
	int hours = x % 24;

	QTime time(hours, minutes, seconds);
	return time.toString("hh:mm:ss");
}

QString Utility::validFileSystemString(const QString &fileName)
{
	QString str(fileName);
	str.replace(":", " -");

	QString expStr = QString("[%1]").arg(QRegExp::escape("^/?<>\\:*|\""));
	str.remove(QRegExp(expStr));

	return str;
}

QString Utility::currentVideoDirectory()
{
    QString videoDirectory;

    QSettings settings;
    if (settings.contains(Utility::VideoDirectoryKey))
    {
        videoDirectory = settings.value(Utility::VideoDirectoryKey).toString();
    }
    else
    {
        videoDirectory = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation) + "/" + applicationName();
        settings.setValue(Utility::VideoDirectoryKey, videoDirectory);
    }

    QDir dir(videoDirectory);
    if (!dir.exists())
    {
        if (!dir.mkpath(videoDirectory))
        {
            QMessageBox::critical(QApplication::activeWindow(), Utility::applicationName(), 
                QApplication::translate("Utility", "Unable to create folder %1.").arg(videoDirectory), 
                QMessageBox::Ok);
        }
    }

    return videoDirectory;
}

QString Utility::currentLibrarySourceUrl()
{
    QString librarySourceUrl;

    QSettings settings;
    if (settings.contains(Utility::LibrarySourceUrlKey))
    {
        librarySourceUrl = settings.value(Utility::LibrarySourceUrlKey).toString();
    }
    else
    {
        librarySourceUrl = Utility::fallbackLibrarySourceUrl();
        settings.setValue(Utility::LibrarySourceUrlKey, librarySourceUrl);
    }

    return librarySourceUrl;
}

QString Utility::fallbackLibrarySourceUrl()
{
	return "http://glancyfamily.net/ldsml/lds-media-library-source.xml";
}

QString Utility::removableDrive()
{
    QString removablePath;
#ifdef Q_OS_WIN
    QFileInfoList driveList = QDir::drives();
    foreach (QFileInfo fi, driveList)
    {
        if (fi.absolutePath() != "A:/" && fi.absolutePath() != "B:/")
        {
            UINT type = ::GetDriveType((LPCWSTR)fi.filePath().utf16());
            if (type == DRIVE_REMOVABLE)
            {
                removablePath = fi.filePath();
                break;
            }
        }
    }
#endif
    return removablePath;
}

qint64 Utility::availableSpace(const QString &path)
{
    qint64 available = 0;
#ifdef Q_OS_WIN
    ULARGE_INTEGER freeBytesToCaller;
    freeBytesToCaller.QuadPart = 0L;

    if(!::GetDiskFreeSpaceEx((LPCWSTR)path.utf16(), &freeBytesToCaller, NULL, NULL ))
    {
        qDebug() << "ERROR: Call to GetDiskFreeSpaceEx() failed.";
    }

    available = freeBytesToCaller.QuadPart;
#endif
#ifdef Q_OS_MAC
    struct statfs statf;
    statfs(path.toStdString().c_str(), &statf);
    available = statf.f_bfree * statf.f_bsize;
#endif
    return available;
}

bool Utility::isValidVideoDirectory(const QString &path)
{
    QFileInfo fi(path);

    if (fi.exists() && fi.isDir() && fi.isWritable())
        return true;

    return false;
}

bool Utility::isLocalPath(const QString &urlStr)
{
    QUrl url(urlStr);

    if (url.isLocalFile())
        return true;

    if (QFile::exists(urlStr))
        return true;

    return false;
}

QString Utility::localPath(const QString &urlStr)
{
    QString path;

    QUrl url(urlStr);
    if (url.isLocalFile())
    {
        path = url.toLocalFile();
    }
    else if (QFile::exists(urlStr))
    {
        path = urlStr;
    }

    return path;
}

Utility::FolderThumbnailName Utility::folderThumbnailName()
{
    FolderThumbnailName name = StandardFolderThumbnailName;

    QSettings settings;
    if (settings.contains(Utility::FolderThumbnailNameKey))
    {
        name = static_cast<FolderThumbnailName>(settings.value(Utility::FolderThumbnailNameKey).toInt());
    }
    else
    {
        settings.setValue(Utility::FolderThumbnailNameKey, static_cast<int>(name));
    }

    return name;
}

void Utility::setFolderThumbnailName(FolderThumbnailName name)
{
    QSettings settings;
    settings.setValue(Utility::FolderThumbnailNameKey, static_cast<int>(name));
}
