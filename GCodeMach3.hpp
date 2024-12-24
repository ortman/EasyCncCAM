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
	CMD lastCMD = NONE;
	CMD lastCooland = NONE;

public:
	virtual inline const String Comment(const String& c) {
		String s = c;
		s.Replace("(", "");
		s.Replace(")", "");
		lastCMD = NONE;
		return String("(") << s << ")\n";
	}
	
	inline const String Move0(double x = NAN, double y = NAN, double z = NAN, double f = NAN) {
		String sb;
		if (lastCMD != G00) sb << " G00";
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
		lastCMD = G00;
		if (sb.GetLength() == 0) return "";
		return sb.Mid(1) << "\n";
	}
	
	inline const String Move1(double x = NAN, double y = NAN, double z = NAN, double f = NAN) {
		String sb;
		if (lastCMD != G01) sb << " G01";
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
		lastCMD = G01;
		if (sb.GetLength() == 0) return "";
		return sb.Mid(1) << "\n";
	}
	
	inline const String RunSpindele(int speed, bool clockwise = true) {
		lastCMD = S;
		return String("S") << IntStr(speed) << (clockwise ? " M3\n" : " M4\n") ;
	}
	
	inline const String SetFlat(GCode::Flat f) {
		switch (f) {
			case XY: {
				lastCMD = G17;
				return "G17\n";
			}
			case XZ: {
				lastCMD = G18;
				return "G18\n";
			}
			default: {
				lastCMD = G19;
				return "G19\n";  //YZ
			}
		};
	}
	
	inline const String Inch() {
		lastCMD = G20;
		return "G20\n";
	}
	
	inline const String Metric() {
		lastCMD = G21;
		return "G21\n";
	}
	
	inline const String SpindeleOff() {
		lastCMD = M5;
		return "M5\n";
	}
	
	inline const String CoolantOn() {
		if (lastCooland == M8) return "";
		lastCooland = lastCMD = M8;
		return "M8\n";
	}
	
	inline const String CoolantOff() {
		if (lastCooland == M9) return "";
		lastCooland = lastCMD = M9;
		return "M9\n";
	}
	
	inline const String End() {
		lastCMD = M30;
		return "M30\n";
	}

};

#endif