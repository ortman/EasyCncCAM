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
		if (tool.type == Tool::Thread) {
			for (const Pointf& pd : drills) {
				p.Move(
					pd.x /* + toolRadius * cos(M_PI_2) */,
					pd.y    - toolRadius * sin(-M_PI_2)
				);
				p.Arc(pd, toolRadius, M_PI_2, M_PI_2 * 3.);
			}
			p.Stroke(Settings::threadLineWidth / scale, Settings::threadColor);
		} else {
			for (const Pointf& pd : drills) {
				p.Circle(pd.x, pd.y, toolRadius);
			}
			p.Stroke(Settings::drillLineWidth / scale, Settings::drillColor);
		}
		if (isDrawDrillCenter && tool.type != Tool::Thread) {
			double lineLen2 = toolRadius + 5. / scale;
			for (const Pointf& pd : drills) {
				p.Move(pd.x, pd.y - lineLen2); p.Line(pd.x, pd.y + lineLen2);
				p.Move(pd.x - lineLen2, pd.y); p.Line(pd.x + lineLen2, pd.y);
			}
			p.Stroke(Settings::measurersLineWidth / scale, Settings::drillCenterColor);
		}
		if (isMeasurers && drills.GetCount() > 0) {
			const Pointf &d = drills[0];
			DrawMeasureDiameter(p, scale, d.x, d.y, tool.diameter,
					(String(tool.type == Tool::Thread ? "M" : "Ø") + DblStr(tool.diameter)),
					tool.type == Tool::Thread ? -30. : 45.);
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