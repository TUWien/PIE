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

#include "Settings.h"
#include "Utils.h"

#pragma warning(push, 0)	// no warnings from includes
#include <QDebug>
#include <QFileInfo>
#include <QCoreApplication>
#include <QDir>
#include <QThreadPool>
#include <QApplication>
#pragma warning(pop)

namespace pie {

// AppSettings --------------------------------------------------------------------
AppSettings::AppSettings() {

	mName = "AppSettings";
	defaultSettings();
}

void AppSettings::defaultSettings() {

	recentFiles = QStringList();
}

void AppSettings::addRecentFile(const QString& filePath) {
	
	recentFiles.push_front(filePath);

	if (recentFiles.size() > 50)
		recentFiles.pop_back();
}

void AppSettings::load(QSettings& settings) {

	settings.beginGroup(mName);

	recentFiles = settings.value("recentFiles", recentFiles).toStringList();

	settings.endGroup();
}

void AppSettings::save(QSettings& settings) const {

	settings.beginGroup(mName);

	settings.setValue("recentFiles", recentFiles);

	settings.endGroup();
}

// -------------------------------------------------------------------- DefaultSettings
DefaultSettings::DefaultSettings() : QSettings(Settings::instance().settingsPath(), QSettings::IniFormat) {}
//DefaultSettings::DefaultSettings() : QSettings() {}

// Settings --------------------------------------------------------------------
Settings::Settings() {
}

Settings::~Settings() {

	//save();	// we need an external save (it's sa(v)er)
}

Settings& Settings::instance() { 
	
	static Settings inst;
	return inst; 
}

void Settings::init() {

	// init settings
	DefaultSettings settings;
	load(settings);

	if (isPortable())
		qInfo() << "settings loaded from [portable]: " << settingsPath();
	else
		qInfo() << "Settings loaded from: " << settingsPath();
}

AppSettings& Settings::app() {
	return mApp;
}

bool Settings::isPortable() const {

	QFileInfo fi(QCoreApplication::applicationDirPath(), "settings.ini");
	return fi.absoluteFilePath() == settingsPath();
}

QString Settings::settingsPath() const {

	if (!mSettingsPath.isEmpty())
		return mSettingsPath;

	QFileInfo fi(QCoreApplication::applicationDirPath(), "settings.ini");

	if (fi.exists())
		return fi.absoluteFilePath();

	fi = QFileInfo(Utils::appDataPath(), "settings.ini");

	return fi.absoluteFilePath();
}


QString Settings::getDefaultSettingsFile() const {

	return QFileInfo(QCoreApplication::applicationDirPath(), "default.ini").absoluteFilePath();
}

void Settings::applyDefault() {

	DefaultSettings ds;

	if (ds.value("firstTime", true).toBool()) {
		QSettings defaultSettings(getDefaultSettingsFile(), QSettings::IniFormat);
		copySettings(defaultSettings, ds);

		ds.setValue("firstTime", false);

		// add the sample to the recent files
		QString appPath = QApplication::applicationDirPath();
		QString samplePath = QFileInfo(appPath + QDir::separator() + "fcs-sample", "fcs-sample.xml").absoluteFilePath();
		app().addRecentFile(samplePath);

		save(ds);

		qDebug() << "default settings applied...";
	}
}

void Settings::copySettings(const QSettings & src, QSettings & dst) const {

	// copy default settings
	if (src.allKeys().count() > 0) {
		QStringList keys = src.allKeys();
		
		for (const QString& key : keys) {
			dst.setValue(key, src.value(key));
		}
	}
}

void Settings::importSettings(const QString & settingsPath) {

	QSettings settings(settingsPath, QSettings::IniFormat);
	instance().load(settings);

	DefaultSettings ds;
	instance().save(ds);
}

void Settings::load(QSettings& settings) {

	applyDefault();

	mApp.load(settings);
}

void Settings::save(QSettings& settings) const {

	//bool force = false;	// not really needed?!
	mApp.save(settings);
}


}
