#include <gtest/gtest.h>
#define private public
#include "../study-to-think/grading_scheme.h"
#include "../study-to-think/student.h"

TEST(AttendanceTest, GetScore)
{
	ItemAttendance attendance;
	ASSERT_EQ(attendance.getCurrentItemType(), 0);
	attendance.setParams("15");
	for (int i = 0; i < 15; i++)
	{
		attendance.studentAttendance["111"].push_back(CheckInType::NORMAL);
	} // 100
	for (int i = 0; i < 15; i++)
	{
		if (i < 10)
		{
			attendance.studentAttendance["222"].push_back(CheckInType::NORMAL);
		}
		else
		{
			attendance.studentAttendance["222"].push_back(CheckInType::ABSENT);
		}
	} // T letter
	for (int i = 0; i < 15; i++)
	{
		if (i < 8)
		{
			attendance.studentAttendance["333"].push_back(CheckInType::NORMAL);
		}
		else
		{
			attendance.studentAttendance["333"].push_back(CheckInType::LATE);
		}
	} // some lates
	Student stu111("111", "111"), stu222("222", "222"), stu333("333", "333");
	ASSERT_EQ(attendance.getScore(stu111), 100);
	ASSERT_ANY_THROW(
	{ 
		attendance.getScore(stu222);
	});
	ASSERT_EQ(attendance.getScore(stu333), 70);
	SUCCEED();
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}