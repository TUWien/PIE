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
#include <QWidget>
#pragma warning(pop)

#ifndef DllExport
#ifdef DK_DLL_EXPORT
#define DllExport Q_DECL_EXPORT
#elif defined(DK_DLL_IMPORT)
#define DllExport Q_DECL_IMPORT
#else
#define DllExport
#endif
#endif

class QStandardItemModel;

namespace pie {

class RecentFilesWidget : public QWidget {
	Q_OBJECT

public:
	RecentFilesWidget(QWidget* parent = 0);

	void setRecentFiles(const QStringList& filePaths);

signals:
	void loadFileSignal(const QString& filePath) const;

public slots:
	void loadFile(const QModelIndex& index);

protected:
	void createLayout();

	QStringList mRecentFiles;
	QStandardItemModel* mModel = 0;
};

class OpenFilesWidget : public QWidget {
	Q_OBJECT

public:
	OpenFilesWidget(QWidget* parent = 0);

signals:
	void loadFileSignal(const QString& filePath) const;

protected:
	void createLayout();
};


class WelcomeWidget : public QWidget {
	Q_OBJECT

public:
	WelcomeWidget(QWidget* parent = 0);

	void updateInfo();

signals:
	void loadFileSignal(const QString& filePath) const;

protected:
	void createLayout();

	RecentFilesWidget* mRecentFilesWidget = 0;

};
}
