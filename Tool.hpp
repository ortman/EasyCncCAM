#ifndef _EasyCncCAM_Tool_hpp_
#define _EasyCncCAM_Tool_hpp_

#include <CtrlLib/CtrlLib.h>

using namespace Upp;

struct Tool : ValueType<Tool, 10013, Comparable<Tool, Moveable<Tool>>> {
//public:
	enum Type {
		Other = 0,
		Drill = 1,
		Mill
	};
	Tool::Type type;
	double diameter;
	double length;
	double speed;
//public:
	Tool() {}
	Tool(Tool::Type t, double d, double l, double s) : type(t), diameter(d), length(l), speed(s) {}
	Tool(const Value& v) {
		type = typeFromString(v["type"]);
		diameter = v["diameter"];
		length = v["length"];
		speed = v["speed"];
	}

	operator Value() const {
		//return RichValue<Tool>(*this);
		Value v;
		v("type") = typeToString(type);
		v("diameter") = diameter;
		v("length") = length;
		v("speed") = speed;
		return v;
	}
	int Compare(const Tool& o) const { return CombineCompare((int)type, (int)o.type)(diameter, o.diameter)(length, o.length)(speed, o.speed); }
	void Serialize(Stream& s) {
		//String t = typeToString(type);
		int t = type;
		s % t % diameter % length % speed;
	}
	String ToString() const {
		return typeToString(type, true) + " " + DblStr(diameter) + "x" + DblStr(length);
	}
	hash_t GetHashValue() const { return CombineHash((int)(type), diameter, length, speed); }
	bool IsNullInstance() const { return IsNull(diameter); }
	
	static String typeToString(Type type, bool localize = false) {
		switch (type) {
			case Type::Mill: {
				return localize ? t_("Mill") : "Mill";
			}
			default: {
				return localize ? t_("Drill") : "Drill";
			}
		}
	}
	static Type typeFromString(String type) {
		if (type == "Mill") return Mill;
		if (type == "Drill") return Drill;
		if (type == t_("Mill")) return Mill;
		if (type == t_("Drill")) return Drill;
		return Other;
	}
	
	void Xmlize(XmlIO& xio) { XmlizeByJsonize(xio, *this); };
	
	void Jsonize(JsonIO& json) {
		if (json.IsStoring()) {
			String t = typeToString(type);
			json
				("type", t)
				("diameter", diameter)
				("length", length)
				("speed", speed)
			;
		} else {
			Value v = json.Get();
			if (!v.IsNull()) {
				type = typeFromString(v["type"]);
				diameter = v["diameter"];
				length = v["length"];
				speed = v["speed"];
			}
		}
	}
	
	Tool& operator=(const Tool &t) {
		if (this != &t) {
			type = t.type;
			diameter = t.diameter;
			length = t.length;
			speed = t.speed;
		}
		return *this;
	}
	int PolyCompare(const Value& v) const {
		return Compare(v);
	}
	static dword ValueTypeNo() { return 10013; }
};

INITBLOCK {
    Value::Register<Tool>();
}

#endif
