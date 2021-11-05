// grading_scheme.h: Definition of rating items.

#ifndef GRADING_SCHEME_H
#define GRADING_SCHEME_H

#include <memory>
#include <map>
#include <vector>
#include "student.h"

class ItemInfo;


class RatingItem
{
	//int getItemType();
public:
	static const char* db_TableName;
	static const char* db_ColumnNames;
	static const char* db_KeyColumn;
	int weight; // in percent
	std::string name;
	std::unique_ptr<ItemInfo> item;
	// toDbRepr();
	// fromDbRepr();

	

	std::string getDbTuple() const;
	std::string getDbKeyValue() const;
	static int selectCallback(void*, int, char**, char**);

};


struct ItemOp {
	std::string displayName;
	void (*func)(ItemInfo*);
	void operator()(ItemInfo*) const;

	ItemOp(std::string displayName, void (*func)(ItemInfo*))
		: displayName(std::move(displayName)), func(func) { }
};

class ItemInfo
{
public:
	virtual int getScore(const Student& student) const = 0;
	//virtual void ratingInitiate() = 0;
	virtual const std::vector<ItemOp>& getOps() const = 0;
	virtual int getCurrentItemType() const = 0;
	virtual void setParams(const char* paramStr) = 0;
	virtual std::string getParams() const = 0;
	// ratingToDbRepr(); // transform student grade information to db format
	// ratingFromDbRepr();
};

enum class CheckInType {
	NORMAL,
	LATE,
	ABSENT
};

class ItemAttendance : public ItemInfo {
private:
	StudentIdMap<std::vector<CheckInType>> studentAttendance;
	int sessionNumber = 0; // how many teaching sessions
	static std::vector<ItemOp> ops;
public:
	virtual int getScore(const Student& student) const;
	virtual const std::vector<ItemOp>& getOps() const;
	virtual int getCurrentItemType() const;
	virtual void setParams(const char* paramStr);
	virtual std::string getParams() const;
	int getSessionNumber() const;
};

class ItemManual : public ItemInfo {
private:
	StudentIdMap<int> studentScores;
	const StudentVector* students = nullptr;
	static std::vector<ItemOp> ops;
	friend void manualEnterGrade(ItemInfo* itemInfo);
	friend void manualUpdateGrade(ItemInfo* itemInfo);
	friend void initializeItemManual();
public:
	virtual int getScore(const Student& student) const;
	virtual const std::vector<ItemOp>& getOps() const;
	virtual int getCurrentItemType() const;
	virtual void setParams(const char* paramStr);
	virtual std::string getParams() const;
	ItemManual(const StudentVector* students);
	ItemManual() = default;
};

// void manualEnterGrade(ItemInfo* itemInfo);



void initializeItemAttendance();


#endif