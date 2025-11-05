#ifndef _TOOL_H_
#define _TOOL_H_

#include <CtrlLib/CtrlLib.h>

using namespace Upp;

struct Tool : ValueType<Tool, 10013, Comparable<Tool, Moveable<Tool>>> {
	enum Type {
		None = 0,
		Drill = 1,
		Mill,
		Thread
	};
	Tool::Type type;
	double diameter;
	double length;
	int speed;
	int feedRateXY;
	int feedRateZ;
	double threadToolDiameter;
	double threadStep;

	Tool() : Tool(Drill, 0., 10., 6000, 0, 0, 0., 0.) {}
	Tool(Tool::Type t, double d, double l,
			int s, int fXY, int fZ,
			double tTD, double tS) :
			type(t), diameter(d), length(l),
			speed(s), feedRateXY(fXY), feedRateZ(fZ),
			threadToolDiameter(tTD), threadStep(tS) {
	}
	Tool(const Value& v) {
		type = typeFromString(v["type"]);
		diameter = v["diameter"];
		length = v["length"];
		speed = v["speed"];
		feedRateXY =  v["feedrateXY"];
		if (type == Drill) feedRateXY = 0;
		feedRateZ = v["feedrateZ"];
		threadToolDiameter = v["threadToolDiameter"];
		threadStep = v["threadStep"];
		if (type != Thread) {
			threadStep = 0.;
			threadToolDiameter = 0.;
		}
	}

	operator Value() const {
		Value v;
		v("type") = typeToString(type);
		v("diameter") = diameter;
		v("length") = length;
		v("speed") = speed;
		v("feedrateXY") = feedRateXY;
		v("feedrateZ") = feedRateZ;
		v("threadToolDiameter") = threadToolDiameter;
		v("threadStep") = threadStep;
		return v;
	}
	
	int Compare(const Tool& o) const { return CombineCompare((int)type, (int)o.type)(diameter, o.diameter)(length, o.length); }
	
	void Serialize(Stream& s) {
		int t = type;
		s % t % diameter % length;
	}
	
	String ToString() const {
		return typeToString(type, true) + " " + DblStr(diameter) + "x" + DblStr(length);
	}
	
	hash_t GetHashValue() const { return CombineHash((int)(type), diameter, length); }
	
	bool IsNullInstance() const { return IsNull(diameter); }
	
	static String typeToString(Type type, bool localize = false) {
		switch (type) {
			case Mill: return localize ? t_("Mill") : "Mill";
			case Thread: return localize ? t_("Thread") : "Thread";
			case Drill: return localize ? t_("Drill") : "Drill";
			default: return localize ? t_("None") : "None";
		}
	}
	
	static Type typeFromString(String type) {
		if (type == "Mill") return Mill;
		if (type == "Drill") return Drill;
		if (type == "Thread") return Thread;
		if (type == t_("Mill")) return Mill;
		if (type == t_("Drill")) return Drill;
		if (type == t_("Thread")) return Thread;
		return None;
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
				("feedrateXY", feedRateXY)
				("feedrateZ", feedRateZ)
				("threadToolDiameter", threadToolDiameter)
				("threadStep", threadStep)
			;
		} else {
			Value v = json.Get();
			if (!v.IsNull()) {
				type = typeFromString(v["type"]);
				diameter = v["diameter"];
				length = v["length"];
				speed = v["speed"];
				feedRateXY = v["feedrateXY"];
				feedRateZ = v["feedrateZ"];
				threadToolDiameter = v["threadToolDiameter"];
				threadStep = v["threadStep"];
			}
		}
	}
	
	Tool& operator=(const Tool &t) {
		if (this != &t) {
			type = t.type;
			diameter = t.diameter;
			length = t.length;
			speed = t.speed;
			feedRateXY = t.feedRateXY;
			feedRateZ = t.feedRateZ;
			threadToolDiameter = t.threadToolDiameter;
			threadStep = t.threadStep;
		}
		return *this;
	}
	
	int PolyCompare(const Value& v) const {
		return Compare(v);
	}
	
	bool isValid() {
		if (type != Drill && type != Mill && type != Thread) return false;
		if (diameter <= 0.) return false;
		if (length <= 0.1) return false;
		if (speed < 1) return false;
		if (feedRateXY < 0 || (type == Drill && feedRateXY != 0)) return false;
		if (feedRateZ <= 0) return false;
		if (type == Thread && (threadToolDiameter <= 0. || threadStep <= 0.)) return false;
		return true;
	}
	
	static dword ValueTypeNo() { return 10013; }
};

INITBLOCK {
    Value::Register<Tool>();
}

#endif
