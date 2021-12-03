#ifndef TOTAL_GRADE_H_
#define TOTAL_GRADE_H_

#include <variant>

namespace stt::grade_utils
{

	enum class SpecialGrade
	{
		T
	};

	using TotalGradeRepr = std::variant<double, SpecialGrade>;

	class TotalGrade
	{
	private:
		TotalGradeRepr repr;
	public:
		TotalGrade(const TotalGradeRepr& repr);
		TotalGrade(double x);
		TotalGrade(SpecialGrade x);
		TotalGrade() = default;
		TotalGrade operator+(const TotalGrade& other) const;
		TotalGrade operator-(const TotalGrade& other) const;
		TotalGrade operator*(const TotalGrade& other) const;
		TotalGrade operator/(const TotalGrade& other) const;
		TotalGrade& operator+=(const TotalGrade& other);
		TotalGrade& operator-=(const TotalGrade& other);
		TotalGrade& operator*=(const TotalGrade& other);
		TotalGrade& operator/=(const TotalGrade& other);
		bool operator==(const TotalGrade& other) const;
		std::string toString() const;
	};

	// visitor
	template <class NumOp>
	struct TotalGradeOp
	{
	private:
		NumOp _ins;
	public:
		constexpr TotalGradeRepr operator()(double a, double b)
		{
			return _ins(a, b);
		}
		constexpr TotalGradeRepr operator()(SpecialGrade a, SpecialGrade b)
		{
			return a;
		}
		constexpr TotalGradeRepr operator()(double a, SpecialGrade b)
		{
			return b;
		}
		constexpr TotalGradeRepr operator()(SpecialGrade a, double b)
		{
			return a;
		}
	};

	struct TotalGradeToString
	{
		std::string operator()(double x);

		std::string operator()(SpecialGrade x);
	};
}

#endif
