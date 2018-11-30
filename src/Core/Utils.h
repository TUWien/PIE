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
#include <QSharedPointer>
#include <QSettings>
#include <QTime>

#include <opencv2/core.hpp>
#pragma warning(pop)

#pragma warning (disable: 4251)	// inlined Qt functions in dll interface
#pragma warning (disable: 4714)	// force inline

#ifndef DllExport
#ifdef DLL_CORE_EXPORT
#define DllExport Q_DECL_EXPORT
#else
#define DllExport Q_DECL_IMPORT
#endif
#endif

// converts a version (e.g. 3.1.0) to a comparable int
#define PIE_VERSION(major, minor, revision) (major << 16 | minor << 8 | revision)
#define PIE_OPENCV_VERSION PIE_VERSION(CV_MAJOR_VERSION, CV_MINOR_VERSION, CV_VERSION_REVISION)

// Qt defines
class QSettings;

namespace pie {	

// some basic defines - yes, we try to not create too many macros...
#define DEG2RAD		0.017453292519943
#define RAD2DEG 	57.295779513082323

#define WHO_IS_CUTE "Anna"

// read defines
class DllExport Utils {

public:
	static Utils& instance();
	
	void initFramework() const;
	void registerVersion() const;
	static int versionToInt(char major, char minor, char revision);
	static double rand();

	static bool hasGui();

	static bool loadToBuffer(const QString& filePath, QByteArray& ba);
	static QString appDataPath();
	static QString createFilePath(const QString& filePath, const QString& attribute, const QString& newSuffix = QString());
	static QString timeStampFileName(const QString& attribute = "", const QString& suffix = ".txt");
	static QString baseName(const QString& filePath);
	static QString colorToString(const QColor& col);

	static QJsonObject readJson(const QString& filePath);
	static int64 writeJson(const QString& filePath, const QJsonObject& jo);
	static void initDefaultFramework();

	/// <summary>
	/// Returns true if the filePath contains a valid file.
	/// Symbolic links are resolved
	/// </summary>
	/// <param name="filePath">The file path.</param>
	/// <returns>True if the extension is valid.</returns>
	static bool isValidFile(const QString& filePath);

	static QPixmap dragScreenShot(QWidget* w, double opacity = 0.5);

	// little number thingies
	template<typename num>
	static num clamp(num val, num min, num max) {
	
		if (val < min)
			val = min;
		if (val > max)
			val = max;

		return val;
	}

private:
	Utils();
	Utils(const Utils&);
};

class DllExport Converter {

public:
	static QPolygon stringToPoly(const QString& pointList);
	static QString polyToString(const QPolygon& poly);

	static QPointF cvPointToQt(const cv::Point& pt);
	static cv::Point2d qPointToCv(const QPointF& pt);

	static QRectF cvRectToQt(const cv::Rect& r);
	static cv::Rect2d qRectToCv(const QRectF& r);

};

/**
* A small class which measures the time.
* This class is designed to measure the time of a method, especially
* intervals and the total time can be measured.
**/
class DllExport Timer {

public:

	/**
	* Initializes the class and stops the clock.
	**/
	Timer();

	friend DllExport QDataStream& operator<<(QDataStream& s, const Timer& t);
	friend DllExport QDebug operator<< (QDebug d, const Timer &t);

	QString getTotal() const;
	virtual QDataStream& put(QDataStream& s) const;
	QString stringifyTime(int ct) const;
	int elapsed() const;
	void start();

protected:
	QTime mTimer;
};

namespace ColorManager {

	DllExport QColor randColor(double alpha = 1.0);
	DllExport QColor getColor(int idx, double alpha = 1.0);
	DllExport QVector<QColor> colors();

	DllExport QColor lightGray(double alpha = 1.0);
	DllExport QColor darkGray(double alpha = 1.0);
	DllExport QColor red(double alpha = 1.0);
	DllExport QColor green(double alpha = 1.0);
	DllExport QColor blue(double alpha = 1.0);
	DllExport QColor pink(double alpha = 1.0);
	DllExport QColor white(double alpha = 1.0);
	DllExport QColor black(double alpha = 1.0);
	DllExport QColor background(double alpha = 1.0);
	DllExport QColor foreground(double alpha = 1.0);

	DllExport QColor alpha(const QColor& col, double a);

	DllExport QPixmap colorizePixmap(const QPixmap& pm, const QColor& col, double opacity = 1.0);

	// add your favorite colors here
}

/// <summary>
/// Flags turns enums into typesave flags
/// It is strongly related (copied) from 
/// Useage:
/// 	enum mDrawFlags {
///			draw_none				= 0x00,
///			draw_ellipse			= 0x01,
///			draw_stats				= 0x02,
///			draw_center				= 0x04,
///			};
///		typedef Flags<mDrawFlags> DrawFlags;
///
/// http://stackoverflow.com/questions/1448396/how-to-use-enums-as-flags-in-c/33971769#33971769
/// thanks @Fabio A.
/// </summary>
template <typename EnumType, typename Underlying = int>
class Flags {
    typedef Underlying Flags::* RestrictedBool;

public:
	Flags() : mFlags(Underlying()) {}

    Flags(EnumType f) :
        mFlags(1 << f) {}

    Flags(const Flags& o):
        mFlags(o.mFlags) {}

    Flags& operator |=(const Flags& f) {
        mFlags |= f.mFlags;
        return *this;
    }

    Flags& operator &=(const Flags& f) {
        mFlags &= f.mFlags;
        return *this;
    }

    friend Flags operator |(const Flags& f1, const Flags& f2) {
        return Flags(f1) |= f2;
    }

    friend Flags operator &(const Flags& f1, const Flags& f2) {
        return Flags(f1) &= f2;
    }

    Flags operator ~() const {
        Flags result(*this);
        result.mFlags = ~result.mFlags;
        return result;
    }

    operator RestrictedBool() const {
		return mFlags ? &Flags::mFlags : 0;
    }

    Underlying value() const {
        return mFlags;
    }

protected:
    Underlying mFlags;
};

}
