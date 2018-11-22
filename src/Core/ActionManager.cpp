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


#include "ActionManager.h"

#pragma warning(push, 0)	// no warnings from includes
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QSettings>
#include <QFileInfo>
#include <QDir>
#pragma warning(pop)

namespace pie {

// ActionManager --------------------------------------------------------------------
ActionManager::ActionManager() {
	init();
}

ActionManager::~ActionManager() {}

ActionManager& ActionManager::instance() { 

	static ActionManager inst;
	return inst; 
}

QMenu* ActionManager::fileMenu(QWidget* parent /* = 0 */) const {

	QMenu* m = new QMenu(QObject::tr("&File"), parent);

	m->addAction(mFileAction[file_open_database]);

	return m;
}

QMenu* ActionManager::viewMenu(QWidget* parent /* = 0 */) const {

	QMenu* m = new QMenu(QObject::tr("&View"), parent);
	
	m->addAction(mViewAction[view_zoom_in]);
	m->addAction(mViewAction[view_zoom_out]);

	m->addAction(mViewAction[view_increase_num_plots]);
	m->addAction(mViewAction[view_decrease_num_plots]);
	m->addSeparator();
	
	m->addAction(mViewAction[view_new_tab]);
	m->addAction(mViewAction[view_close_tab]);
	m->addSeparator();

	return m;
}

QMenu* ActionManager::toolsMenu(QWidget* parent /* = 0 */) const {

	QMenu* m = new QMenu(QObject::tr("&Tools"), parent);

	m->addAction(mToolsAction[tools_about]);
	m->addSeparator();

	return m;
}

QAction* ActionManager::action(FileMenuActions action) {
	return mFileAction[action];
}

QAction* ActionManager::action(ViewMenuActions action) {
	return mViewAction[action];
}

QAction* ActionManager::action(ToolsMenuActions action) {
	return mToolsAction[action];
}

QVector<QAction*> ActionManager::fileActions() {
	return mFileAction;
}

QVector<QAction*> ActionManager::viewActions() {
	return mViewAction;
}

QVector<QAction*> ActionManager::toolsActions() {
	return mToolsAction;
}

void ActionManager::init() {

	createActions();
	connectDefaultActions();
}

void ActionManager::createActions() {

	// file actions
	mFileAction.resize(file_end);

	mFileAction[file_open_database] = new QAction(QObject::tr("Open Collection"), 0);
	mFileAction[file_open_database]->setToolTip(QObject::tr("Load database to visualize it's collection."));
	mFileAction[file_open_database]->setShortcut(QKeySequence::Open);

	// view actions
	mViewAction.resize(view_end);

	mViewAction[view_zoom_in] = new QAction(QObject::tr("Zoom &In"), 0);
	mViewAction[view_zoom_in]->setToolTip(QObject::tr("Zoom into a plot for more details."));
	mViewAction[view_zoom_in]->setShortcut(QKeySequence(sc_view_zoom_in));
	mViewAction[view_zoom_in]->setAutoRepeat(true);

	mViewAction[view_zoom_out] = new QAction(QObject::tr("Zoom &Out"), 0);
	mViewAction[view_zoom_out]->setToolTip(QObject::tr("Zoom out of a plot to have a better overview."));
	mViewAction[view_zoom_out]->setShortcut(QKeySequence(sc_view_zoom_out));
	mViewAction[view_zoom_out]->setAutoRepeat(true);

	mViewAction[view_decrease_num_plots] = new QAction(QObject::tr("&Decrease Plot Columns"), 0);
	mViewAction[view_decrease_num_plots]->setToolTip(QObject::tr("Decrease the number of plots per row."));
	mViewAction[view_decrease_num_plots]->setShortcut(QKeySequence(sc_view_decrease_num_plots));

	mViewAction[view_increase_num_plots] = new QAction(QObject::tr("&Increase Plot Columns"), 0);
	mViewAction[view_increase_num_plots]->setToolTip(QObject::tr("Increase the number of plots per row."));
	mViewAction[view_increase_num_plots]->setShortcut(QKeySequence(sc_view_increase_num_plots));

	mViewAction[view_new_tab] = new QAction(QObject::tr("&New Tab"), 0);
	mViewAction[view_new_tab]->setToolTip(QObject::tr("Creates a new tab."));
	mViewAction[view_new_tab]->setShortcut(QKeySequence(sc_view_new_tab));

	mViewAction[view_close_tab] = new QAction(QObject::tr("Close &Tab"), 0);
	mViewAction[view_close_tab]->setToolTip(QObject::tr("Close the current tab."));
	mViewAction[view_close_tab]->setShortcut(QKeySequence(sc_view_close_tab));

	// tools actions
	mToolsAction.resize(tools_end);

	mToolsAction[tools_reverse_solar_system] = new QAction(QObject::tr("&Reverse Solar System"), 0);
	mToolsAction[tools_reverse_solar_system]->setToolTip(QObject::tr("Reverses solar system."));
	mToolsAction[tools_reverse_solar_system]->setShortcut(sc_tools_solar);

	mToolsAction[tools_about] = new QAction(QObject::tr("&About PIE"), 0);
	mToolsAction[tools_about]->setToolTip(QObject::tr("Information about this software."));
	mToolsAction[tools_about]->setShortcut(QKeySequence::HelpContents);

}

void ActionManager::connectDefaultActions() {

	// TODO...
}

}
