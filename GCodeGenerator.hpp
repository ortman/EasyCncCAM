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
		s << g->SetFlat(GCode::XY)
			<< g->Metric();
			
		for (Operation *op : operations) {
			s << g->Comment(op->ToString());
			s << op->gcode(g);
		}

		s << g->CoolantOff()
			<< g->SpindeleOff()
			<< g->End();

		return s;
	}
};

#endif