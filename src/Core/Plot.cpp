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

#include "Plot.h"
#include "WidgetManager.h"
#include "ActionManager.h"
#include "Utils.h"
#include "PlotGL.h"
#include "PlotWidgets.h"
#include "Settings.h"

#pragma warning(push, 0)	// no warnings from includes
#include <QGridLayout>
#include <QDebug>
#include <QMouseEvent>
#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QObject>
#pragma warning(pop)

namespace pie {

	// DkPlotParams --------------------------------------------------------------------
	DotPlotParams::DotPlotParams(QObject* parent, bool isGlobal) : PlotParams(parent, isGlobal) {

		mMode = m_dot_plot;

		if (!isGlobal) {
			// connect with global params
			DotPlotParams* gb = GlobalPlotParams::instance().dotPlotParams();
			connectWith(gb);

			// take global settings
			mDisplayPercent = gb->displayPercent();
			mPointSize = gb->pointSize();
			mPointAlpha = gb->alpha();
		}
	}

	void DotPlotParams::connectWith(DotPlotParams* param) {

		if (param) {
			connect(param, SIGNAL(pointSizeChanged(int)), this, SLOT(setPointSize(int)));
			connect(param, SIGNAL(displayPercentChanged(int)), this, SLOT(setDisplayPercent(int)));
			connect(param, SIGNAL(pointAlphaChanged(int)), this, SLOT(setAlpha(int)));
		}
	}

	void DotPlotParams::copyTo(DotPlotParams* o) const {

		if (o) {
			DotPlotParams::copyTo(o);

			o->setPointSize(mPointSize);
			o->setAlpha(mPointAlpha);
			o->setDisplayPercent(mDisplayPercent);
		}
	}

	void DotPlotParams::save(QSettings& settings) const {

		PlotParams::save(settings);

		settings.setValue("displayPercent", mDisplayPercent);
		settings.setValue("pointAlpha", mPointAlpha);
		settings.setValue("pointSize", mPointSize);
	}

	void DotPlotParams::load(QSettings& settings) {

		PlotParams::load(settings);

		mDisplayPercent = settings.value("displayPercent", mDisplayPercent).toInt();
		mPointAlpha = settings.value("pointAlpha", mPointAlpha).toInt();
		mPointSize = settings.value("pointSize", mPointSize).toInt();
	}

	int DotPlotParams::pointSize() const {

		return mPointSize;
	}

	void DotPlotParams::setPointSize(int pointSize) {

		bool tS = mPointSize != pointSize;
		mPointSize = pointSize;

		if (tS)		emit pointSizeChanged(pointSize);
	}

	void DotPlotParams::setAlpha(int alpha) {

		bool tS = mPointAlpha != alpha;
		mPointAlpha = alpha;

		if (tS)		emit pointAlphaChanged(alpha);
	}

	void DotPlotParams::setAlphaPercent(int alpha) {
		setAlpha(qRound(alpha / 100.0 * 255));
	}


	int DotPlotParams::alpha() const {

		return mPointAlpha;
	}

	int DotPlotParams::displayPercent() const {

		return mDisplayPercent;
	}

	void DotPlotParams::setDisplayPercent(int displayPercent) {

		bool ts = mDisplayPercent != displayPercent;
		mDisplayPercent = displayPercent;

		if (ts)		emit displayPercentChanged(displayPercent);
	}

	// DotPlot --------------------------------------------------------------------
	DotPlot::DotPlot(QSharedPointer<Collection> collection, QWidget* parent /* = 0 */) : BasePlot(parent) {

		mP = new DotPlotParams(this);
		setObjectName("DotPlot");

		mViewPort = new DotViewPort(collection, mP, this);
		mViewPort->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

		createLayout();

		// viewport connects
		connect(mXAxisLabel, SIGNAL(changeAxisIndex(const QPoint&)), mViewPort, SLOT(setAxisIndex(const QPoint&)));
		connect(mYAxisLabel, SIGNAL(changeAxisIndex(const QPoint&)), mViewPort, SLOT(setAxisIndex(const QPoint&)));
	}

