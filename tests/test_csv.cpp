#include <gtest/gtest.h>
#include <vector>
#include "../study-to-think/csv_io.h"
#include "../study-to-think/student.h"
#include "../study-to-think/grading_scheme.h"
#include "../study-to-think/db_access.h"

#ifdef WIN32
#define COPY_DB_FILE(filename) system("copy /Y .\\" filename ".db .\\" filename "_copy.db")
#else
#define COPY_DB_FILE(filename) system("cp ./" filename ".db ./" filename "_copy.db")
#endif


TEST(CsvTest, StudentCsv)
{
	COPY_DB_FILE("test_crud");
	DbSession testSession("test_crud_copy.db");
	std::string csvStr;
	csvStr = stt::csv_io::dbToCsv<Student>(&testSession);
#ifdef WIN32
	system("del .\\testdb.db");
#else
	system("rm ./testdb.db");
#endif
	std::string csvStr2 = csvStr.substr(csvStr.find('\n') + 1);
	DbSession newSession("./testdb.db");
	stt::csv_io::csvToDb<Student>(csvStr2, &newSession);
	std::vector<Student> stus;
	newSession.retrieveAll(stus);
	ASSERT_EQ(stus.size(), 1);
	ASSERT_EQ(stus[0].id, "1809853J-I011-0065");
	ASSERT_EQ(stus[0].name, "HE YUJIE");
	ASSERT_EQ(stus[0].extraInfo, "");
	SUCCEED();
}

TEST(CsvTest, RatingItemCsv)
{
	COPY_DB_FILE("test_crud");
	DbSession testSession("test_crud_copy.db");
	std::string csvStr;
	csvStr = stt::csv_io::dbToCsv<RatingItem>(&testSession);
#ifdef WIN32
	system("del .\\testdb.db");
#else
	system("rm ./testdb.db");
#endif
	std::string csvStr2 = csvStr.substr(csvStr.find('\n') + 1);
	DbSession newSession("./testdb.db");
	stt::csv_io::csvToDb<RatingItem>(csvStr2, &newSession);
	std::vector<RatingItem> items;
	newSession.retrieveAll(items);
	ASSERT_EQ(items.size(), 1);
	ASSERT_EQ(items[0].name, "Check In");
	ASSERT_EQ(items[0].weight, 20);
	ASSERT_EQ(items[0].item->getCurrentItemType(), 0);
	ASSERT_EQ(items[0].item->getParams(), "15");
	SUCCEED();
}


int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}