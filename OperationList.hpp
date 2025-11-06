#ifndef _OPERATION_LIST_HPP_
#define _OPERATION_LIST_HPP_

#include <Core/Core.h>
#include "OperationDrillArray.hpp"
#include "OperationDrillRoundless.hpp"
#include "OperationMilling.hpp"

using namespace Upp;

class OperationList : public ColumnList {
	struct OperationListDisplay : public Display {
		virtual void Paint(Draw& w, const Rect& r, const Value& q, Color ink, Color paper, dword style) const {
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
	};
	
	OperationListDisplay operationListDisplay;
public:
	Event<> WhenRemove;
	
	OperationList() {
		SetDisplay(operationListDisplay);
	}
	
	virtual bool HotKey(dword key) {
		switch (key) {
			case K_DELETE: {
				int i = GetCursor();
				if (i >= 0) {
					Remove(i);
					if (GetCursor() == -1 && GetCount() > 0) {
						SetCursor(GetCount() - 1);
					}
					WhenRemove();
				}
				return true;
			}
		}
		return false;
	}

};



#endif