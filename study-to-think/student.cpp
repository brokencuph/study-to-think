#include "student.h"
// student.cpp: Definition of student-related information

Student::Student(std::string name, StudentIdType id) : name(std::move(name)), id(std::move(id))
{
}
