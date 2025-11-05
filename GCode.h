#ifndef _GCODE_H_
#define _GCODE_H_

#include <Core/Core.h>
using namespace Upp;

#define CMD_STR(C) #C""
#define CMD_STR_END(C) #C"\n"

class GCode {
public:
	enum Flat {
		XY, XZ, YZ
	};
	enum CMD {
		NONE = 0,
		G0,
		G1,
		G2,
		G3,
		G17,
		G18,
		G19,
		G20,
		G21,
		G40,
		G49,
		G54, G55, G56, G57, G58, G59,
		G80,
		G90,
		G91,
		
		S,
		
		T,
		
		M3,
		M4,
		M5,
		M6,
		M8,
		M9,
		M30
	};
	
	virtual inline const String Start() { return ""; }
	virtual inline const String End() { return ""; }
	virtual inline const String Comment(const String& c) { return ""; }
	virtual inline const String Label(int num, const String& c) { return ""; }
	
	virtual inline const String Move0(double x = NAN, double y = NAN, double z = NAN, double f = NAN) { return ""; }
	virtual inline const String MoveZ0(double z = NAN, double f = NAN) {
		return Move0(NAN, NAN, z, f);
	}
	virtual inline const String Move1(double x = NAN, double y = NAN, double z = NAN, double f = NAN) { return ""; }
	virtual inline const String MoveZ1(double z = NAN, double f = NAN) {
		return Move1(NAN, NAN, z, f);
	}
	virtual inline const String Move2(double x = NAN, double y = NAN, double i = NAN, double j = NAN, double z = NAN) { return ""; }
	virtual inline const String Move3(double x = NAN, double y = NAN, double i = NAN, double j = NAN, double z = NAN) { return ""; }
	
	virtual inline const String RunSpindele(int speed, bool clockwise = true) { return ""; }
	virtual inline const String SetFlat(GCode::Flat f) { return ""; }
	virtual inline const String Inch() { return ""; }
	virtual inline const String Metric() { return ""; }
	virtual inline const String CoordinateSystem(int num) { return ""; }
	virtual inline const String AbsolutePos() { return ""; }
	virtual inline const String IncrementPos() { return ""; }
	virtual inline const String SpindeleOff() { return ""; }
	virtual inline const String CoolantOn() { return ""; }
	virtual inline const String CoolantOff() { return ""; }
	virtual inline const String ToolCompensationOff() { return ""; }
	virtual inline const String ToolLenCorrectionOff() { return ""; }
	virtual inline const String CiclesOff() { return ""; }
	
	virtual inline const String SetTool(Tool& t) { return ""; }

};

#endif
