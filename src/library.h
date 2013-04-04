#ifndef LIBRARY_H
#define LIBRARY_H

#include "remotefile.h"
#include <QString>

class Library : public RemoteFile
{
public:
	Library();
	~Library();

	QString name() const;
	void setName(const QString &name);

	QString description() const;
	void setDescription(const QString &desc);

private:
	QString _name, _description;
};

#endif // LIBRARY_H
