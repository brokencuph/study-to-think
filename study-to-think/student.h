// student.h: Definition of student-related information

#ifndef STUDENT_H_
#define STUDENT_H_

#include <string>
#include <map>
#include <vector>

typedef std::string ExtraStudentInfo;
typedef std::string StudentIdType;

class Student {
private:

public:
	std::string name;
	StudentIdType id;
	ExtraStudentInfo extraInfo;

	Student(std::string name, StudentIdType id);
	// toDbRepr()
	// fromDbRepr()
};

template <class TT>
using StudentIdMap = std::map<StudentIdType, TT>;

using StudentVector = std::vector<Student>;

#endif