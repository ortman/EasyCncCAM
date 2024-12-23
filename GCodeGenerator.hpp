#ifndef __GCODE_GENERATOR__
#define __GCODE_GENERATOR__

#include <Core/Core.h>
#include "GCodeMach3.hpp"

using namespace Upp;

class GCodeGenerator {
public:
	static String Generate(const Vector<Operation*> &operations, GCode* g) {
		String s;
		for (Operation *op : operations) {
			s << op->gcode(g);
		}
		s << g->CoolantOff() << g->SpindeleOff() << g->End();
		return s;
	}
};

#endif