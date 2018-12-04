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
#include "Processor.h"
#include "PageData.h"
#include "GeneralWidgets.h"

#pragma warning(disable: 4714)	// disable force inline warnings from Qt

#pragma warning(push, 0)	// no warnings from includes
#include <QListWidget>
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
	AxisButton::AxisButton(const QString& text, Qt::Orientation orientation, QWidget* parent) : OrButton(text, orientation, parent) {
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

		for (int idx = 0; idx < AbstractMapper::m_end; idx++) {

			auto map = AbstractMapper::create((AbstractMapper::Type)idx);

			QAction* a = new QAction(map->name(), this);
			a->setData(idx);
			connect(a, SIGNAL(triggered()), this, SLOT(actionClicked()));
			m->addAction(a);
		}

		m->exec(pos);
		m->deleteLater();

	}

	void AxisButton::actionClicked() {

		QAction* action = static_cast<QAction*>(this->sender());

		if (!action)
			return;

		QPoint p(-1,-1);
		int idx = action->data().toInt();

		if (mOrientation == Qt::Horizontal)
			p.setX(idx);
		else
			p.setY(idx);

		setText(action->text());
		
		emit changeAxisIndex(p);
	}

	// MenuButton --------------------------------------------------------------------
	MenuButton::MenuButton(QWidget* parent /* = 0 */) : QPushButton(parent) {

		mPixmap = QPixmap(":/pie/img/bars.png");

		setIcon(ColorManager::colorizePixmap(mPixmap, ColorManager::blue()));
		setFlat(true);	
	}

	void MenuButton::setPlotParams(PlotParams* params) {
		mParams = params;
	}

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

		QPixmap pmd(":/pie/img/dot-plot.png");
		pmd = ColorManager::colorizePixmap(pmd, ColorManager::blue());
		QPushButton* dotPlotButton = new QPushButton(pmd, tr(""), this);
		dotPlotButton->setIconSize(QSize(64, 64));
		dotPlotButton->setFlat(true);
		dotPlotButton->setObjectName("newPlotButton");

		QGridLayout* layout = new QGridLayout(this);
		layout->setRowStretch(0, 10);
		layout->setColumnStretch(0, 10);
		layout->addWidget(newPlotLabel, 1, 1, 1, 2, Qt::AlignHCenter);
		layout->addWidget(dotPlotButton, 2, 2, 1, 2, Qt::AlignHCenter);
		layout->setRowStretch(10, 10);
		layout->setColumnStretch(10, 10);

		connect(dotPlotButton, SIGNAL(clicked()), this, SIGNAL(newDotPlotSignal()));
	}

	// -------------------------------------------------------------------- DocumentItem 
	DocumentItem::DocumentItem(QSharedPointer<Document>& document, QListWidget* parent) : QListWidgetItem(parent) {

		mDoc = document;

		QPixmap colIcon(32, 32);
		QPainter p(&colIcon);
		p.setPen(Qt::NoPen);
		p.setBrush(document->color());
		p.drawRect(colIcon.rect());

		QString name = document->pages()[0]->collectionName() + " " + document->name() + " [" + QString::number(document->numPages()) + "]";

		setText(name);
		setIcon(colIcon);
	}

	QSharedPointer<Document> DocumentItem::document() const {
		return mDoc;
	}

	// -------------------------------------------------------------------- LegendWidget 
	LegendWidget::LegendWidget(QSharedPointer<Collection> collection, QWidget* parent) : Widget(parent) {
		
		mCollection = collection;
		createLayout();
		updateList(collection);
	}

	void LegendWidget::contextMenuEvent(QContextMenuEvent * ev) {

		QMenu* m = new QMenu(tr("Choose a Color"), this);

		ColorPicker* colWidget = new ColorPicker(this);

		QWidgetAction* wa = new QWidgetAction(m);
		wa->setDefaultWidget(colWidget);

		m->exec(ev->pos());
		m->deleteLater();
	}

	void LegendWidget::createLayout() {

		mLegendList = new QListWidget(this);

		QVBoxLayout* layout = new QVBoxLayout(this);
		layout->setContentsMargins(0, 0, 0, 0);
		layout->addWidget(mLegendList);

		auto clickEvent = [&](QListWidgetItem* li) { 
			
			DocumentItem* di = dynamic_cast<DocumentItem*>(li);

			if (di) {
				mCollection->selectAll(false);
				di->document()->setSelected(true);
				emit updateSignal();
			}
		};

		auto doubleClickEvent = [&](QListWidgetItem* li) {

			DocumentItem* di = dynamic_cast<DocumentItem*>(li);

			if (di) {
				mCollection->selectAll(false);
				di->document()->setSelected(true);
				emit updateSignal();

				// TODO: make this work
				//QPoint pos = QWidget::mapFromGlobal(QCursor::pos()); 

				//QMenu* m = new QMenu(tr("Choose a Color"), this);

				//ColorPicker* colWidget = new ColorPicker(this);
				//colWidget->resize(100, 100);

				//QWidgetAction* wa = new QWidgetAction(m);
				//wa->setDefaultWidget(colWidget);

				//m->exec(pos);
				//m->deleteLater();
			}
		};

		connect(mLegendList, &QListWidget::itemClicked, this, clickEvent);
		connect(mLegendList, &QListWidget::itemDoubleClicked, this, doubleClickEvent);
	}

	void LegendWidget::updateList(QSharedPointer<Collection> col) {

		for (auto d : col->documents()) {

			mLegendList->addItem(new DocumentItem(d, mLegendList));
		}
	}
}
