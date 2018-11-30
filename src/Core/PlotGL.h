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

#include "PageData.h"
#include "BasePlot.h"
#include "Plot.h"

#pragma warning(push, 0)	// no warnings from includes
#include <QWidget>
#include <QOpenGLWidget>
#include <QAction>
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
class DotPlotParams;
class DotPlot;

namespace pie {

	class DllExport DotViewPort : public QOpenGLWidget {
		Q_OBJECT

	public:
		DotViewPort(DotPlotParams* params, DotPlot* parent = 0);
	virtual ~DotViewPort() {}

		void moveView(const QPointF& dxy);
		void zoom(float factor, const QPointF& center = QPointF());

		void setSelected(bool selected);

	signals:

	public slots:
		//virtual void setAxisIndex(const QPoint& dims);
		void resetView();
		void zoomIn();
		void zoomOut();

	protected:
		virtual void initializeGL();
		virtual void resizeGL(int w, int h);
		virtual void paintGL();

		void applyGLTransform(bool apply = true);

		virtual bool drawGL();
		virtual bool drawPoints();
		//bool drawPointsLabels(const cv::Mat& data, QSharedPointer<DkFcsData> mFcs) const;
		//bool drawPointsSelection(const cv::Mat& data, const DkSelectionModel& model) const;

		// annotations
		//void drawSelection(QPainter& p) const;
		void drawEmpty(QPainter& p);
		//void drawSunSystem(QPainter& p, DkSolarSystem* system) const;
		//void drawSolarSystemGL(DkSolarSystem* system) const;
		void drawDimArrows(QPainter& p, const QPoint& axisDims) const;
		void drawArrow(QPainter& p, const QPoint& start, const QPoint& end, double angle) const;
		//QString mapMouseCoords(const QPoint& coords) const;

		void mouseMoveEvent(QMouseEvent *ev);
		void mousePressEvent(QMouseEvent *ev);
		void mouseReleaseEvent(QMouseEvent *ev);
		void wheelEvent(QWheelEvent *ev);
		QPointF toGLCoords(const QPoint& p) const;

		bool parentHasFocus() const;
		
		void map(QPainter& painter) const;

		QPoint mFirstMousePos;
		QPoint mLastMousePos;
		QSharedPointer<Collection> mCollection;

		//DkSolarSystem* mSystem = 0;
		DotPlotParams* mP;
		DotPlot* mParent;

		bool mIsSelected = false;

		//QSharedPointer<DkSelection> mActiveSelection;
	};

}