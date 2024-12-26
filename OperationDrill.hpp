#ifndef _OPERATION_DRILL_H_
#define _OPERATION_DRILL_H_

#include "Operation.hpp"

class OperationDrill : public Operation {
protected:
	Vector<Pointf> drills;
	double depth = 10.;
	Pointf center = {0., 0.};
	bool isDrawDrillCenter = false;
	OperationDrill(){};
	
	virtual void calculateDraw() {
		Sizef size = getDrawSize();
		this->shiftDraw = {center.x - size.cx/2, center.y - size.cy/2};
		Size imgSize = {(int)(size.cx * scale), (int)(size.cy * scale)};
		if (draw) delete draw;
		draw = new ImageDraw(imgSize);
		if (draw == NULL) return;
		//draw->Alpha().DrawRect({0, 0, (int)(imgSize.cx), (int)(imgSize.cy)}, GrayColor(100));
		//draw->DrawRect({0, 0, (int)(imgSize.cx), (int)(imgSize.cy)}, LtGreen());

		double diameter = tool.diameter * scale;
		double radius = diameter / 2.;
		int x, y;
		for (Pointf pd : drills) {
			x = (int)((pd.x - shiftDraw.x) * scale - radius);
			y = (int)((pd.y - shiftDraw.y) * scale - radius);
			DrawAlphaEllipse(x, y, (int)(diameter), (int)(diameter), Null, Settings::drillLineWidth, Settings::drillColor);
			if (isDrawDrillCenter) {
				DrawAlphaLine(
					(int)((pd.x - shiftDraw.x) * scale),
					(int)((pd.y - shiftDraw.y - 3) * scale - radius),
					(int)((pd.x - shiftDraw.x) * scale),
					(int)((pd.y - shiftDraw.y + 3) * scale + radius),
					Settings::measurersLineWidth, Settings::drillCenterColor);
				DrawAlphaLine(
					(int)((pd.x - shiftDraw.x - 3) * scale - radius),
					(int)((pd.y - shiftDraw.y) * scale),
					(int)((pd.x - shiftDraw.x + 3) * scale + radius),
					(int)((pd.y - shiftDraw.y) * scale),
					Settings::measurersLineWidth, Settings::drillCenterColor);
			}
		}
		if (isDrawMeasure) {
			if (drills.GetCount() > 0) {
				Pointf &d = drills[0];
				DrawMeasureDiameter(d.x, d.y, tool.diameter);
			}
		}
	}
	
public:
	
	OperationDrill(Operation* operation) {
		tool = operation->getTool();
		OperationDrill *dr = dynamic_cast<OperationDrill*>(operation);
		if (dr) {
			depth = dr->depth;
			center = dr->center;
			isDrawDrillCenter = dr->isDrawDrillCenter;
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
		
	//virtual void calculateDrills() {};
	
	bool getDrawDrillCenter() {return isDrawDrillCenter;}
	void setDrawDrillCenter(bool b = true) {
		isDrawDrillCenter = b;
		calculate();
	}
		
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