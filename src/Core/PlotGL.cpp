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

#include "PlotGL.h"
#include "Plot.h"
#include "WidgetManager.h"
#include "ActionManager.h"
#include "Utils.h"

#pragma warning(push, 0)	// no warnings from includes
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>
#pragma warning(pop)

namespace pie {

	DotViewPort::DotViewPort(DotPlotParams* params, DotPlot* parent) : QOpenGLWidget(parent) {

		mP = params;
		mParent = parent;
		setObjectName("DotViewPort");
		setMouseTracking(true);
		//setAttribute(Qt::WA_NoSystemBackground);

		//QGLWidget::setFormat(QGLFormat(QGL::SampleBuffers));

		// if I take a look at this: why not simply emit an update signal?
		connect(mP, SIGNAL(pointSizeChanged()), this, SLOT(update()));
		connect(mP, SIGNAL(pointAlphaChanged()), this, SLOT(update()));
		connect(mP, SIGNAL(displayPercentChanged()), this, SLOT(update()));
		connect(mP, SIGNAL(axisIndexChanged()), this, SLOT(update()));

		ActionManager& m = ActionManager::instance();
		connect(m.action(ActionManager::view_zoom_in), SIGNAL(triggered()), this, SLOT(zoomIn()));
		connect(m.action(ActionManager::view_zoom_out), SIGNAL(triggered()), this, SLOT(zoomOut()));
		//connect(m.action(ActionManager::view_reset), SIGNAL(triggered()), this, SLOT(resetView()));
		//connect(m.action(ActionManager::view_update), SIGNAL(triggered()), this, SLOT(update()));
	};

	void DotViewPort::initializeGL() {

		// member init
		// turn point sprites on
		//glEnable(GL_POINT_SPRITE);
		glPointSize((GLfloat)mP->pointSize());

		// turn alpha blending on
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
	}

	void DotViewPort::resizeGL(int w, int h) {

		// setup viewport, projection etc.:
	//	glViewport(0, 0, (GLint)w, (GLint)h);	// doesn't do anything on linux...?

		//// next two lines should clear the buffer - but they don't
		//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mP->viewMatrix().reset();
		mP->viewMatrix().scale(2.0f / w, 2.0f / h);
	}

	void DotViewPort::paintGL() {

		QPainter p(this);

		QStyleOption opt;
		opt.init(this);

		style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
		p.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);

		// GL rendering
		p.beginNativePainting();
		drawGL();
		p.endNativePainting();

		//drawSelection(p);

