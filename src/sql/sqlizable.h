#ifndef SQLIZABLE_H
#define SQLIZABLE_H

#include <QObject>

namespace novastory
{

class Sqlizable : public QObject
{
	Q_OBJECT
public:
	Sqlizable();
	~Sqlizable() {};

	bool insertSQL();
	bool updateSQL();
	bool removeSQL();
	bool syncSQL(QList<QString> basis);
};

}

#endif
