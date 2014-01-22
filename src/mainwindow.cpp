#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "downloadmanager.h"
#include "categorynode.h"
#include "video.h"
#include "medialibrary.h"
#include "thumbnailmanager.h"
#include "videolistmodel.h"
#include "inspector.h"
#include "metadata.h"
#include "copydialog.h"
#include "sortfilterproxymodel.h"
#include "utility.h"
#include "flowwidget.h"
#include "aboutdialog.h"
#include "preferencesdialog.h"
#include "videofile.h"
#include "thumbnailfile.h"
#include "opendialog.h"
#include "library.h"
#include "librarylistmodel.h"
#include "copymanager.h"

#include <QContextMenuEvent>
#include <QDesktopServices>
#include <QSettings>
#include <QDir>
#include <QProgressBar>
#include <QStackedWidget>
#include <QListView>
#include <QTreeView>
#include <QDockWidget>
#include <QFileDialog>
#include <QSet>
#include <QHBoxLayout>

#ifdef Q_OS_MAC
#define SHOW_FLOW 1
#else
#define SHOW_FLOW 1
#endif

MainWindow::MainWindow(QWidget *parent) 
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	_currentView = SmallIconView;

    QCoreApplication::setOrganizationName("Charles Glancy");
    QCoreApplication::setOrganizationDomain("glancyfamily.net");
    QCoreApplication::setApplicationName(Utility::applicationName());

    // Load an application style
    //QFile styleFile(":/pvm/qss/style.qss");
    //styleFile.open(QFile::ReadOnly);

    // Apply the loaded stylesheet
    //QString style(styleFile.readAll());
    //styleFile.close();
    //qApp->setStyleSheet(style);

#if SHOW_FLOW
    _flowWidget = new FlowWidget(this);
    _flowWidget->setStyle(FlowWidget::Flow);
    _flowWidget->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->actionFlowView->setVisible(true);
#else
    ui->actionFlowView->setVisible(false);
#endif

    _listView = new QListView();
    _listView->setViewMode(QListView::IconMode);
    _listView->setWordWrap(true);
	_listView->setResizeMode(QListView::Adjust);
    _listView->setSelectionMode(QAbstractItemView::SingleSelection);
    _listView->setSelectionBehavior(QAbstractItemView::SelectItems);
    _listView->setAttribute(Qt::WA_MacShowFocusRect, 0);

    _detailView = new QTreeView();
    _detailView->setRootIsDecorated(false);
    _detailView->setSelectionMode(QAbstractItemView::SingleSelection);
    _detailView->setSelectionBehavior(QAbstractItemView::SelectRows);
    _detailView->setAttribute(Qt::WA_MacShowFocusRect, 0);

    _stackedWidget = new QStackedWidget();
    _stackedWidget->addWidget(_listView);
    _stackedWidget->addWidget(_detailView);
#if SHOW_FLOW
    _stackedWidget->addWidget(_flowWidget);
#endif
    _stackedWidget->setCurrentIndex(DetailView);
    setCentralWidget(_stackedWidget);

    _downloadPixmap = QPixmap(":/pvm/images/download_button.png");
    _pausePixmap = QPixmap(":/pvm/images/pause_button.png");
	
    ui->actionOpen->setIcon(QPixmap(":/pvm/images/open_button.png"));
    ui->actionOpen->setIconText(tr("Open Library"));
    ui->actionDownloadVideo->setIcon(_downloadPixmap);
    ui->actionDownloadVideo->setIconText(tr("Download"));
    ui->actionExportFiles->setIcon(QPixmap(":/pvm/images/usb_button.png"));
    ui->actionExportFiles->setIconText(tr("Export to USB"));
	ui->actionPlayVideo->setIcon(QPixmap(":/pvm/images/play_button.png"));
	ui->actionPlayVideo->setIconText(tr("Play Video"));

	ui->actionDetailView->setIcon(QPixmap(":/pvm/images/detail_view.png"));
	ui->actionSmallIconView->setIcon(QPixmap(":/pvm/images/small_icons.png"));
	ui->actionLargeIconView->setIcon(QPixmap(":/pvm/images/large_icons.png"));
	ui->actionFlowView->setIcon(QPixmap(":/pvm/images/flow_view.png"));

    _searchLineEdit = new QLineEdit(this);
    _searchLineEdit->setMaximumWidth(180);
    _searchLineEdit->setPlaceholderText(tr("Search"));

    QWidget* expandingSpacer = new QWidget();
    expandingSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QWidget* fixedSpacer = new QWidget();
    fixedSpacer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    fixedSpacer->setFixedWidth(10);

    QToolBar *mainToolBar = new QToolBar(this);
    mainToolBar->setMovable(false);
    mainToolBar->setIconSize(QSize(32,32));
    mainToolBar->addAction(ui->actionOpen);
    mainToolBar->addAction(ui->actionDownloadVideo);
    mainToolBar->addAction(ui->actionExportFiles);
    mainToolBar->addAction(ui->actionPlayVideo);
	mainToolBar->addSeparator();
	mainToolBar->addAction(ui->actionDetailView);
	mainToolBar->addAction(ui->actionSmallIconView);
	mainToolBar->addAction(ui->actionLargeIconView);
