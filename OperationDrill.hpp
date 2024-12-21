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

		double diameter = (tool.diameter + 1) * scale;
		double radius = diameter / 2;
		int x,y;
		for (Pointf pd : drills) {
			x = (int)((pd.x - shiftDraw.x) * scale - radius);
			y = (int)((pd.y - shiftDraw.y) * scale - radius);
			DrawAlphaEllipse(x, y, (int)(diameter), (int)(diameter), Null, 2, Blue());
			if (isDrawDrillCenter) {
				DrawAlphaLine(
					(int)((pd.x - shiftDraw.x) * scale),
					(int)((pd.y - shiftDraw.y - 3) * scale - radius),
					(int)((pd.x - shiftDraw.x) * scale),
					(int)((pd.y - shiftDraw.y + 3) * scale + radius),
					1/*(drill < 20) ? pen : PEN_DASHDOT*/, LtRed());
				DrawAlphaLine(
					(int)((pd.x - shiftDraw.x - 3) * scale - radius),
					(int)((pd.y - shiftDraw.y) * scale),
					(int)((pd.x - shiftDraw.x + 3) * scale + radius),
					(int)((pd.y - shiftDraw.y) * scale),
					1/*(drill < 20) ? pen : PEN_DASHDOT*/, LtRed());
			}
		}
		if (isDrawMeasure) {
			if (drills.GetCount() > 0) {
				Pointf &d = drills[0];
				DrawMeasureDiameter(d.x, d.y, tool.diameter, 45);
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
	
};

#endif