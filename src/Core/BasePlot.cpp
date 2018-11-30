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

#include "BasePlot.h"
#include "WidgetManager.h"
#include "ActionManager.h"
#include "Utils.h"

#pragma warning(push, 0)	// no warnings from includes
#include <QGridLayout>
#include <QDebug>
#include <QMouseEvent>
#include <QAction>
#pragma warning(pop)

namespace pie {


	// PlotParams --------------------------------------------------------------------
	PlotParams::PlotParams(QObject* parent, bool /*isGlobal*/) : QObject(parent) {

		//if (!isGlobal) {
		//	// connect with global params
		//	PlotParams* gb = DkGlobalPlotParams::instance().params();
		//	connectWith(gb);

		//	// take global settings
		//	mMinimumSize = gb->minimumSize();
		//	mNumColumns = gb->numColumns();
		//}
	}

	void PlotParams::connectWith(PlotParams* params) {

		if (params) {	// do not try to connect with global itself
			connect(params, SIGNAL(minimumSizeChanged(const QSize&)), this, SLOT(setMinimumSize(const QSize&)), Qt::UniqueConnection);
		}
	}

	void PlotParams::copyTo(PlotParams* o) const {

		if (o) {
			o->setAxisIndex(mAxisIndex);
			o->setXAxisName(mXAxisName);
			o->setYAxisName(mYAxisName);
			o->setMinimumSize(mMinimumSize);
			o->setNumColumns(mNumColumns);
			o->setMode(mMode);
		}
	}

	void PlotParams::save(QSettings& settings) const {

		settings.beginGroup("PlotParams");
		//settings.setValue("axisIndex", mAxisIndex);
		settings.setValue("xAxisName", mXAxisName);
		settings.setValue("yAxisName", mYAxisName);
		settings.setValue("minimumSize", mMinimumSize);
		settings.setValue("numColumns", mNumColumns);
		settings.setValue("mode", mMode);
		settings.endGroup();
	}

	void PlotParams::load(QSettings& settings) {

		settings.beginGroup("PlotParams");

		// load axis index for compatibility
		mAxisIndex = settings.value("axisIndex", mAxisIndex).toPoint();
		mXAxisName = settings.value("xAxisName", mXAxisName).toString();
		mYAxisName = settings.value("yAxisName", mYAxisName).toString();
		mMinimumSize = settings.value("minimumSize", mMinimumSize).toSize();
		mNumColumns = settings.value("numColumns", mNumColumns).toInt();
		mMode = (Mode)settings.value("mode", mMode).toInt();

		settings.endGroup();
	}

	PlotParams::Mode PlotParams::mode() const {
		return mMode;
	}

	void PlotParams::setMode(const Mode& mode) {
		mMode = mode;
	}

	QPoint PlotParams::axisIndex() const {

		return mAxisIndex;
	}

	QString PlotParams::xAxisName() const {
		return mXAxisName;
	}

	QString PlotParams::yAxisName() const {
		return mYAxisName;
	}

	void PlotParams::setAxisIndex(const QPoint& axisIndex) {

		if (mAxisIndex == axisIndex) {
			emit axisIndexChanged(mAxisIndex);
			return;
		}

		// -1 == keep the existing axis dimension
		if (axisIndex.x() != -1)
			mAxisIndex.setX(axisIndex.x());
		if (axisIndex.y() != -1)
			mAxisIndex.setY(axisIndex.y());

		emit axisIndexChanged(mAxisIndex);
	}

	void PlotParams::setMinimumSize(const QSize& size) {
		mMinimumSize = size;
		emit minimumSizeChanged(size);
	}

	void PlotParams::setNumColumns(int numColumns) {

		mNumColumns = numColumns;
		emit numColumnsChanged(numColumns);
	}

	void PlotParams::decreaseNumColumns() {

		if (mNumColumns > 1)
			setNumColumns(--mNumColumns);
	}

	void PlotParams::increaseNumColumns() {

		if (mNumColumns < 30)
			setNumColumns(++mNumColumns);

	}

	QSize PlotParams::minimumSize() const {
		return mMinimumSize;
	}

	int PlotParams::numColumns() const {
		return mNumColumns;
	}

	void PlotParams::setXAxisName(const QString & axisName) {
		mXAxisName = axisName;
	}

	void PlotParams::setYAxisName(const QString & axisName) {
		mYAxisName = axisName;
	}

	QTransform& PlotParams::worldMatrix() {
		return mWorldMatrix;
	}

	void PlotParams::setWorldMatrix(const QTransform& worldMatrix) {
		mWorldMatrix = worldMatrix;
		emit worldMatrixChanged(worldMatrix);
	}

	QTransform& PlotParams::viewMatrix() {
		return mViewMatrix;
	}
	void PlotParams::setViewMatrix(const QTransform& viewMatrix) {
		mViewMatrix = viewMatrix;
		emit viewMatrixChanged(viewMatrix);
	}

	// -------------------------------------------------------------------- BasePlot 
	BasePlot::BasePlot(QSharedPointer<Collection> collection, QWidget* parent /* = 0 */) : Widget(parent) {

		setFocusPolicy(Qt::StrongFocus);
		mCollection = collection;

		// connects
		ActionManager& m = ActionManager::instance();
		connect(m.action(m.edit_remove_plot), SIGNAL(triggered()), this, SLOT(closeRequested()));

		setAcceptDrops(true);
	}

	BasePlot::~BasePlot() {
	}

	bool BasePlot::isSelected() const {

		return mSelected;
	}

	void BasePlot::setSelected(bool selected) {

		// this results in a repaint
		// so only do it if we really need that
		if (selected != mSelected) {
			mSelected = selected;
			setProperty("selected", selected);
		}
	}

	bool BasePlot::isFullScreen() const {
		return mFullScreen;
	}

	void BasePlot::setFullScreen(bool fullScreen) {

		mFullScreen = fullScreen;
		emit fullScreenSignal();
	}

	void BasePlot::mouseDoubleClickEvent(QMouseEvent *event) {

		setFullScreen(!mFullScreen);	// toggle fullscreen (or better naming: single view)
		Widget::mouseDoubleClickEvent(event);
	}

	void BasePlot::mousePressEvent(QMouseEvent *event) {

		Widget::mousePressEvent(event);
	}

	void BasePlot::mouseReleaseEvent(QMouseEvent *event) {

		if (event->button() == Qt::LeftButton && (event->modifiers() & Qt::ControlModifier || event->modifiers() & Qt::ShiftModifier)) {
			setSelected(!isSelected());	// toggle selection
		}
		if (event->button() == Qt::LeftButton && event->modifiers() == Qt::NoModifier) {
			emit clearSelectionSignal();
		}
		if (event->button() == Qt::LeftButton && event->modifiers() & Qt::ShiftModifier) {
			emit shiftSelectionSignal(isSelected());
		}
		else if (event->button() == Qt::LeftButton)
			emit startShiftSelectionSignal();

		Widget::mouseReleaseEvent(event);
	}

	void BasePlot::closeRequested() const {

		if (hasFocus() || isSelected())
			emit closeSignal();

	}

}