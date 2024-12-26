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
	inline const String Start() {
		lastCMD = NONE;
		return "%\n";
	}
	
	inline const String End() {
		lastCMD = NONE;
		return "M30\n%\n";
	}
	
	inline const String Comment(const String& c) {
		String s = c;
		s.Replace("(", "");
		s.Replace(")", "");
		lastCMD = NONE;
		return String("(") << s << ")\n";
	}
	
	inline const String Label(int num, const String& c) {
		if (num < 0) num = 0;
		else if (num > 99999) num = 99999;
		return String("O") << IntStr(num) << Comment(c);
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
		if (sb.GetLength() == 0) return "";
		lastCMD = G00;
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
		if (sb.GetLength() == 0) return "";
		lastCMD = G01;
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
				return CMD_STR_END(G17);
			}
			case XZ: {
				lastCMD = G18;
				return CMD_STR_END(G18);
			}
			default: {  //YZ
				lastCMD = G19;
				return CMD_STR_END(G19);
			}
		};
	}
	
	inline const String Inch() {
		lastCMD = G20;
		return CMD_STR_END(G20);
	}
	
	inline const String Metric() {
		lastCMD = G21;
		return CMD_STR_END(G21);
	}
	
	inline const String CoordinateSystem(int num) {
		if (num < 1 || num > 6) return "";
		lastCMD = (CMD)(G54 + num - 1);
		switch (lastCMD) {
			case G54: return CMD_STR_END(G54);
			case G55: return CMD_STR_END(G55);
			case G56: return CMD_STR_END(G56);
			case G57: return CMD_STR_END(G57);
			case G58: return CMD_STR_END(G58);
			default:  return CMD_STR_END(G59);
		}
	}
	
	inline const String AbsolutePos() {
		lastCMD = G90;
		return CMD_STR_END(G90);
	}
	
	inline const String IncrementPos() {
		lastCMD = G91;
		return CMD_STR_END(G91);
	}
	
	inline const String SpindeleOff() {
		lastCMD = M5;
		return CMD_STR_END(M5);
	}
	
	inline const String CoolantOn() {
		if (lastCooland == M8) return "";
		lastCooland = lastCMD = M8;
		return CMD_STR_END(M8);
	}
	
	inline const String CoolantOff() {
		if (lastCooland == M9) return "";
		lastCooland = lastCMD = M9;
		return CMD_STR_END(M9);
	}
	
	inline const String ToolCompensationOff() {
		return CMD_STR_END(G40);
	}
	
	inline const String ToolLenCorrectionOff() {
		return CMD_STR_END(G49);
	}
	
	inline const String CiclesOff() {
		return CMD_STR_END(G80);
	}
	
	inline const String SetTool(Tool& t) {
		return "T1 M6\n";
	}

};

#endif