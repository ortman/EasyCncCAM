#ifndef _OPERATION_DRILL_ROUNDLESS_H_
#define _OPERATION_DRILL_ROUNDLESS_H_

#include "OperationDrill.hpp"

class OperationDrillRoundless : public OperationDrill {
private:
	double radius = 15.;
	int count = 6;
	double startAngle = 0.;
	double sector = 360.;
	
	void calculate() {
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
		calculateDraw();
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
	};
	
	double getRadius() {return radius;}
	void setRadius(const double r) {
		radius = r;
		calculate();
	}
	
	int getCount() {return count;}
	void setCount(const int c) {
		count = c;
		calculate();
	}
	
	double getStartAngle() {return startAngle;}
	void setStartAngle(const double a) {
		startAngle = a;
		calculate();
	}
	
	double getSector() {return sector;}
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
	
	Sizef getDrawSize() {
		double width = radius * 2. + tool.diameter + OPERATION_DRAW_OFFSET;
		return {width, width};
	}
	
	virtual String ToString() {
		return String(t_("Drilling")) + "(" + DblStr(tool.diameter) + " x " + DblStr(depth) + ") " + t_("Radius:") + " " + DblStr(radius) + ", " + t_("Count:") + " " + IntStr(count);
	}
	
	virtual void calculateDraw() {
		OperationDrill::calculateDraw();
		if (isDrawMeasure) {
			if (sector > 0. && sector < 360.) {
				double radiusArc = radius/2.;
				Size ds = getDrawSize();
				Pointf c = Pointf(
					center.x - shiftDraw.x,
					center.y - shiftDraw.y
				);
				String textAngle = DblStr(sector) + "°";
				Size textAngleSz = GetTextSize(textAngle, Settings::measurersFont);
				DrawAlphaArc(RectC(
						(int)((c.x - radiusArc) * scale),
						(int)((c.y - radiusArc) * scale),
						(int)(radiusArc * 2. * scale),
						(int)(radiusArc * 2. * scale)
					),
					Point(
						(int)((c.x + radiusArc * cos((sector + startAngle) * M_PI/180.)) * scale),
						(int)((c.y + radiusArc * sin((sector + startAngle) * M_PI/180.)) * scale)
					),
					Point(
						(int)((c.x + radiusArc * cos(startAngle * M_PI/180.)) * scale),
						(int)((c.y + radiusArc * sin(startAngle * M_PI/180.)) * scale)
					),
					Settings::measurersLineWidth, Settings::measurersColor
				);
				DrawAlphaLine(
					(int)(c.x * scale),
					(int)(c.y * scale),
					(int)((c.x + radius * cos((sector + startAngle) * M_PI/180.)) * scale),
					(int)((c.y + radius * sin((sector + startAngle) * M_PI/180.)) * scale),
					Settings::measurersLineWidth, Settings::measurersColor
				);
				DrawAlphaLine(
					(int)(c.x * scale),
					(int)(c.y * scale),
					(int)((c.x + radius * cos(startAngle * M_PI/180.)) * scale),
					(int)((c.y + radius * sin(startAngle * M_PI/180.)) * scale),
					Settings::measurersLineWidth, Settings::measurersColor
				);
				DrawAlphaArc(RectC(
						(int)((c.x - radius) * scale),
						(int)((c.y - radius) * scale),
						(int)(radius * 2 * scale),
						(int)(radius * 2 * scale)
					),
					Point(
						(int)((c.x + radius * cos((sector + startAngle) * M_PI/180.)) * scale),
						(int)((c.y + radius * sin((sector + startAngle) * M_PI/180.)) * scale)
					),
					Point(
						(int)((c.x + radius * cos(startAngle * M_PI/180.)) * scale),
						(int)((c.y + radius * sin(startAngle * M_PI/180.)) * scale)
					),
					Settings::measurersLineWidth, Settings::measurersColor
				);
				DrawMeasureRadius(center.x, center.y, radius, -startAngle - sector / 2.);
				DrawAlphaTextA(
					(int)((c.x + (radiusArc+2) * cos((-sector/2 - startAngle) * M_PI/180.)) * scale - textAngleSz.cx/2.),
					(int)((c.y + (radiusArc+2) * sin((sector/2. + startAngle) * M_PI/180.)) * scale - textAngleSz.cy/2.),
					0, textAngle, Settings::measurersFont, Settings::measurersColor);
			} else {
				double delta = (tool.diameter + OPERATION_DRAW_OFFSET)/2.;
				DrawAlphaEllipse(
					(int)(delta * scale), (int)(delta * scale),
					(int)((radius*2.) * scale), (int)((radius*2.) * scale),
					Null, Settings::measurersLineWidth, Settings::measurersColor);
					DrawMeasureRadius(center.x, center.y, radius);
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
				operation->setRadius(r);
				Action();
			}
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