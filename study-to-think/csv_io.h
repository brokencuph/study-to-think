#ifndef CSV_IO_H_
#define CSV_IO_H_

#include <string>
#include <sstream>
#include "db_access.h"
#include "student.h"


namespace stt::csv_io
{
	template <class DType>
	class CsvLineIo
	{
	public:
		static inline const char* db_TableName = DType::db_TableName;
		static inline const char* db_ColumnNames = DType::db_ColumnNames;

		CsvLineIo()
		{

		}

		CsvLineIo(std::string csvLine) : csvLine(std::move(csvLine))
		{

		}

		std::string getDbTuple() const
		{
			std::string ans = csvLine;
			size_t lastStart = 0;
			for (size_t i = 0; i <= ans.size(); i++)
			{
				if (i < ans.size() && ans[i] == '"')
				{
					ans[i] = '\'';
				}
				else if (i == ans.size() || ans[i] == ',')
				{
					if (i == 0 || ans[i - 1] != '\'')
					{
						ans.insert(ans.begin() + i, '\'');
						ans.insert(ans.begin() + lastStart, '\'');
						i += 2;
					}
					lastStart = i + 1;
				}
			}
			return "(" + ans + ")";
		}

		static int selectCallback(void* obj, int num, char** vals, char** names)
		{
			CsvLineIo<DType>* csvObj = static_cast<CsvLineIo<DType>*>(obj);
			for (size_t i = 0; i < num; i++)
			{
				if (i > 0)
				{
					csvObj->csvLine += ",";
				}
				csvObj->csvLine += "\"";
				if (vals[i])
				{
					csvObj->csvLine += vals[i];
				}
				csvObj->csvLine += "\"";
			}
			return 0;
		}
		std::string csvLine;

	};

	template <class DType>
	std::string dbToCsv(DbSession* db)
	{
		std::vector<CsvLineIo<DType>> vecLines;
		db->retrieveAll(vecLines);
		std::string ans = DType::db_ColumnNames;
		ans = ans.substr(1, ans.size() - 2) + "\n";
		for (const auto& x : vecLines)
		{
			ans += x.csvLine + "\n";
		}
		return ans;
	}

	template <class DType>
	void csvToDb(const std::string& csvStr, DbSession* db)
	{
		std::istringstream ss(csvStr);
		std::string str;
		while (std::getline(ss, str))
		{
			db->insert(CsvLineIo<DType>(str));
		}
	}
}

#endif