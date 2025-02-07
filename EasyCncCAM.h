#ifndef _EASY_CNC_CAM_H_
#define _EASY_CNC_CAM_H_

#include <CtrlLib/CtrlLib.h>

using namespace Upp;

#include "ViewerCAM.hpp"
#include "Resource.h"
#include "SettingsWindow.hpp"
#include "ToolEditor.hpp"

class EasyCncCAM : public WithEasyCncCAMLayout<TopWindow> {
	struct OperationListDisplay : public Display {
		virtual void Paint(Draw& w, const Rect& r, const Value& q, Color ink, Color paper, dword style) const;
	};
private:
	MenuBar menu;
	SettingsWindow settingsWindow;
	FileSel sel;
	OperationListDisplay operationListDisplay;
	Operation *currentOperation = NULL;
	Vector<Operation*> operations;
	OperationArrayTab operationArrayTab;
	OperationRoundlessTab operationRoundlessTab;
	OperationMillingTab operationMillingTab;
	int FindOperation(Operation *op);
	void updateOperationTab();
public:
	EasyCncCAM();
	~EasyCncCAM();
};

#endif
