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

 The READ project  has  received  funding  from  the European  Union�s  Horizon  2020
 research  and innovation programme under grant agreement No 674943

 related links:
 [1] https://cvl.tuwien.ac.at/
 [2] https://transkribus.eu/Transkribus/
 [3] https://github.com/TUWien/
 [4] https://nomacs.org
 *******************************************************************************************************/

#include "BaseWidgets.h"

#include "Settings.h"
#include "Utils.h"

#pragma warning(push, 0)	// no warnings from includes
#include <QStyle>
#include <QStyleOption>
#include <QPainter>
#include <QMenu>
#include <QContextMenuEvent>
#include <QDebug>
#include <QScrollBar>
#include <QHBoxLayout>
#include <QBoxLayout>
#include <QComboBox>
#include <QAction>
#include <QInputDialog>
#include <QApplication>
#include <QMessageBox>
#pragma warning(pop)

namespace pie {

	// DkWidget (our base widget class) --------------------------------------------------------------------
	Widget::Widget(QWidget* parent /* = 0 */) : QWidget(parent) {

		setObjectName("DkWidget");
	}

	bool Widget::setProperty(const char* name, const QVariant& value) {

		bool ok = QWidget::setProperty(name, value);
		style()->unpolish(this);
		style()->polish(this);

		return ok;
	}

	void Widget::registerAction(QAction* action) {
		connect(this, SIGNAL(visibleSignal(bool)), action, SLOT(setChecked(bool)));
	}

	void Widget::setVisible(bool visible) {
	
		emit visibleSignal(visible);
		QWidget::setVisible(visible);
	}

	void Widget::paintEvent(QPaintEvent *event) {

		// fixes stylesheets which are not applied to custom widgets
		QStyleOption opt;
		opt.init(this);
		QPainter p(this);
		style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

		QWidget::paintEvent(event);
	}

	// DkNamedWidget --------------------------------------------------------------------
	NamedWidget::NamedWidget(const QString& name, QWidget* parent) : Widget(parent) {
		mName = name;
	}

	QString NamedWidget::name() const {
		return mName;
	}

	// DkVerticalLabel --------------------------------------------------------------------
	OrButton::OrButton(const QString& text /* = QString */, Qt::Orientation orientation, QWidget* parent /*= 0*/) : QPushButton(text, parent) {
	
		mOrientation = orientation;
		setFlat(true);
		setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	}

	QSize OrButton::sizeHint() const {

		int offset = objectName() == "DkTabLabel" ? 3 : 0;

		if (mOrientation == Qt::Vertical) {
			QSize s(QPushButton::sizeHint());
			return QSize(s.height()+offset, s.width());
		}
		else
			return QPushButton::sizeHint();
	}

	QSize OrButton::minimumSizeHint() const {

		return QSize(0, 0);	// that does what we want
	}

	bool OrButton::setProperty(const char* name, const QVariant& value) {

		bool ok = QPushButton::setProperty(name, value);
		style()->unpolish(this);
		style()->polish(this);

		return ok;
	}

	void OrButton::setChecked(bool checked) {

		setProperty("checked", checked);

		QPushButton::setChecked(checked);
	}

	void OrButton::paintEvent(QPaintEvent* ev) {

		if (mOrientation == Qt::Vertical) {

			QStyleOption opt;
			opt.init(this);

			QRect r(0, 0, -height(), width());

			QPainter p(this);
			style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
			//p.setRenderHints(QPainter::Antialiasing);
			p.rotate(-90);

			// currently vertical labels are always centered sorry for ignoring the style here...
			p.drawText(r, Qt::AlignHCenter | Qt::AlignVCenter, QPushButton::text());

			// I gave up on the stylesheets here
			if (objectName() == "TabLabel") {
			
				QColor col = isChecked() ? p.pen().color() : QColor(200, 200, 200);
				p.setBrush(col);
				p.setPen(Qt::NoPen);
				QRect er(0, 0, -height(), 3);
				p.drawRect(er);
			}

		}
		else
			QPushButton::paintEvent(ev);
	}

