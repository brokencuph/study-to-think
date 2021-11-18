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
	"params"	TEXT NOT NULL,
	PRIMARY KEY("item_name")))",

	R"(CREATE TABLE "student_grade" (
	"student_id"	TEXT NOT NULL,
	"item_name"	TEXT NOT NULL,
	"grade_info"	TEXT,
	FOREIGN KEY("item_name") REFERENCES "rating_item"("item_name"),
	FOREIGN KEY("student_id") REFERENCES "student"("student_id")))"
};

void DbSession::open(const char fileName[])
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
				throw std::runtime_error(std::string(msg));
			}
		}
	}
}

sqlite3* DbSession::get()
{
	return conn;
}

DbSession::DbSession(const char fileName[])
{
	conn = nullptr;
	open(fileName);
}

DbSession::~DbSession()
{
	if (conn != nullptr)
	{
		sqlite3_close(conn);
		conn = nullptr;
	}
}

DbSession::DbSession(DbSession&& other) noexcept
{
	this->conn = other.conn;
	other.conn = nullptr;
}

std::string DbSession::polishForSql(const std::string& str)
{
	std::string ans = str;
	return "'" + ans + "'";
}

std::string DbSession::polishForSql(int x)
{
	return std::to_string(x);
}

bool DbSession::checkStringLiteral(const std::string& str)
{
	return str.find('\'') == std::string::npos;
}
