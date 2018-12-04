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


#include "GeneralWidgets.h"
#include "Utils.h"

#pragma warning(push, 0)	// no warnings from includes
#include <QLineEdit>
#include <QGridLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QSlider>
#include <QMenu>
#include <QWidgetAction>
#pragma warning(pop)

namespace pie {
	// -------------------------------------------------------------------- ColorEdit 
	ColorEdit::ColorEdit(const QColor& col, QWidget* parent) : Widget(parent) {

		createLayout();
		setColor(col);
	}

	void ColorEdit::createLayout() {

		mColBoxes.resize(c_end);

		for (int idx = 0; idx < mColBoxes.size(); idx++) {
			mColBoxes[idx] = new QSpinBox(this);
			mColBoxes[idx]->setMinimum(0);
			mColBoxes[idx]->setMaximum(255);
			connect(mColBoxes[idx], SIGNAL(valueChanged(int)), this, SLOT(colorChanged()));
		}

		mColBoxes[r]->setPrefix("R: ");
		mColBoxes[g]->setPrefix("G: ");
		mColBoxes[b]->setPrefix("B: ");

		mColHash = new QLineEdit(this);
		connect(mColHash, SIGNAL(textEdited(const QString&)), this, SLOT(hashChanged(const QString&)));
		connect(mColHash, SIGNAL(editingFinished()), this, SLOT(hashEditFinished()));

		QGridLayout* gl = new QGridLayout(this);
		gl->addWidget(mColBoxes[r], 1, 1);
		gl->addWidget(mColBoxes[g], 2, 1);
		gl->addWidget(mColBoxes[b], 3, 1);
		gl->addWidget(mColHash, 4, 1);
	}

	void ColorEdit::setColor(const QColor & col) {
		mColor = col;

		mColBoxes[r]->setValue(col.red());
		mColBoxes[g]->setValue(col.green());
		mColBoxes[b]->setValue(col.blue());

		mColHash->setText(mColor.name());
	}

	QColor ColorEdit::color() const {
		return mColor;
	}

	void ColorEdit::colorChanged() {

		mColor = QColor(mColBoxes[r]->value(), mColBoxes[g]->value(), mColBoxes[b]->value());
		mColHash->setText(mColor.name());

		emit newColor(mColor);
	}

	void ColorEdit::hashChanged(const QString& name) {

		if (!name.startsWith("#"))
			mColHash->setText("#" + mColHash->text());
	}

	void ColorEdit::hashEditFinished() {

		QColor nc;
		nc.setNamedColor(mColHash->text());

		if (nc.isValid()) {
			setColor(nc);
			emit newColor(nc);
		}
		else
			mColHash->setText(mColor.name());
	}


	// -------------------------------------------------------------------- ColorPane 
	ColorPane::ColorPane(QWidget* parent) : Widget(parent) {
		mColor = QColor(255, 0, 0);
	}

	QColor ColorPane::color() const {
		return pos2Color(mPos);
	}

	void ColorPane::setHue(int hue) {
		mColor.setHsvF(hue / 360.0, mColor.saturationF(), mColor.valueF());
		update();
		emit colorSelected(color());
	}

	double ColorPane::hue() const {
		return mColor.hueF();
	}

	void ColorPane::setColor(const QColor & col) {
		setHue(col.hue());
		setPos(color2Pos(col));
	}

	void ColorPane::paintEvent(QPaintEvent * ev) {

		QPainter p(this);
		p.setPen(Qt::NoPen);
		p.setRenderHint(QPainter::Antialiasing);

		// setup corners (white, pure color, black, black)
		QColor c00, c01, c11, c10;
		c00.setHsvF(mColor.hueF(), 0, 1);
		c01.setHsvF(mColor.hueF(), 1, 1);
		c10.setHsvF(mColor.hueF(), 0, 0);
		c11.setHsvF(mColor.hueF(), 1, 0);

		int w = width();
		int h = height();

		// draw
		for (int idx = 0; idx < h; idx++) {

			QColor ccs = ipl(c00, c10, (double)idx / h);
			QColor cce = ipl(c01, c11, (double)idx / h);

			QLinearGradient g(QPoint(0, 0), QPoint(w, 0));
			g.setColorAt(0, ccs);
			g.setColorAt(1, cce);

			QRect r(0, idx, w, 1);
			p.fillRect(r, g);
		}

		QColor c = color();

		// draw current selection
		QPen pen;
		pen.setColor(brightness(c) < 0.5 ? Qt::white : Qt::black);
		p.setPen(pen);
		QRectF cPick(0, 0, 11, 11);
		cPick.moveCenter(mPos);
		p.drawEllipse(cPick);

		QWidget::paintEvent(ev);
	}

	void ColorPane::mouseMoveEvent(QMouseEvent * me) {

		if (me->buttons() == Qt::LeftButton)
			setPos(me->pos());
		QWidget::mouseMoveEvent(me);
	}

	void ColorPane::mousePressEvent(QMouseEvent * me) {

		if (me->button() == Qt::LeftButton)
			setPos(me->pos());
		QWidget::mousePressEvent(me);
	}

