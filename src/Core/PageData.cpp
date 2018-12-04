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
#include "Utils.h"

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

	ImageData PageData::image() const {
		return mImg;
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

	QString PageData::text() const {
		return mContent;
	}

	PageData PageData::fromJson(const QJsonObject & jo) {

		PageData pd;
		pd.mXmlFilePath = jo.value("xmlName").toString();
		pd.mContent = jo.value("content").toString();
		pd.mCollectionName = jo.value("collection").toString();
		pd.mDocumentName = jo.value("document").toString();
		pd.mImg = ImageData::fromJson(jo);

		QJsonArray regions = jo.value("regions").toArray();
		for (auto r : regions)
			pd.mRegions << QSharedPointer<Region>::create(Region::fromJson(r.toObject()));

		return pd;
	}
	
	// -------------------------------------------------------------------- ImageData 
	ImageData::ImageData() {
	}

	QString ImageData::name() const {
		return mFileName;
	}

	int ImageData::width() const {
		return mSize.width();
	}

	int ImageData::height() const {
		return mSize.height();
	}

	ImageData ImageData::fromJson(const QJsonObject & jo) {
		
		ImageData id;

		id.mFileName = jo.value("imgName").toString();

		int w = jo.value("width").toInt(0);
		int h = jo.value("height").toInt(0);

		id.mSize = QSize(w, h);

		return id;
	}

	// -------------------------------------------------------------------- BaseCollection 
	BaseCollection::BaseCollection(const QString& name) {
		mName = name;
	}
	
	QString BaseCollection::name() const {
		return mName;
	}

	void BaseCollection::setColor(const QColor & col) {
		mColor = col;
	}

	QColor BaseCollection::color() const {
		return mColor;
	}

	// -------------------------------------------------------------------- Document 
	Document::Document(const QString & name) : BaseCollection(name) {
	}

	bool Document::isEmpty() const {
		return mPages.isEmpty();
	}

	int Document::numPages() const {
		return mPages.size();
	}
	QVector<QSharedPointer<PageData> > Document::pages() const {
		return mPages;
	}

	Document Document::fromJson(const QJsonObject & jo) {

		Document d(jo["name"].toString());
		d.setColor(ColorManager::randColor());

		QJsonArray entities = jo.value("pages").toArray();
		for (auto p : entities)
			d.mPages << QSharedPointer<PageData>::create(PageData::fromJson(p.toObject()));

		return d;
	}

	// -------------------------------------------------------------------- Collection 
	Collection::Collection(const QString& name) : BaseCollection(name) {
	}

	Collection Collection::fromJson(const QJsonObject & jo, const QString& name) {

		Collection c(name);

		QJsonArray entities = jo.value("documents").toArray();
		for (auto p : entities)
			c.mDocuments << QSharedPointer<Document>::create(Document::fromJson(p.toObject()));

		return c;

	}

	bool Collection::isEmpty() const {
		return mDocuments.isEmpty();
	}

	int Collection::numPages() const {
		
		int s = 0;
		for (auto d : mDocuments)
			s += d->numPages();
		
		return s;
	}

	int Collection::numDocuments() const {
		return mDocuments.size();
	}

	QVector<QSharedPointer<PageData>> Collection::pages() const {

		QVector<QSharedPointer<PageData> > ps;

		for (auto d : mDocuments)
			ps << d->pages();

		return ps;
	}

	QVector<QSharedPointer<Document>> Collection::documents() const {
		return mDocuments;
	}

	QString Collection::toString() const {

		int nr = numRegions();
		int nt = numTextPages();

		QString msg;
		msg += QString::number(numPages()) + " pages found in " + QString::number(numDocuments()) + " documents\n";
		msg += QString::number(numDocuments()) + " documents\n";
		msg += QString::number(nr) + " regions (" + QString::number((double)nr / numPages()) + " per page)\n";
		msg += QString::number(nt) + " pages with text";

		return msg;
	}

	int Collection::numRegions() const {

		int nr = 0;
		for (auto p : pages())
			nr += p->numRegions();

		return nr;
	}

	int Collection::numTextPages() const {

		int ntp = 0;
		for (auto p : pages()) {
			if (!p->text().isEmpty())
				ntp++;
		}

		return ntp;
	}

}