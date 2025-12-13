#ifndef _OPERATION_DRILL_ROUNDLESS_HPP_
#define _OPERATION_DRILL_ROUNDLESS_HPP_

#include "OperationDrill.hpp"

enum ODR_RD {
	ODR_Radius = 1,
	ODR_Diameter = 2
};

class OperationDrillRoundless : public OperationDrill {
private:
	double radius = 15.;
	int count = 6;
	double startAngle = 0.;
	double sector = 360.;
	
	void calculate() override {
		drills.clear();
		double shiftAngle;
		if (sector < 360. && count > 1) {
			shiftAngle = M_2PI * sector / 360. / (count - 1);
		} else {
			shiftAngle = M_2PI / count;
		}
		for (int i=0; i<count; ++i) {
			drills.Add({
				center.x + radius * cos(startAngle * M_PI/180. + shiftAngle*i),
				center.y + radius * sin(startAngle * M_PI/180. + shiftAngle*i)
			});
		}
	}
	
public:
	OperationDrillRoundless() {
		calculate();
	}
	
	OperationDrillRoundless(Operation *operation) : OperationDrill(operation) {
		calculate();
	}
	
	OperationDrillRoundless(Pointf center, double radius, int count, double startAngle, double sector) {
		setRoundless(center, radius, count, startAngle, sector);
	}
	
	double getRadius() {
		return radius;
	}
	
	void setRadius(const double r) {
		radius = r;
		calculate();
	}
	
	int getCount() {
		return count;
	}
	
	void setCount(const int c) {
		count = c;
		calculate();
	}
	
	double getStartAngle() {
		return startAngle;
	}
	
	void setStartAngle(const double a) {
		startAngle = a;
		calculate();
	}
	
	double getSector() {
		return sector;
	}
	
	void setSector(const double s) {
		sector = s;
		calculate();
	}
	
	void setRoundless(Pointf center, double radius, int count, double startAngle, double sector) {
		if (radius <= 0. || count <= 0) return;
		this->center = center;
		this->radius = radius;
		this->count = count;
		this->startAngle = startAngle;
		this->sector = sector;
		calculate();
	}
	
	Rectf getRect() override {
		double width2 = (radius * 2. + tool.diameter) / 2.;
		double height2 = (radius * 2. + tool.diameter) / 2.;
		return {
			-width2 + center.x,
			-height2 + center.y,
			width2 + center.x,
			height2 + center.y
		};
	}
	
	virtual String ToString() override {
		return String(t_("Drilling")) + "(" + DblStr(tool.diameter) + " x " + DblStr(depth) + ") " + t_("Radius:") + " " + DblStr(radius) + ", " + t_("Count:") + " " + IntStr(count);
	}
	
	virtual void Draw(DrawPainter& p, Rectf& viewRect, double scale, bool isMeasurers = false, bool isDrawDrillCenter = false) override {
		OperationDrill::Draw(p, viewRect, scale, isMeasurers, isDrawDrillCenter);
		if (isMeasurers) {
			if (sector > 0. && sector < 360.) {
				double radiusArc = radius / 2.;
				p.Arc(center, radius, startAngle * M_PI/180., (sector) * M_PI/180.);
				p.Move(center); p.Line(
					center.x + radius * cos((sector + startAngle) * M_PI/180.),
					center.y - radius * sin((-sector - startAngle) * M_PI/180.)
				);
				p.Move(center); p.Line(
					center.x + radius * cos(startAngle * M_PI/180.),
					center.y - radius * sin(-startAngle * M_PI/180.)
				);
				p.Arc(center, radius / 2., startAngle * M_PI/180., (sector) * M_PI/180.);
				p.Stroke(Settings::measurersLineWidth / scale, Settings::measurersColor);
				DrawMeasureRadius(p, scale,
					center.x, center.y,
					radius,
					"R" + DblStr(radius),
					startAngle + sector / 2.
				);
				String textAngle = DblStr(sector) + "°";
				Size textAngleSz = GetTextSize(textAngle, Settings::measurersFont);
				DrawText(p, scale,
				         (center.x + (radiusArc + 20./scale) * cos((-sector/2. - startAngle) * M_PI/180.)) - textAngleSz.cx/2./scale,
				         (center.y - (radiusArc + 20./scale) * sin((-sector/2. - startAngle) * M_PI/180.)) + textAngleSz.cy/2./scale,
				         0., textAngle, Settings::measurersFont, Settings::measurersColor);
			} else {
				p.Circle(center.x, center.y, radius);
				p.Stroke(Settings::measurersLineWidth / scale, Settings::measurersColor);
				DrawMeasureRadius(p, scale, center.x, center.y, radius, "R" + DblStr(radius));
			}
		}
	}

};

