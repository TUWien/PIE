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
#include <QPushButton>
#include <QDockWidget>
#include <QLabel>
#include <QTimer>
#include <QScrollArea>
#include <QDoubleSpinBox>
#pragma warning(pop)

#ifndef DllExport
#ifdef DLL_CORE_EXPORT
#define DllExport Q_DECL_EXPORT
#else
#define DllExport Q_DECL_IMPORT
#endif
#endif

class QSpinBox;
class QSlider;
class QColorDialog;
class QBoxLayout;
class QComboBox;

namespace pie {

	class DllExport Widget : public QWidget {
		Q_OBJECT

	public:
		Widget(QWidget* parent = 0);
		virtual ~Widget() {}

		bool setProperty(const char* name, const QVariant & value);
		void registerAction(QAction* action);

	public slots:
		virtual void setVisible(bool visible);

	signals:
		void visibleSignal(bool visible) const;

	protected:
		void paintEvent(QPaintEvent* e);

	};

	class NamedWidget : public Widget {
		Q_OBJECT

	public:
		NamedWidget(const QString& name, QWidget* parent);

		QString name() const;

	protected:
		QString mName;
	};

	class DllExport OrButton : public QPushButton {
		Q_OBJECT

	public:
		OrButton(const QString& text = QString(), Qt::Orientation orientation = Qt::Horizontal, QWidget* parent = 0);
		virtual ~OrButton() {}

		virtual QSize sizeHint() const;
		virtual QSize minimumSizeHint() const;
		bool setProperty(const char* name, const QVariant & value);
	
	public slots:
		void setChecked(bool checked);

	protected:
		void paintEvent(QPaintEvent* ev);

		Qt::Orientation mOrientation;
	};

	//class DkDockWidget : public QDockWidget {
	//	Q_OBJECT
	//
	//public:
	//	DkDockWidget(const QString& title, QWidget* parent = 0, Qt::WindowFlags flags = 0 );
	//	virtual ~DkDockWidget();
	//
	//	void registerAction(QAction* action);
	//	void setDisplaySettings(QBitArray* displayBits);
	//	bool getCurrentDisplaySetting() const;
	//	static bool testDisplaySettings(const QBitArray& displaySettingsBits);
	//	Qt::DockWidgetArea getDockLocationSettings(const Qt::DockWidgetArea& defaultArea) const;
	//
	//	void save() const;
	//	void load();
	//
	//public slots:
	//	virtual void setVisible(bool visible);
	//	virtual void setVisibleAndSave(bool visible, bool saveSetting = true);
	//
	//signals:
	//		void visibleSignal(bool visible);
	//
	//protected:
	//	virtual void closeEvent(QCloseEvent* event);
	//	virtual void paintEvent(QPaintEvent* event);
	//
	//	QBitArray* displaySettingsBits;
	//
	//};

	class ResizableScrollArea : public QScrollArea {
		Q_OBJECT

	public:
		ResizableScrollArea(QWidget * parent = 0);
		virtual ~ResizableScrollArea() {}

		void updateSize();

		virtual QSize sizeHint() const;
		virtual QSize minimumSizeHint() const;

	protected:
		bool eventFilter(QObject * o, QEvent * e);

	};

}
