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
};

class ItemInfo
{
public:
	virtual int getScore(const Student& student) const = 0;
	virtual void ratingInitiate() = 0;
	virtual const std::vector<std::unique_ptr<ItemInfo>>& getOps() const = 0;
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
	static std::vector<std::unique_ptr<ItemInfo>> ops;
public:
	virtual int getScore(const Student& student) const;
	virtual const std::vector<std::unique_ptr<ItemInfo>>& getOps() const;
};

class ItemManual : public ItemInfo {
private:
	StudentIdMap<int> studentScores;
public:
	virtual int getScore(const Student& student) const;
	virtual const std::vector<std::unique_ptr<ItemInfo>>& getOps() const;
};