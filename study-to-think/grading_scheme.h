// grading_scheme.h: Definition of rating items.

#ifndef GRADING_SCHEME_H
#define GRADING_SCHEME_H

#include <memory>
#include <map>
#include <vector>
#include "student.h"
#include "db_access.h"
#include "total_grade.h"

using namespace stt::grade_utils;

class Student;

class ItemInfo;

class StudentGradeDBO;

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
	virtual void setItemName(std::string itemName) = 0;
	virtual std::string getItemName() const = 0;
	virtual TotalGrade getScore(const Student& student) const = 0;
	//virtual void ratingInitiate() = 0;
	virtual const std::vector<ItemOp>& getOps() const = 0;
	virtual int getCurrentItemType() const = 0;
	virtual void setParams(const char* paramStr) = 0;
	virtual std::string getParams() const = 0;
	// ratingToDbRepr(); // transform student grade information to db format
	// ratingFromDbRepr();
	virtual void setStudents(const std::vector<Student>* stus) = 0;
	virtual void fillScoreFromDb(const std::vector<StudentGradeDBO>& vec) = 0;
	static std::string typeNames[];
};

enum class CheckInType {
	NORMAL,
	LATE,
	LEAVEEARLY,
	ABSENT
};

extern const char* checkInTypeNames[];
extern const std::map<std::string, CheckInType> checkInTypeIds;

class ItemAttendance : public ItemInfo {
private:
	std::string itemName;
	const StudentVector* students = nullptr;
	int sessionNumber = 0; // how many teaching sessions
	static std::vector<ItemOp> ops;
public:
	StudentIdMap<std::vector<CheckInType>> studentAttendance;
	void setItemName(std::string itemName) override;
	std::string getItemName() const override;
	TotalGrade getScore(const Student& student) const override;
	const std::vector<ItemOp>& getOps() const override;
	int getCurrentItemType() const override;
	void setParams(const char* paramStr) override;
	std::string getParams() const override;
	void setStudents(const std::vector<Student>* stus) override;
	void fillScoreFromDb(const std::vector<StudentGradeDBO>& vec) override;
	int getSessionNumber() const;
	void modifySessionNumber(int newSessionNumber) ;
	static std::string scoreRepr(const std::vector<CheckInType>&);
};

class ItemManual : public ItemInfo {
private:
	std::string itemName;
	const StudentVector* students = nullptr;
	static std::vector<ItemOp> ops;
	friend void manualEnterGrade(ItemInfo* itemInfo);
	friend void manualUpdateGrade(ItemInfo* itemInfo);
	friend void initializeItemManual();
public:
	StudentIdMap<int> studentScores;
	void setItemName(std::string itemName) override;
	std::string getItemName() const override;
	TotalGrade getScore(const Student& student) const override;
	const std::vector<ItemOp>& getOps() const override;
	int getCurrentItemType() const override;
	void setParams(const char* paramStr) override;
	std::string getParams() const override;
	void setStudents(const std::vector<Student>* stus) override;
	void fillScoreFromDb(const std::vector<StudentGradeDBO>& vec) override;
	ItemManual(const StudentVector* students);
	ItemManual() = default;
};

// void manualEnterGrade(ItemInfo* itemInfo);

class AbstractStudentScore
{
public:
	virtual std::string serialize() const = 0;
	static std::unique_ptr<AbstractStudentScore> fromString(int type, const std::string& str);
};

class AttendanceScore : public AbstractStudentScore
{
public:
	explicit AttendanceScore(const std::string& str);
	virtual std::string serialize() const;
};

class ManualScore : public AbstractStudentScore
{
private:
	int score;
public:
	explicit ManualScore(const std::string& str);
	virtual std::string serialize() const;
};

class StudentGradeDBO
{
public:
	static const char* db_TableName;
	static const char* db_ColumnNames;
	static const char* db_UpsertColumn;

	StudentIdType studentId;
	std::string itemName;
	//std::unique_ptr<AbstractStudentScore> score;
	std::string scoreRepr;

	std::string getDbTuple() const;

	static int selectCallback(void*, int, char**, char**);
};

void initializeItemAttendance();


#endif