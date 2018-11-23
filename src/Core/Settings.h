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
#include <QSettings>
#include <QSharedPointer>
#include <QColor>
#include <QBitArray>
#include <QVector>
#pragma warning(pop)

#pragma warning(disable: 4251)	// TODO: remove

#ifndef DllExport
#ifdef DLL_CORE_EXPORT
#define DllExport Q_DECL_EXPORT
#else
#define DllExport Q_DECL_IMPORT
#endif
#endif

namespace pie {

class DllExport GenericSettings {

public:
	GenericSettings() { mName = "GenericSettings"; }
	virtual ~GenericSettings() {}

	virtual void load(QSettings& ) {}
	virtual void save(QSettings&) const {}

protected:
	virtual void defaultSettings() {}
	
	QString mName = "GenericSettings";
};

class DllExport AppSettings : public GenericSettings {

public:

	AppSettings();

	QStringList recentFiles;

	void addRecentFile(const QString& filePath);

	void load(QSettings& settings) override;
	void save(QSettings& settings) const override;

protected:
	void defaultSettings() override;
};

//class DllExport DkAllPlotSettings : public GenericSettings {
//
//public:
//	DkAllPlotSettings();
//
//	void load(QSettings& settings);
//	void save(QSettings& settings, const GenericSettings& init, bool force = false) const override;
//
//	// plot things
//	int minPlotSize;
//	int maxPlotSize;
//	bool shaderPlot;
//	bool showPlotInfo;
//	bool showEllipseNames;
//	QString baseGate;
//
//protected:
//	void defaultSettings() override;
//};

class DllExport DefaultSettings : public QSettings {

public:
	DefaultSettings();
};

class DllExport Settings {

public:
	static Settings& instance();
	~Settings();

	AppSettings& app();

	void load(QSettings& settings);
	void save(QSettings& settings) const;

	QString settingsPath() const;
	bool isPortable() const;

	void init();

	static void importSettings(const QString & settingsPath);


private:
	Settings();
	QString mSettingsPath;

	QString getDefaultSettingsFile() const;

	void applyDefault();
	void copySettings(const QSettings& src, QSettings& dst) const;

	AppSettings mApp;
};

}