	void DotPlot::createLayout() {

		mXAxisLabel = new AxisButton(tr("Select"), Qt::Horizontal, this);
		mXAxisLabel->setObjectName("axisLabel");
		mYAxisLabel = new AxisButton(tr("Select"), Qt::Vertical, this);
		mYAxisLabel->setObjectName("axisLabel");

		mMenuButton = new MenuButton(this);
		mMenuButton->setPlotParams(mP);
		//mMenuButton->addPlotSettings(new DotPlotSettings(mMenuButton));

		// dummy widget which gives us space for focus effect
		QWidget* selectionBorder = new QWidget(this);
		selectionBorder->setFixedHeight(2);

		QWidget* hWidget = new QWidget(this);
		QHBoxLayout* hL = new QHBoxLayout(hWidget);
		hL->setSpacing(0);
		hL->setContentsMargins(0, 0, 0, 0);
		hL->addWidget(mYAxisLabel);
		hL->addWidget(mViewPort);

		QWidget* labelWidget = new QWidget(this);
		QHBoxLayout* labelLayout = new QHBoxLayout(labelWidget);
		labelLayout->setContentsMargins(0, 0, 0, 0);
		labelLayout->addWidget(mMenuButton);
		labelLayout->addWidget(mXAxisLabel);

		// this dummy widget adds space to the x-axis such that it aligns to the plot
		QWidget* dummy = new QWidget(this);
		QHBoxLayout* dLayout = new QHBoxLayout(dummy);
		dLayout->setContentsMargins(0, 0, 0, 0);
		dLayout->setSpacing(0);

		QVBoxLayout* vL = new QVBoxLayout(this);
		vL->setSpacing(0);
		vL->setContentsMargins(0, 0, 0, 0);
		vL->addWidget(selectionBorder);
		vL->addWidget(hWidget);
		vL->addWidget(dummy);
		vL->addWidget(labelWidget);
	}

	void DotPlot::setMinimumSize(const QSize& size) {

		BasePlot::setMinimumSize(size);
	}

	void DotPlot::setAxisIndex(const QPoint& index) {
		mViewPort->setAxisIndex(index);
	}

	QPoint DotPlot::axisIndex() const {
		return (mP) ? mP->axisIndex() : QPoint();
	}

	void DotPlot::setFullScreen(bool fullScreen) {

		if (fullScreen)
			mP->setPointSize(mP->pointSize() + 3);
		else
			mP->setPointSize(qMax(mP->pointSize() - 3, 1));

		BasePlot::setFullScreen(fullScreen);
	}

	void DotPlot::setSelected(bool selected) {

		mViewPort->setSelected(selected);
		BasePlot::setSelected(selected);
	}

	//void DotPlot::mapAxisIndex() {

	//	// lookup dim names
	//	if (mViewPort->fcsData()) {

	//		// this is for compatibility
	//		if (mP->xAxisName().isEmpty() && mP->yAxisName().isEmpty()) {

	//			// enrich the data with the axis name
	//			QVector<DkDimParam> p = mViewPort->fcsData()->header().param(mP->axisIndex());

	//			if (mP->axisIndex().x() != -1)
	//				mP->setXAxisName(p[0].preferredName());
	//			if (mP->axisIndex().y() != -1)
	//				mP->setYAxisName(p[1].preferredName());
	//		}
	//		else {
	//			QPoint axis;
	//			axis.setX(mViewPort->fcsData()->header().bestDimIndex(mP->xAxisName()));
	//			axis.setY(mViewPort->fcsData()->header().bestDimIndex(mP->yAxisName()));

	//			if (axis.x() != -1 && axis.y() != -1)
	//				mP->setAxisIndex(axis);
	//		}
	//	}
	//}

	//void DotPlot::addParams(const PlotParams* params) {
	//	params->copyTo(mP);	// update our parameters
	//	//mapAxisIndex();
	//}

	//PlotParams* DotPlot::params() const {
	//	return mP;
	//}

	MenuButton * DotPlot::menuButton() const {
		return mMenuButton;
	}

