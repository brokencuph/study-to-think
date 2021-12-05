#include <functional>
#include <sstream>
#include <iomanip>
#include "total_grade.h"

namespace stt::grade_utils
{
	TotalGrade TotalGrade::operator+(const TotalGrade& other) const
	{
		return std::visit(TotalGradeOp<std::plus<double>>(), this->repr, other.repr);
	}

	TotalGrade TotalGrade::operator-(const TotalGrade& other) const
	{
		return std::visit(TotalGradeOp<std::minus<double>>(), this->repr, other.repr);
	}

	TotalGrade TotalGrade::operator*(const TotalGrade& other) const
	{
		return std::visit(TotalGradeOp<std::multiplies<double>>(), this->repr, other.repr);
	}

	TotalGrade TotalGrade::operator/(const TotalGrade& other) const
	{
		return std::visit(TotalGradeOp<std::divides<double>>(), this->repr, other.repr);
	}

	TotalGrade& TotalGrade::operator+=(const TotalGrade& other)
	{
		return *this = *this + other;
	}

	TotalGrade& TotalGrade::operator-=(const TotalGrade& other)
	{
		return *this = *this - other;
	}

	TotalGrade& TotalGrade::operator*=(const TotalGrade& other)
	{
		return *this = *this * other;
	}

	TotalGrade& TotalGrade::operator/=(const TotalGrade& other)
	{
		return *this = *this / other;
	}

	bool TotalGrade::operator==(const TotalGrade& other) const
	{
		return repr == other.repr;
	}

	std::string TotalGrade::toString() const
	{
		return std::visit(TotalGradeToString(), repr);
	}

	double TotalGrade::toDouble() const
	{
		if (repr.index() == 0)
		{
			return std::get<double>(repr);
		}
		else
		{
			return -(int)std::get<SpecialGrade>(repr);
		}
	}

	TotalGrade::TotalGrade(const TotalGradeRepr& repr)
		: repr(repr)
	{

	}
	TotalGrade::TotalGrade(double x)
		: repr(x)
	{

	}
	TotalGrade::TotalGrade(SpecialGrade x)
		: repr(x)
	{
	}
	std::string TotalGradeToString::operator()(double x)
	{
		std::stringstream ss;
		ss << std::fixed << std::setprecision(2) << x;
		return ss.str();
	}
	std::string TotalGradeToString::operator()(SpecialGrade x)
	{
		return "T"; // currently only one SpecialGrade considered
	}
}
