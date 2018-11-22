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
#include <QTabWidget>
#include <QMainWindow>
#pragma warning(pop)

#pragma warning (disable: 4251)	// inlined Qt functions in dll interface

#ifndef DllCoreExport
#ifdef DLL_CORE_EXPORT
#define DllCoreExport Q_DECL_EXPORT
#else
#define DllCoreExport Q_DECL_IMPORT
#endif
#endif

// Qt defines
class QMimeData;

namespace pie {

// read defines

	class PlotWidget;

	class DllCoreExport TabWidget : public QTabWidget {
		Q_OBJECT

	public:
		TabWidget(QWidget* parent = 0);

	public slots:
	//	void removeTab(int index = -1);
	//	void tabChanged(int index);
	//	bool loadFile(const QString& filePath, const QString& day = "", const QString& tube = "", int ind = -1);
	//	//void showWelcome();
		int addTab(QWidget* w, const QString& info = tr("New Tab"));

	private:
		void dragEnterEvent(QDragEnterEvent* ev);
		void dropEvent(QDropEvent* ev);
		bool loadFile(const QString& filePath);

		bool loadFromMime(const QMimeData* mimeData);
	};

	class DllCoreExport MainWindow : public QMainWindow {
		Q_OBJECT

	public:
		MainWindow(QWidget* parent = 0, Qt::WindowFlags flags = 0);

	private:
		void createLayout();
		void createMenu();
		//void loadStyleSheet();
		//void loadSettings();

		// main widget
		TabWidget* mTabWidget = 0;

	};

}