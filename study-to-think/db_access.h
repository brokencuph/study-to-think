// db_access.h: Read/Write runtime data from/to sqlite

#ifndef DB_ACCESS_H_
#define DB_ACCESS_H_

#include <sqlite3.h>

class DbSession
{
	sqlite3* conn;
public:
	DbSession(const char fileName[]);

	~DbSession();
};

#endif
