#ifndef _OPERATION_DRILL_H_
#define _OPERATION_DRILL_H_

#include "Operation.hpp"

class OperationDrill : public Operation {
protected:
	Vector<Pointf> drills;
	double depth = 10.;
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
				draw.DrawEllipse(x - radiusScaled, y - radiusScaled, (int)diameterScaled, (int)diameterScaled, Null, Settings::drillLineWidth, Settings::drillColor);
			}
			if (isDrawDrillCenter) {
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
				DrawMeasureDiameter(draw, d.x * scale - shiftX, imgSz.cy - (d.y * scale - shiftY), diameterScaled, ("Ø" + DblStr(tool.diameter)));
			}
		}
	}
	
	OperationDrill(Operation* operation) {
		tool = operation->getTool();
		OperationDrill *dr = dynamic_cast<OperationDrill*>(operation);
		if (dr) {
			depth = dr->depth;
			center = dr->center;
		} else {
//			OperationMilling *ml = dynamic_cast<OperationMilling*>(operation);
//			if (ml) {
//				drill = ml->getTool();
//				depth = ml->getDepth();
//			}
		}
	}

	
	~OperationDrill(){};
	
	Pointf getCenter() {return center;}
	void setCenter(const Pointf p) {
		center = p;
		calculate();
	}
	
	Vector<Pointf>& getDrills() {return drills;}
		
	double getDepth() {return depth;}
	
	void setDepth(double d) {depth = d;}
		
	virtual String ToString() {
		return String(t_("Drill(")) + DblStr(tool.diameter) + "x" + DblStr(depth) + ")";
	}
	
	virtual const String gcode(GCode *g) {
		String s;
		calculate();
		//s << g->CoordinateSystem(1);
		s << g->SetTool(tool);
		s << g->RunSpindele(tool.speed);
		s	<< g->CoolantOn();
		double d = tool.diameter;
		double len = min(tool.length, depth);
		s << g->MoveZ0(-safeToolH);
		for (Pointf p : drills) {
				s << g->Move0(p.x, p.y);
				s << g->MoveZ0(0.5);
				for (double l = d; l < len; l += d) {
					s << g->MoveZ1(-l, tool.feedRateZ);
					s << g->MoveZ0(1.);
					s << g->MoveZ0(-l + 0.5);
				}
				s << g->MoveZ1(-len, tool.feedRateZ);
				s << g->MoveZ0(-safeToolH);
		}
		s	<< g->CoolantOff();
		return s;
	}
};

#endif