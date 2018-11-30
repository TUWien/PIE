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
#include "BaseWidgets.h"

#pragma warning(push, 0)	// no warnings from includes
#include <QWidget>
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
class QGridLayout;
class QMimeData;

namespace pie {

	// pie defines

	class DllExport PlotParams : public QObject {
		Q_OBJECT

	public:
		PlotParams(QObject* parent, bool isGlobal = false);
		virtual ~PlotParams() {}

		virtual void save(QSettings& settings) const;
		virtual void load(QSettings& settings);

		virtual void connectWith(PlotParams* params);
		virtual void copyTo(PlotParams* o) const;

		QPoint axisIndex() const;
		QString xAxisName() const;
		QString yAxisName() const;

		QTransform& worldMatrix();
		void setWorldMatrix(const QTransform& worldMatrix);

		QTransform& viewMatrix();
		void setViewMatrix(const QTransform& viewMatrix);

		QSize minimumSize() const;
		int numColumns() const;

		// the mode is needed for creation
		enum Mode {
			m_generic,
			m_hist,
			m_dot_plot,

			m_end,
		};

		Mode mode() const;
		void setMode(const Mode& mode);

	public slots:
		void setAxisIndex(const QPoint& axisIndex);
		void setXAxisName(const QString& axisName);
		void setYAxisName(const QString& axisName);
		void setMinimumSize(const QSize& size);
		void setNumColumns(int numColumns);
		void increaseNumColumns();
		void decreaseNumColumns();

	signals:
		void axisIndexChanged(const QPoint& axisIndex = QPoint()) const;
		void worldMatrixChanged(const QTransform& worldMatrix = QTransform()) const;
		void viewMatrixChanged(const QTransform& viewMatrix = QTransform()) const;
		void showAxisLineChanged(bool show = true);
		void showLabelChanged(bool show = true) const;
		void showTickChanged(bool show = true) const;
		void minimumSizeChanged(const QSize& size = QSize()) const;
		void numColumnsChanged(int numColumns) const;
		void loadPlotsSignal(const QString& name) const;
		void savePlotsSignal(const QString& name) const;

	protected:

		QPoint mAxisIndex = QPoint(-1, -1);
		QString mXAxisName;
		QString mYAxisName;

		QTransform mWorldMatrix;
		QTransform mViewMatrix;

		QSize mMinimumSize = QSize(0, 0);	// deprecated
		int mNumColumns = 5;

		Mode mMode = m_generic;
	};

	class DllExport BasePlot : public Widget {
		Q_OBJECT

	public:
		BasePlot(QWidget* parent = 0);
		virtual ~BasePlot();

		bool isFullScreen() const;
		virtual void setFullScreen(bool fullScreen);

		bool isSelected() const;
		virtual void setSelected(bool selected);

		//virtual QPoint axisIndex() const = 0;
		//virtual DkPlotParams* params() const = 0;

	public slots:
		virtual void setAxisIndex(const QPoint& index) = 0;
		virtual void closeRequested() const;

	signals:
		void closeSignal() const;
		void fullScreenSignal() const;
		void clearSelectionSignal() const;
		void startShiftSelectionSignal() const;
		void shiftSelectionSignal(bool selected) const;
		void updateLayoutSignal() const;
		void movePlot(size_t src, size_t dst) const;

	protected:
		virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
		virtual void mousePressEvent(QMouseEvent *event) override;
		virtual void mouseReleaseEvent(QMouseEvent *event) override;

		bool mSelected = false;
		bool mFullScreen = false;
	};

}