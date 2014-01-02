#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include <QList>
#include <QPair>

#include "utility.h"

class DownloadManager;
class QModelIndex;
class QItemSelection;
class MediaLibrary;
class ThumbnailManager;
class CategoryNode;
class ThumbnailFile;
class VideoFile;
class RemoteFile;
class Video;
class VideoListModel;
class QProgressBar;
class QPoint;
class QAbstractItemView;
class SortFilterProxyModel;
class Inspector;
class QStackedWidget;
class QTreeView;
class QListView;
class BreadcrumbBar;
class MetaData;
class QProgressDialog;
class FlowWidget;
class QAction;
class QLineEdit;
class LibraryListModel;
class Library;
class CopyManager;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
	enum View
	{
		SmallIconView,
		LargeIconView,
        FlowView,
		DetailView
	};

    enum StackedIndex
    {
        ListIndex   = 0,
        DetailIndex = 1,
        FlowIndex   = 2
    };

    enum DownloadActionState
    {
        DownloadState,
        PauseState,
        ResumeState
    };

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void exit();
	void about();
	void preferences();
	void open();
	void openRecentFile();
    void downloadAll();
	void exportFiles();
	void playVideo();
    void downloadVideo();
	void setDetailView();
	void setSmallIconView();
    void setLargeIconView();
    void setFlowView();
	void setStatusMessage(const QString& message);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
	void fileDownloaded(RemoteFile *file);
	void videoDownloaded(RemoteFile *file);
	void videoSelectionChanged(const QItemSelection& current, const QItemSelection& previous);
    void treeSelectionChanged(const QItemSelection& current, const QItemSelection& previous);
	void videoActivated(const QModelIndex &index);
	void allDownloadsProgress(qint64 bytesReceived, qint64 bytesTotal);
    void allDownloadsFinished();
    void videoContextMenuRequested(const QPoint &point);
    void categoryContextMenuRequested(const QPoint &point);
    void contextMenuAboutToShow();
	void fileMenuAboutToShow();
	void categoryContextMenuAboutToShow();
	void metaDataFinished(VideoFile *file);
    void updateActionStates();
	void copyingFinished();

private:
	void downloadCategoryThumbnails(CategoryNode *node);
	void downloadVideoThumbnails();
	void queryVideoMetadata();
	void requestVideoFileSizes();
	void setCurrentView(View view);
	Video * currentVideo();
    CategoryNode * currentCategory();
    bool exportFiles(CategoryNode *node, const QString &destDir, QList<QString> &dirList, QList<QPair<QString, QString> > &fileList, Utility::FolderThumbnailName name);
    void readSettings();
    void writeSettings();
    void updateRecentFileActions();
    void addToRecentFiles(Library *library);
    void openFile(const QString &filename);
	void downloadCategory(CategoryNode *node, QSet<VideoFile*> &videoFiles);
	void pauseCategory(CategoryNode *node, QSet<VideoFile*> &videoFiles);
    void setDownloadAction(DownloadActionState state, bool enabled);
	bool okToExit();
	bool okToCloseLibrary();

private:
    Ui::MainWindow *ui;
	View _currentView;
    MediaLibrary *_library;
	VideoListModel *_videoListModel;
	DownloadManager *_downloadManager;
	ThumbnailManager *_thumbnailManager;
	QMenu *_videoContextMenu;
	QMenu *_categoryContextMenu;
	QProgressBar *_progressBar;
    SortFilterProxyModel *_videoListProxyModel;
    Inspector *_inspector;
    QStackedWidget *_stackedWidget;
    QListView *_listView;
    QTreeView *_treeView, *_detailView;
	BreadcrumbBar *_breadcrumbBar;
	MetaData *_metaData;
    FlowWidget *_flowWidget;
    QLineEdit *_searchLineEdit;
    QPixmap _downloadPixmap, _pausePixmap;
	RemoteFile *_libraryFile;
    LibraryListModel *_libraryListModel;
    CopyManager *_copyManager;

    enum { MaxRecentFiles = 10 };
    QAction *_recentFileActions[MaxRecentFiles];
};

#endif // MAINWINDOW_H
