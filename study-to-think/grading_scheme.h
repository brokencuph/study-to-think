// grading_scheme.h: Definition of rating items.

#include <memory>
#include <map>
#include <vector>
#include "student.h"

class ItemInfo;


class RatingItem
{
	int weight; // in percent
	std::unique_ptr<ItemInfo> item;
	// toDbRepr();
	// fromDbRepr();
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
	int sessionNumber; // how many teaching sessions
	static std::vector<ItemOp> ops;
public:
	virtual int getScore(const Student& student) const;
	virtual const std::vector<ItemOp>& getOps() const;
};

class ItemManual : public ItemInfo {
private:
	StudentIdMap<int> studentScores;
	const StudentVector* students;
	static std::vector<ItemOp> ops;
	friend void manualEnterGrade(ItemInfo* itemInfo);
	friend void manualUpdateGrade(ItemInfo* itemInfo);
	friend void initializeItemManual();
public:
	virtual int getScore(const Student& student) const;
	virtual const std::vector<ItemOp>& getOps() const;
	ItemManual(const StudentVector* students);
};

// void manualEnterGrade(ItemInfo* itemInfo);



void initializeItemAttendance();

