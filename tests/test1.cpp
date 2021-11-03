#include <vector>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "../study-to-think/db_access.h"
#include "../study-to-think/student.h"

//using namespace ::testing;

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
	using namespace ::testing;
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
	std::vector<Student> stus;
	testSession.retrieveAll(stus);
	ASSERT_THAT(stus, UnorderedElementsAre(AllOf(Field(&Student::name, Eq("HE YUJIE is 250")), Field(&Student::id, Eq("1809853J-I011-0065"))),
		AllOf(Field(&Student::name, Eq("ZHANG XIUBO")), Field(&Student::id, Eq("1809853G-I011-0014")))));
	SUCCEED();
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	testing::InitGoogleMock(&argc, argv);
	return RUN_ALL_TESTS();
}