#if SHOW_FLOW
	mainToolBar->addAction(ui->actionFlowView);
#endif
    mainToolBar->addWidget(expandingSpacer);
    mainToolBar->addWidget(_searchLineEdit);
    mainToolBar->addWidget(fixedSpacer);
    addToolBar(Qt::TopToolBarArea, mainToolBar);

    // adjust margin for mac focus rect
    mainToolBar->layout()->setContentsMargins(0, 0, 0, 4);

    _inspector = new Inspector(this);
    QDockWidget *bottomDock = new QDockWidget(this);
    bottomDock->setObjectName("InspectorPanel");
    bottomDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    bottomDock->setTitleBarWidget(new QWidget());
    bottomDock->setWidget(_inspector);
    addDockWidget(Qt::BottomDockWidgetArea, bottomDock);

    _treeView = new QTreeView();
    _treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    _treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    _treeView->setAttribute(Qt::WA_MacShowFocusRect, 0);

    QDockWidget *leftDock = new QDockWidget(this);
    leftDock->setObjectName("CategoryTree");
    leftDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    leftDock->setTitleBarWidget(new QWidget());
    leftDock->setWidget(_treeView);
    addDockWidget(Qt::LeftDockWidgetArea, leftDock);

	_progressBar = new QProgressBar(this);
	_progressBar->setMaximumWidth(200);
	_progressBar->setMaximumHeight(20);
	statusBar()->addPermanentWidget(_progressBar);
	_progressBar->setVisible(false);

    _library = new MediaLibrary(this);
	_videoListModel = new VideoListModel(this);

	connect(_library, SIGNAL(videoChanged(Video*)), _videoListModel, SLOT(videoChanged(Video*)));
    connect(_library, SIGNAL(modelReset()), _videoListModel, SLOT(libraryModelReset()));
    connect(_library, SIGNAL(videoChanged(Video*)), _inspector, SLOT(videoChanged(Video*)));
    connect(_library, SIGNAL(categoryChanged(CategoryNode*)), _inspector, SLOT(categoryChanged(CategoryNode*)));

	_treeView->setModel(_library);
    _treeView->expandAll();

	_videoListProxyModel = new SortFilterProxyModel(this);
	_videoListProxyModel->setSourceModel(_videoListModel);
	_videoListProxyModel->setDynamicSortFilter(true);
    _videoListProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

	_listView->setModel(_videoListProxyModel);
	_detailView->setModel(_videoListProxyModel);
	_detailView->setSortingEnabled(true);
    _detailView->setColumnWidth(VideoListModel::OrdinalColumn, 50);
    _detailView->setColumnWidth(VideoListModel::NameColumn, 200);
	_detailView->setColumnHidden(VideoListModel::SmallIconColumn, true);
    _detailView->setColumnHidden(VideoListModel::MediumIconColumn, true);
	_detailView->setColumnHidden(VideoListModel::LargeIconColumn, true);

#if SHOW_FLOW
    _flowWidget->setModelColumn(VideoListModel::LargeIconColumn);
    _flowWidget->setModel(_videoListProxyModel);
    connect(_flowWidget, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(videoSelectionChanged(const QItemSelection&, const QItemSelection&)));
    connect(_flowWidget, SIGNAL(activated(const QModelIndex&)), this, SLOT(videoActivated(const QModelIndex&)));
