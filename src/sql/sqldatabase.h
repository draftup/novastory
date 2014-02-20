#ifndef SQLDATABASE_H
#define SQLDATABASE_H

#include <QSqlDatabase>

namespace novastory
{

class SqlDatabase : public QSqlDatabase
{
public:
	SqlDatabase(bool openOnStart = false);
	virtual ~SqlDatabase();
	static SqlDatabase& Instance()
	{
		static SqlDatabase theSingleInstance(true);
		return theSingleInstance;
	}
};

}

#endif
