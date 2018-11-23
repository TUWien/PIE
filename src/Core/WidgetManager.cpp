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


#include "WidgetManager.h"
#include "Settings.h"
#include "Utils.h"
#include "ActionManager.h"

#pragma warning(push, 0)	// no warnings from includes
#include <QFileDialog>
#include <QAction>
#include <QApplication>
#pragma warning(pop)

namespace pie {

	DialogManager::DialogManager() {

		const ActionManager& am = ActionManager::instance();
		connect(am.action(ActionManager::file_open_database), SIGNAL(triggered()), this, SLOT(openDialog()));
	}

	DialogManager& DialogManager::instance() {

		static DialogManager dm;

		return dm;

	}

	void DialogManager::openDialog() {

		QStringList openFilters;
		openFilters << tr("Collection (*.json)");

		const auto& s = Settings::instance().app();
		QString ldir = !s.recentFiles.isEmpty() ? s.recentFiles[0] : "";

		// load system default open dialog
		QString filePath = QFileDialog::getOpenFileName(
			wm::dialogParent(), 
			tr("Open Collection"),
			ldir,
			openFilters.join(";;"));

		if (filePath.isEmpty())
			return;

		emit loadFileSignal(filePath);
	}
}

QWidget * pie::wm::dialogParent() {

	QWidget* dp = 0;

	for (QWidget* w : QApplication::topLevelWidgets()) {
		dp = w;

		if (dp->objectName() == "MainWindow")
			break;
	}

	return dp;
}
