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
	
	Rectf getRect() {
		double width2 = (radius * 2. + tool.diameter) / 2.;
		double height2 = (radius * 2. + tool.diameter) / 2.;
		return {
			-width2 + center.x,
			-height2 + center.y,
			width2 + center.x,
			height2 + center.y
		};
	}
	
	virtual String ToString() {
		return String(t_("Drilling")) + "(" + DblStr(tool.diameter) + " x " + DblStr(depth) + ") " + t_("Radius:") + " " + DblStr(radius) + ", " + t_("Count:") + " " + IntStr(count);
	}
	
	virtual void Draw(ImageDraw& draw, Size& imgSz, Rectf& viewRect, bool isMeasurers = false, bool isDrawDrillCenter = false) {
		OperationDrill::Draw(draw, imgSz, viewRect, isMeasurers, isDrawDrillCenter);
		if (isMeasurers) {
			Sizef viewSize = viewRect.GetSize();
			double scale = min(imgSz.cx / viewSize.cx, imgSz.cy / viewSize.cy);
			double shiftX = viewRect.left * scale - (imgSz.cx - viewSize.cx * scale) / 2.;
			double shiftY = viewRect.top * scale - (imgSz.cy - viewSize.cy * scale) / 2.;
			double radiusScale = radius * scale;
			int diameterScale = (int)(radiusScale * 2.);
			if (sector > 0. && sector < 360.) {
				double radiusArc = radiusScale/2.;
				Pointf c = Pointf(
					center.x * scale - shiftX,
					imgSz.cy - (center.y * scale - shiftY)
				);
				draw.DrawArc(RectC(
						(int)(c.x - radiusArc),
						(int)(c.y - radiusArc),
						(int)(radiusArc * 2.),
						(int)(radiusArc * 2.)
					),
					Point(
						(int)((c.x + radiusArc * cos(startAngle * M_PI/180.))),
						(int)((c.y + radiusArc * sin(-startAngle * M_PI/180.)))
					),
					Point(
						(int)((c.x + radiusArc * cos((sector + startAngle) * M_PI/180.))),
						(int)((c.y + radiusArc * sin((-sector - startAngle) * M_PI/180.)))
					),
					Settings::measurersLineWidth, Settings::measurersColor
				);
				
				draw.DrawLine(
					(int)(c.x),
					(int)(c.y),
					(int)((c.x + radiusScale * cos((sector + startAngle) * M_PI/180.))),
					(int)((c.y + radiusScale * sin((-sector - startAngle) * M_PI/180.))),
					Settings::measurersLineWidth, Settings::measurersColor
				);
				draw.DrawLine(
					(int)(c.x),
					(int)(c.y),
					(int)((c.x + radiusScale * cos(startAngle * M_PI/180.))),
					(int)((c.y + radiusScale * sin(-startAngle * M_PI/180.))),
					Settings::measurersLineWidth, Settings::measurersColor
				);
				draw.DrawArc(RectC(
						(int)(c.x - radiusScale),
						(int)(c.y - radiusScale),
						diameterScale,
						diameterScale
					),
					Point(
						(int)((c.x + radiusScale * cos(startAngle * M_PI/180.))),
						(int)((c.y + radiusScale * sin(-startAngle * M_PI/180.)))
					),
					Point(
						(int)((c.x + radiusScale * cos((sector + startAngle) * M_PI/180.))),
						(int)((c.y + radiusScale * sin((-sector - startAngle) * M_PI/180.)))
					),
					Settings::measurersLineWidth, Settings::measurersColor
				);
				DrawMeasureRadius(draw,
					center.x * scale - shiftX,
					imgSz.cy - (center.y * scale - shiftY),
					radiusScale,
					"R" + DblStr(radius),
					startAngle + sector / 2.
				);
				String textAngle = DblStr(sector) + "°";
				Size textAngleSz = GetTextSize(textAngle, Settings::measurersFont);
				draw.DrawText(
					(int)((c.x + (radiusArc+20.) * cos((-sector/2. - startAngle) * M_PI/180.)) - textAngleSz.cx/2.),
					(int)((c.y + (radiusArc+20.) * sin((-sector/2. - startAngle) * M_PI/180.)) - textAngleSz.cy/2.),
					0, textAngle, Settings::measurersFont, Settings::measurersColor);
					
			} else {
				draw.DrawEllipse(
					(int)(center.x * scale - shiftX - radiusScale),
					(int)(imgSz.cy - (center.y * scale - shiftY + radiusScale)),
					diameterScale,
					diameterScale,
					Null, Settings::measurersLineWidth, Settings::measurersColor);
				DrawMeasureRadius(draw, center.x * scale - shiftX, imgSz.cy - (center.y * scale - shiftY), radiusScale, "R" + DblStr(radius));
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