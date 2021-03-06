// student.h: Definition of student-related information

#ifndef STUDENT_H_
#define STUDENT_H_

#include <string>
#include <map>
#include <vector>
//#include "grading_scheme.h"
#include "total_grade.h"

using namespace stt::grade_utils;

typedef std::string ExtraStudentInfo;
typedef std::string StudentIdType;

class RatingItem;

class Student {
private:

public:
	static const char* db_TableName;
	static const char* db_ColumnNames;
	static const char* db_KeyColumn;
	std::string name;
	StudentIdType id;
	ExtraStudentInfo extraInfo;

	Student(std::string name, StudentIdType id);
	Student() = default;
	// toDbRepr()
	// fromDbRepr()
	TotalGrade getTotalScore(const std::vector<RatingItem>& items) const;
	std::string getDbTuple() const;
	std::string getDbKeyValue() const;
	static int selectCallback(void*, int, char**, char**);
};

template <class TT>
using StudentIdMap = std::map<StudentIdType, TT>;

using StudentVector = std::vector<Student>;

#endif