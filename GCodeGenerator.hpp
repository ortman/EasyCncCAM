#ifndef __GCODE_GENERATOR__
#define __GCODE_GENERATOR__

#include <Core/Core.h>
#include "GCodeMach3.hpp"

using namespace Upp;

class GCodeGenerator {
public:
	static String Generate(const Vector<Operation*> &operations, GCode* g) {
		String s;
		if (g == NULL) return s;
		s << g->Start();
		s << g->SetFlat(GCode::XY);
		s << g->Metric();
			
		for (Operation *op : operations) {
			if (op->getTool().isValid()) {
				s << g->Comment(op->ToString());
				s << op->gcode(g);
			} else {
				ErrorOK(Format(t_("Tool (%s) is not valid parameters!"), op->getTool().ToString()));
			}
		}

		s << g->CoolantOff();
		s << g->SpindeleOff();
		s << g->End();

		return s;
	}
};

#endif