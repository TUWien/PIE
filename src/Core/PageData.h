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
#pragma warning(pop)

#ifndef DllCoreExport
#ifdef DLL_CORE_EXPORT
#define DllCoreExport Q_DECL_EXPORT
#else
#define DllCoreExport Q_DECL_IMPORT
#endif
#endif

#pragma warning(disable: 4251)  // dll-interface

// Qt defines
class QSettings;

namespace pie {	

class DllCoreExport Region : public BaseElement {

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

	Region(Type type = type_unknown, const QSize& s = QSize());

	static Region fromJson(const QJsonObject& jo);

private:
	QSize mSize;
	Type mType;
};

class DllCoreExport ImageData : public BaseElement {

public:
	ImageData();

	static ImageData fromJson(const QJsonObject& jo);

private:
	QString mFileName;
	QSize mSize;
};

class DllCoreExport PageData : public BaseElement {

public:
	PageData();

	static PageData fromJson(const QJsonObject& jo);

private:
	QString mXmlFilePath;
	QString mContent;
	ImageData mImg;

	QVector<Region> mRegions;
};

class DllCoreExport Collection {

public:
	Collection();

	static Collection fromJson(const QJsonObject& jo);

	int size() const;

private:
	QVector<PageData> mPages;
};

}
