#include "grading_scheme.h"
// grading_scheme.cpp: Definition of rating items.

void ItemOp::operator()(ItemInfo* info) const
{
	this->func(info);
}
