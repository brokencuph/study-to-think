#ifndef STAT_UTILS_H_
#define STAT_UTILS_H_

#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <QString>

namespace stt::stat_utils
{
	template <class ForIt>
	double averageScore(ForIt it1, ForIt it2)
	{
		double sum = 0.0;
		size_t cnt = 0;
		for (auto it = it1; it != it2; it++)
		{
			cnt++;
			sum += *it;
		}
		return sum / cnt;
	}

	template <class ForIt>
	QString qAverageScore(ForIt it1, ForIt it2)
	{
		std::ostringstream ss;
		ss << std::fixed << std::setprecision(2)
			<< averageScore(it1, it2);
		return QString::fromStdString(ss.str());
	}
}

#endif