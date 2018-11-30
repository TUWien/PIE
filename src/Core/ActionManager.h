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
#include <QSharedPointer>
#include <QVector>
#pragma warning(pop)

#pragma warning(disable: 4251)	// disable dll interface warning

#ifndef DllExport
#ifdef DLL_CORE_EXPORT
#define DllExport Q_DECL_EXPORT
#else
#define DllExport Q_DECL_IMPORT
#endif
#endif

class QAction;
class QMenu;
class QSettings;

namespace pie {

class DllExport ActionManager {

public:
	static ActionManager& instance();
	~ActionManager();

	enum FileMenuActions {
		file_open_database,
		
		file_end,
	};

	enum ViewMenuActions {

		view_zoom_in,
		view_zoom_out,

		view_new_tab,
		view_close_tab,

		view_decrease_num_plots,
		view_increase_num_plots,

		view_end
	};

	enum EditMenuActions {
		edit_add_dot_plot,
		edit_remove_plot,

		edit_select_all,

		edit_end
	};

	enum ToolsMenuActions {
		tools_about,

		// hidden
		tools_reverse_solar_system,

		tools_end
	};

	enum shortcuts {

		// file shortcuts
		//sc_open			= Qt::CTRL + Qt::SHIFT + Qt::Key_O,
		sc_view_zoom_in = Qt::Key_Plus,
		sc_view_zoom_out = Qt::Key_Minus,
		sc_view_decrease_num_plots = Qt::CTRL + Qt::Key_Plus,	// decreasing numbers -> increases size (that's why it is plus)
		sc_view_increase_num_plots = Qt::CTRL + Qt::Key_Minus,

		sc_view_new_tab = Qt::CTRL + Qt::Key_T,
		sc_view_close_tab = Qt::CTRL + Qt::Key_W,

		sc_add_dot_plot = Qt::CTRL + Qt::Key_D,

		sc_tools_solar = Qt::Key_R,

		sc_end
	};

	QMenu* fileMenu(QWidget* parent = 0) const;
	QMenu* viewMenu(QWidget* parent = 0) const;
	QMenu* editMenu(QWidget* parent = 0) const;
	QMenu* toolsMenu(QWidget* parent = 0) const;

	QAction* action(FileMenuActions action) const;
	QAction* action(ViewMenuActions action) const;
	QAction* action(EditMenuActions action) const;
	QAction* action(ToolsMenuActions action) const;

	QVector<QAction*> fileActions() const;
	QVector<QAction*> viewActions() const;
	QVector<QAction*> editActions() const;
	QVector<QAction*> toolsActions() const;

protected:
	ActionManager();
	ActionManager(ActionManager const&);	// remove
	void operator=(ActionManager const&);	// remove
	void init();
	void createActions();
	void connectDefaultActions();

	QVector<QAction*> mFileAction;
	QVector<QAction*> mViewAction;
	QVector<QAction*> mEditAction;
	QVector<QAction*> mToolsAction;
};

}
