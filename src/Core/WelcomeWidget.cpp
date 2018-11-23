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


#include "WelcomeWidget.h"

#include "Settings.h"
#include "ActionManager.h"
#include "Utils.h"

#pragma warning(push, 0)	// no warnings from includes
#include <QListView>
#include <QStandardItemModel>
#include <QFileInfo>
#include <QList>
#include <QVBoxLayout>
#include <QAction>
#include <QLabel>
#include <QPushButton>
#include <QDebug>
#pragma warning(pop)

namespace pie {

// DkRecentFilesWidget --------------------------------------------------------------------
RecentFilesWidget::RecentFilesWidget(QWidget* parent) : QWidget(parent) {
	
	createLayout();
}

void RecentFilesWidget::createLayout() {

	QLabel* title = new QLabel(tr("Recent Files"), this);

	mModel = new QStandardItemModel(this);
	
	QListView* listView = new QListView(this);
	listView->setModel(mModel);
	listView->setIconSize(QSize(32, 32));

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setContentsMargins(10, 10, 10, 10);
	layout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
	layout->addWidget(title);
	layout->addWidget(listView);

	connect(listView, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(loadFile(const QModelIndex&)));
}

void RecentFilesWidget::loadFile(const QModelIndex& index) {

	QStandardItem* item = mModel->itemFromIndex(index);

	if (item) {
		emit loadFileSignal(item->toolTip());
		qDebug() << "opening" << item->toolTip();
	}
}

void RecentFilesWidget::setRecentFiles(const QStringList& filePaths) {
	
	mRecentFiles = filePaths;
	mModel->clear();

	for (const QString& cPath : mRecentFiles) {

		QFileInfo fInfo(cPath);

		QString iPath = ":/pie/img/open-pie.png";
		QStandardItem* item = new QStandardItem(QIcon(iPath), fInfo.fileName());
		item->setEditable(false);
		item->setToolTip(fInfo.absoluteFilePath());
		
		mModel->appendRow(QList<QStandardItem*>() << item);
	}
}

// OpenFiles --------------------------------------------------------------------
OpenFilesWidget::OpenFilesWidget(QWidget* parent) : QWidget(parent) {
	createLayout();
}

void OpenFilesWidget::createLayout() {

	QLabel* title = new QLabel(tr("Open a Collection"), this);
	title->setAlignment(Qt::AlignHCenter);

	// TODO: check y we can't load svgs
	QPushButton* pbOpen = new QPushButton(ColorManager::colorizePixmap(QPixmap(":/pie/img/open.png"), ColorManager::blue()), tr("Open"), this);
	pbOpen->setToolTip(tr("Open a PIE Database file"));
	pbOpen->setIconSize(QSize(96, 96));

	QGridLayout* layout = new QGridLayout(this);
	layout->setContentsMargins(10, 10, 10, 10);
	layout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
	layout->addWidget(title, 1, 1, 1, 2);
	layout->addWidget(pbOpen, 2, 1);
	//layout->addWidget(listView);

	ActionManager& m = ActionManager::instance();
	connect(pbOpen, SIGNAL(clicked()), m.action(ActionManager::file_open_database), SIGNAL(triggered()));
}

// DkStartWidget --------------------------------------------------------------------
WelcomeWidget::WelcomeWidget(QWidget* parent) : QWidget(parent) {
	
	setObjectName("WelcomeWidget");
	createLayout();
	updateInfo();
}

void WelcomeWidget::createLayout() {

	mRecentFilesWidget = new RecentFilesWidget(this);
	mRecentFilesWidget->setRecentFiles(Settings::instance().app().recentFiles);

	OpenFilesWidget* openFiles = new OpenFilesWidget(this);

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	layout->addWidget(openFiles);
	layout->addWidget(mRecentFilesWidget);

	connect(mRecentFilesWidget, SIGNAL(loadFileSignal(const QString&)), this, SIGNAL(loadFileSignal(const QString&)));
}

void WelcomeWidget::updateInfo() {

	mRecentFilesWidget->setRecentFiles(Settings::instance().app().recentFiles);

	if (Settings::instance().app().recentFiles.empty())
		mRecentFilesWidget->hide();
}

}