	//// DkDockWidget --------------------------------------------------------------------
	//DkDockWidget::DkDockWidget(const QString& title, QWidget* parent /* = 0 */, Qt::WindowFlags flags /* = 0 */ ) : QDockWidget(title, parent, flags) {
	//	
	//	displaySettingsBits = 0;
	//	setObjectName("DkDockWidget");
	//}
	//
	//DkDockWidget::~DkDockWidget() {
	//}
	//
	//void DkDockWidget::load() {
	//
	//	DefaultSettings settings;
	//	settings.beginGroup(objectName());
	//
	//	resize(settings.value("size", QSize(1,1)).toSize());
	//	move(settings.value("pos", QPoint(0,0)).toPoint());
	//	setFloating(settings.value("floating", false).toBool());
	//
	//	settings.endGroup();
	//}
	//
	//void DkDockWidget::save() const {
	//	
	//	DefaultSettings settings;
	//	settings.beginGroup(objectName());
	//
	//	settings.setValue("size", size());
	//	settings.setValue("floating", isFloating());
	//	settings.setValue("pos", pos());
	//
	//	settings.endGroup();
	//}
	//
	//void DkDockWidget::registerAction(QAction* action) {
	//	connect(this, SIGNAL(visibleSignal(bool)), action, SLOT(setChecked(bool)));
	//}
	//
	//void DkDockWidget::setDisplaySettings(QBitArray* displayBits) {
	//	displaySettingsBits = displayBits;
	//}
	//
	//bool DkDockWidget::getCurrentDisplaySetting() const {
	//
	//	if (!displaySettingsBits)
	//		return false;
	//
	//	return testDisplaySettings(*displaySettingsBits);
	//}
	//
	//bool DkDockWidget::testDisplaySettings(const QBitArray& displaySettingsBits) {
	//
	//	if (DkSettings::instance().app().currentAppMode < 0 || DkSettings::instance().app().currentAppMode >= displaySettingsBits.size()) {
	//		qDebug() << "[WARNING] illegal app mode: " << DkSettings::instance().app().currentAppMode;
	//		return false;
	//	}
	//
	//	return displaySettingsBits.testBit(DkSettings::instance().app().currentAppMode);
	//}
	//
	//void DkDockWidget::setVisible(bool visible) {
	//	setVisibleAndSave(visible);
	//}
	//
	//void DkDockWidget::setVisibleAndSave(bool visible, bool saveSetting) {
	//
	//	QDockWidget::setVisible(visible);
	//	emit visibleSignal(visible);	// if this gets slow -> put it into hide() or show()
	//
	//	if (saveSetting && displaySettingsBits && displaySettingsBits->size() > DkSettings::instance().app().currentAppMode) {
	//		displaySettingsBits->setBit(DkSettings::instance().app().currentAppMode, visible);
	//	}
	//}
	//
	//void DkDockWidget::closeEvent(QCloseEvent* event) {
	//
	//	setVisible(false);
	//
	//	QDockWidget::closeEvent(event);
	//}
	//
	//Qt::DockWidgetArea DkDockWidget::getDockLocationSettings(const Qt::DockWidgetArea& defaultArea) const {
	//
	//	DefaultSettings settings;
	//	Qt::DockWidgetArea location = (Qt::DockWidgetArea)settings.value(objectName(), defaultArea).toInt();
	//
	//	return location;
	//}
	//
	//void DkDockWidget::paintEvent(QPaintEvent *event) {
	//
	//	// fixes stylesheets which are not applied to custom widgets
	//	QStyleOption opt;
	//	opt.init(this);
	//	QPainter p(this);
	//	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
	//
	//	QDockWidget::paintEvent(event);
	//}

	// -------------------------------------------------------------------- ResizableScrollArea 
	ResizableScrollArea::ResizableScrollArea(QWidget * parent /* = 0 */) : QScrollArea(parent) {

	}

	bool ResizableScrollArea::eventFilter(QObject * o, QEvent * e) {

		if (widget() && o == widget() && e->type() == QEvent::Resize)
			updateSize();

		return false;
	}

	void ResizableScrollArea::updateSize() {

		if (!widget())
			return;

		updateGeometry();

		if (this->verticalScrollBarPolicy() == Qt::ScrollBarAlwaysOff) {

			int height = widget()->minimumSizeHint().height();

			if (horizontalScrollBar()->isVisible())
				height += horizontalScrollBar()->height();
			setMinimumHeight(height);
		}
		if (this->horizontalScrollBarPolicy() == Qt::ScrollBarAlwaysOff) {

			int width = widget()->minimumSizeHint().width();

			if (verticalScrollBar()->isVisible())
				width += verticalScrollBar()->height();
			setMinimumWidth(width);
		}
	}

	QSize ResizableScrollArea::sizeHint() const {

		if (!widget())
			return QScrollArea::sizeHint();

		widget()->updateGeometry();

		QSize s = QScrollArea::sizeHint();
		QSize ws = widget()->sizeHint();

		if (this->verticalScrollBarPolicy() == Qt::ScrollBarAlwaysOff)
			s.setHeight(ws.height());
		if (this->horizontalScrollBarPolicy() == Qt::ScrollBarAlwaysOff)
			s.setWidth(ws.width());

		return s;
	}

	QSize ResizableScrollArea::minimumSizeHint() const {

		if (!widget())
			return QScrollArea::minimumSizeHint();

		QSize s = QScrollArea::minimumSizeHint();
		QSize ws = widget()->minimumSizeHint();

		if (this->verticalScrollBarPolicy() == Qt::ScrollBarAlwaysOff)
			s.setHeight(ws.height());
		if (this->horizontalScrollBarPolicy() == Qt::ScrollBarAlwaysOff)
			s.setWidth(ws.width());

		return s;
	}
}