class OperationRoundlessTab : public WithOperationRoudless<ParentCtrl> {
private:
	OperationDrillRoundless *operation = NULL;
	
public:
	Event<> WhenPushToolEditor;
	OperationRoundlessTab() {
		CtrlLayout(*this);
		updateToolList();
		dlRadius.Add({
			{ODR_Radius , t_("Radius")},
			{ODR_Diameter , t_("Diameter")}
		});
		bToolEditor.WhenPush = [=] {
			WhenPushToolEditor();
		};
		dlTool.WhenAction = [=] {
			int i = dlTool.GetIndex();
			if (operation && i >= 0) {
				operation->setTool(dlTool.GetKey(i));
				Action();
			}
		};
		eDepth.WhenAction = [=] {
			double d = eDepth;
			if (operation && d > 0.) {
				operation->setDepth(d);
				Action();
			}
		};
		eCenterShiftX.WhenAction = [=] {
			if (operation) {
				Pointf center = operation->getCenter();
				operation->setCenter({eCenterShiftX, center.y});
				Action();
			}
		};
		eCenterShiftY.WhenAction = [=] {
			if (operation) {
				Pointf center = operation->getCenter();
				operation->setCenter({center.x, eCenterShiftY});
				Action();
			}
		};
		eRadius.WhenAction = [=] {
			double r = eRadius;
			if (operation && r > 0.) {
				operation->setRadius(r / (int)dlRadius.GetData());
				Action();
			}
		};
		dlRadius.WhenAction = [=] {
			if (dlRadius.GetData() == ODR_Radius) {
				eRadius <<= (double)~eRadius / 2.;
			} else {
				eRadius <<= (double)~eRadius * 2.;
			}
			eRadius.WhenAction();
		};
		eCount.WhenAction = [=] {
			int c = eCount;
			if (operation && c > 0) {
				operation->setCount(c);
				Action();
			}
		};
		eStartAngle.WhenAction = [=] {
			if (operation) {
				operation->setStartAngle(eStartAngle);
				Action();
			}
		};
		eSector.WhenAction = [=] {
			if (operation) {
				operation->setSector(eSector);
				Action();
			}
		};
	}
	
	OperationDrillRoundless* setOperation(Operation *operation) {
		dlRadius <<= ODR_Radius;
		if (operation == NULL) {
			this->operation = NULL;
			dlTool = -1;
			eDepth.Clear();
			eCenterShiftX.Clear();
			eCenterShiftY.Clear();
			eRadius.Clear();
			eCount.Clear();
			eStartAngle.Clear();
			eSector.Clear();
			return NULL;
		}
		OperationDrillRoundless *drRoundless = dynamic_cast<OperationDrillRoundless*>(operation);
		bool isCreateNew = (drRoundless == NULL);
		if (isCreateNew) {
			drRoundless = new OperationDrillRoundless(operation);
		}
		this->operation = drRoundless;
		dlTool <<= drRoundless->getTool();
		eDepth <<= drRoundless->getDepth();
		eCenterShiftX <<= drRoundless->getCenter().x;
		eCenterShiftY <<= drRoundless->getCenter().y;
		eRadius <<= drRoundless->getRadius();
		eCount <<= drRoundless->getCount();
		eStartAngle <<= drRoundless->getStartAngle();
		eSector <<= drRoundless->getSector();
		
		return isCreateNew ? drRoundless : NULL;
	}
	
	void updateToolList() {
		dlTool.Clear();
		for (Tool &t : Settings::tools) {
			dlTool.Add(t, t.ToString());
		}
		if (operation != NULL) dlTool <<= operation->getTool();
	}
};

#endif