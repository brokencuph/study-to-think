#include <gtest/gtest.h>
#include "../study-to-think/db_access.h"

TEST(DbTest, Creation)
{
	DbSession testSession("testdb.db");
}