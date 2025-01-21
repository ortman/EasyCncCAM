#ifndef __GCODE_GENERATOR__
#define __GCODE_GENERATOR__

#include <Core/Core.h>
#include "GCodeMach3.hpp"

using namespace Upp;

class GCodeGenerator {
public:
	static String Generate(const Vector<Operation*> &operations, GCode* g) {
		String s;
		if (g == NULL) return s;
		s << g->Start();
		s << g->Label(1, "Program name");
		s << g->CoordinateSystem(1);
		s << g->AbsolutePos();
		s << g->SetFlat(GCode::XY);
		s << g->ToolCompensationOff();
		s << g->CiclesOff();
		//s << G00; // ???
		s << g->ToolLenCorrectionOff();
		s << g->Metric();
			
		for (Operation *op : operations) {
			if (op->getTool().isValid()) {
				s << g->Comment(op->ToString());
				s << op->gcode(g);
			} else {
				ErrorOK(Format(t_("Tool (%s) is not valid parameters!"), op->getTool().ToString()));
			}
		}

		s << g->CoolantOff();
		s << g->SpindeleOff();
		s << g->End();

		return s;
	}

	static String GenerateFileName(const Vector<Operation*> &operations) {
		Tool tool;
		double depth = 0.;
		int count = 0;
		OperationDrill *dr;
		for (Operation* o : operations) {
			if (tool.diameter == 0.) tool = o->getTool();
			if (tool == o->getTool()) {
				if (o->getDepth() > depth) depth = o->getDepth();
				dr = dynamic_cast<OperationDrill*>(o);
				if (dr) {
					count += dr->getDrills().GetCount();
				}
			}
		}
		if (count) {
			return Format("D%0.4g`x%0.5g(%i).", tool.diameter, depth, count) + Settings::fileExt;
		} else {
			return Format("D%0.4g`x%0.5g.", tool.diameter, depth) + Settings::fileExt;
		}
	}

	static String GetSaveDirectory() {
		Array<FileSystemInfo::FileInfo> root;
		#ifdef PLATFORM_WIN32
			root = StdFileSystemInfo().Find(Null);
			for (int i = 0; i < root.GetCount(); i++) {
				String n = root[i].filename;
				if (n != "A:\\" && n != "B:\\" && root[i].root_style == FileSystemInfo::ROOT_REMOVABLE) {
					return n;
				}
			}
		#endif

		#ifdef PLATFORM_POSIX
			root = StdFileSystemInfo().Find("/media/*");
			for (int i = 0; i < root.GetCount(); i++) {
				String fn = root[i].filename;
				if (*fn != '.' && fn.Find("floppy") < 0)
					return "/media/" + fn;
			}
		#endif

		return "";
	}
};

#endif