	// DkGlobalPlotParams --------------------------------------------------------------------
	GlobalPlotParams::GlobalPlotParams() {

		//mBaseViewPort = 0;
		mParams = new DotPlotParams(0, true);

		DefaultSettings ds;
		load(ds);

		ActionManager& m = ActionManager::instance();

		QObject::connect(m.action(m.view_decrease_num_plots), SIGNAL(triggered()), mParams, SLOT(decreaseNumColumns()));
		QObject::connect(m.action(m.view_increase_num_plots), SIGNAL(triggered()), mParams, SLOT(increaseNumColumns()));
	}

	GlobalPlotParams::~GlobalPlotParams() {

		delete mParams;
		mParams = 0;
	}

	DotPlotParams* GlobalPlotParams::dotPlotParams() const {
		return mParams;
	}

	PlotParams* GlobalPlotParams::params() const {
		return mParams;
	}

	//void GlobalPlotParams::setInitialPlots(const QVector<PlotParams*>& params) {

	//	// housekeeping
	//	for (PlotParams* p : mPlotSheet.plots())
	//		p->deleteLater();
	//	mPlotSheet.clear();

	//	for (const PlotParams* p : params) {

	//		DkPlotParams* nP = 0;

	//		// we need to own these params
	//		if (p->mode() == DkPlotParams::m_generic)
	//			nP = new DkPlotParams(mParams);
	//		else if (p->mode() == DkPlotParams::m_hist)
	//			nP = new DkHistParams(mParams);
	//		else if (p->mode() == DkPlotParams::m_dot_plot)
	//			nP = new DkDotPlotParams(mParams);
	//		else
	//			qWarning() << "unknown mode: " << p->mode();

	//		if (nP) {
	//			p->copyTo(nP);
	//			mPlotSheet.append(nP);
	//		}
	//	}
	//}

	//PlotSheet GlobalPlotParams::loadPlots(const QString& profileName) {

	//	QString loadProfile = profileName;

	//	DefaultSettings settings;
	//	settings.beginGroup("DkPlots");

	//	if (loadProfile.isEmpty()) {

	//		QStringList profileStrings = settings.childGroups();
	//		loadProfile = settings.value("DefaultProfileString", (profileStrings.empty()) ? "" : profileStrings[0]).toString();
	//	}

	//	mPlotSheet = DkPlotSheet::loadPlots(settings, mParams, loadProfile);
	//	settings.endGroup();

	//	return mPlotSheet;
	//}

	//PlotSheet DkGlobalPlotParams::currentPlots() {

	//	if (mPlotSheet.empty()) {
	//		return loadPlots();
	//	}

	//	return mPlotSheet;
	//}

	//void GlobalPlotParams::copySettings(QSettings & sFrom, QSettings & sTo) {

	//	QVector<PlotSheet> ps = GlobalPlotParams::instance().loadAllPlots(sFrom);

	//	sTo.beginGroup("DkPlots");
	//	for (auto p : ps)
	//		p.save(sTo);
	//	sTo.endGroup();
	//}

	GlobalPlotParams& GlobalPlotParams::instance() {

		static GlobalPlotParams inst;
		return inst;
	}

	void GlobalPlotParams::save(QSettings& settings) const {

		settings.beginGroup("GlobalParams");
		mParams->save(settings);
		settings.endGroup();
	}

	//void DkGlobalPlotParams::saveAllPlots(QSettings & settings, const QVector<DkPlotSheet>& plots) const {

	//	for (const DkPlotSheet& ps : plots) {
	//		ps.save(settings);
	//	}
	//}

	void GlobalPlotParams::load(QSettings& settings) {

		settings.beginGroup("GlobalParams");
		mParams->load(settings);
		settings.endGroup();
	}

	//QVector<DkPlotSheet> DkGlobalPlotParams::loadAllPlots(QSettings& settings) {

	//	settings.beginGroup("DkPlots");
	//	QStringList profiles = settings.childGroups();

	//	QVector<DkPlotSheet> ap;

