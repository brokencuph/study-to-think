#include <gtest/gtest.h>
#include "../study-to-think/db_access.h"
#include "../study-to-think/student.h"

TEST(DbTest, Creation)
{
	system("del .\\testdb.db");
	DbSession testSession("testdb.db");
	Student stu("HE YUJIE", "1809853J-I011-0065");
	testSession.insert(stu);
	Student retrStu("", "1809853J-I011-0065");
	testSession.retrieveByKey(retrStu);
	ASSERT_EQ(retrStu.name, stu.name);
	SUCCEED();
}

TEST(DbTest, StudentCRU)
{
	system("copy /Y ..\\..\\..\\..\\test_crud.db .\\test_crud.db");
	DbSession testSession("test_crud.db");
	Student initialStu("", "1809853J-I011-0065");
	testSession.retrieveByKey(initialStu);
	ASSERT_EQ(initialStu.name, "HE YUJIE");
	testSession.insert(Student("ZHANG XIUBO", "1809853G-I011-0014"));
	Student insertedStu("", "1809853G-I011-0014");
	testSession.retrieveByKey(insertedStu);
	ASSERT_EQ(insertedStu.name, "ZHANG XIUBO");
	Student updatedStu = initialStu;
	updatedStu.name += " is 250";
	testSession.updateByKey(updatedStu);
	testSession.retrieveByKey(initialStu);
	ASSERT_EQ(updatedStu.name, initialStu.name);
	SUCCEED();
}