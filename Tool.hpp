#ifndef _EasyCncCAM_Tool_hpp_
#define _EasyCncCAM_Tool_hpp_

#include <CtrlLib/CtrlLib.h>

using namespace Upp;

class Tool : public Moveable<Tool> {
public:
	enum Type {
		Drill = 1,
		Mill
	};
	Tool::Type type;
	double diameter;
	double length;
	double speed;
public:
	static Vector<Tool*> tools;
	Tool(Tool::Type t, double d, double l, double s) : type(t), diameter(d), length(l), speed(s) {}
	String ToString() {
		return typeToString(type) + " " + DblStr(diameter) + "x" + DblStr(length);
	}
	static String typeToString(Tool::Type type) {
		switch (type) {
			case Type::Mill: {
				return t_("Mill");
			}
			default: {
				return t_("Drill");
			}
		}
	}
};

Vector<Tool*> Tool::tools;

#endif
