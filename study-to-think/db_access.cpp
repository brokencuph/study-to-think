// db_access.cpp: Read/Write runtime data from/to sqlite


#include <stdexcept>
#include <sqlite3.h>
#include "db_access.h"

static const char* createCommands[] =
{
	R"(CREATE TABLE "student" (
	"student_id"	TEXT NOT NULL UNIQUE,
	"name"	TEXT NOT NULL,
	"extra_info"	TEXT,
	PRIMARY KEY("student_id")))",

	R"(CREATE TABLE "rating_item" (
	"item_name"	TEXT NOT NULL UNIQUE,
	"item_type"	INTEGER NOT NULL,
	"weight"	INTEGER NOT NULL CHECK(weight > 0 AND weight <= 100),
	PRIMARY KEY("item_name")))",

	R"(CREATE TABLE "student_grade" (
	"student_id"	TEXT NOT NULL,
	"item_name"	TEXT NOT NULL,
	"grade_info"	BLOB,
	FOREIGN KEY("item_name") REFERENCES "rating_item"("item_name"),
	FOREIGN KEY("student_id") REFERENCES "student"("student_id")))"
};

DbSession::DbSession(const char fileName[])
{
	int res;
	if ((res = sqlite3_open_v2(fileName, &this->conn, SQLITE_OPEN_READWRITE, nullptr)) != SQLITE_OK)
	{
		// file may not exist, try to create
		res = sqlite3_open_v2(fileName, &this->conn, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
		if (res != SQLITE_OK)
		{
			throw std::invalid_argument("Failed to open database file.");
		}
		// create the tables
		for (const char* cmd : createCommands)
		{
			char* errmsg = (char*)1;
			sqlite3_exec(conn, cmd, nullptr, nullptr, &errmsg); // execute create SQLs
			if (errmsg) // if error occurs, errmsg will be non-zero
			{
				std::string msg(errmsg);
				sqlite3_free(errmsg);
				throw std::invalid_argument(std::string(msg));
			}
		}
	}
}

DbSession::~DbSession()
{
	sqlite3_close(conn);
}
