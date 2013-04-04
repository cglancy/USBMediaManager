#include "library.h"

Library::Library()
{
	setAlwaysDownload(true);
}

Library::~Library()
{

}

QString Library::name() const
{
	return _name;
}

void Library::setName(const QString &name)
{
	_name = name;
}

QString Library::description() const
{
	return _description;
}

void Library::setDescription(const QString &desc)
{
	_description = desc;
}