	void ColorPane::mouseReleaseEvent(QMouseEvent * me) {

		if (me->button() == Qt::LeftButton)
			setPos(me->pos());
		QWidget::mouseReleaseEvent(me);
	}

	void ColorPane::resizeEvent(QResizeEvent * re) {

		setPos(mPos);
		QWidget::resizeEvent(re);
	}

	QPoint ColorPane::color2Pos(const QColor & col) const {

		return QPoint(qRound(col.saturationF()*width()), qRound((1.0 - col.valueF())*height()));
	}

	QColor ColorPane::pos2Color(const QPoint & pos) const {

		// setup corners (white, pure color, black, black)
		QColor c00, c01, c11, c10;
		c00.setHsvF(mColor.hueF(), 0, 1);
		c01.setHsvF(mColor.hueF(), 1, 1);
		c10.setHsvF(mColor.hueF(), 0, 0);
		c11.setHsvF(mColor.hueF(), 1, 0);

		QColor ccs = ipl(c00, c10, (double)pos.y() / height());
		QColor cce = ipl(c01, c11, (double)pos.y() / height());

		return ipl(ccs, cce, (double)pos.x() / width());
	}

	QColor ColorPane::ipl(const QColor& c0, const QColor& c1, double alpha) const {

		assert(alpha >= 0 && alpha <= 1.0);

		double r = c0.redF() * (1.0 - alpha) + c1.redF() * alpha;
		double g = c0.greenF() * (1.0 - alpha) + c1.greenF() * alpha;
		double b = c0.blueF() * (1.0 - alpha) + c1.blueF() * alpha;

		return QColor::fromRgbF(r, g, b);
	}

	void ColorPane::setPos(const QPoint & pos) {

		mPos.setX(qMin(qMax(pos.x(), 0), width()));
		mPos.setY(qMin(qMax(pos.y(), 0), height()));

		update();
		emit colorSelected(color());
	}

	double ColorPane::brightness(const QColor & col) const {

		return std::sqrt(
			col.redF() * col.redF() * 0.241 +
			col.greenF() * col.greenF() * 0.691 +
			col.blueF() * col.blueF() * 0.068
		);
	}

	// -------------------------------------------------------------------- ColorPicker 
	ColorPicker::ColorPicker(QWidget* parent) : Widget(parent) {

		createLayout();
		QMetaObject::connectSlotsByName(this);
	}

	void ColorPicker::createLayout() {

		int bs = 20;

		// color pane
		mColorPane = new ColorPane(this);
		mColorPane->setObjectName("mColorPane");
		mColorPane->setBaseSize(100, 100);
		mColorPane->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

		// hue slider
		QSlider* hueSlider = new QSlider(this);
		hueSlider->setObjectName("cpHueSlider");
		hueSlider->setMaximum(360);
		hueSlider->setValue(360);
		hueSlider->setFixedWidth(bs);

		// current color
		mColorPreview = new QLabel("", this);
		mColorPreview->setFixedHeight(bs);

		QPixmap pm(":/pie/img/bars.png");
		QPushButton* mMenu = new QPushButton(ColorManager::colorizePixmap(pm, QColor(255,255,255)), "", this);
		mMenu->setObjectName("flatWhite");
		mMenu->setFlat(true);
		mMenu->setFixedSize(bs, bs);

		QGridLayout* hb = new QGridLayout(this);
		hb->setContentsMargins(0, 0, 0, 0);
		hb->addWidget(mColorPane, 0, 0);
		hb->addWidget(hueSlider, 0, 1);
		hb->addWidget(mColorPreview, 1, 0);
		hb->addWidget(mMenu, 1, 1);

		connect(hueSlider, SIGNAL(valueChanged(int)), mColorPane, SLOT(setHue(int)));
		connect(mColorPane, SIGNAL(colorSelected(const QColor&)), this, SIGNAL(colorSelected(const QColor&)));
		connect(mColorPane, SIGNAL(colorSelected(const QColor&)), this, SLOT(setColor(const QColor&)));
		connect(mMenu, SIGNAL(clicked()), this, SLOT(showMenu()));

		setColor(mColorPane->color());
	}

	void ColorPicker::showMenu(const QPoint & pos) {

		if (!mContextMenu) {

			mContextMenu = new QMenu(this);
			mColorEdit = new ColorEdit(color(), this);
			connect(mColorEdit, SIGNAL(newColor(const QColor&)), this, SLOT(setColor(const QColor&)));
			connect(mColorEdit, SIGNAL(newColor(const QColor&)), mColorPane, SLOT(setColor(const QColor&)));

			QWidgetAction* a = new QWidgetAction(this);
			a->setDefaultWidget(mColorEdit);
			mContextMenu->addAction(a);
		}

		mColorEdit->setColor(color());
		mContextMenu->exec(!pos.isNull() ? pos : mapToGlobal(geometry().bottomRight()));
	}

	void ColorPicker::setColor(const QColor& col) {

		mColorPreview->setStyleSheet("background-color: " + Utils::colorToString(col));
	}

	QColor ColorPicker::color() const {

		return mColorPane->color();
	}

	void ColorPicker::contextMenuEvent(QContextMenuEvent * cme) {

		showMenu(cme->globalPos());

		// do not propagate
		//QWidget::contextMenuEvent(cme);
	}
}