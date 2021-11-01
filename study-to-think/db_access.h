// db_access.h: Read/Write runtime data from/to sqlite

#ifndef DB_ACCESS_H_
#define DB_ACCESS_H_

#include <sstream>
#include <iostream>
#include <sqlite3.h>


// wrapper for sqlite3 handle
class DbSession
{
	sqlite3* conn;
public:
	void open(const char fileName[]);

	sqlite3* get();

	DbSession(const char fileName[]);

	~DbSession();

	DbSession(const DbSession& other) = delete;

	DbSession(DbSession&&) noexcept;

	template <class T>
	void insert(const T& obj)
	{
		using std::stringstream;
		stringstream ss;
		ss << "INSERT INTO " << T::db_TableName
			<< " VALUES " << obj.getDbTuple();
		//std::cerr << ss.str() << std::endl;
		char* errmsg = (char*)1;
		sqlite3_exec(conn, ss.str().c_str(), nullptr, nullptr, &errmsg);
		if (errmsg != nullptr)
		{
			std::string msg(errmsg);
			sqlite3_free(errmsg);
			throw std::runtime_error(std::string(msg));
		}
	}

	template <class T>
	void updateByKey(const T& obj)
	{
		using std::stringstream;
		stringstream ss;
		ss << "UPDATE " << T::db_TableName
			<< " SET " << T::db_ColumnNames << " = " << obj.getDbTuple()
			<< " WHERE " << T::db_KeyColumn << " = " << obj.getDbKeyValue();
		//std::cerr << ss.str() << std::endl;
		char* errmsg;
		sqlite3_exec(conn, ss.str().c_str(), nullptr, nullptr, &errmsg);
		if (errmsg != nullptr)
		{
			std::string msg(errmsg);
			sqlite3_free(errmsg);
			throw std::runtime_error(std::string(msg));
		}
	}

	template <class T>
	void retrieveByKey(T& obj)
	{
		using std::stringstream;
		stringstream ss;
		ss << "SELECT * FROM " << T::db_TableName
			<< " WHERE " << T::db_KeyColumn << " = " << obj.getDbKeyValue();
		//std::cerr << ss.str() << std::endl;
		char* errmsg;
		sqlite3_exec(conn, ss.str().c_str(), T::selectCallback, &obj, &errmsg);
		if (errmsg != nullptr)
		{
			std::string msg(errmsg);
			sqlite3_free(errmsg);
			throw std::runtime_error(std::string(msg));
		}
	}

	static std::string polishForSql(const std::string&);

	static std::string polishForSql(int);
};

#endif
