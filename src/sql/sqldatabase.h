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
};

}

#endif
