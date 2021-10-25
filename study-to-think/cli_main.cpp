// study-to-think.cpp : Defines the entry point for the application.
//
#include <iostream>
#include "cli_main.h"
#include "student.h"
#include "grading_scheme.h"

using namespace std;

int getOpChoice(const vector<ItemOp>& ops) {
	while (true)
	{
		cout << "Please select operation:" << endl;
		int i = 0;
		for (auto op : ops)
		{
			cout << ++i << ". " << op.displayName << endl;
		}
		cout << "? ";
		int choice;
		cin >> choice;
		if (choice > 0 && choice <= ops.size())
		{
			string tmpBuf;
			getline(cin, tmpBuf);
			return choice;
		}
		else
		{
			cout << "Please enter a number in 1-" << ops.size() << endl;
		}
	}
}

void executeOp(ItemInfo* item) {
	int choice = getOpChoice(item->getOps());
	item->getOps()[choice - 1].func(item);
}

int main()
{
	cout << "Study-To-Think Grading System " STT_VERSION << endl;
	initializeItemManual();
	StudentVector stus
	{
		{"a", "1"},
		{"aaa", "30"}
	};
	std::unique_ptr<ItemManual> testItem(new ItemManual(&stus));
	while (true)
	{
		executeOp(testItem.get());
	}
	return 0;
}
