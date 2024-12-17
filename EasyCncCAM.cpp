#include "EasyCncCAM.h"

#define TFILE <EasyCncCAM/Translation.t>
#include <Core/t.h>

EasyCncCAM::EasyCncCAM() {
	CtrlLayout(*this, "Easy CNC CAM");
	Sizeable().Zoomable();
	bGenerate.SetStyle(Button::StyleOk());
	oShowCoordinates <<= true;
	oShowDrillCenters <<= false;
	oShowCoordinates.WhenAction = [=] {
		viewer.drawCoordinates(oShowCoordinates);
	};
	oShowDrillCenters.WhenAction = [=] {
		viewer.drawDrillCenter(oShowDrillCenters);
	};
	
	bShowAll.WhenAction = [=] {
		viewer.showAllView();
	};
	
	bRemoveOperation.Disable();
	bAddOperation.WhenPush = [=] {
		OperationDrill *o = new OperationDrillRoundless();
		o->setDrawDrillCenter(oShowDrillCenters);
		operations.Add(o);
		clOperations.Add((int64)o);
		clOperations.SetCursor(clOperations.GetCount()-1);
		viewer.Refresh();
	};
	
	clOperations.SetDisplay(operationListDisplay);
	clOperations.WhenSel = [=] {
		int i = clOperations.GetCursor();
		bRemoveOperation.Enable(i>=0);
		if (i >= 0) {
			currentOperation = operations[i];
			OperationDrillArray *drArray = dynamic_cast<OperationDrillArray*>(currentOperation);
			int tab = 0;
			if (drArray == NULL) {
				OperationDrillRoundless *drRoundless = dynamic_cast<OperationDrillRoundless*>(currentOperation);
				tab = (drRoundless != NULL) ? 1 : 2;
			}
			if (tOperationSettings.Get() != tab) {
				tOperationSettings.Set(tab);
			}
			updateOperationTab();
		}
	};
	
	bRemoveOperation.WhenPush = [=] {
		int i = clOperations.GetCursor();
		if (i >= 0) {
			operations.Remove(i);
			delete currentOperation;
			currentOperation = NULL;
			clOperations.Remove(i);
			updateOperationTab();
		}
		//bRemoveOperation.Enable(clOperations.GetCursor()>=0);
	};
	
	tOperationSettings.Add(operationArrayTab, t_("Array"));
	tOperationSettings.Add(operationRoundlessTab, t_("Roundless"));
	tOperationSettings.Add(operationMillingTab, t_("Milling"));
	operationArrayTab.setOperation(currentOperation);
	operationArrayTab.WhenAction = [=] {
		viewer.Refresh();
		clOperations.Refresh();
	};
	operationRoundlessTab.WhenAction = [=] {
		viewer.Refresh();
		clOperations.Refresh();
	};
	operationMillingTab.WhenAction = [=] {
		viewer.Refresh();
		clOperations.Refresh();
	};
		
	operations.Add(currentOperation);
	clOperations.Add((int64)currentOperation);
	
	tOperationSettings.WhenAction = [=]{
		updateOperationTab();
	};
	
	viewer.setOperations(&operations);
	
	clOperations.SetCursor(clOperations.GetCount()-1);
	
	bGenerate.WhenPush = [=] {
		if (sel.ExecuteSaveAs()) {
			ErrorOK("Oops(. This functionality is not exist!");
		}
	};
}

EasyCncCAM::~EasyCncCAM() {
	viewer.setOperations(NULL);
	operationArrayTab.setOperation(NULL);
	operationRoundlessTab.setOperation(NULL);
	operationMillingTab.setOperation(NULL);
	currentOperation = NULL;
	int cnt = operations.GetCount();
	for (int i=cnt-1; i>=0; --i) {
		if (operations[i] != NULL) delete operations[i];
		//operations.Remove(i);
	}
}

int EasyCncCAM::FindOperation(Operation *op) {
	int cnt = operations.GetCount();
	for (int i=0; i<cnt; ++i) {
		if (operations[i] == op) return i;
	}
	return -1;
}

void EasyCncCAM::updateOperationTab() {
	switch (tOperationSettings.Get()) {
		case 0: {
			OperationDrill *op = operationArrayTab.setOperation(currentOperation);
			if (op) { // transform operation
				op->setDrawDrillCenter(oShowDrillCenters);
				int idx = FindOperation(currentOperation);
				if (idx >= 0) {
					operations[idx] = op;
					clOperations.Set(idx, (int64)op);
					delete currentOperation;
					currentOperation = op;
				//} else {
				//	operations.Add(op);
				}
			}
			break;
		}
		case 1: {
			OperationDrill *op = operationRoundlessTab.setOperation(currentOperation);
			if (op) { // transform operation
				op->setDrawDrillCenter(oShowDrillCenters);
				int idx = FindOperation(currentOperation);
				if (idx >= 0) {
					operations[idx] = op;
					clOperations.Set(idx, (int64)op);
					delete currentOperation;
					currentOperation = op;
				//} else {
				//	operations.Add(op);
				}
			}
			break;
		}
		case 2: {
			OperationMilling *op = operationMillingTab.setOperation(currentOperation);
			if (op) { // transform operation
				int idx = FindOperation(currentOperation);
				if (idx >= 0) {
					operations[idx] = op;
					clOperations.Set(idx, (int64)op);
					delete currentOperation;
					currentOperation = op;
				//} else {
				//	operations.Add(op);
				}
			}
			break;
		}
	}
	viewer.Refresh();
}

void EasyCncCAM::OperationListDisplay::Paint(Draw& w, const Rect& r, const Value& q, Color ink, Color paper, dword style) const {
	PaintBackground(w, r, q, ink, paper, style);
	Operation* op = (Operation*)(int64)q;
	if (op != NULL) {
		String s = op->ToString();
		int tcy = GetTextSize(s, StdFont()).cy;
	    //int c = r.Deflated(1).GetSize().cy;
	    int c = 0;
		w.DrawText(r.left + c + 3, r.top+(r.GetHeight() - tcy) / 2, s, StdFont(), ink);
	}
}

GUI_APP_MAIN {
	int lang = GetSystemLNG();
	lang = SetLNGCharset(lang, CHARSET_UTF8);
	SetLanguage(lang);
	EasyCncCAM().Run();
}
