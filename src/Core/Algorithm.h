/*******************************************************************************************************
 PIE is the Page Image Explorer developed at CVL/TU Wien for the EU project READ.

 Copyright (C) 2018 Markus Diem <diem@caa.tuwien.ac.at>
 Copyright (C) 2018 Florian Kleber <kleber@caa.tuwien.ac.at>

 This file is part of PIE.

 ReadFramework is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 ReadFramework is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

 The READ project  has  received  funding  from  the European  Union’s  Horizon  2020
 research  and innovation programme under grant agreement No 674943

 related links:
 [1] https://cvl.tuwien.ac.at/
 [2] https://transkribus.eu/Transkribus/
 [3] https://github.com/TUWien/
 [4] https://nomacs.org
 *******************************************************************************************************/

#pragma once

#pragma warning(push, 0)	// no warnings from includes
#include <QString>

#include <opencv2/core.hpp>
#pragma warning(pop)

#pragma warning (disable: 4251)	// inlined Qt functions in dll interface

#ifndef DllExport
#ifdef DLL_CORE_EXPORT
#define DllExport Q_DECL_EXPORT
#else
#define DllExport Q_DECL_IMPORT
#endif
#endif

// Qt defines

namespace pie {

// pie defines

namespace Math {

	/// <summary>
	/// Computes robust statistical moments (quantiles).
	/// </summary>
	/// <param name="valuesIn">The statistical set (samples).</param>
	/// <param name="momentValue">The statistical moment value (0.5 = median, 0.25 and 0.75 = quartiles).</param>
	/// <param name="interpolated">A flag if the value should be interpolated if the length of the list is even.</param>
	/// <returns>The statistical moment.</returns>
	template <typename numFmt>
	static double statMoment(const QList<numFmt>& valuesIn, double momentValue, bool interpolated = true) {

		// no stat moment if we have 1 value
		if (valuesIn.size() == 1)
			return valuesIn[0];

		// return mean if we have two & interpolation is turned on
		if (valuesIn.size() == 2 && interpolated) {
			return (valuesIn[0] + valuesIn[1]) / 2.0;
		}
		else if (valuesIn.size() == 2)
			return valuesIn[0];

		QList<numFmt> values = valuesIn;
		qSort(values);

		size_t lSize = values.size();
		double moment = -1;
		unsigned int momIdx = cvCeil(lSize*momentValue);
		unsigned int idx = 1;

		// find the statistical moment
		for (auto val : values) {

			// skip
			if (idx < momIdx) {
				idx++;
				continue;
			}

			// compute mean between this and the next element
			if (lSize % 2 == 0 && momIdx < lSize && interpolated)
				moment = ((double)val + values[idx + 1])*0.5;
			else
				moment = (double)val;
			break;
		}

		return moment;
	}
}

}