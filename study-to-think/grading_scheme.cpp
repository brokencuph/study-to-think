#include <iostream>
#include <string>
#include "grading_scheme.h"
// grading_scheme.cpp: Definition of rating items.

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

ItemManual::ItemManual(const StudentVector* students) : students(students)
{

}

const std::vector<ItemOp>& ItemAttendance::getOps() const
{
	return ItemAttendance::ops;
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
