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

#include "BasePageElement.h"

#pragma warning(push, 0)	// no warnings from includes
#include <QVector>
#include <QSize>

#include <functional>
#pragma warning(pop)

#ifndef DllExport
#ifdef DLL_CORE_EXPORT
#define DllExport Q_DECL_EXPORT
#else
#define DllExport Q_DECL_IMPORT
#endif
#endif

#pragma warning(disable: 4251)  // dll-interface

// Qt defines
class QSettings;

namespace pie {	

class DllExport Region : public BaseElement {

public:
	enum Type {
		type_unknown = 0,
		type_root,
		type_table_region,
		type_table_cell,
		type_text_region,
		type_text_line,
		type_word,
		type_separator,
		type_image,
		type_graphic,
		type_chart,
		type_noise,
		type_border,

		type_end
	};
	enum Property {
		p_width = 0,
		p_height,
		p_area,

		prop_end
	};

	Region(Type type = type_unknown, const QSize& s = QSize());

	QSize size() const;
	
	// properties
	double area() const;
	double width() const;
	double height() const;

	static Region fromJson(const QJsonObject& jo);

private:
	QSize mSize;
	Type mType;
};

class DllExport ImageData : public BaseElement {

public:
	ImageData();

	QString name() const;

	static ImageData fromJson(const QJsonObject& jo);

private:
	QString mFileName;
	QSize mSize;
};

class DllExport PageData : public BaseElement {

public:
	PageData();

	int numRegions() const;
	QVector<QSharedPointer<Region> > regions() const;
	QString name() const;

	double averageRegion(std::function<double(const Region&)> prop) const;

	static PageData fromJson(const QJsonObject& jo);

private:
	QString mXmlFilePath;
	QString mContent;
	ImageData mImg;

	QVector<QSharedPointer<Region> > mRegions;
};

class DllExport Collection {

public:
	Collection(const QString& name = "");

	static Collection fromJson(const QJsonObject& jo, const QString& name);

	bool isEmpty();

	int size() const;
	QString name() const;
	QVector<QSharedPointer<PageData> > pages() const;

private:
	QString mName;
	QVector<QSharedPointer<PageData> > mPages;
};

}
