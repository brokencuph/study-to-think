#include <gtest/gtest.h>
#include "../study-to-think/total_grade.h"

using namespace stt::grade_utils;

TEST(GradeUtilsTest, Manipulation)
{
	TotalGrade x = 40;
	x += TotalGrade(40);
	ASSERT_EQ(x, TotalGrade(80));
	x -= TotalGrade(50);
	ASSERT_EQ(x, TotalGrade(30));
	x *= TotalGrade(4);
	ASSERT_EQ(x, TotalGrade(120));
	x /= TotalGrade(120);
	ASSERT_EQ(x, TotalGrade(1));
	x += TotalGrade(SpecialGrade::T);
	ASSERT_EQ(x, TotalGrade(SpecialGrade::T));
	x += TotalGrade(SpecialGrade::T);
	ASSERT_EQ(x, TotalGrade(SpecialGrade::T));
	x += TotalGrade(5);
	ASSERT_EQ(x, TotalGrade(SpecialGrade::T));
	SUCCEED();
}

TEST(GradeUtilsTest, Show)
{
	TotalGrade x = 40.5, y = SpecialGrade::T;
	ASSERT_EQ(x.toString(), "40.50");
	x = TotalGrade(100.0);
	ASSERT_EQ(x.toString(), "100.00");
	ASSERT_EQ(y.toString(), "T");
	SUCCEED();
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}