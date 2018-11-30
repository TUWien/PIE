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
	class DotViewPort;
	class MenuButton;
	class AxisButton;
	class NewPlotWidget;

	class DllExport DotPlotParams : public PlotParams {
		Q_OBJECT

	public:
		DotPlotParams(QObject* parent, bool isGlobal = false);
		virtual ~DotPlotParams() {}

		using PlotParams::connectWith;
		using PlotParams::copyTo;
		virtual void connectWith(DotPlotParams* param);
		virtual void copyTo(DotPlotParams* o) const;

		virtual void save(QSettings& settings) const;
		virtual void load(QSettings& settings);

		int pointSize() const;
		int displayPercent() const;
		int alpha() const;

	public slots:
		void setPointSize(int pointSize);
		void setDisplayPercent(int displayPercent);
		void setAlpha(int alpha);
		void setAlphaPercent(int alpha);

	signals:
		void pointSizeChanged(int pointSize = 1) const;
		void displayPercentChanged(int displayPercent = 100) const;
		void pointAlphaChanged(int alpha = 255) const;

	protected:

		int mDisplayPercent = 100;
		int mPointSize = 1;
		int mPointAlpha = 100;
	};
	
	// this class will hold the toolbar (if needed)
	class DllExport DotPlot : public BasePlot {
		Q_OBJECT

	public:
		DotPlot(QSharedPointer<Collection> collection, QWidget* parent = 0);
		virtual ~DotPlot() {}

		void showDecorations(bool show = true);
		DotViewPort* viewport() const;
		MenuButton* menuButton() const;

		virtual QPoint axisIndex() const;
		virtual void setFullScreen(bool fullScreen) override;
		
		virtual void setSelected(bool selected);

	public slots:
		//void axisIndexChanged(const QPoint& axisIndex);
		//virtual void setAxisIndex(const QPoint& index);
		void setMinimumSize(const QSize& size);

	protected:
		void createLayout();
		void mapAxisIndex();	// apply the axis name (different between experiments) to the axis index

		AxisButton* mXAxisLabel;
		AxisButton* mYAxisLabel;

		DotViewPort* mViewPort;

		DotPlotParams* mP;
		MenuButton* mMenuButton;
	};

	class PlotWidget : public Widget {
		Q_OBJECT

	public:
		PlotWidget(QSharedPointer<Collection> collection, QWidget* parent = 0);
		virtual ~PlotWidget();

		QString title() const;
		//void clear();

	public slots:
		virtual void setVisible(bool show);
		void setNumColumns(int numColumns = -1);
		void addPlot(bool update = true);
		void removePlot();
		void singlePlot();

		void selectAll(bool selected = true);
		void selectPlots(bool selected = true, int from = 0, int to = -1);
		void clearSelection();
		void shiftSelection(bool selected);
		void startShiftSelection();

		//void saveDisplayParams();
		//void savePlots(const QString& name);
		//void loadPlots(const QString& name);
		//void print(DkPrintHandler& ph);

	signals:
		void displayedDimsSignal(const QVector<QPoint>& dims) const;

	protected:
		void resizeEvent(QResizeEvent *ev) override;
		void closeEvent(QCloseEvent* ev) override;
		
		void updateLayout();
		void createLayout();
		void connectPlot(BasePlot* plot);

		//void saveSettings() const;

		//void backup();

		QVector<BasePlot*> mPlots;

		NewPlotWidget* mNewPlotWidget;

		int mLastShiftIdx = -1;
		int mNumColumns = 3;
		QGridLayout* oLayout;

		QSharedPointer<Collection> mCollection;
	};

}