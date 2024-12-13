#ifndef _EasyCncCAM_EasyCncCAM_h
#define _EasyCncCAM_EasyCncCAM_h

#include <CtrlLib/CtrlLib.h>

using namespace Upp;

#include "ViewerCAM.hpp"

#define LAYOUTFILE <EasyCncCAM/EasyCncCAM.lay>
#include <CtrlCore/lay.h>

#define IMAGEFILE <EasyCncCAM/EasyCncCAM.iml>
#include <Draw/iml_header.h>

class EasyCncCAM : public WithEasyCncCAMLayout<TopWindow> {
	struct OperationListDisplay : public Display {
		virtual void Paint(Draw& w, const Rect& r, const Value& q, Color ink, Color paper, dword style) const;
	};
private:
	FileSel sel;
	OperationListDisplay operationListDisplay;
	Operation *currentOperation = new OperationDrillArray();
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
