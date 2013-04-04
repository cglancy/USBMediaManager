#include "inspector.h"
#include "categorynode.h"
#include "video.h"
#include "utility.h"
#include "thumbnailfile.h"
#include "videofile.h"

#include <QPainter>
#include <QMediaMetaData>
#include <QUrl>
#include <QLocale>

Inspector::Inspector(QWidget *parent)
	: QFrame(parent), _currentVideo(0), _currentCategory(0)
{
    setBackgroundRole(QPalette::Base);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFocusPolicy(Qt::NoFocus);
    
#if 0
    _list.append(QPair<QString, QString>("Line 1:", "A first line"));
    _list.append(QPair<QString, QString>("Line 2:", "A second line"));
    _list.append(QPair<QString, QString>("A very long line 3:", "A third line"));
    _list.append(QPair<QString, QString>("Line 4:", "A fourth line"));
    _list.append(QPair<QString, QString>("Line 5:", "A fifth line"));
    _list.append(QPair<QString, QString>("Line 6:", "A sixth line. Which may be long also."));
#endif
}

Inspector::~Inspector()
{

}

void Inspector::setVideo(Video *video)
{
	if (!video)
		return;

	_currentVideo = video;
	_currentCategory = 0;
	updateInternal();
}

void Inspector::setCategory(CategoryNode *category)
{
	if (!category)
		return;

	_currentCategory = category;
	_currentVideo = 0;
	updateInternal();
}

void Inspector::updateInternal()
{
	clear();

	if (_currentCategory)
	{
		setPixmap(_currentCategory->thumbnailFile()->pixmap(ThumbnailFile::LargePixmapSize));
		setName(_currentCategory->name());
        setType(tr("Category"));

        CategoryNodeStats stats = _currentCategory->stats();

        appendPair(tr("Videos:"), QString::number(stats.totalVideoCount));
        appendPair(tr("Size:"), Utility::fileSizeString(stats.totalVideoSize + stats.thumbnailSize, false));
	}
	else if (_currentVideo)
	{
		setPixmap(_currentVideo->thumbnailFile()->pixmap(ThumbnailFile::LargePixmapSize));
		setName(_currentVideo->name());
        setType(_currentVideo->videoFile()->type());

        QUrl url(_currentVideo->videoFile()->url());
        appendPair(tr("Website:"), url.host());
		appendPair(tr("File Name:"), _currentVideo->videoFile()->fileName());
		appendPair(tr("Size:"), Utility::fileSizeString(_currentVideo->videoFile()->fileSize()));

        QString stateStr;
        switch (_currentVideo->videoFile()->state())
        {
        case RemoteFile::DownloadedState:
            stateStr = tr("Yes");
            break;
        case RemoteFile::NotDownloadedState:
            stateStr = tr("No");
            break;
        case RemoteFile::DownloadingState:
            stateStr = QString::number(_currentVideo->videoFile()->percentComplete(), 'f', 0) + QLocale::system().percent();
            break;
        case RemoteFile::DownloadPausedState:
            stateStr = QString::number(_currentVideo->videoFile()->percentComplete(), 'f', 0) + QLocale::system().percent() + tr(" (Paused)");
            break;
        case RemoteFile::DownloadErrorState:
            stateStr = _currentVideo->videoFile()->errorMessage();
            break;
        }

        appendPair(tr("Downloaded:"), stateStr);

		QMap<QString, QString> metaData = _currentVideo->videoFile()->metaData();
		appendPair(tr("Length:"), metaData.value(QMediaMetaData::Duration));
		appendPair(tr("Resolution:"), metaData.value(QMediaMetaData::Resolution));
		appendPair(tr("Contributing Artist:"), metaData.value(QMediaMetaData::ContributingArtist));
		appendPair(tr("Composer:"), metaData.value(QMediaMetaData::ContributingArtist));
		appendPair(tr("Year:"), metaData.value(QMediaMetaData::Year));
		appendPair(tr("Genre:"), metaData.value(QMediaMetaData::Genre));
	}

	update();
}

void Inspector::categoryChanged(CategoryNode *category)
{
	if (category && _currentCategory == category)
		updateInternal();
}

void Inspector::videoChanged(Video *video)
{
	if (_currentVideo && _currentVideo == video)
		updateInternal();
}

void Inspector::setPixmap(const QPixmap &pixmap)
{
    _pixmap = pixmap;
}

void Inspector::setName(const QString &name)
{
    _name = name;
}

void Inspector::setType(const QString &type)
{
    _type = type;
}

void Inspector::appendPair(const QString &label, const QString &value)
{
	if (!value.isEmpty())
		_list.append(QPair<QString, QString>(label, value));
}

void Inspector::clear()
{
    _pixmap = QPixmap();
    _name = "";
    _type = "";
    _list.clear();
    update();
}

QSize Inspector::minimumSizeHint() const
{
    return QSize(300, 50);
}

void Inspector::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);

    if (_pixmap.isNull() || _pixmap.height() == 0)
        return;

    double ratio = _pixmap.width() / (double)_pixmap.height();
    int imageHeight = qMin(height() - 8, 256);
    imageHeight = qMax(imageHeight, 24);
    int imageWidth = (int)(imageHeight * ratio);

    _scaledPixmap = _pixmap.scaled(imageWidth, imageHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QPainter painter(this);

    int top = 4;
    painter.drawPixmap(10, top, _scaledPixmap);

    int hSpacing = 12;
    int x = 10 + imageWidth + hSpacing;
    int y = top;

    QFont origFont = font();
    QFont boldFont = origFont;
    boldFont.setBold(true);
    painter.setFont(boldFont);

    QFontMetrics fm1(boldFont);

    int textHeight = fm1.lineSpacing();
    int nameWidth = fm1.width(_name);

    painter.drawText(QRect(x, y, nameWidth, textHeight), Qt::AlignLeft | Qt::AlignBottom, _name);
 
    painter.setFont(origFont);
    QFontMetrics fm2(origFont);

    textHeight = fm2.lineSpacing();
    int typeWidth = fm2.width(_type);

    painter.drawText(QRect(x, y + textHeight, typeWidth, textHeight), Qt::AlignLeft | Qt::AlignBottom, _type);

    int linesPerColumn = imageHeight / textHeight;

    QList<QPair<QString, QString> > workingList = _list;

    x += qMax(nameWidth, typeWidth) + hSpacing;

    while (!workingList.isEmpty())
    {
        int maxFirstWidth = 0;
        int maxSecondWidth = 0;

        int count = qMin(linesPerColumn, workingList.count());

        for (int i = 0; i < count; i++)
        {
            maxFirstWidth = qMax(fm2.width(workingList[i].first), maxFirstWidth);
            maxSecondWidth = qMax(fm2.width(workingList[i].second), maxSecondWidth);
        }

        for (int i = 0; i < count; i++)
        {
            painter.drawText(QRect(x, y, maxFirstWidth, textHeight), Qt::AlignRight | Qt::AlignBottom, workingList[i].first);
            painter.drawText(QRect(x + maxFirstWidth + 2, y, maxSecondWidth, textHeight), Qt::AlignLeft | Qt::AlignBottom, workingList[i].second);

            y += textHeight;
        }

        for (int i = 0; i < count; i++)
            workingList.removeFirst();

        x += maxFirstWidth + 2 + maxSecondWidth + hSpacing;
        y = top;
    }
}
