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

#include "PlotWidgets.h"
#include "Utils.h"

#pragma warning(disable: 4714)	// disable force inline warnings from Qt

#pragma warning(push, 0)	// no warnings from includes
#include <QMouseEvent>
#include <QMenu>
#include <QGridLayout>
#include <QWidgetAction>
#include <QDebug>
#include <QRadioButton>
#include <QButtonGroup>
#include <QPushButton>
#include <QPainter>
#include <QStyleOption>
#include <QCheckBox>
#include <QApplication>
#include <QDataStream>
#pragma warning(pop)


namespace pie {

	// AxisButton --------------------------------------------------------------------
	AxisButton::AxisButton(const QString& text /* = QString */, Qt::Orientation orientation, QWidget* parent) : OrButton(text, orientation, parent) {

		connect(this, SIGNAL(clicked()), this, SLOT(actionClicked()));
	}

	void AxisButton::mousePressEvent(QMouseEvent *ev) {
	
		OrButton::mousePressEvent(ev);
	}

	void AxisButton::mouseReleaseEvent(QMouseEvent *ev) {

		if (ev->button() == Qt::LeftButton)
			openMenu(ev->globalPos());

		OrButton::mouseReleaseEvent(ev);
	}

	void AxisButton::contextMenuEvent(QContextMenuEvent *ev) {

		openMenu(ev->globalPos());
		OrButton::contextMenuEvent(ev);
	}

	void AxisButton::openMenu(const QPoint& pos) {

		QMenu* m = new QMenu(tr("Axis"), this);

		//for (const DkDimParam& p : mFcs->header().param()) {

		//	QAction* a = new QAction(p.fullName(), this);
		//	connect(a, SIGNAL(triggered()), this, SLOT(actionClicked()));
		//	m->addAction(a);
		//}

		m->exec(pos);
		m->deleteLater();

	}

	void AxisButton::actionClicked() {

		//QAction* sender = static_cast<QAction*>(this->sender());


		//QVector<DkDimParam> param = mFcs->header().param();

		//for (int idx = 0; idx < param.size(); idx++) {

		//	// find the action and send the corresponding event
		//	if (param[idx].fullName() == sender->text()) {

		//		QPoint p;

		//		if (mOrientation == Qt::Horizontal) {
		//			p.setX(idx);
		//			p.setY(-1);
		//		}
		//		else {
		//			p.setX(-1);
		//			p.setY(idx);
		//		}

		//		emit changeAxisIndex(p);
		//		break;
		//	}
		//}
	}

	// MenuButton --------------------------------------------------------------------
	MenuButton::MenuButton(QWidget* parent /* = 0 */) : QPushButton(parent) {

		mPixmap = QPixmap(":/pie/img/plot-menu.png");

		setIcon(ColorManager::colorizePixmap(mPixmap, ColorManager::blue()));
		setFlat(true);	
	}

	//void MenuButton::setPlotParams(DkPlotParams* params) {
	//
	//	mParams = params;
	//}

	//void MenuButton::addPlotSettings(DkAbstractPlotSettings* plotSettings) {
	//	
	//	plotSettings->hide();
	//	mPlotSettings.append(plotSettings);
	//}

	void MenuButton::openMenu(const QPoint& pos) {

		QMenu* m = new QMenu(tr("Plot Menu"), this);

		//DkPlotSettings* settingsWidget = new DkPlotSettings(this);
		//settingsWidget->setPlotParams(mParams);
	
		//// add specialized settings widgets
		//for (DkAbstractPlotSettings* s : mPlotSettings)
		//	settingsWidget->addPlotSettings(s);

		QWidgetAction* a = new QWidgetAction(this);
		//a->setDefaultWidget(settingsWidget);
		m->addAction(a);

		m->exec(pos);
		m->deleteLater();
	}

	void MenuButton::mousePressEvent(QMouseEvent *e) {
		QPushButton::mousePressEvent(e);
	}

	void MenuButton::mouseReleaseEvent(QMouseEvent *e) {

		if (e->button() == Qt::LeftButton)
			openMenu(e->globalPos());

		QPushButton::mouseReleaseEvent(e);
	}

	void MenuButton::enterEvent(QEvent *e) {

		setIcon(ColorManager::colorizePixmap(mPixmap, ColorManager::pink()));
		QPushButton::enterEvent(e);
	}

	void MenuButton::leaveEvent(QEvent *e) {

		setIcon(ColorManager::colorizePixmap(mPixmap, ColorManager::blue()));
		QPushButton::leaveEvent(e);
	}

	// NewPlotWidget --------------------------------------------------------------------
	NewPlotWidget::NewPlotWidget(QWidget* parent /* = 0 */) : Widget(parent) {

		createLayout();
	}

	void NewPlotWidget::createLayout() {

		QLabel* newPlotLabel = new QLabel(tr("Create a new Plot:"), this);
		newPlotLabel->setObjectName("newPlotLabel");

		QPixmap pmd(":/pie/img/plot-dot.png");
		pmd = ColorManager::colorizePixmap(pmd, ColorManager::blue());
		QPushButton* dotPlotButton = new QPushButton(pmd, tr(""), this);
		dotPlotButton->setIconSize(QSize(64, 64));
		dotPlotButton->setFlat(true);
		dotPlotButton->setObjectName("newPlotButton");

		QGridLayout* layout = new QGridLayout(this);
		layout->setRowStretch(0, 10);
		layout->setColumnStretch(0, 10);
		layout->addWidget(newPlotLabel, 1, 1, 1, 2, Qt::AlignHCenter);
		layout->addWidget(dotPlotButton, 2, 2);
		layout->setRowStretch(10, 10);
		layout->setColumnStretch(10, 10);

		connect(dotPlotButton, SIGNAL(clicked()), this, SIGNAL(newDotPlotSignal()));
	}

}
