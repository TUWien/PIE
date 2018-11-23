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

#include "PieUi.h"
#include "Plot.h"
#include "Utils.h"
#include "PageData.h"
#include "DatabaseLoader.h"
#include "WelcomeWidget.h"
#include "Settings.h"
#include "ActionManager.h"
#include "Utils.h"
#include "WidgetManager.h"

#pragma warning(push, 0)	// no warnings from includes
#include <QTabBar>
#include <QDragEnterEvent>
#include <QDebug>
#include <QMimeData>
#include <QFileInfo>
#include <QApplication>
#include <QDir>
#include <QAction>
#pragma warning(pop)

#ifndef DllExport
#ifdef DLL_CORE_EXPORT
#define DllExport Q_DECL_EXPORT
#else
#define DllExport Q_DECL_IMPORT
#endif
#endif

namespace pie {

	TabWidget::TabWidget(QWidget* parent) : QTabWidget(parent) {

		setObjectName("TabWidget");
		tabBar()->setMovable(true);
		tabBar()->setTabsClosable(true);
		setAcceptDrops(true);

		const ActionManager& am = ActionManager::instance();
		addActions(am.fileActions().toList());
		addActions(am.viewActions().toList());
		addActions(am.toolsActions().toList());

		const DialogManager& dm = DialogManager::instance();

		// connects
		connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(removeTab(int)));

		connect(&dm, SIGNAL(loadFileSignal(const QString&)), this, SLOT(loadFile(const QString&)));
		
		connect(am.action(ActionManager::view_new_tab), SIGNAL(triggered()), this, SLOT(newTab()));
		connect(am.action(ActionManager::view_close_tab), SIGNAL(triggered()), this, SLOT(removeTab()));

		newTab();
	}

	void TabWidget::newTab() {

		WelcomeWidget* ww = new WelcomeWidget(this);
		addTab(ww, tr("Welcome"));

		connect(ww, SIGNAL(loadFileSignal(const QString&)), this, SLOT(loadFile(const QString&)));
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
		
		if (db.parse()) {
			Settings::instance().app().addRecentFile(filePath);
		}
		else {
			// TODO: add error
			qDebug() << "could not parse" << filePath;
			return false;
		}

		PlotWidget* pw = new PlotWidget(db.collection(), this);
		
		addTab(pw, pw->title(), true);

		return true;
	}

	int TabWidget::addTab(QWidget* w, const QString& info, bool selected) {


		int ci = currentIndex();
		if (ci != -1 && currentWidget()->objectName() == "WelcomeWidget")
			removeTab(currentIndex(), true);

		int ni = QTabWidget::addTab(w, QIcon(":/pie/img/pie-icon.png"), info);
		if (selected && ni != -1)
			QTabWidget::setCurrentIndex(ni);

		return ni;
	}

	void TabWidget::removeTab(int index, bool force) {

		if (index == -1)
			index = currentIndex();

		QWidget* w = widget(index);

		// free up space if a tab is closed
		if (w)
			w->deleteLater();

		QTabWidget::removeTab(index);

		if (!count() && !force)
			newTab();
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

		setObjectName("MainWindow");
		createLayout();
		createMenu();
		loadStyleSheet();
		loadSettings();

		resize(500, 300);
	}

	void MainWindow::createLayout() {

		mTabWidget = new TabWidget(this);

		setCentralWidget(mTabWidget);
	}

	void MainWindow::createMenu() {
	}

	void MainWindow::loadStyleSheet() {

		QFileInfo cssInfo(QCoreApplication::applicationDirPath(), "stylesheet.css");

		if (!cssInfo.exists())
			cssInfo = QFileInfo(":/pie/stylesheet.css");

		QFile file(cssInfo.absoluteFilePath());

		if (file.open(QFile::ReadOnly)) {

			QString cssString = file.readAll();

			// replace color placeholders
			cssString.replace("INFO_COLOR", Utils::colorToString(ColorManager::lightGray()));
			cssString.replace("HIGHLIGHT_LIGHT", Utils::colorToString(ColorManager::blue(0.2)));
			cssString.replace("HIGHLIGHT_COLOR", Utils::colorToString(ColorManager::blue()));
			cssString.replace("SELECTION_LIGHT", Utils::colorToString(ColorManager::green(0.5)));
			cssString.replace("SELECTION_COLOR", Utils::colorToString(ColorManager::green()));
			cssString.replace("BACKGROUND_COLOR", Utils::colorToString(ColorManager::white()));
			cssString.replace("FOREGROUND_COLOR", Utils::colorToString(ColorManager::black()));

			qApp->setStyleSheet(cssString);
			file.close();

			qDebug() << "CSS loaded from: " << cssInfo.absoluteFilePath();
			//qDebug().noquote() << "style: \n" << cssString;
		}
		else
			qDebug() << "could not load styles from: " << cssInfo.absoluteFilePath();
	}

	void MainWindow::loadSettings() {
		DefaultSettings settings;

#ifdef WIN32
		// fixes #392 - starting maximized on 2nd screen - tested on win8 only
		QRect r = settings.value("geometryAutoFlow", QRect()).toRect();

		if (r.width() && r.height())	// do not set the geometry if autoflow is loaded the first time
			setGeometry(r);
#endif

		restoreGeometry(settings.value("geometry").toByteArray());
		restoreState(settings.value("windowState").toByteArray());
	}

	void MainWindow::closeEvent(QCloseEvent *ev) {

		DefaultSettings settings;

		settings.setValue("geometryAutoFlow", geometry());
		settings.setValue("geometry", saveGeometry());
		settings.setValue("windowState", saveState());
		
		DefaultSettings ds;
		Settings::instance().save(ds);
		qDebug() << "settings saved to" << Settings::instance().settingsPath();

		QMainWindow::closeEvent(ev);
	}

}