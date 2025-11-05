#ifndef _OPERATION_DRILL_H_
#define _OPERATION_DRILL_H_

#include "Operation.hpp"

class OperationDrill : public Operation {
protected:
	Vector<Pointf> drills;
	Pointf center = {0., 0.};
	OperationDrill(){};
	
public:
	virtual void Draw(ImageDraw& draw, Size& imgSz, Rectf& viewRect, bool isMeasurers = false, bool isDrawDrillCenter = false) {
		Sizef viewSize = viewRect.GetSize();
		double scale = min(imgSz.cx / viewSize.cx, imgSz.cy / viewSize.cy);
		double shiftX = viewRect.left * scale - (imgSz.cx - viewSize.cx * scale) / 2.;
		double shiftY = viewRect.top * scale - (imgSz.cy - viewSize.cy * scale) / 2.;
		double diameterScaled = tool.diameter * scale;
		int radiusScaled = (int)(diameterScaled / 2.);
		int x, y;
		for (Pointf pd : drills) {
			x = (int)(pd.x * scale - shiftX);
			y = imgSz.cy - (int)(pd.y * scale - shiftY);
			if (viewRect.Contains(pd)) {
				if (tool.type == Tool::Thread) {
					draw.DrawArc({x - radiusScaled, y - radiusScaled, x + radiusScaled, y + radiusScaled}, {x, y - radiusScaled}, {x + radiusScaled, y}, Settings::threadLineWidth, Settings::threadColor);
				} else {
					draw.DrawEllipse(x - radiusScaled, y - radiusScaled, (int)diameterScaled, (int)diameterScaled, Null, Settings::drillLineWidth, Settings::drillColor);
				}
			}
			if (isDrawDrillCenter && tool.type != Tool::Thread) {
				int radiusDrillCenter = radiusScaled + (int)(5. * Settings::subsampling);
				draw.DrawLine(
					x, y - radiusDrillCenter,
					x, y + radiusDrillCenter,
					Settings::measurersLineWidth, Settings::drillCenterColor);
				draw.DrawLine(
					x - radiusDrillCenter, y,
					x + radiusDrillCenter, y,
					Settings::measurersLineWidth, Settings::drillCenterColor);
			}
		}
		if (isMeasurers) {
			if (drills.GetCount() > 0) {
				Pointf &d = drills[0];
				DrawMeasureDiameter(draw, d.x * scale - shiftX, imgSz.cy - (d.y * scale - shiftY), diameterScaled,
						(String(tool.type == Tool::Thread ? "M" : "Ø") + DblStr(tool.diameter)),
						tool.type == Tool::Thread ? -30. : 45.);
			}
		}
	}
	
	OperationDrill(Operation* operation) {
		tool = operation->getTool();
		depth = operation->getDepth();
		OperationDrill *dr = dynamic_cast<OperationDrill*>(operation);
		if (dr) {
			center = dr->center;
		}
	}

	~OperationDrill(){};
	
	Pointf getCenter() {return center;}
	void setCenter(const Pointf p) {
		center = p;
		calculate();
	}
	
	Vector<Pointf>& getDrills() {return drills;}
		
	virtual String ToString() {
		return String(tool.type == Tool::Thread ? t_("Threading") : t_("Drilling")) +
				" (" + DblStr(tool.diameter) + "x" + DblStr(depth) + ")";
	}
	
	virtual const String gcode(GCode &g) {
		String s;
		calculate();
		//s << g->CoordinateSystem(1);
		s << g.SetTool(tool);
		s << g.RunSpindele(tool.speed);
		s	<< g.CoolantOn();
		double d = tool.diameter;
		double len = min(tool.length, depth);
		s << g.MoveZ0(-safeToolH);
		for (Pointf p : drills) {
				s << g.Move0(p.x, p.y);
				s << g.MoveZ0(0.5);
				if (tool.type == Tool::Thread) {
					double d = tool.diameter - tool.threadToolDiameter;
					double r = d / 2.;
					if (r > 0.) {
						s << g.Move1(p.x + r, p.y);
						double l = 0.5;
						double halfStep = tool.threadStep / 2.;
						while (l - tool.threadStep > -len) {
							l -= halfStep;
							s << g.Move2(p.x - r, p.y, -r, 0., l);
							l -= halfStep;
							s << g.Move2(p.x + r, p.y, r, 0., l);
						}
						s << g.Move1(p.x, p.y);
					}
				} else {
					for (double l = d; l < len; l += d) {
						s << g.MoveZ1(-l, tool.feedRateZ);
						s << g.MoveZ0(1.);
						s << g.MoveZ0(-l + 0.5);
					}
					s << g.MoveZ1(-len, tool.feedRateZ);
				}
				s << g.MoveZ0(-safeToolH);
		}
		s	<< g.CoolantOff();
		return s;
	}
};

#endif