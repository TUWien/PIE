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



#pragma warning(push, 0)	// no warnings from includes
#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>

#include <opencv2/core.hpp>
#pragma warning(pop)

#include "Processor.h"
#include "PageData.h"
#include "Utils.h"
#include "DatabaseLoader.h"
#include "PieUi.h"
#include "Settings.h"

#if defined(_MSC_BUILD) && !defined(QT_NO_DEBUG_OUTPUT) // fixes cmake bug - really release uses subsystem windows, debug and release subsystem console
#pragma comment (linker, "/SUBSYSTEM:CONSOLE")
#else
#pragma comment (linker, "/SUBSYSTEM:WINDOWS")
#endif

void applyDebugSettings();
bool testFunction();

int main(int argc, char** argv) {

	// check opencv version
	qInfo().nospace() << "I am using OpenCV " << CV_MAJOR_VERSION << "." << CV_MINOR_VERSION << "." << CV_VERSION_REVISION;

	QCoreApplication::setOrganizationName("TU Wien");
	QCoreApplication::setOrganizationDomain("https://cvl.tuwien.ac.at/");
	QCoreApplication::setApplicationName("PIE - Page Image Explorer");
	pie::Utils::instance().initFramework();

#ifdef WIN32
	QApplication app(argc, (char**)argv);		// enable QPainter
#else
	QCoreApplication app(argc, (char**)argv);	// enable headless
#endif

	// CMD parser --------------------------------------------------------------------
	QCommandLineParser parser;

	parser.setApplicationDescription("Welcome to the Page Image Explorer (PIE).");
	parser.addHelpOption();
	parser.addVersionOption();
	parser.addPositionalArgument("databasepath", QObject::tr("Path to a PIE database."));

	// xml path
	QCommandLineOption testOpt(QStringList() << "test", QObject::tr("If set, Unit Tests are performed"));
	parser.addOption(testOpt);

	parser.process(*QCoreApplication::instance());
	// CMD parser --------------------------------------------------------------------

	// stop processing if little tests are preformed
	if (testFunction())
		return 0;

	// apply debug settings - convenience if you don't want to always change the cmd args
	//applyDebugSettings(dc);

	pie::DefaultSettings ds;
	pie::Settings::instance().app().load(ds);
	
	qDebug() << "lol <-- help me, I am drowning";

	// for now
	if (parser.isSet(testOpt)) {
		pie::DatabaseLoader db("C:/temp/db.json");
		db.parse();

		pie::test::Processor(*db.collection());
	}
	// show them what we've got
	else {
		pie::MainWindow* win = new pie::MainWindow();
		win->show();
		//win->resize(1785, 500);

		app.exec();

		delete win;
	}

	// save settings
	//config.save();
	return 0;	// thanks
}

void applyDebugSettings() {

	// add your debug overwrites here...
}

bool testFunction() {

	// tests the line distance to point function
	return false;
}