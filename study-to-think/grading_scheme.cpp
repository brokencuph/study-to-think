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

const char* checkInTypeNames[] =
{
	"Normal",
	"Late",
	"Early Leave",
	"Absent"
};

const std::map<std::string, CheckInType> checkInTypeIds
{
	{"Normal", CheckInType::NORMAL},
	{"Late", CheckInType::LATE},
	{"Early Leave", CheckInType::LEAVEEARLY},
	{"Absent", CheckInType::ABSENT}
};

void ItemOp::operator()(ItemInfo* info) const
{
	this->func(info);
}

void ItemManual::setItemName(std::string itemName)
{
	this->itemName = itemName;
}

std::string ItemManual::getItemName() const
{
	return itemName;
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
		if (getItemName() != obj.itemName)
		{
			continue;
		}
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

void ItemAttendance::setItemName(std::string itemName)
{
	this->itemName = std::move(itemName);
}

std::string ItemAttendance::getItemName() const
{
	return itemName;
}

int ItemAttendance::getScore(const Student& student) const
{
	// TODO: implement the logic
	// modify by ZHANG Xiubo 2021-11-25
	StudentIdType selected_ID;
	selected_ID = student.id;
	int numberOfLateAndLeaveEarly = 0;
	int numberOfAbsentOriginal = 0;
	int numberOfAbsentWithTranslate = 0;
	double absentRate = 0.0;
	for (auto i = (studentAttendance.at(selected_ID)).begin(); i != (studentAttendance.at(selected_ID).end()); i++)
	{
		if (*i == CheckInType::LATE || *i == CheckInType::LEAVEEARLY)
			numberOfLateAndLeaveEarly++;
		if (*i == CheckInType::ABSENT)
			numberOfAbsentOriginal++;


	}
	numberOfAbsentWithTranslate = numberOfAbsentOriginal + numberOfLateAndLeaveEarly / 3;
	absentRate = 1.0 * numberOfAbsentWithTranslate / (1.0 * sessionNumber);
	if (absentRate < 0.1)
		return 100;
	else if (absentRate >= 0.1 && absentRate <= 0.15)
		return 70;
	else if (absentRate > 0.15 && absentRate <= 0.3)
		return 20;
	else
	{
		throw std::runtime_error("T letter");
		//TO be continue by the help of 1809853ji011006
	}
	return 0;
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
	this->studentAttendance.clear();
	for (const Student& stu : *this->students)
	{
		if (sessionNumber <= 0)
		{
			this->studentAttendance.emplace(stu.id, std::vector<CheckInType>());
		}
		else
		{
			this->studentAttendance.emplace(stu.id, std::vector<CheckInType>(sessionNumber, CheckInType::NORMAL));
		}
	}
}

void ItemAttendance::fillScoreFromDb(const std::vector<StudentGradeDBO>& vec)
{
	for (const auto& dbo : vec)
	{
		if (getItemName() != dbo.itemName)
		{
			continue;
		}
		auto it = studentAttendance.find(dbo.studentId);
		if (it == studentAttendance.end())
		{
			continue;
		}
		// the format of repr: <CheckInType1> <CheckInType2> ... <CheckInTypeN>
		std::stringstream ss(dbo.scoreRepr);
		it->second = std::vector<CheckInType>(sessionNumber);
		for (int i = 0; i < sessionNumber; i++)
		{
			int tmp;
			ss >> tmp;
			it->second[i] = (CheckInType)tmp;
		}
	}
}

int ItemAttendance::getSessionNumber() const
{
	return sessionNumber;
}

void ItemAttendance::modifySessionNumber(int newSessionNumber) 
{
	sessionNumber = newSessionNumber;
}

std::string ItemAttendance::scoreRepr(const std::vector<CheckInType>& vec)
{
	std::stringstream ss;
	for (size_t i = 0; i < vec.size(); i++)
	{
		if (i > 0)
		{
			ss << " ";
		}
		ss << (int)vec[i];
	}
	return ss.str();
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
	ratingItem->item->setItemName(ratingItem->name);
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
