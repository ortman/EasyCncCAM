#ifndef _EASY_CNC_CAM_H_
#define _EASY_CNC_CAM_H_

#include <CtrlLib/CtrlLib.h>

using namespace Upp;

#include "OperationList.hpp"
#include "ViewerCAM.hpp"
#include "Resource.h"
#include "SettingsWindow.hpp"
#include "ToolEditor.hpp"

class EasyCncCAM : public WithEasyCncCAMLayout<TopWindow> {
private:
	MenuBar menu;
	SettingsWindow settingsWindow;
	FileSel sel;
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
