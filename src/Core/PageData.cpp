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

#include "PageData.h"
#include "Algorithm.h"

#pragma warning(push, 0)	// no warnings from includes
#include <QJsonObject>
#include <QJsonArray>
#include <QSharedPointer>
#include <QDebug>
#pragma warning(pop)

namespace pie {
	
	// -------------------------------------------------------------------- Region 
	Region::Region(Type type, const QSize & s) {
		mType = type;
		mSize = s;
	}

	QSize Region::size() const {
		return mSize;
	}

	double Region::area() const {
		return mSize.width()*mSize.height();
	}

	double Region::width() const {
		return mSize.width();
	}

	double Region::height() const {
		return mSize.height();
	}

	Region Region::fromJson(const QJsonObject & jo) {

		Region r;
		r.mType = (Type)jo.value("type").toInt(0);

		int w = jo.value("width").toInt(0);
		int h = jo.value("height").toInt(0);

		r.mSize = QSize(w, h);
		
		return r;
	}

	// -------------------------------------------------------------------- PageData 
	PageData::PageData() {
	}

	int PageData::numRegions() const {
		return mRegions.size();
	}

	double PageData::averageRegion(std::function<double(const Region&)> prop) const {

		QList<double> sizes;
		for (const auto r : regions()) {
			sizes << prop(*r);
		}

		return Math::statMoment(sizes, 0.5);
	}
	
	QVector<QSharedPointer<Region>> PageData::regions() const {
		return mRegions;
	}

	QString PageData::name() const {
		return mImg.name();
	}

	PageData PageData::fromJson(const QJsonObject & jo) {

		PageData pd;
		pd.mXmlFilePath = jo.value("xmlName").toString();
		pd.mContent = jo.value("content").toString();
		pd.mImg = ImageData::fromJson(jo);

		QJsonArray regions = jo.value("regions").toArray();
		for (auto r : regions)
			pd.mRegions << QSharedPointer<Region>::create(Region::fromJson(r.toObject()));

		return pd;
	}

	// -------------------------------------------------------------------- Collection 
	Collection::Collection() {
	}

	Collection Collection::fromJson(const QJsonObject & jo) {

		Collection c;

		QJsonArray pages = jo.value("imgs").toArray();
		for (auto p : pages)
			c.mPages << QSharedPointer<PageData>::create(PageData::fromJson(p.toObject()));

		return c;

	}

	int Collection::size() const {
		return mPages.size();
	}

	QVector<QSharedPointer<PageData>> Collection::pages() const {
		return mPages;
	}
	
	// -------------------------------------------------------------------- ImageData 
	ImageData::ImageData() {
	}

	QString ImageData::name() const {
		return mFileName;
	}

	ImageData ImageData::fromJson(const QJsonObject & jo) {
		
		ImageData id;

		id.mFileName = jo.value("imgName").toString();

		int w = jo.value("width").toInt(0);
		int h = jo.value("height").toInt(0);

		id.mSize = QSize(w, h);

		return id;
	}
}