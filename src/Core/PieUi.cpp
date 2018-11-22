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

#include "PieUI.h"
#include "Plot.h"
#include "Utils.h"
#include "PageData.h"
#include "DatabaseLoader.h"

#pragma warning(push, 0)	// no warnings from includes
#include <QTabBar>
#include <QDragEnterEvent>
#include <QDebug>
#include <QMimeData>
#pragma warning(pop)

#ifndef DllCoreExport
#ifdef DLL_CORE_EXPORT
#define DllCoreExport Q_DECL_EXPORT
#else
#define DllCoreExport Q_DECL_IMPORT
#endif
#endif

namespace pie {

	TabWidget::TabWidget(QWidget* parent) : QTabWidget(parent) {

		setObjectName("TabWidget");
		tabBar()->setMovable(true);
		setAcceptDrops(true);
	}

	void TabWidget::dragEnterEvent(QDragEnterEvent * ev) {

		if (ev->mimeData()->hasUrls()) {

			for (const QUrl& url : ev->mimeData()->urls()) {

				if (Utils::isValidFile(url.toLocalFile())) {
					ev->acceptProposedAction();
					break;
				}
			}
		}

		qDebug() << "drag enter event...";

		QTabWidget::dragEnterEvent(ev);
	}

	void TabWidget::dropEvent(QDropEvent * ev) {

		// if we ever support drag events
		if (ev->source() == this)
			return;

		if (!loadFromMime(ev->mimeData())) {
			qDebug() << "could not load dropped contents...";
		}

		QTabWidget::dropEvent(ev);

	}

	bool TabWidget::loadFile(const QString & filePath) {

		DatabaseLoader db(filePath);
		db.parse();

		PlotWidget* pw = new PlotWidget(db.collection(), this);
		addTab(pw, pw->title());

		return true;
	}

	int TabWidget::addTab(QWidget* w, const QString& info) {

		int ni = QTabWidget::addTab(w, QIcon(":/flowView/img/autoflow-icon.png"), info);
		return ni;
	}

	bool TabWidget::loadFromMime(const QMimeData * mimeData) {

		if (mimeData->hasUrls()) {

			bool success = false;

			for (const QUrl& url : mimeData->urls()) {

				QString filePath = url.toLocalFile();

				if (Utils::isValidFile(filePath)) {

					if (loadFile(filePath))
						success = true;
					else
						qDebug() << "could not load: " << filePath;
				}
			}

			return success;
		}

		return false;

	}

	// -------------------------------------------------------------------- MainWindow 
	MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags flags) : QMainWindow(parent, flags) {

		createLayout();
		createMenu();
		resize(500, 300);
	}

	void MainWindow::createLayout() {

		mTabWidget = new TabWidget(this);

		setCentralWidget(mTabWidget);
	}

	void MainWindow::createMenu() {
	}

}