		drawEmpty(p);
	}

	void DotViewPort::applyGLTransform(bool apply) {

		if (apply) {
			// map transforms from Qt to GL
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(-1, 1, -1, 1, -1, 1);

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			QPointF p(mP->worldMatrix().dx(), mP->worldMatrix().dy());
			p = mP->viewMatrix().map(p);

			glTranslatef((GLfloat)p.x(), (GLfloat)p.y(), 0.0f);
			glScalef((GLfloat)mP->worldMatrix().m11(), (GLfloat)mP->worldMatrix().m22(), 1.0f);
		}
		else {
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
		}
	}

	bool DotViewPort::drawGL() {

		Timer dt;

		QColor bg = ThemeManager::instance().background();

		// clean screen
		glClearColor((GLclampf)bg.redF(), (GLclampf)bg.greenF(), (GLclampf)bg.blueF(), (GLclampf)bg.alphaF());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// enable alpha blending
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);

		applyGLTransform();

		int err = glGetError();
		//qDebug() << "error code: " << err;

		bool success = drawPoints();

		err = glGetError();

		if (err != GL_NO_ERROR)
			qWarning() << "gl error while drawing: " << err;

		//glFlush();
		//qDebug() << "scene rendered in: " << dt.getTotal();

		return success;
	}

	void DotViewPort::drawEmpty(QPainter& p) {

		QString displayText;

		if (mP->axisIndex().x() == -1 || mP->axisIndex().y() == -1)
			displayText = tr("Please select features.");

		//// this should never happen
		//if (!mBaseWidget->eventCount())
		//	displayText = tr("Ich sehe gerade\nes ist ein bisschen\nwas durcheinander gekommen...");

		drawDimArrows(p, mP->axisIndex());

		// currently vertical labels are always centered sorry for ignoring the style here...
		QPen pen;
		pen.setColor(ColorManager::blue());
		p.setPen(pen);
		p.drawText(rect(), Qt::AlignHCenter | Qt::AlignVCenter | Qt::TextWordWrap, displayText);

	}

	void DotViewPort::drawArrow(QPainter& p, const QPoint & start, const QPoint & end, double angle) const {

		// TODO: move to a painter class

		// create arrow head
		QPolygon arrowHead;
		int ah = 10;
		arrowHead.append(QPoint(0, 0));
		arrowHead.append(QPoint(3, ah));
		arrowHead.append(QPoint(-3, ah));
		arrowHead.append(QPoint(0, 0));

		QPoint as(start);
		QPoint ae(end);
		QPoint ahOffset = (angle == 180 || angle == 0) ? QPoint(0, -ah) : QPoint(ah, 0);
		float neg = (angle == 180 || angle == 0) ? 1.0f : -1.0f;
		QPainterPath path;

		// draw arrow to horizontal axis
		path.moveTo(as);
		path.cubicTo(as + QPoint(40, 40)*neg, ae + ahOffset * 5, ae + ahOffset);

		QPainterPath pathA;

		// draw arrowHead
		QTransform t;
		t.rotate(angle);
		QPolygon hArrowHead = t.map(arrowHead);
		hArrowHead.translate(ae);
		pathA.addPolygon(hArrowHead);

		p.drawPath(path);
		p.fillPath(pathA, p.pen().brush());
	}

	void DotViewPort::drawDimArrows(QPainter& p, const QPoint& axisDims) const {

		QPen arrowPen;
		arrowPen.setWidth(2);
		arrowPen.setColor(ColorManager::pink());
		arrowPen.setBrush(ColorManager::pink());

		QPen oldPen = p.pen();
		p.setPen(arrowPen);

		QPoint as(rect().center() + QPoint(20, 20));
		QPoint ae(rect().center().x(), rect().bottom());

		if (axisDims.x() == -1)
			drawArrow(p, as, ae, 180);

		// draw arrow to vertical axis
		as -= QPoint(40, 40);
		ae = QPoint(rect().left(), rect().center().y());

		if (axisDims.y() == -1)
			drawArrow(p, as, ae, -90);

		// restore pen
		p.setPen(oldPen);
	}

	bool DotViewPort::drawPoints() {

		if (!mCollection || mP->axisIndex().x() == -1 || mP->axisIndex().y() == -1)
			return false;	// nothing todo here

		glPointSize((GLfloat)mP->pointSize());
		glBegin(GL_POINTS);
		
		// TODO:p
		//cv::Mat events = mFcs->data();

		//bool success = drawPointsLabels(events, mFcs);
		//if (success)
		//	drawPointsSelection(events, *mFcs->selectionModel());

		glEnd();

		//return success;
		return false;
	}

	//bool DotViewPort::drawPointsLabels(const cv::Mat& events, QSharedPointer<DkFcsData> fcs) const {

	//	float skipFactor = (mP->displayPercent() == 100) ? 0.f : 1.f - mP->displayPercent() / 100.f;

	//	QPoint dims = mP->axisIndex();

	//	if (dims.x() < 0 || dims.x() > events.cols || dims.y() < 0 || dims.y() > events.cols)
	//		return false;	// illegal axis

	//	QVector<DkDimParam> params = mFcs->header().param(dims);
	//	float mX = (float)params[0].logicle()->m()*0.5f;
	//	float mY = (float)params[1].logicle()->m()*0.5f;

	//	QSharedPointer<DkEventGroupManager> groupManager = fcs->groupManager();
	//	QSharedPointer<DkPdfGroupManager> gmmGroupManager = qSharedPointerDynamicCast<DkPdfGroupManager>(groupManager);
	//	QVector<QSharedPointer<DkEventGroup> > groups = groupManager->eventGroups();

	//	// this is a sanity check
	//	bool syncedData = groupManager->isSynced(events, false);
	//	if (!syncedData)
	//		qDebug() << "data is out of sync...";

	//	// iterate over all gmm colors
	//	// TODO make sure blasts are drawn last (for boolean gates...)
	//	for (int cIdx = 0; cIdx < groups.size(); cIdx++) {

	//		// hide current points
	//		if (!groups[cIdx]->visible())
	//			continue;

	//		// set the color
	//		const QColor& col = groups[cIdx]->color();
	//		float alpha = col.alpha() == 255 ? mP->alpha() / 255.0f : col.alpha() / 255.0f;
	//		glColor4f((GLfloat)col.redF(), (GLfloat)col.greenF(), (GLfloat)col.blueF(), (GLfloat)alpha);

	//		// the 'all' group has an empty label vector
	//		cv::Mat labels = groups[cIdx]->events();
	//		const unsigned int* label = labels.ptr<unsigned int>();
	//		int count = groups[cIdx]->empty() || !syncedData ? events.rows : labels.rows;

	//		// draw the points
	//		float skip = 0.f;
	//		for (int rIdx = 0; rIdx < count; ++rIdx, skip += skipFactor) {

	//			if (skip >= 1.f) {
	//				skip -= 1.f;
	//				continue;
	//			}

	//			// NOTE: syncedData implies that label[rIdx] is not valid - hence it must be checked _before_
	//			int dataIdx = labels.empty() || !syncedData || (int)label[rIdx] >= events.rows ? rIdx : label[rIdx];
	//			const float* sample = events.ptr<float>(dataIdx);

	//			glVertex3f(sample[dims.x()] / mX - 1.0f, sample[dims.y()] / mY - 1.0f, 0.0f);
	//		}
	//	}

	//	return true;
	//}

	//bool DotViewPort::drawPointsSelection(const cv::Mat & events, const DkSelectionModel & model) const {

	//	if (model.selections().empty())
	//		return true;


	//	float skipFactor = (mP->displayPercent() == 100) ? 0.f : 1.f - mP->displayPercent() / 100.f;

	//	QPoint dims = mP->axisIndex();

	//	if (dims.x() < 0 || dims.x() > events.cols || dims.y() < 0 || dims.y() > events.cols)
	//		return false;	// illegal axis

	//	QVector<DkDimParam> params = mFcs->header().param(dims);
	//	float mX = (float)params[0].logicle()->m()*0.5f;
	//	float mY = (float)params[1].logicle()->m()*0.5f;

	//	cv::Mat flags = model.selectedEvents();

	//	if (flags.cols < events.rows) {
	//		qDebug() << "cannot draw selection - data out of sync!";
	//		return false;
	//	}

	//	// iterate over all gmm colors
	//	int count = events.rows;

	//	// draw the points
	//	float skip = 0.0f;
	//	const unsigned char* fPtr = flags.ptr<unsigned char>();

	//	QColor col = DkSettings::instance().app().selectionColor;
	//	glColor4f((GLfloat)col.redF(), (GLfloat)col.greenF(), (GLfloat)col.blueF(), (GLfloat)col.alphaF());

	//	for (int rIdx = 0; rIdx < count; ++rIdx, ++fPtr, skip += skipFactor) {

	//		if (skip >= 1.f) {
	//			skip -= 1.f;
	//			continue;
	//		}

	//		// is the event selected?
	//		if (!*fPtr)
	//			continue;

	//		// NOTE: syncedData implies that label[rIdx] is not valid - hence it must be checked _before_
	//		const float* sample = events.ptr<float>(rIdx);
	//		glVertex3f(sample[dims.x()] / mX - 1.0f, sample[dims.y()] / mY - 1.0f, 0.0f);
	//	}

	//	return true;
	//}

	void DotViewPort::mousePressEvent(QMouseEvent *ev) {

		if (ev->buttons() & Qt::LeftButton) {
			mFirstMousePos = ev->pos();
			mLastMousePos = ev->pos();
		}

		// event selection
		//if (isSelectingEvents(ev)) {

			// TODO:p
			//QVector<DkDimParam> p(mFcs->header().param(mP->axisIndex()));

			//if (p.size() == 2) {
			//	mActiveSelection = QSharedPointer<DkSelection>(new DkSelection(p[0], p[1]));

			//	QPointF center = DkUtils::instance().mapFromScreen(ev->pos(), mP->worldMatrix(), size());
			//	mActiveSelection->setEllipse(DkEllipse(center));
			//	mActiveSelection->setEditing(true);
			//}
		//}

		QOpenGLWidget::mousePressEvent(ev);
	}

	void DotViewPort::mouseReleaseEvent(QMouseEvent *ev) {

		QVector2D dist(mFirstMousePos - ev->pos());

		// TODO:p
		//if (mActiveSelection && mFcs) {

		//	if (mActiveSelection->ellipse().axis().width() > 0.01) {

		//		// apply selection
		//		mActiveSelection->setEditing(false);
		//		mActiveSelection->createFlags(mFcs->data());
		//		mFcs->selectionModel()->addSelection(mActiveSelection);
		//	}
		//	mActiveSelection.clear();
		//}

		// clean up
		mLastMousePos = QPoint();
		QOpenGLWidget::mouseReleaseEvent(ev);
	}

	//bool DotViewPort::isSelectingEvents(const QMouseEvent* ev) {

	//	bool isSelecting = (ev->buttons() & Qt::MiddleButton) ||	// either with the mouse wheel
	//		(ev->buttons() & Qt::LeftButton) &&						// or by activating select events & mouse click
	//		DkActionManager::instance().action(DkActionManager::edit_select_events)->isChecked();

	//	return isSelecting;
	//}

	void DotViewPort::mouseMoveEvent(QMouseEvent *ev) {

		//// user selection
		//if (mActiveSelection && isSelectingEvents(ev)) {

		//	QPointF mp = DkUtils::instance().mapFromScreen(ev->pos(), mP->worldMatrix(), size());

		//	DkEllipse e = mActiveSelection->ellipse();
		//	QVector2D vec(mp - e.center());
		//	e.setAxis(QSizeF(vec.length(), vec.length()));
		//	e.setAngle(QVector2D::dotProduct(vec.normalized(), QVector2D(0, 1)));
		//	mActiveSelection->setEllipse(e);
		//	update();
		//}

		if (!mLastMousePos.isNull() &&
			(ev->buttons() & Qt::LeftButton) &&
			ev->modifiers() == Qt::NoModifier /*&&
			!isSelectingEvents(ev)*/) {	// if not zoomed, ctrl must be hit to move view
			QPointF dxy;
			dxy.setX(mLastMousePos.x() - ev->pos().x());
			dxy.setY(mLastMousePos.y() - ev->pos().y());
			moveView(dxy);

			mLastMousePos = ev->pos();
			return; // do not propagate
		}

		QOpenGLWidget::mouseMoveEvent(ev);
	}

	void DotViewPort::wheelEvent(QWheelEvent *ev) {

		if (hasFocus() || parentHasFocus()) {
			zoom((float)ev->delta() / 1200.0f, ev->pos());

			ev->accept();
		}
		else
			QOpenGLWidget::wheelEvent(ev);
	}

	void DotViewPort::zoomIn() {

		if (parentHasFocus())
			zoom(.1f);
	}

	void DotViewPort::zoomOut() {

		if (parentHasFocus())
			zoom(-.1f);
	}

	void DotViewPort::zoom(float factor, const QPointF& center) {

		factor += 1.0f;

		QPointF vpc(width()*0.5f, height()*0.5f);
		QPointF c = center.isNull() ? vpc : center;

		QPointF nc = vpc - c;
		nc.setX(-nc.x());
		QTransform t = mP->worldMatrix();
		nc = t.inverted().map(nc);
		t.translate(nc.x() - factor * nc.x(), nc.y() - factor * nc.y());
		t.scale(factor, factor);
		mP->setWorldMatrix(t);

		update();
	}

	void DotViewPort::setSelected(bool selected) {
		mIsSelected = selected;
	}

	void DotViewPort::moveView(const QPointF& dxy) {

		//qDebug() << "translating by: " << dxy;
		QTransform t = mP->worldMatrix();
		t.translate(-dxy.x() / t.m11(), dxy.y() / t.m22());
		mP->setWorldMatrix(t);

		update();
	}

	//QString DotViewPort::mapMouseCoords(const QPoint& pos) const {

	//	if (!DkSettings::instance().plot().showPlotInfo)
	//		return QString();

	//	QString info;

	//	// coordintate visualization
	//	QVector<DkDimParam> dimParams = mFcs->header().param(mP->axisIndex());

	//	if (mFcs && dimParams.size() == 2) {

	//		double range = DkSettings::instance().data().logicleRange;

	//		// transform from screen
	//		QPointF mapped;
	//		mapped.setX(DkUtils::instance().mapFromScreen(pos.x(), mP->worldMatrix(), size(), Qt::Horizontal) * range);
	//		mapped.setY(DkUtils::instance().mapFromScreen(height() - pos.y(), mP->worldMatrix(), size(), Qt::Vertical) * range);

	//		// apply inverse data mapping
	//		mapped.setX(dimParams[0].transformInv(mapped.x()));
	//		mapped.setY(dimParams[1].transformInv(mapped.y()));

	//		info = dimParams[0].preferredName() + ": " + QString::number(mapped.x(), 'e', 2) + ", ";
	//		info += dimParams[1].preferredName() + ": " + QString::number(mapped.y(), 'e', 2);
	//	}

	//	return info;
	//}

	QPointF DotViewPort::toGLCoords(const QPoint& p) const {

		QPointF glp;
		glp.setX((float)p.x() / width()*2.0f);
		glp.setY((float)p.y() / height()*2.0f);

		return glp;
	}

	void DotViewPort::map(QPainter & painter) const {

		// map to view
		QTransform t = mP->worldMatrix();
		QTransform tp(t.m11()*width(), 0.0, 0.0, t.m22()*height(), width() / 2.0f + t.dx(), height() / 2.0f - t.dy());
		painter.setWorldTransform(tp);
		painter.translate(-0.5f, -0.5f);
	}

	void DotViewPort::resetView() {

		if (parentHasFocus()) {
			mP->setWorldMatrix(QTransform());
			update();
		}
	}

	bool DotViewPort::parentHasFocus() const {

		auto p = dynamic_cast<QWidget*>(parent());
		return hasFocus() || (p && (p->hasFocus() || mIsSelected));
	}

	//void DotViewPort::setAxisIndex(const QPoint& dims) {

	//	mP->setAxisIndex(dims);

	//	// enrich with the name
	//	QVector<DkDimParam> p = mFcs->header().param(mP->axisIndex());

	//	if (dims.x() != -1)
	//		mP->setXAxisName(p[0].preferredName());
	//	if (dims.y() != -1)
	//		mP->setYAxisName(p[1].preferredName());

	//	update();
	//}

}