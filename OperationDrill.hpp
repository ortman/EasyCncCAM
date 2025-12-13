#ifndef _OPERATION_DRILL_HPP_
#define _OPERATION_DRILL_HPP_

#include "Operation.hpp"

class OperationDrill : public Operation {
protected:
	Vector<Pointf> drills;
	Pointf center = {0., 0.};
	OperationDrill(){};
	
public:
	virtual void Draw(DrawPainter &p, Rectf& viewRect, double scale, bool isMeasurers = false, bool isDrawDrillCenter = false) {
		const double toolRadius = tool.diameter / 2.;
		for (const Pointf& pd : drills) {
			p.Circle(pd.x, pd.y, toolRadius);
		}
		p.Stroke(Settings::drillLineWidth / scale, Settings::drillColor);
		if (isDrawDrillCenter) {
			double lineLen2 = toolRadius + 5. / scale;
			for (const Pointf& pd : drills) {
				p.Move(pd.x, pd.y - lineLen2); p.Line(pd.x, pd.y + lineLen2);
				p.Move(pd.x - lineLen2, pd.y); p.Line(pd.x + lineLen2, pd.y);
			}
			p.Stroke(Settings::measurersLineWidth / scale, Settings::drillCenterColor);
		}
		if (isMeasurers && drills.GetCount() > 0) {
			const Pointf &d = drills[0];
			DrawMeasureDiameter(p, scale, d.x, d.y, tool.diameter, ("Ø" + DblStr(tool.diameter)));
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
		return String(t_("Drill(")) + DblStr(tool.diameter) + "x" + DblStr(depth) + ")";
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
				for (double l = d; l < len; l += d) {
					s << g.MoveZ1(-l, tool.feedRateZ);
					s << g.MoveZ0(1.);
					s << g.MoveZ0(-l + 0.5);
				}
				s << g.MoveZ1(-len, tool.feedRateZ);
				s << g.MoveZ0(-safeToolH);
		}
		s	<< g.CoolantOff();
		return s;
	}
};

#endif