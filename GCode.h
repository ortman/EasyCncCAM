#ifndef _GCODE_H_
#define _GCODE_H_

#include <Core/Core.h>
using namespace Upp;

class GCode {
public:
	virtual inline const String Comment(const String& c) { return ""; }
	
	virtual inline const String G0(double x = NAN, double y = NAN, double z = NAN, double f = NAN) { return ""; }
	virtual inline const String G1(double x = NAN, double y = NAN, double z = NAN, double f = NAN) { return ""; }
	
	virtual inline const String SpindeleOff() { return ""; }
	virtual inline const String CoolantOn() { return ""; }
	virtual inline const String CoolantOff() { return ""; }
	virtual inline const String End() { return ""; }
};

#endif
