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

#include "BaseWidgets.h"

#pragma warning(push, 0)	// no warnings from includes
#include <QPixmap>
#pragma warning(pop)

#ifndef DllExport
#ifdef DLL_CORE_EXPORT
#define DllExport Q_DECL_EXPORT
#else
#define DllExport Q_DECL_IMPORT
#endif
#endif

class QRadioButton;
class QCheckBox;
class QGridLayout;
class QSettings;
class QMimeData;

namespace pie {

	class PlotParams;

	class DllExport AxisButton : public OrButton {
		Q_OBJECT

	public:
		AxisButton(const QString& text = QString(), Qt::Orientation orientation = Qt::Horizontal, QWidget* parent = 0);

	public slots:
		void actionClicked();

	signals:
		void changeAxisIndex(const QPoint& idx) const;

	protected:
		void mousePressEvent(QMouseEvent *ev);
		void mouseReleaseEvent(QMouseEvent *ev);
		void contextMenuEvent(QContextMenuEvent *ev);
		void openMenu(const QPoint& pos);

	};

	class DllExport MenuButton : public QPushButton {
		Q_OBJECT

	public:
		MenuButton(QWidget* parent = 0);
		virtual ~MenuButton() {}

		void setPlotParams(PlotParams* params);
		//void addPlotSettings(AbstractPlotSettings* plotSettings);

	protected:
		void mousePressEvent(QMouseEvent *e);
		void mouseReleaseEvent(QMouseEvent *e);
		void enterEvent(QEvent *e);
		void leaveEvent(QEvent *e);

		virtual void openMenu(const QPoint& pos);

		QPixmap mPixmap;
		PlotParams* mParams = 0;
		//QVector<AbstractPlotSettings*> mPlotSettings;
	};

	class DllExport NewPlotWidget : public Widget {
		Q_OBJECT

	public:
		NewPlotWidget(QWidget* parent = 0);

	signals:
		void newDotPlotSignal();

	protected:
		void createLayout();
	};
}
