#include <Core/Core.h>
#include "Operation.hpp"

using namespace Upp;

#define MIN_SCALE 0.2
#define MAX_SCALE 6.0

class ViewerCAM : public Ctrl {
private:
	Size coordSize = {100, 100};
	void drawCoordinates(Draw &w, Point p) {
		w.DrawLine(p.x, p.y, p.x + coordSize.cx, p.y, 1, Red());
		w.DrawLine(p.x, p.y - coordSize.cy, p.x, p.y, 1, Green());
	}
	Vector<Operation*>* operations = NULL;
	bool isDrawCoordinates = true;
	bool isDrawDrillCenter = false;
	double scale = 1.;
	Pointf shiftDrag = {0., 0.};
	Pointf startDrag = {0, 0};
public:
	void drawCoordinates(bool isDraw = true) {
		isDrawCoordinates = isDraw;
		Refresh();
	}
	
	void drawDrillCenter(bool isDraw = true) {
		isDrawDrillCenter = isDraw;
		if (operations != NULL) {
			for (Operation *op : *operations) {
				if (op) {
					OperationDrill *dr = dynamic_cast<OperationDrill*>(op);
					if (dr) dr->setDrawDrillCenter(isDraw);
				}
			}
		}
		Refresh();
	}
	
	void Paint(Draw &w) {
		Size sz = GetSize();
		w.DrawRect(sz, Color(240, 240, 255));
		Pointf shift = {(double)sz.cx / 2 + shiftDrag.x, (double)sz.cy / 2 + shiftDrag.y};
		if (operations != NULL) {
			for (Operation* o : *operations) {
				if (o != NULL) {
					ImageDraw *operImage = o->getDraw();
					if (operImage != NULL) {
						Sizef size = o->getDrawSize();
						w.DrawImage(
							(int)(shift.x + o->getDrawShift().x * scale),
							(int)(shift.y + o->getDrawShift().y * scale),
							(int)(size.cx * scale),
							(int)(size.cy * scale),
							*operImage);
					}
				}
			}
		}
		if (isDrawCoordinates) drawCoordinates(w, shift);
	}
	
	void setOperations(Vector<Operation*>* o) {
		operations = o;
		Refresh();
	}
	
	void MouseWheel(Point p, int zdelta, dword keyflags) {
		if (zdelta > 0) {
			if (scale > MAX_SCALE) return;
			scale *= 1.2;
		} else {
			if (scale < MIN_SCALE) return;
			scale /= 1.2;
		}
		
		double s = Operation::getScale() / OPERATION_SCALE - scale;
		if (s > 0.5 || s < - 0.5) {
			Operation::setScale(scale * OPERATION_SCALE);
			if (operations != NULL) {
				for (Operation *op : *operations) {
					if (op != NULL) op->calculateDraw();
				}
			}
		}
		Refresh();
	}
	
	virtual void LeftDown(Point p, dword keyflags) {
		startDrag = {p.x - shiftDrag.x, p.y - shiftDrag.y};
		Image m = Ctrl::OverrideCursor(Image::SizeAll());
	}
	
  virtual void MouseMove(Point p, dword keyflags) {
    if (startDrag.x > 0) {
      shiftDrag.x = p.x - startDrag.x;
      shiftDrag.y = p.y - startDrag.y;
      Refresh();
    }
  }
    
	virtual void LeftUp(Point p, dword keyflags) {
		startDrag = {-1, -1};
		Ctrl::OverrideCursor(Image::Arrow());
	}
	
	void showAllView() {
		Size sz, viewSize = GetSize();
		Rectf operationsRect;
		Pointf sft;
		startDrag.x = -1;
		shiftDrag = {0., 0.};
		if (operations != NULL && operations->GetCount() > 0) {
			int cnt = operations->GetCount();
			Operation *o = *operations[0];
			sz = o->getDrawSize();
			sft = o->getDrawShift();
			operationsRect = {sft.x, sft.y, sz.cx + sft.x, sz.cy + sft.y};
			for (int i=1; i<cnt; ++i) {
				o = *operations[i];
				if (o != NULL) {
					sz = o->getDrawSize();
					sft = o->getDrawShift();
					if (sft.x < operationsRect.left) operationsRect.left = sft.x;
					if (sft.y < operationsRect.top) operationsRect.top = sft.y;
					if (sz.cx + sft.x > operationsRect.right) operationsRect.right = sz.cx + sft.x;
					if (sz.cy + sft.y > operationsRect.bottom) operationsRect.bottom = sz.cy + sft.y;
				}
			}
			double scaleX = viewSize.cx / operationsRect.GetSize().cx;
			double scaleY = viewSize.cy / operationsRect.GetSize().cy;
			scale = min(scaleX, scaleY);
			if (scale > MAX_SCALE) scale = MAX_SCALE;
			else if (scale < MIN_SCALE) scale = MIN_SCALE;
		} else {
			scale = 1.;
		}
		double s = Operation::getScale() / OPERATION_SCALE - scale;
		if (s > 0.5 || s < - 0.5) {
			Operation::setScale(scale * OPERATION_SCALE);
			if (operations != NULL) {
				for (Operation *op : *operations) {
					if (op != NULL) op->calculateDraw();
				}
			}
		}
		Refresh();
	}

};