#ifndef INSPECTOR_H
#define INSPECTOR_H

#include <QFrame>
#include <QPixmap>
#include <QList>

class QPaintEvent;
class CategoryNode;
class Video;

class Inspector : public QFrame
{
	Q_OBJECT

public:
	Inspector(QWidget *parent);
	~Inspector();

	void setVideo(Video *video);
	void setCategory(CategoryNode *category);

    void clear();

    virtual QSize minimumSizeHint() const;

public slots:
	void categoryChanged(CategoryNode *category);
	void videoChanged(Video *video);

protected:
    virtual void paintEvent(QPaintEvent *event);
	void updateInternal(); 
	void setPixmap(const QPixmap &pixmap);
	void setName(const QString &name);
    void setType(const QString &type);
	void appendPair(const QString &label, const QString &value);

private:
	Video *_currentVideo;
	CategoryNode *_currentCategory;
    QPixmap _pixmap, _scaledPixmap;
    QString _name, _type;
    QList<QPair<QString, QString> > _list;
};

#endif // INSPECTOR_H
