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

#include "BaseWidgets.h"

#pragma warning(push, 0)	// no warnings from includes
#include <QObject>
#include <QColor>
#pragma warning(pop)

#pragma warning(disable: 4251)	// disable dll interface warning

#ifndef DllExport
#ifdef DLL_CORE_EXPORT
#define DllExport Q_DECL_EXPORT
#else
#define DllExport Q_DECL_IMPORT
#endif
#endif

class QWidget;

namespace pie {

	class DllExport ColorEdit : public Widget {
		Q_OBJECT

	public:
		ColorEdit(const QColor& col = QColor(), QWidget* parent = 0);

		void setColor(const QColor& col);
		QColor color() const;

	signals:
		void newColor(const QColor& col);

	public slots:
		void colorChanged();
		void hashChanged(const QString& name);
		void hashEditFinished();

	protected:
		void createLayout();

		enum cols {
			r = 0,
			g,
			b,

			c_end
		};

		QVector<QSpinBox*> mColBoxes;
		QLineEdit* mColHash;
		QColor mColor;
	};

	class DllExport ColorPane : public Widget {
		Q_OBJECT

	public:
		ColorPane(QWidget* parent = 0);

		QColor color() const;
		double hue() const;

	signals:
		void colorSelected(const QColor& col) const;

	public slots:
		void setHue(int hue);
		void setColor(const QColor& col);

	protected:
		void paintEvent(QPaintEvent* ev) override;
		void mouseMoveEvent(QMouseEvent* me) override;
		void mouseReleaseEvent(QMouseEvent* me) override;
		void mousePressEvent(QMouseEvent* me) override;
		void resizeEvent(QResizeEvent* re) override;

		QPoint color2Pos(const QColor& col) const;
		QColor pos2Color(const QPoint& pos) const;
		QColor ipl(const QColor& c0, const QColor& c1, double alpha) const;
		void setPos(const QPoint& pos);

		double brightness(const QColor& col) const;

		QColor mColor = QColor(255, 0, 0);
		QPoint mPos = QPoint(0, 0);
	};

	class DllExport ColorPicker : public Widget {
		Q_OBJECT

	public:
		ColorPicker(QWidget* parent = 0);

		QColor color() const;

	signals:
		void colorSelected(const QColor& col);

	public slots:
		void setColor(const QColor& col);
		void showMenu(const QPoint& pos = QPoint());

	protected:
		void contextMenuEvent(QContextMenuEvent* cme) override;
		void createLayout();

		ColorPane* mColorPane = 0;
		QLabel* mColorPreview = 0;
		QMenu* mContextMenu = 0;
		ColorEdit* mColorEdit = 0;
	};
}
