#include "Resource.h"
#include "Tool.hpp"
#include "Settings.hpp"

class ToolEditor : public WithToolEditorLayout<TopWindow> {
private:
	struct ToolListDisplay : public Display {
		virtual void Paint(Draw& w, const Rect& r, const Value& q, Color ink, Color paper, dword style) const {
			PaintBackground(w, r, q, ink, paper, style);
		Tool* tool = (Tool*)(int64)q;
		if (tool != NULL) {
				String s = tool->ToString();
				int tcy = GetTextSize(s, StdFont()).cy;
		    int c = 0;
				w.DrawText(r.left + c + 3, r.top+(r.GetHeight() - tcy) / 2, s, StdFont(), ink);
			}
		}
	};
	ToolListDisplay toolListDisplay;
	Tool *selectedTool = NULL;
public:
	ToolEditor() {
		CtrlLayout(*this, t_("Tool editor"));
		Sizeable().Zoomable();
		bAdd.WhenPush = [=] {
			Tool *t = new Tool(Tool::Drill, 0, 0, 0);
			Tool::tools.Add(t);
			clTools.Add((int64)t);
			clTools.SetCursor(clTools.GetCount()-1);
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
			if (i >= 0 && i < Tool::tools.GetCount()) {
				Tool *t = Tool::tools[i];
				selectedTool = t;
				dlType <<= t->type;
				eDiameter <<= t->diameter;
				eLength <<= t->length;
				eSpeed <<= t->speed;
			}
		};
		dlType.WhenAction = [=] {
			if (dlType.GetIndex() >=0 && selectedTool != NULL) {
				selectedTool->type = (Tool::Type)(dlType.GetIndex()+1);
			}
			clTools.Refresh();
		};
		eDiameter.WhenAction = [=] {
			if (selectedTool != NULL) {
				selectedTool->diameter = eDiameter;
			}
			clTools.Refresh();
		};
		eLength.WhenAction = [=] {
			if (selectedTool != NULL) {
				selectedTool->length = eLength;
			}
			clTools.Refresh();
		};
		eSpeed.WhenAction = [=] {
			if (selectedTool != NULL) {
				selectedTool->speed = eSpeed;
			}
			clTools.Refresh();
		};
		
		bOk.WhenPush = [=] {
			Settings::Save();
			Close();
		};
		
		bCancel.WhenPush = [=] {
			Close();
		};
	}

	~ToolEditor() {
		selectedTool = NULL;
		for (Tool *t : Tool::tools) {
			delete t;
		}
		Tool::tools.clear();
	}
	
	int Run(bool appmodal = false) {
		clTools.Clear();
		for (Tool *t : Tool::tools) {
			clTools.Add((int64)t);
		}
		if (clTools.GetCount() > 0) clTools.SetCursor(0);
		return TopWindow::Run(appmodal);
	}
};