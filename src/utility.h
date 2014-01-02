#ifndef UTILITY_H
#define UTILITY_H

#include <QSize>
#include <QString>

class Utility
{
public:
	Utility();
	~Utility();

    static QString VideoDirectoryKey;
    static QString LibrarySourceUrlKey;
    static QString LastExportDirectoryKey;
    static QString RecentLibraryListKey;
    static QString GeometryKey;
    static QString WindowStateKey;
    static QString FolderThumbnailNameKey;

    enum FolderThumbnailName
    {
        StandardFolderThumbnailName = 0,
        PathPlusExtensionThumbnailName = 1
    };
    static FolderThumbnailName folderThumbnailName();
    static void setFolderThumbnailName(FolderThumbnailName name);

	static QString fileSizeString(qint64 size, bool kb = true);
	static QString resolutionString(const QSize &size);
	static QString durationString(qint64 milliseconds);
	static QString validFileSystemString(const QString &fileName);
    static QString currentVideoDirectory();
    static QString currentLibrarySourceUrl();
	static QString applicationName();
	static QString applicationVersionString();
	static QString fallbackLibrarySourceUrl();
    static QString removableDrive();
    static qint64 availableSpace(const QString &path);
    static bool isValidVideoDirectory(const QString &path);
    static bool isLocalPath(const QString &urlStr);
    static QString localPath(const QString &url);
};

#endif // UTILITY_H
