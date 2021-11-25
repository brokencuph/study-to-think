
// student.cpp: Definition of student-related information
#include "grading_scheme.h"
#include "student.h"
#include "db_access.h"
const char* Student::db_TableName = "student";
const char* Student::db_ColumnNames = "(student_id,name,extra_info)";
const char* Student::db_KeyColumn = "student_id";

Student::Student(std::string name, StudentIdType id) : name(std::move(name)), id(std::move(id))
{
}

int Student::getTotalScore(const std::vector<RatingItem>& items) const
{
	int ans = 0;
	for (const RatingItem& item : items)
	{
		ans += item.item->getScore(*this) * item.weight;
	}
	return ans / 100;
}

std::string Student::getDbKeyValue() const
{
	return DbSession::polishForSql(this->id);
}

int Student::selectCallback(void* obj, int num, char** vals, char** names)
{
	Student* student = static_cast<Student*>(obj);
	student->id = vals[0];
	student->name = vals[1];
	if (vals[2] != nullptr)
	{
		student->extraInfo = vals[2];
	}
	return 0;
}

std::string Student::getDbTuple() const
{
	return "(" + DbSession::polishForSql(this->id) + "," + DbSession::polishForSql(this->name)
		+ "," + DbSession::polishForSql(this->extraInfo) + ")";
}