// db_access.h: Read/Write runtime data from/to sqlite

#ifndef DB_ACCESS_H_
#define DB_ACCESS_H_

#include <sqlite3.h>


// wrapper for sqlite3 handle
class DbSession
{
	sqlite3* conn;
public:
	void open(const char fileName[]);

	DbSession(const char fileName[]);

	~DbSession();

	DbSession(const DbSession& other) = delete;

	DbSession(DbSession&&) noexcept;
};

#endif