	//	for (auto pn : profiles) {
	//		ap << DkPlotSheet::loadPlots(settings, DkUtils::getMainWindow(), pn);
	//	}

	//	settings.endGroup();

	//	return ap;
	//}

	// PlotWidget --------------------------------------------------------------------
	PlotWidget::PlotWidget(QSharedPointer<Collection> collection, QWidget* parent /* = 0 */) : Widget(parent) {

		mCollection = collection;

		createLayout();
		setAcceptDrops(true);

		addPlot();
	}

	PlotWidget::~PlotWidget() {
		//saveSettings();
	}

	QString pie::PlotWidget::title() const {
		return mCollection->name();
	}

	void PlotWidget::createLayout() {

		// holds everything & is put into the scroll area (for correct scrolling)
		QWidget* dummy = new QWidget(this);

		oLayout = new QGridLayout(dummy);
		oLayout->setContentsMargins(0, 0, 0, 0);
		oLayout->setSpacing(2);

		mNewPlotWidget = new NewPlotWidget(this);
		mNewPlotWidget->hide();

		ResizableScrollArea* scrollArea = new ResizableScrollArea(this);
		scrollArea->setObjectName("ScrollAreaPlots");
		scrollArea->setWidgetResizable(true);
		scrollArea->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
		scrollArea->setWidget(dummy);

		QVBoxLayout* layout = new QVBoxLayout(this);
		layout->setContentsMargins(0, 0, 0, 0);
		layout->addWidget(scrollArea);

		ActionManager& m = ActionManager::instance();
		connect(m.action(m.edit_add_dot_plot), SIGNAL(triggered()), this, SLOT(addPlot()));
		connect(m.action(m.edit_select_all), SIGNAL(triggered(bool)), this, SLOT(selectAll(bool)));

		connect(mNewPlotWidget, SIGNAL(newDotPlotSignal()), this, SLOT(addPlot()));

		connect(GlobalPlotParams::instance().params(), SIGNAL(numColumnsChanged(int)), this, SLOT(setNumColumns(int)));
		//connect(DkGlobalPlotParams::instance().params(), SIGNAL(loadPlotsSignal(const QString&)), this, SLOT(loadPlots(const QString&)));
		//connect(DkGlobalPlotParams::instance().params(), SIGNAL(savePlotsSignal(const QString&)), this, SLOT(savePlots(const QString&)));
	}

	void PlotWidget::updateLayout() {

		assert(mCollection);

		int size = qRound((double)width() / mNumColumns);

		int idx = 0;
		for (; idx < mPlots.size(); idx++) {
			oLayout->addWidget(mPlots[idx], idx / mNumColumns, idx % mNumColumns);
			mPlots[idx]->show();
			mPlots[idx]->setMinimumHeight(size);
		}

		mNewPlotWidget->show();

		// finally add the new plot widget
		oLayout->addWidget(mNewPlotWidget, idx / mNumColumns, idx % mNumColumns);
	}

	void PlotWidget::addPlot(bool update) {

		DotPlot* plot = new DotPlot(mCollection, this);
		//plot->addParams(params);
		plot->hide();

		connectPlot(plot);
		mPlots.append(plot);

		if (update)
			updateLayout();
	}

	void PlotWidget::connectPlot(BasePlot* plot) {

		connect(plot, SIGNAL(closeSignal()), this, SLOT(removePlot()));
		connect(plot, SIGNAL(fullScreenSignal()), this, SLOT(singlePlot()));
		connect(plot, SIGNAL(clearSelectionSignal()), this, SLOT(clearSelection()));
		connect(plot, SIGNAL(shiftSelectionSignal(bool)), this, SLOT(shiftSelection(bool)));
		connect(plot, SIGNAL(startShiftSelectionSignal()), this, SLOT(startShiftSelection()));
	}

	//void PlotWidget::clear() {
	//}

