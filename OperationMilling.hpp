#ifndef _OPERATION_DRILL_MILLING_H_
#define _OPERATION_DRILL_MILLING_H_

#include "Operation.hpp"

class OperationMilling : public Operation {
private:
	double depth;
	
	void calculate() {
	}
public:
	OperationMilling() {
		Tool tool;
		OperationMilling(tool, 0.1);
	}
	OperationMilling(Operation *operation) {
		OperationDrill *dr = dynamic_cast<OperationDrill*>(operation);
		if (dr) {
			tool = dr->getTool();
			depth = dr->getDepth();
		}
	}
	OperationMilling(Tool &tool, double depth) {
		this->tool = tool;
		this->depth = depth;
	}
	
	double getDepth() {return depth;}
	
	void setDepth(double d) {
		depth = d;
		calculate();
	}
	Sizef getDrawSize() {
		return {1., 1.};
	}

	virtual String ToString() {
		return String(t_("Milling")) + "(" + DblStr(tool.diameter) + " x " + DblStr(depth) + ")";
	}
	virtual const String gcode(GCode *g) {
		return "";
	}
};

class OperationMillingTab : public WithOperationMilling<ParentCtrl> {
private:
	OperationMilling *operation = NULL;
public:
	Event<> WhenPushToolEditor;
	OperationMillingTab() {
		CtrlLayout(*this);
		updateToolList();
		bToolEditor.WhenPush = [=] {
			WhenPushToolEditor();
		};
		dlTool.WhenAction = [=] {
			int i = dlTool.GetIndex();
			if (operation && i >= 0) {
				operation->setTool(dlTool.GetKey(i));
				Action();
			}
		};
		eDepth.WhenAction = [=] {
			double d = eDepth;
			if (operation && d > 0.) {
				operation->setDepth(d);
				Action();
			}
		};
	}
	OperationMilling* setOperation(Operation *operation) {
		if (operation == NULL) {
			this->operation = NULL;
			dlTool = -1;
			eDepth.Clear();
			return NULL;
		}
		OperationMilling *milling = dynamic_cast<OperationMilling*>(operation);
		bool isCreateNew = (milling == NULL);
		if (isCreateNew) {
			milling = new OperationMilling(operation);
		}
		this->operation = milling;
		dlTool <<= milling->getTool();
		eDepth <<= milling->getDepth();
		return isCreateNew ? milling : NULL;
	}
	void updateToolList() {
		dlTool.Clear();
		for (Tool &t : Settings::tools) {
			dlTool.Add(t, t.ToString());
		}
		if (operation != NULL) dlTool <<= operation->getTool();
	}
};

#endif