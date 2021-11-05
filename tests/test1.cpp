#include <vector>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "../study-to-think/db_access.h"
#include "../study-to-think/student.h"
#include "../study-to-think/grading_scheme.h"

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

TEST(DbTest, RatingItemCRU)
{
	using namespace ::testing;
	system("copy /Y ..\\..\\..\\..\\test_crud.db .\\test_crud.db");
	DbSession testSession("test_crud.db");
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
	SUCCEED();
}


int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	testing::InitGoogleMock(&argc, argv);
	return RUN_ALL_TESTS();
}