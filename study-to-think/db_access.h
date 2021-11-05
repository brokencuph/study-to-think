// db_access.h: Read/Write runtime data from/to sqlite

#ifndef DB_ACCESS_H_
#define DB_ACCESS_H_

#include <sstream>
#include <iostream>
#include <vector>
#include <sqlite3.h>

#ifndef NDEBUG
#define SQL_DEBUG
#endif

// wrapper for sqlite3 handle
class DbSession
{
	sqlite3* conn;

	// first parameter points to a vector<T>
	template <class T>
	static int sqliteVectorCallback(void* obj, int num, char** vals, char** names)
	{
		std::vector<T>& vec = *static_cast<std::vector<T>*>(obj);
		T newObj;
		int res = T::selectCallback(&newObj, num, vals, names);
		if (res != 0)
		{
			return res;
		}
		vec.push_back(std::move(newObj));
		return 0;
	}
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
#ifdef SQL_DEBUG
		std::cerr << ss.str() << std::endl;
#endif
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
#ifdef SQL_DEBUG
		std::cerr << ss.str() << std::endl;
#endif
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
#ifdef SQL_DEBUG
		std::cerr << ss.str() << std::endl;
#endif
		char* errmsg;
		sqlite3_exec(conn, ss.str().c_str(), T::selectCallback, &obj, &errmsg);
		if (errmsg != nullptr)
		{
			std::string msg(errmsg);
			sqlite3_free(errmsg);
			throw std::runtime_error(std::string(msg));
		}
	}

	template <class T>
	void retrieveAll(std::vector<T>& vec)
	{
		using std::stringstream;
		stringstream ss;
		ss << "SELECT * FROM " << T::db_TableName;
#ifdef SQL_DEBUG
		std::cerr << ss.str() << std::endl;
#endif
		char* errmsg;
		sqlite3_exec(conn, ss.str().c_str(), DbSession::sqliteVectorCallback<T>, &vec, &errmsg);
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
