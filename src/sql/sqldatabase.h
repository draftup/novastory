#ifndef SQLDATABASE_H
#define SQLDATABASE_H

#include <QSqlDatabase>

namespace novastory
{

class SqlDatabase : public QSqlDatabase
{
public:
	SqlDatabase();
	virtual ~SqlDatabase();
	static SqlDatabase& Instance()
	{
		static SqlDatabase theSingleInstance;
		return theSingleInstance;
	}
};

}

#endif
