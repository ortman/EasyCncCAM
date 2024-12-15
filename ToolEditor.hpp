#include "Resource.h"

class ToolEditor : public WithToolEditorLayout<TopWindow> {
public:
	ToolEditor() {
		CtrlLayout(*this, t_("Tool edit"));
	}
};