#endif

	setCurrentView(DetailView);

    connect(_treeView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(treeSelectionChanged(const QItemSelection&, const QItemSelection&)));
    connect(_listView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(videoSelectionChanged(const QItemSelection&, const QItemSelection&)));
    connect(_detailView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(videoSelectionChanged(const QItemSelection&, const QItemSelection&)));

	connect(_listView, SIGNAL(activated(const QModelIndex&)), this, SLOT(videoActivated(const QModelIndex&)));
	connect(_detailView, SIGNAL(activated(const QModelIndex&)), this, SLOT(videoActivated(const QModelIndex&)));

	_downloadManager = new DownloadManager(this);
	connect(_downloadManager, SIGNAL(fileDownloaded(RemoteFile*)), this, SLOT(fileDownloaded(RemoteFile*)));
    connect(_downloadManager, SIGNAL(allDownloadsProgress(qint64,qint64)), this, SLOT(allDownloadsProgress(qint64,qint64)));
    connect(_downloadManager, SIGNAL(allDownloadsFinished()), this, SLOT(allDownloadsFinished()));
	connect(_downloadManager, SIGNAL(downloadStatus(const QString&)), SLOT(setStatusMessage(const QString&)));
	connect(_downloadManager, SIGNAL(videoDownloaded(RemoteFile*)), SLOT(videoDownloaded(RemoteFile*)));

	_metaData = new MetaData(this);
	_thumbnailManager = new ThumbnailManager(this);
    _copyManager = new CopyManager(this);
	connect(_copyManager, SIGNAL(copyingFinished()), this, SLOT(copyingFinished()));

	connect(_metaData, SIGNAL(metaDataFinished(VideoFile*)), this, SLOT(metaDataFinished(VideoFile*)));

    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(exit()));
	connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
	connect(ui->actionPreferences, SIGNAL(triggered()), this, SLOT(preferences()));
	connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(open()));
    connect(ui->actionDownloadAll, SIGNAL(triggered()), this, SLOT(downloadAll()));
	connect(ui->actionExportFiles, SIGNAL(triggered()), this, SLOT(exportFiles()));
	connect(ui->actionPlayVideo, SIGNAL(triggered()), this, SLOT(playVideo()));
    connect(ui->actionDownloadVideo, SIGNAL(triggered()), this, SLOT(downloadVideo()));

	connect(ui->actionSmallIconView, SIGNAL(triggered()), this, SLOT(setSmallIconView()));
    connect(ui->actionLargeIconView, SIGNAL(triggered()), this, SLOT(setLargeIconView()));
	connect(ui->actionDetailView, SIGNAL(triggered()), this, SLOT(setDetailView()));
    connect(ui->actionFlowView, SIGNAL(triggered()), this, SLOT(setFlowView()));

    _listView->setContextMenuPolicy(Qt::CustomContextMenu);
    _detailView->setContextMenuPolicy(Qt::CustomContextMenu);
	_treeView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(_listView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(videoContextMenuRequested(const QPoint&)));
    connect(_detailView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(videoContextMenuRequested(const QPoint&)));
	connect(_treeView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(categoryContextMenuRequested(const QPoint&)));

    _videoContextMenu = new QMenu(this);
    _videoContextMenu->addAction(ui->actionPlayVideo);
    _videoContextMenu->addAction(ui->actionDownloadVideo);

	_categoryContextMenu = new QMenu(this);
	_categoryContextMenu->addAction(ui->actionDownloadVideo);

    connect(_videoContextMenu, SIGNAL(aboutToShow()), this, SLOT(contextMenuAboutToShow()));
	connect(_categoryContextMenu, SIGNAL(aboutToShow()), this, SLOT(categoryContextMenuAboutToShow()));
	connect(ui->menu_File, SIGNAL(aboutToShow()), this, SLOT(fileMenuAboutToShow()));

    connect(_searchLineEdit, SIGNAL(textChanged(const QString &)), _videoListProxyModel, SLOT(setFilterFixedString(const QString &)));

    _libraryFile = 0;
    _libraryListModel = new LibraryListModel(this, _downloadManager);
    _libraryListModel->setSourceUrl(Utility::currentLibrarySourceUrl());

    QMenu *recentFilesMenu = new QMenu();
    ui->actionRecentFiles->setMenu(recentFilesMenu);

    for (int i = 0; i < MaxRecentFiles; i++)
    {
        _recentFileActions[i] = new QAction(this);
        _recentFileActions[i]->setVisible(false);
        connect(_recentFileActions[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
        recentFilesMenu->addAction(_recentFileActions[i]);
    }

    updateRecentFileActions();
    updateActionStates();
    readSettings();

	QTimer::singleShot(0, Qt::CoarseTimer, this, SLOT(open()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::okToExit()
{
	if (_copyManager->isCopying())
	{
		QMessageBox box(this);
		box.setIcon(QMessageBox::Question);
		box.setModal(true);
		box.setWindowTitle(Utility::applicationName());
		box.setText(tr("Files are still being copied."));
#ifdef Q_OS_MAC
		box.setInformativeText(tr("Are you sure you want to quit?"));
		QPushButton *exitButton = box.addButton(tr("Quit"), QMessageBox::AcceptRole);
#else
		box.setInformativeText(tr("Are you sure you want to exit?"));
		QPushButton *exitButton = box.addButton(tr("Exit"), QMessageBox::AcceptRole);
#endif
		box.addButton(QMessageBox::Cancel);
		box.setDefaultButton(QMessageBox::Cancel);

		box.exec();

		if (box.clickedButton() != exitButton)
			return false;

		_copyManager->cancel();
	}

	if (_downloadManager->isDownloadingVideos())
	{
		QMessageBox box(this);
		box.setIcon(QMessageBox::Question);
		box.setModal(true);
		box.setWindowTitle(Utility::applicationName());
		box.setText(tr("Files are still being downloaded."));
#ifdef Q_OS_MAC
		box.setInformativeText(tr("Are you sure you want to quit?"));
		QPushButton *exitButton = box.addButton(tr("Quit"), QMessageBox::AcceptRole);
#else
		box.setInformativeText(tr("Are you sure you want to exit?"));
		QPushButton *exitButton = box.addButton(tr("Exit"), QMessageBox::AcceptRole);
#endif
		box.addButton(QMessageBox::Cancel);
		box.setDefaultButton(QMessageBox::Cancel);

		box.exec();

		if (box.clickedButton() != exitButton)
			return false;

		_downloadManager->cancelAllVideoDownloads();
	}

	return true;
}

void MainWindow::exit()
{
	if (okToExit())
		QApplication::exit();
}

void MainWindow::about()
{
	AboutDialog dialog(this);
	dialog.exec();
}

void MainWindow::preferences()
{
	PreferencesDialog dialog(this);
	if (dialog.exec() == QDialog::Accepted)
	{
		QSettings settings;
		settings.setValue(Utility::VideoDirectoryKey, dialog.videoDirectory());
        settings.setValue(Utility::LibrarySourceUrlKey, dialog.librarySourceUrl());
        Utility::setFolderThumbnailName(dialog.folderThumbnailName());
	}
}

void MainWindow::open()
{
	if (!okToCloseLibrary())
		return;

	QSettings settings;
    QString librarySourceUrl = settings.value(Utility::LibrarySourceUrlKey).toString();

    if (_libraryListModel->sourceUrl() != librarySourceUrl)
        _libraryListModel->setSourceUrl(librarySourceUrl);

	OpenDialog dialog(this, _libraryListModel);
	if (dialog.exec() == QDialog::Accepted)
	{
		Library *library = dialog.selectedLibrary();

		if (library)
		{
			_libraryFile = library;
			_downloadManager->downloadFile(_libraryFile);
		}
	}
}

void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (!action)
        return;

    QString fileName = action->data().toString();
    QString libraryName = action->text();

    if (!fileName.isEmpty() && QFile::exists(fileName))
    {
        openFile(fileName, libraryName);
    }
}

void MainWindow::openFile(const QString &fileName, const QString &libraryName)
{
    _inspector->clear();

    _library->initLibrary(fileName);

    _treeView->expandAll();
    _videoListModel->filterByCategory(_library->rootNode());
    _detailView->sortByColumn(VideoListModel::NameColumn, Qt::AscendingOrder);

    updateWindowTitle(libraryName);
    updateActionStates();
    downloadCategoryThumbnails(_library->rootNode());
    downloadVideoThumbnails();
    requestVideoFileSizes();
	queryVideoMetadata();
}

void MainWindow::updateWindowTitle(const QString &libraryName)
{
    setWindowTitle(libraryName + " - " + Utility::applicationName());
}

bool MainWindow::okToCloseLibrary()
{
	if (_downloadManager->isDownloadingVideos())
	{
		QMessageBox box(this);
		box.setIcon(QMessageBox::Question);
		box.setModal(true);
		box.setWindowTitle(Utility::applicationName());
		box.setText(tr("Files are still being downloaded."));
		box.setInformativeText(tr("Are you sure you want to open a new library?"));
		QPushButton *openButton = box.addButton(tr("Open"), QMessageBox::AcceptRole);
		box.addButton(QMessageBox::Cancel);
		box.setDefaultButton(QMessageBox::Cancel);

		box.exec();

		if (box.clickedButton() != openButton)
			return false;

		_downloadManager->cancelAllVideoDownloads();
	}

	return true;
}

void MainWindow::fileMenuAboutToShow()
{
    updateActionStates();
}

void MainWindow::downloadAll()
{
	if (ui->actionDownloadAll->data().toBool())
	{
		QList<Video*> videos = _library->videos();

		for (int i = 0; i < videos.count(); i++)
			if (videos[i]->videoFile()->state() == RemoteFile::NotDownloadedState || 
				videos[i]->videoFile()->state() == RemoteFile::DownloadPausedState)
				_downloadManager->downloadVideo(videos[i]->videoFile());
	}
	else
	{
		_downloadManager->cancelAllVideoDownloads();
	}
}

void MainWindow::exportFiles()
{
    if (!_library->rootNode())
        return;

	CategoryNode *rootNode = _library->rootNode();
    CategoryNodeStats stats = rootNode->stats();

	if (stats.downloadedVideoCount != stats.totalVideoCount)
	{
		QMessageBox box(this);
		box.setIcon(QMessageBox::Question);
		box.setModal(true);
		box.setWindowTitle(Utility::applicationName());
		box.setText(tr("%1 of %2 files have been downloaded.").arg(stats.downloadedVideoCount).arg(stats.totalVideoCount));
		box.setInformativeText(tr("Are you sure you want to export?"));
		QPushButton *exportButton = box.addButton(tr("Export"), QMessageBox::AcceptRole);
		box.addButton(QMessageBox::Cancel);
		box.setDefaultButton(QMessageBox::Cancel);

		box.exec();

		if (box.clickedButton() != exportButton)
			return;
	}

	CopyDialog dialog(this, stats.downloadedVideoSize + stats.downloadedThumbnailSize);
	if (dialog.exec() == QDialog::Accepted)
	{
		QString destDir = dialog.destinationDirectory();
        QList<QString> dirList;
        QList<QPair<QString, QString> > fileList;
        Utility::FolderThumbnailName thumbName = Utility::folderThumbnailName();

		for (int i = 0; i < rootNode->childCount(); i++)
        {
            if (!exportFiles(rootNode->child(i), destDir, dirList, fileList, thumbName))
                return; // abort
        }

        _copyManager->startCopying(dirList, fileList);
        updateActionStates();
	}
}

bool MainWindow::exportFiles(CategoryNode *node, const QString &destDir, QList<QString> &dirList, QList<QPair<QString, QString> > &fileList, Utility::FolderThumbnailName thumbName)
{
	if (!node)
		return false;

    QString dirPath = node->path(destDir);
    dirList.append(dirPath);

    if (node->thumbnailFile()->state() == RemoteFile::DownloadedState)
    {
        QString thumbnailPath;

        if (thumbName == Utility::PathPlusExtensionThumbnailName)
            thumbnailPath = dirPath + ".jpg";
        else
            thumbnailPath = dirPath + "/folder.jpg";

        fileList.append(QPair<QString, QString>(node->thumbnailFile()->localPath(), thumbnailPath));
    }

	for (int i = 0; i < node->videoCount(); i++)
	{
		Video *video = node->video(i);
		VideoFile *videoFile = video->videoFile();
		if (videoFile->state() == RemoteFile::DownloadedState)
		{
			QFileInfo fi(videoFile->localPath());
			QString videoPath = dirPath + "/" + Utility::validFileSystemString(video->name()) + "." + fi.suffix();
            fileList.append(QPair<QString, QString>(videoFile->localPath(), videoPath));

            if (video->thumbnailFile()->state() == RemoteFile::DownloadedState)
            {
                QString thumbnailPath = dirPath + "/" + Utility::validFileSystemString(video->name()) + ".jpg";
                fileList.append(QPair<QString, QString>(video->thumbnailFile()->localPath(), thumbnailPath));
            }
		}
	}

	for (int i = 0; i < node->childCount(); i++)
    {
        if (!exportFiles(node->child(i), destDir, dirList, fileList, thumbName))
            return false;
    }

    return true;
}

void MainWindow::copyingFinished()
{
	updateActionStates();
}

Video * MainWindow::currentVideo()
{
    QModelIndex index;

    switch (_stackedWidget->currentIndex())
    {
    case ListIndex:
        index = _listView->currentIndex();
        break;
    case DetailIndex:
        index = _detailView->currentIndex();
        break;
#if SHOW_FLOW
    case FlowIndex:
        index = _flowWidget->currentIndex();
        break;
#endif
    }

    return _videoListModel->video(_videoListProxyModel->mapToSource(index));
}

void MainWindow::downloadVideo()
{
    Video *video = currentVideo();
    CategoryNode *category = currentCategory();

	if (video && video->videoFile()->state() == RemoteFile::DownloadingState)
	{
		_downloadManager->cancelVideoDownload(video->videoFile());
	}
    else if (video && video->videoFile()->state() != RemoteFile::DownloadedState)
    {
        _downloadManager->downloadVideo(video->videoFile());
	}
    else if (category)
    {
		if (ui->actionDownloadVideo->data() == DownloadState || ui->actionDownloadVideo->data() == ResumeState)
		{
			QSet<VideoFile*> videoFiles;
			downloadCategory(category, videoFiles);

			foreach (VideoFile *file, videoFiles)
				_downloadManager->downloadVideo(file);
		}
		else if (ui->actionDownloadVideo->data() == PauseState)
		{
			QSet<VideoFile*> videoFiles;
			pauseCategory(category, videoFiles);

			foreach (VideoFile *file, videoFiles)
				_downloadManager->cancelVideoDownload(file);
		}
    }

    updateActionStates();
}

void MainWindow::playVideo()
{
    Video *video = currentVideo();

	if (video)
	{
		VideoFile *file = video->videoFile();
		if (QFile::exists(file->localPath()))
		{
			QString fileUrl = "file:///" + file->localPath();
			QDesktopServices::openUrl(fileUrl);
		}
	}
}

void MainWindow::categoryContextMenuRequested(const QPoint &pos)
{
	QPoint globalPos = _treeView->viewport()->mapToGlobal(pos);
	_categoryContextMenu->popup(globalPos);
}

void MainWindow::videoContextMenuRequested(const QPoint &pos)
{
    QPoint globalPos;

	switch (_stackedWidget->currentIndex())
	{
	case ListIndex:
		globalPos = _listView->viewport()->mapToGlobal(pos);
		break;
	case DetailIndex:
		globalPos = _detailView->viewport()->mapToGlobal(pos);
		break;
#if SHOW_FLOW
	case FlowIndex:
        globalPos = _flowWidget->mapToGlobal(pos);
		break;
#endif
	}

    _videoContextMenu->popup(globalPos);
}

void MainWindow::contextMenuAboutToShow()
{
    updateActionStates();
}

void MainWindow::downloadCategoryThumbnails(CategoryNode *node)
{
	if (!node)
		return;

	ThumbnailFile *file = node->thumbnailFile();

	if (file->state() == RemoteFile::DownloadedState)
		_thumbnailManager->createThumbnail(file);
	else if (file->state() == RemoteFile::NotDownloadedState)
		_downloadManager->downloadFile(file);

	for (int i = 0; i < node->childCount(); i++)
		downloadCategoryThumbnails(node->child(i));
}

void MainWindow::downloadVideoThumbnails()
{
	QList<Video*> videos = _library->videos();
	for (int i = 0; i < videos.count(); i++)
	{
		ThumbnailFile *file = videos.at(i)->thumbnailFile();

		if (file->state() == RemoteFile::DownloadedState)
			_thumbnailManager->createThumbnail(file);
		else if (file->state() == RemoteFile::NotDownloadedState)
			_downloadManager->downloadFile(file);
	}
}

void MainWindow::requestVideoFileSizes()
{
	QList<Video*> videos = _library->videos();
	for (int i = 0; i < videos.count(); i++)
	{
		Video *video = videos[i];
        if (video->videoFile()->state() != RemoteFile::DownloadedState &&
            video->videoFile()->state() != RemoteFile::DownloadErrorState)
		    _downloadManager->requestVideoSize(video->videoFile());
	}
}

void MainWindow::queryVideoMetadata()
{
	QList<Video*> videos = _library->videos();
	for (int i = 0; i < videos.count(); i++)
	{
		Video *video = videos[i];
		if (video->videoFile()->state() == RemoteFile::DownloadedState)
			_metaData->queryMetaData(video->videoFile());
	}
}

void MainWindow::fileDownloaded(RemoteFile *file)
{
	ThumbnailFile *thumbnailFile = dynamic_cast<ThumbnailFile*>(file);
    Library *libraryFile = dynamic_cast<Library*>(file);

	if (thumbnailFile)
	{
        if (thumbnailFile->state() == RemoteFile::DownloadedState)
		    _thumbnailManager->createThumbnail(thumbnailFile);
	}
	else if (libraryFile && libraryFile == _libraryFile)
	{
		if (libraryFile->state() == RemoteFile::DownloadedState)
		{
            openFile(libraryFile->localPath(), libraryFile->name());
            addToRecentFiles(libraryFile);
		}
		else
		{
			QMessageBox::critical(this, Utility::applicationName(), tr("Unable to download %1").arg(libraryFile->url()), QMessageBox::Ok);
		}
	}
}

void MainWindow::videoSelectionChanged(const QItemSelection &current, const QItemSelection &previous)
{
	Q_UNUSED(current);
	Q_UNUSED(previous);

	Video *video = currentVideo();
	if (video)
	{
		//if (!video->videoFile()->hasMetaData())
		//	_metaData->queryMetaData(video->videoFile());
        _inspector->setVideo(video);
	}
    else
    {
        _inspector->clear();
    }
    
    updateActionStates();
}

void MainWindow::treeSelectionChanged(const QItemSelection &current, const QItemSelection &previous)
{
	Q_UNUSED(current);
	Q_UNUSED(previous);

	CategoryNode *node = _library->categoryNode(_treeView->currentIndex());
	if (node)
	{
		_videoListModel->filterByCategory(node);
        _inspector->setCategory(node);

        if (node->videoCount() > 0)
            _detailView->sortByColumn(VideoListModel::OrdinalColumn, Qt::AscendingOrder);
        else
            _detailView->sortByColumn(VideoListModel::NameColumn, Qt::AscendingOrder);
    }
    else
    {
        _inspector->clear();
    }

    updateActionStates();
}

void MainWindow::videoActivated(const QModelIndex &index)
{
	Q_UNUSED(index);
	playVideo();
}

void MainWindow::setDetailView()
{
	setCurrentView(MainWindow::DetailView);
}

void MainWindow::setSmallIconView()
{
	setCurrentView(MainWindow::SmallIconView);
}

void MainWindow::setLargeIconView()
{
	setCurrentView(MainWindow::LargeIconView);
}

void MainWindow::setFlowView()
{
    setCurrentView(MainWindow::FlowView);
}

void MainWindow::setCurrentView(MainWindow::View view)
{
	switch (view)
	{
	case SmallIconView:
		ui->actionSmallIconView->setChecked(true);
        ui->actionLargeIconView->setChecked(false);
		ui->actionDetailView->setChecked(false);
        ui->actionFlowView->setChecked(false);
		_stackedWidget->setCurrentIndex(ListIndex);
		_listView->setModelColumn(VideoListModel::SmallIconColumn);
        // Fix for Qt 5.2 scroll wheel bug
        _listView->verticalScrollBar()->setSingleStep(32);
		break;
	case LargeIconView:
		ui->actionSmallIconView->setChecked(false);
		ui->actionLargeIconView->setChecked(true);
		ui->actionDetailView->setChecked(false);
        ui->actionFlowView->setChecked(false);
		_stackedWidget->setCurrentIndex(ListIndex);
		_listView->setModelColumn(VideoListModel::MediumIconColumn);
        // Fix for Qt 5.2 scroll wheel bug
        _listView->verticalScrollBar()->setSingleStep(45);
        break;
	case DetailView:
		ui->actionSmallIconView->setChecked(false);
		ui->actionLargeIconView->setChecked(false);
		ui->actionDetailView->setChecked(true);
        ui->actionFlowView->setChecked(false);
		_stackedWidget->setCurrentIndex(DetailIndex);
        break;
    case FlowView:
        ui->actionSmallIconView->setChecked(false);
        ui->actionLargeIconView->setChecked(false);
        ui->actionDetailView->setChecked(false);
        ui->actionFlowView->setChecked(true);
        _stackedWidget->setCurrentIndex(FlowIndex);
		break;
	}

    _inspector->clear();
}

void MainWindow::allDownloadsProgress(qint64 bytesReceived, qint64 bytesTotal)
{
	_progressBar->setVisible(true);
    _progressBar->setMaximum(bytesTotal / 1024 / 1024);
    _progressBar->setValue(bytesReceived / 1024 / 1024);
}

void MainWindow::allDownloadsFinished()
{
    _progressBar->reset();
	_progressBar->setVisible(false);

    updateActionStates();
}

void MainWindow::setStatusMessage(const QString& message)
{
	statusBar()->showMessage(message);
}

void MainWindow::videoDownloaded(RemoteFile *file)
{
	VideoFile *videoFile = dynamic_cast<VideoFile*>(file);
	if (videoFile && videoFile->state() == RemoteFile::DownloadedState)
	{
		_metaData->queryMetaData(videoFile);
	}

    updateActionStates();
}

void MainWindow::metaDataFinished(VideoFile *file)
{
	Q_UNUSED(file);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	if (okToExit())
	{
		writeSettings();
		event->accept();
	}
	else
	{
		event->ignore();
	}
}

void MainWindow::readSettings()
{
    QSettings settings;
    restoreGeometry(settings.value(Utility::GeometryKey).toByteArray());
    restoreState(settings.value(Utility::WindowStateKey).toByteArray());
}

void MainWindow::writeSettings()
{
    QSettings settings;
    settings.setValue(Utility::GeometryKey, saveGeometry());
    settings.setValue(Utility::WindowStateKey, saveState());
}

void MainWindow::updateRecentFileActions()
{
    QSettings settings;
    QStringList libraryList = settings.value(Utility::RecentLibraryListKey).toStringList();
    int count = qMin(libraryList.count(), (int)MaxRecentFiles);
    for (int i = 0; i < count; i++)
    {
        QStringList libraryIdParts = libraryList[i].split(";");
        if (libraryIdParts.count() == 2)
        {
            _recentFileActions[i]->setText(libraryIdParts[0]);
            _recentFileActions[i]->setData(libraryIdParts[1]);
            _recentFileActions[i]->setVisible(true);
        }
        else
        {
            _recentFileActions[i]->setVisible(false);
        }
    }

    for (int i = count; i < MaxRecentFiles; i++)
    {
        _recentFileActions[i]->setVisible(false);
    }
}

void MainWindow::addToRecentFiles(Library *library)
{
    if (!library)
        return;

    QString libraryId = library->name() + ";" + library->localPath();

    QSettings settings;
    QStringList libraryList = settings.value(Utility::RecentLibraryListKey).toStringList();

    libraryList.removeAll(libraryId);
    libraryList.prepend(libraryId);
    while (libraryList.size() > MaxRecentFiles)
        libraryList.removeLast();

    settings.setValue(Utility::RecentLibraryListKey, libraryList);

    updateRecentFileActions();
}

void MainWindow::categoryContextMenuAboutToShow()
{
    updateActionStates();
}

CategoryNode * MainWindow::currentCategory()
{
    return _library->categoryNode(_treeView->currentIndex());
}

void MainWindow::downloadCategory(CategoryNode *node, QSet<VideoFile*> &videoFiles)
{
	for (int i = 0; i < node->videoCount(); i++)
	{
		Video *video = node->video(i);
		if (video->videoFile()->state() != RemoteFile::DownloadedState)
			videoFiles.insert(video->videoFile());
	}

	for (int i = 0; i < node->childCount(); i++)
		downloadCategory(node->child(i), videoFiles);
}

void MainWindow::pauseCategory(CategoryNode *node, QSet<VideoFile*> &videoFiles)
{
	for (int i = 0; i < node->videoCount(); i++)
	{
		Video *video = node->video(i);
		if (video->videoFile()->state() == RemoteFile::DownloadingState)
			videoFiles.insert(video->videoFile());
	}

	for (int i = 0; i < node->childCount(); i++)
		pauseCategory(node->child(i), videoFiles);
}

void MainWindow::updateActionStates()
{
    if (_library && _library->rootNode())
    {
        _searchLineEdit->setEnabled(true);
        ui->actionDownloadAll->setEnabled(true);

        if (_copyManager->isCopying())
            ui->actionExportFiles->setEnabled(false);
        else
            ui->actionExportFiles->setEnabled(true);

        if (_downloadManager->isDownloadingVideos())
        {
            ui->actionDownloadAll->setText(tr("Pause All Downloads"));
            ui->actionDownloadAll->setData(false);
        }
        else
        {
            ui->actionDownloadAll->setText(tr("Download All"));
            ui->actionDownloadAll->setData(true);
        }
    }
    else
    {
        ui->actionDownloadAll->setEnabled(false);
        ui->actionDownloadAll->setText(tr("Download All"));
        ui->actionExportFiles->setEnabled(false);
        _searchLineEdit->setEnabled(false);
    }

    Video *video = currentVideo();
    CategoryNode *category = currentCategory();

    if (video)
    {
        if (video->videoFile()->state() == RemoteFile::DownloadingState)
        {
            setDownloadAction(PauseState, true);
            ui->actionPlayVideo->setEnabled(false);
        }
        else if (video->videoFile()->state() == RemoteFile::DownloadPausedState)
        {
            setDownloadAction(ResumeState, true);
            ui->actionPlayVideo->setEnabled(false);
        }
        else if (video->videoFile()->state() == RemoteFile::DownloadedState)
        {
            setDownloadAction(DownloadState, false);
            ui->actionPlayVideo->setEnabled(true);
        }
        else
        {
            setDownloadAction(DownloadState, true);
            ui->actionPlayVideo->setEnabled(false);
        }
    }
    else if (category)
    {
        ui->actionPlayVideo->setEnabled(false);

        CategoryNodeStats stats = category->stats();

        if (stats.downloadingVideoCount > 0)
            setDownloadAction(PauseState, true);
        else if (stats.totalVideoCount > stats.downloadedVideoCount)
            setDownloadAction(DownloadState, true);
        else if (stats.pausedVideoCount > 0)
            setDownloadAction(ResumeState, true);
        else
            setDownloadAction(DownloadState, false);
    }
    else
    {
        setDownloadAction(DownloadState, false);
        ui->actionPlayVideo->setEnabled(false);
    }
}

void MainWindow::setDownloadAction(DownloadActionState state, bool enabled)
{
    switch (state)
    {
    case ResumeState:
        ui->actionDownloadVideo->setEnabled(enabled);
        ui->actionDownloadVideo->setIcon(_downloadPixmap);
        ui->actionDownloadVideo->setText(tr("Resume Download"));
        break;
    case PauseState:
        ui->actionDownloadVideo->setEnabled(enabled);
        ui->actionDownloadVideo->setIcon(_pausePixmap);
        ui->actionDownloadVideo->setText(tr("Pause Download"));
		ui->actionDownloadVideo->setData(state);
        break;
    case DownloadState:
    default:
        ui->actionDownloadVideo->setEnabled(enabled);
        ui->actionDownloadVideo->setIcon(_downloadPixmap);
        ui->actionDownloadVideo->setText(tr("Download"));
        break;
    }

	ui->actionDownloadVideo->setData(state);
}