	void PlotWidget::shiftSelection(bool selected) {

		DotPlot* pw = dynamic_cast<DotPlot*>(QObject::sender());

		if (!pw)
			return;

		for (int idx = 0; idx < mPlots.size(); idx++) {

			if (mPlots[idx] == pw && mLastShiftIdx != -1) {
				clearSelection();
				selectPlots(selected, mLastShiftIdx, idx);
				break;
			}
			else if (mPlots[idx] == pw) {
				mLastShiftIdx = idx;
				break;
			}
		}

	}

	void PlotWidget::startShiftSelection() {
		mLastShiftIdx = -1;
		shiftSelection(true);
	}

	void PlotWidget::selectAll(bool selected /* = true */) {

		selectPlots(selected);

		//ActionManager::instance().action(ActionManager::edit_select_all)->setChecked(selected);
	}

	void PlotWidget::selectPlots(bool selected /* = true */, int from /* = 0 */, int to /* = -1 */) {

		if (mPlots.empty())
			return;

		if (to == -1)
			to = mPlots.size() - 1;

		if (from > to) {
			int tmp = to;
			to = from;
			from = tmp;
		}

		for (int idx = from; idx <= to && idx < mPlots.size(); idx++)
			mPlots.at(idx)->setSelected(selected);
	}

	void PlotWidget::clearSelection() {
		selectAll(false);
	}

	void PlotWidget::removePlot() {

		DotPlot* w = dynamic_cast<DotPlot*>(QObject::sender());

		if (!w) {
			qWarning() << "Could not cast to BasePlot on close request...";
			return;
		}

		int idx = mPlots.indexOf(w);

		if (idx >= 0) {
			mPlots.remove(idx);
			updateLayout();
			w->deleteLater();
		}
	}

	void PlotWidget::singlePlot() {

		DotPlot* plot = dynamic_cast<DotPlot*>(QObject::sender());

		if (!plot) {
			qWarning() << "illegal object called singlePlot()";
			return;
		}

		for (QWidget* w : mPlots) {

			if (w != plot)
				w->setVisible(!plot->isFullScreen());
			else
				w->setVisible(true);
		}

		//mNewPlotWidget->setVisible(!plot->isFullScreen());
	}

	void PlotWidget::setNumColumns(int numColumns) {

		if (mNumColumns == numColumns)
			return;

		if (numColumns != -1)
			mNumColumns = numColumns > 0 ? numColumns : 1;

		updateLayout();
	}

	void PlotWidget::resizeEvent(QResizeEvent *ev) {

		setNumColumns();

		QWidget::resizeEvent(ev);
	}

	void PlotWidget::setVisible(bool show) {


		if (show) {
			// TODO: we can save RAM here...
			//DkBasicGLWidget* vp = DkGlobalPlotParams::instance().baseViewPort();
			//vp->setFcs(mFcs);

			//if (mPlots.empty())
			//	createPlots(ps);
		}
		else {
			//qDebug() << "saving widget parameter...";
			//backup();
		}

		QWidget::setVisible(show);
	}

	//void PlotWidget::savePlots(const QString & name) {

	//	DefaultSettings settings;
	//	settings.beginGroup("DkPlots");

	//	DkPlotSheet ps(name, params());
	//	ps.save(settings);
	//	settings.endGroup();
	//}

	//void PlotWidget::loadPlots(const QString & name) {

	//	createPlots(DkGlobalPlotParams::instance().loadPlots(name));
	//}

	//void PlotWidget::backup() {

	//	saveDisplayParams();

	//	// creation & deletion goes here because
	//	// the plots consume a lot of RAM (now only the displayed tab needs it)
	//	for (BasePlot* p : mPlots) {
	//		p->deleteLater();
	//	}

	//	mPlots.clear();
	//}

	void PlotWidget::closeEvent(QCloseEvent* ev) {

		//saveSettings();

		QWidget::closeEvent(ev);
	}

	//void PlotWidget::saveSettings() const {

	//	QVector<DkPlotParams*> plots;
	//	for (BasePlot* p : mPlots)
	//		plots.append(p->params());

	//	DkGlobalPlotParams::instance().setInitialPlots(plots);

	//	qDebug() << "PlotWidget: settings saved...";

	//}



}