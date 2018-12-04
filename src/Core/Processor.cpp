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

#include "Processor.h"

#pragma warning(push, 0)	// no warnings from includes
#include <QDebug>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#pragma warning(pop)

namespace pie {

	double cmp::numRegions(const PageData * pd) {
	
		assert(pd);
		return pd->numRegions();
	}

	bool test::Processor(const Collection& c) {

		auto widths		= [&](const Region& r) { return r.width(); };
		auto heights	= [&](const Region& r) { return r.height(); };
		auto areas		= [&](const Region& r) { return r.area(); };

		for (auto p : c.pages()) {
			double w = p->averageRegion(widths);
			double h = p->averageRegion(heights);
			double s = p->averageRegion(areas);

			qDebug() << p->name() << "[" << w << "x" << h << "]" << s;
		}

		return true;
	}
	
	// -------------------------------------------------------------------- DisplayConverter 
	DisplayConverter::DisplayConverter(QSharedPointer<Collection> Collection) {
		mCollection = Collection;
	}

	cv::Mat DisplayConverter::map(QSharedPointer<AbstractMapper> mapper) const {
		return mapper->process(mCollection.data());
	}

	// -------------------------------------------------------------------- Mapper 
	AbstractMapper::AbstractMapper() {
	}

	QSharedPointer<AbstractMapper> AbstractMapper::create(const Type& type) {

		switch (type) {
		case m_reg_width:	return QSharedPointer<WidthMapper>::create();
		case m_reg_height:	return QSharedPointer<HeightMapper>::create();
		case m_reg_area:	return QSharedPointer<AreaMapper>::create();
		case m_img_width:	return QSharedPointer<PageWidthMapper>::create();
		case m_img_height:	return QSharedPointer<PageHeightMapper>::create();
		}

		if (type != m_undefined)
			qWarning() << "[AbstractMapper] unknown type:" << type;
		return QSharedPointer<AbstractMapper>();
	}

	AbstractMapper::Type AbstractMapper::type() const {
		return mType;
	}

	QString AbstractMapper::name() const {
		return mName;
	}

	// -------------------------------------------------------------------- RegionMapper 
	cv::Mat RegionMapper::process(Collection * c) const {

		if (!c) {
			qWarning() << "cannot process empty Collection";
			return cv::Mat();
		}

		std::function<double(const Region&)> fr = processor();

		// OpenGL only knows floats
		cv::Mat dv(1, c->numPages(), CV_32FC1);
		float* px = dv.ptr<float>();

		for (auto p : c->pages()) {

			*px = (float)p->averageRegion(fr);
			px++;
		}

		cv::normalize(dv, dv, 0, 1, cv::NORM_MINMAX);
		dv *= 2.0f;
		dv -= 1.0f;

		return dv;
	}

	// -------------------------------------------------------------------- PageMapper 
	cv::Mat PageMapper::process(Collection * c) const {

		if (!c) {
			qWarning() << "cannot process empty Collection";
			return cv::Mat();
		}

		std::function<double(const PageData&)> pr = processor();

		// OpenGL only knows floats
		cv::Mat dv(1, c->numPages(), CV_32FC1);
		float* px = dv.ptr<float>();

		for (auto p : c->pages()) {

			*px = (float)pr(*p);
			px++;
		}

		// map to screen
		cv::normalize(dv, dv, 0, 1, cv::NORM_MINMAX);
		dv *= 2.0f;
		dv -= 1.0f;

		return dv;
	}


	// -------------------------------------------------------------------- WidthMapper 
	WidthMapper::WidthMapper() {
		mName = QObject::tr("Region Width");
		mType = m_reg_width;
	}

	std::function<double(const Region&)> WidthMapper::processor() const {
		return [&](const Region& r) { return r.width(); };
	}

	// -------------------------------------------------------------------- HeightMapper 
	HeightMapper::HeightMapper() {
		mName = QObject::tr("Region Height");
		mType = m_reg_height;
	}
	std::function<double(const Region&)> HeightMapper::processor() const {
		return [&](const Region& r) { return r.height(); };
	}

	// -------------------------------------------------------------------- AreaMapper 
	AreaMapper::AreaMapper() {
		mName = QObject::tr("Region Area");
		mType = m_reg_area;
	}

	std::function<double(const Region&)> AreaMapper::processor() const {
		return [&](const Region& r) { return r.area(); };
	}

	// -------------------------------------------------------------------- PageWidthMapper 
	PageWidthMapper::PageWidthMapper() {
		mName = QObject::tr("Image Width");
		mType = m_img_width;
	}

	std::function<double(const PageData&)> PageWidthMapper::processor() const {
		return [&](const PageData& pd) { return pd.image().width(); };
	}

	// -------------------------------------------------------------------- PageHeightMapper 
	PageHeightMapper::PageHeightMapper() {
		mName = QObject::tr("Image Height");
		mType = m_img_height;
	}

	std::function<double(const PageData&)> PageHeightMapper::processor() const {
		return [&](const PageData& pd) { return pd.image().height(); };
	}
}

 