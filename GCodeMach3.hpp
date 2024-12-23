#ifndef _GCODE_MACH3_H_
#define _GCODE_MACH3_H_

#include "GCode.h"

#define DblStrMach3(d) FormatDouble(d, 3, FD_POINT | FD_FIX)

class GCodeMach3 : public GCode {
private:
	double lastX = NAN;
	double lastY = NAN;
	double lastZ = NAN;
	double lastF = NAN;
	double lastS = NAN;
public:
	
	virtual inline const String Comment(const String& c) {
		return String("(") << c << ")\n";
	}
	
	inline const String G0(double x = NAN, double y = NAN, double z = NAN, double f = NAN) {
		String sb("G00");
		if (!IsNaN(x) && lastX != x) {
			sb << " X" << DblStrMach3(x);
			lastX = x;
		}
		if (!IsNaN(y) && lastY != y) {
			sb << " Y" << DblStrMach3(y);
			lastY = y;
		}
		if (!IsNaN(z) && lastZ != z) {
			sb << " Z" << DblStrMach3(z);
			lastZ = z;
		}
		if (!IsNaN(f) && lastF != f) {
			sb << " F" << DblStrMach3(f);
			lastF = f;
		}
		return sb << "\n";
	}
	
	inline const String G1(double x = NAN, double y = NAN, double z = NAN, double f = NAN) {
		String sb("G01");
		if (!IsNaN(x) && lastX != x) {
			sb << " X" << DblStrMach3(x);
			lastX = x;
		}
		if (!IsNaN(y) && lastY != y) {
			sb << " Y" << DblStrMach3(y);
			lastY = y;
		}
		if (!IsNaN(z) && lastZ != z) {
			sb << " Z" << DblStrMach3(z);
			lastZ = z;
		}
		if (!IsNaN(f) && lastF != f) {
			sb << " F" << DblStrMach3(f);
			lastF = f;
		}
		return sb << "\n";
	}
	
	inline const String SpindeleOff() {
		return "M5\n";
	}
	
	inline const String CoolantOn() {
		return "M8\n";
	}
	
	inline const String CoolantOff() {
		return "M9\n";
	}
	
	inline const String End() {
		return "M30\n";
	}
	
};

#endif