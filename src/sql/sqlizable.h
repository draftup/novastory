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
	bool removeSQL(const QList<QString>& basis);
	bool removeSQL(const QString& basis);
	bool syncSQL(const QList<QString>& basis);
	bool syncSQL(const QString& basis);
};

}

#endif
