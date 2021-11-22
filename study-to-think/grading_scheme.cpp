#include <iostream>
#include <string>
#include <algorithm>
#include "grading_scheme.h"
#include "db_access.h"
// grading_scheme.cpp: Definition of rating items.

const char* RatingItem::db_TableName = "rating_item";
const char* RatingItem::db_KeyColumn = "item_name";
const char* RatingItem::db_ColumnNames = "(item_name,item_type,weight,params)";

std::vector<ItemOp> ItemAttendance::ops;
std::vector<ItemOp> ItemManual::ops;

std::string ItemInfo::typeNames[] =
{
	"Attendance",
	"Manual"
};

void ItemOp::operator()(ItemInfo* info) const
{
	this->func(info);
}

int ItemManual::getScore(const Student& student) const
{
	return this->studentScores.at(student.id);
}

const std::vector<ItemOp>& ItemManual::getOps() const
{
	return ItemManual::ops;
}

int ItemManual::getCurrentItemType() const
{
	return 1;
}

void ItemManual::setParams(const char* paramStr)
{
	// do nothing
}

std::string ItemManual::getParams() const
{
	return "";
}

void ItemManual::setStudents(const std::vector<Student>* stus)
{
	this->students = stus;
	this->studentScores.clear();
	for (const Student& stu : *this->students)
	{
		this->studentScores.emplace(stu.id, 0);
	}
}

void ItemManual::fillScoreFromDb(const std::vector<StudentGradeDBO>& vec)
{
	for (const StudentGradeDBO& obj : vec)
	{
		auto it = studentScores.find(obj.studentId);
		if (it == studentScores.end())
		{
			continue;
		}
		it->second = atoi(obj.scoreRepr.c_str());
	}
}

ItemManual::ItemManual(const StudentVector* students) : students(students)
{

}

int ItemAttendance::getScore(const Student& student) const
{
	// TODO: implement the logic
	throw std::runtime_error("not implemented");
}

const std::vector<ItemOp>& ItemAttendance::getOps() const
{
	return ItemAttendance::ops;
}

int ItemAttendance::getCurrentItemType() const
{
	return 0;
}

void ItemAttendance::setParams(const char* paramStr)
{
	sessionNumber = atoi(paramStr);
}

std::string ItemAttendance::getParams() const
{
	return std::to_string(sessionNumber);
}

void ItemAttendance::setStudents(const std::vector<Student>* stus)
{
	this->students = stus;
}

void ItemAttendance::fillScoreFromDb(const std::vector<StudentGradeDBO>& vec)
{
	
}

int ItemAttendance::getSessionNumber() const
{
	return sessionNumber;
}

void manualEnterGrade(ItemInfo* itemInfo)
{
	using namespace std;
	ItemManual* itemManual = dynamic_cast<ItemManual*>(itemInfo);
	const StudentVector& stus = *(itemManual->students);
	for (const Student& stu : stus)
	{
		auto enterGrade = [&stu]
		{
			while (true) 
			{
				cout << stu.name << ',' << stu.id << ": ";
				int tmp;
				cin >> tmp;
				if (tmp >= 0 && tmp <= 100) 
				{
					return tmp;
				}
				else 
				{
					cout << "Score must be 0-100, retry" << endl;
				}
			}
		};
		int verifiedGrade = enterGrade();
		itemManual->studentScores[stu.id] = verifiedGrade;
	}
}

void manualUpdateGrade(ItemInfo* itemInfo)
{
	using namespace std;
	ItemManual* itemManual = dynamic_cast<ItemManual*>(itemInfo);
	const StudentVector& stus = *(itemManual->students);
	cout << "Enter Student ID: ";
	string stuId;
	getline(cin, stuId);
	auto it = itemManual->studentScores.find(stuId);
	if (it == itemManual->studentScores.end())
	{
		cout << "No student " << stuId << " found." << endl;
		return;
	}
	else
	{

		auto enterGrade = [&]
		{
			while (true)
			{
				cout << stuId << ": ";
				int tmp;
				cin >> tmp;
				if (tmp >= 0 && tmp <= 100)
				{
					return tmp;
				}
				else
				{
					cout << "Score must be 0-100, retry" << endl;
				}
			}
		};
		int verifiedGrade = enterGrade();
		itemManual->studentScores[stuId] = verifiedGrade;

	}
}

void initializeItemManual()
{
	ItemManual::ops =
	{
		{ "Update All Students", manualEnterGrade},
		{ "Update One Student", manualUpdateGrade}
	};
}

std::string RatingItem::getDbTuple() const
{
	return "(" + DbSession::polishForSql(name) + "," +
		DbSession::polishForSql(item->getCurrentItemType()) + "," +
		DbSession::polishForSql(weight) + "," +
		DbSession::polishForSql(item->getParams()) + ")";
}

std::string RatingItem::getDbKeyValue() const
{
	return DbSession::polishForSql(name);
}

int RatingItem::selectCallback(void* obj, int num, char** vals, char** names)
{
	RatingItem* ratingItem = static_cast<RatingItem*>(obj);
	ratingItem->name = vals[0];
	ratingItem->weight = atoi(vals[2]);
	int itemTypeId = atoi(vals[1]);
	switch (itemTypeId)
	{
	case 0:
		ratingItem->item = std::unique_ptr<ItemInfo>(new ItemAttendance);
		break;
	case 1:
		ratingItem->item = std::unique_ptr<ItemInfo>(new ItemManual);
		break;
	default:
		throw std::invalid_argument("no such item type " + std::to_string(itemTypeId));
	}
	ratingItem->item->setParams(vals[3]);
	return 0;
}

//std::unique_ptr<AbstractStudentScore> AbstractStudentScore::fromString(int type, const std::string& str)
//{
//	switch (type)
//	{
//	case 0:
//		return std::unique_ptr<AbstractStudentScore>(new AttendanceScore(str));
//	case 1:
//		return std::unique_ptr<AbstractStudentScore>(new ManualScore(str));
//	}
//}

ManualScore::ManualScore(const std::string& str)
	: score(atoi(str.c_str()))
{
}

std::string ManualScore::serialize() const
{
	return std::to_string(score);
}

const char* StudentGradeDBO::db_TableName = "student_grade";
const char* StudentGradeDBO::db_ColumnNames = "(student_id,item_name,grade_info)";
const char* StudentGradeDBO::db_UpsertColumn = "grade_info";

std::string StudentGradeDBO::getDbTuple() const
{
	return "(" + DbSession::polishForSql(studentId)
		+ "," + DbSession::polishForSql(itemName)
		+ "," + DbSession::polishForSql(scoreRepr)
		+ ")";
}

int StudentGradeDBO::selectCallback(void* obj, int num, char** vals, char** names)
{
	StudentGradeDBO* gradeObj = static_cast<StudentGradeDBO*>(obj);
	gradeObj->studentId = vals[0];
	gradeObj->itemName = vals[1];
	gradeObj->scoreRepr = vals[2];
	return 0;
}
