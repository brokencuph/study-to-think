#include <iostream>
#include <string>
#include "grading_scheme.h"
#include "db_access.h"
// grading_scheme.cpp: Definition of rating items.

const char* RatingItem::db_TableName = "rating_item";
const char* RatingItem::db_KeyColumn = "item_name";
const char* RatingItem::db_ColumnNames = "(item_name,item_type,weight,params)";

std::vector<ItemOp> ItemAttendance::ops;
std::vector<ItemOp> ItemManual::ops;

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
	// TODO: to be done
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
