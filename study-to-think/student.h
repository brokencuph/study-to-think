// student.h: Definition of student-related information

#include <string>
#include <map>

typedef std::string ExtraStudentInfo;
typedef std::string StudentIdType;

class Student {
private:

public:
	std::string name;
	StudentIdType id;
	ExtraStudentInfo extraInfo;

	// toDbRepr()
	// fromDbRepr()
};

template <class TT>
using StudentIdMap = std::map<StudentIdType, TT>;