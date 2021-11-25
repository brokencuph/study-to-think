#include <vector>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "../study-to-think/db_access.h"
#include "../study-to-think/student.h"
#include "../study-to-think/grading_scheme.h"

//using namespace ::testing;

#ifdef WIN32
#define COPY_DB_FILE(filename) system("copy /Y .\\" filename ".db .\\" filename "_copy.db")
#else
#define COPY_DB_FILE(filename) system("cp ./" filename ".db ./" filename "_copy.db")
#endif

TEST(DbTest, Creation)
{
#ifdef WIN32
	system("del .\\testdb.db");
#else
	system("rm ./testdb.db");
#endif
	DbSession testSession("testdb.db");
	Student stu("HE YUJIE", "1809853J-I011-0065");
	testSession.insert(stu);
	Student retrStu("", "1809853J-I011-0065");
	testSession.retrieveByKey(retrStu);
	ASSERT_EQ(retrStu.name, stu.name);
	SUCCEED();
}

TEST(DbTest, StudentCRUD)
{
	using namespace ::testing;
	COPY_DB_FILE("test_crud");
	DbSession testSession("test_crud_copy.db");
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
	testSession.removeByKey(initialStu);
	Student removedStu("", "1809853J-I011-0065");
	testSession.retrieveByKey(removedStu);
	ASSERT_EQ(removedStu.name, "");
	SUCCEED();
}

TEST(DbTest, RatingItemCRUD)
{
	using namespace ::testing;
	COPY_DB_FILE("test_crud");
	DbSession testSession("test_crud_copy.db");
	std::vector<RatingItem> itemList;
	testSession.retrieveAll(itemList);
	ASSERT_EQ(itemList.size(), 1);
	ASSERT_EQ(itemList[0].name, "Check In");
	ASSERT_EQ(itemList[0].weight, 20);
	ASSERT_EQ(itemList[0].item->getCurrentItemType(), 0);
	ASSERT_EQ(((ItemAttendance*)itemList[0].item.get())->getSessionNumber(), 15);
	RatingItem newItem;
	newItem.name = "Exam 1";
	newItem.weight = 30;
	newItem.item = std::unique_ptr<ItemInfo>(new ItemManual);
	testSession.insert(newItem);
	RatingItem anotherNewItem;
	anotherNewItem.name = "Exam 1";
	testSession.retrieveByKey(anotherNewItem);
	ASSERT_EQ(anotherNewItem.name, newItem.name);
	ASSERT_EQ(anotherNewItem.weight, newItem.weight);
	ASSERT_EQ(anotherNewItem.item->getCurrentItemType(), newItem.item->getCurrentItemType());
	testSession.removeByKey(itemList[0]);
	RatingItem removedItem;
	removedItem.name = "Check In";
	removedItem.weight = 0;
	testSession.retrieveByKey(removedItem);
	ASSERT_EQ(removedItem.weight, 0);
	SUCCEED();
}

TEST(DbTest, StudentGradeDBOUpsert)
{
	COPY_DB_FILE("test_crud");
	DbSession testSession("test_crud_copy.db");
	StudentGradeDBO g1;
	g1.itemName = "Check In";
	g1.studentId = "1809853J-I011-0065";
	g1.scoreRepr = "1111";
	testSession.upsert(g1);
	std::vector<StudentGradeDBO> lstGrade;
	testSession.retrieveAll(lstGrade);
	ASSERT_EQ(lstGrade.size(), 1);
	ASSERT_EQ(lstGrade[0].itemName, g1.itemName);
	ASSERT_EQ(lstGrade[0].studentId, g1.studentId);
	ASSERT_EQ(lstGrade[0].scoreRepr, g1.scoreRepr);
	g1.scoreRepr = "2222";
	testSession.upsert(g1);
	lstGrade.clear();
	testSession.retrieveAll(lstGrade);
	ASSERT_EQ(lstGrade.size(), 1);
	ASSERT_EQ(lstGrade[0].itemName, g1.itemName);
	ASSERT_EQ(lstGrade[0].studentId, g1.studentId);
	ASSERT_EQ(lstGrade[0].scoreRepr, g1.scoreRepr);
	SUCCEED();
}


int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	testing::InitGoogleMock(&argc, argv);
	return RUN_ALL_TESTS();
}