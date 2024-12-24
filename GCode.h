#ifndef _GCODE_H_
#define _GCODE_H_

#include <Core/Core.h>
using namespace Upp;

class GCode {
public:
	enum Flat {
		XY, XZ, YZ
	};
	enum CMD {
		NONE = 0,
		G00,
		G01,
		G0,
		G1,
		G17,
		G18,
		G19,
		G20,
		G21,
		
		S,
		
		M5,
		M8,
		M9,
		M30
	};
	
	virtual inline const String Comment(const String& c) { return ""; }
	
	virtual inline const String Move0(double x = NAN, double y = NAN, double z = NAN, double f = NAN) { return ""; }
	virtual inline const String Move1(double x = NAN, double y = NAN, double z = NAN, double f = NAN) { return ""; }
	
	virtual inline const String RunSpindele(int speed, bool clockwise = true) { return ""; }
	virtual inline const String SetFlat(GCode::Flat f) { return ""; }
	virtual inline const String Inch() { return ""; }
	virtual inline const String Metric() { return ""; }
	virtual inline const String SpindeleOff() { return ""; }
	virtual inline const String CoolantOn() { return ""; }
	virtual inline const String CoolantOff() { return ""; }
	virtual inline const String End() { return ""; }

};

#endif
