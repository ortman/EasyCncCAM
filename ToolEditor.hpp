#include "Resource.h"
#include "Tool.hpp"
#include "Settings.hpp"

class ToolEditor : public WithToolEditorLayout<TopWindow> {
private:
	struct ToolListDisplay : public Display {
		virtual void Paint(Draw& w, const Rect& r, const Value& q, Color ink, Color paper, dword style) const {
			PaintBackground(w, r, q, ink, paper, style);
			Tool tool = q;
			String s = tool.ToString();
			int tcy = GetTextSize(s, StdFont()).cy;
	    int c = 0;
			w.DrawText(r.left + c + 3, r.top+(r.GetHeight() - tcy) / 2, s, StdFont(), ink);
		}
	};
	ToolListDisplay toolListDisplay;
	Tool selectedTool;
public:
	ToolEditor() {
		CtrlLayout(*this, t_("Tool editor"));
		Sizeable().Zoomable();
		bAdd.WhenPush = [=] {
			Tool t(Tool::Drill, 0, 0, 0);
			clTools.Add(t);
			clTools.SetCursor(clTools.GetCount()-1);
		};
		bRemove.WhenPush = [=] {
			int i = clTools.GetCursor();
			if (i >= 0) {
				clTools.Remove(i);
			}
		};
		
		dlType.Add(Tool::Drill, Tool::typeToString(Tool::Drill, true));
		dlType.Add(Tool::Mill, Tool::typeToString(Tool::Mill, true));
		
		dlSearchType.Add(0, t_("Any"));
		dlSearchType.Add(Tool::Drill, Tool::typeToString(Tool::Drill, true));
		dlSearchType.Add(Tool::Mill, Tool::typeToString(Tool::Mill, true));
		dlSearchType = 0;
		
		clTools.SetDisplay(toolListDisplay);
		clTools.WhenSel = [=] {
			int i = clTools.GetCursor();
			if (i >= 0) {
				selectedTool = clTools.GetValue(i);
				dlType <<= selectedTool.type;
				eDiameter <<= selectedTool.diameter;
				eLength <<= selectedTool.length;
				eSpeed <<= selectedTool.speed;
			} else {
				dlType = -1;
				eDiameter.Clear();
				eLength.Clear();
				eSpeed.Clear();
			}
		};
		dlType.WhenAction = [=] {
			int i = clTools.GetCursor();
			int t = dlType.GetIndex()+1;
			if (t > 0 && i >= 0) {
				selectedTool.type = (Tool::Type)t;
				clTools.Set(i , selectedTool);
				clTools.Refresh();
			}
		};
		eDiameter.WhenAction = [=] {
			int i = clTools.GetCursor();
			if (i >= 0) {
				selectedTool.diameter = eDiameter;
				clTools.Set(i , selectedTool);
				clTools.Refresh();
			}
		};
		eLength.WhenAction = [=] {
			int i = clTools.GetCursor();
			if (i >= 0) {
				selectedTool.length = eLength;
				clTools.Set(i , selectedTool);
				clTools.Refresh();
			}
		};
		eSpeed.WhenAction = [=] {
			int i = clTools.GetCursor();
			if (i >= 0) {
				selectedTool.speed = eSpeed;
				clTools.Set(i , selectedTool);
				clTools.Refresh();
			}
		};
		
		bOk.WhenPush = [=] {
			Tool::tools.Clear();
			int cnt = clTools.GetCount();
			for (int i = 0; i < cnt; ++i) {
				Tool::tools.Add(clTools.GetValue(i));
			}
			Settings::Save();
			Close();
			WhenClose(); // TODO:: Why?
		};
		
		bCancel.WhenPush = [=] {
			Close();
		};
	}

	~ToolEditor() {
		//Tool::tools.clear();
	}
	
	int Run(bool appmodal = false) {
		clTools.Clear();
		for (Tool t : Tool::tools) {
			clTools.Add(t);
		}
		if (clTools.GetCount() > 0) clTools.SetCursor(0);
		return TopWindow::Run(appmodal);
	}
};