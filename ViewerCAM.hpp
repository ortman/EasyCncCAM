#ifndef _VIEWER_CAM_HPP_
#define _VIEWER_CAM_HPP_

#include <Core/Core.h>
#include "OperationDrillArray.hpp"
#include "OperationDrillRoundless.hpp"
#include "OperationMilling.hpp"

using namespace Upp;

#define MIN_SCALE 0.2
#define MAX_SCALE 20.0

class ViewerCAM : public Ctrl {
private:
	Size coordSize = {100, 100};
	ImageDraw *iDraw = NULL;
	Size iDrawSize;
	Vector<Operation*>* operations = NULL;
	bool isDrawCoordinates = true;
	bool isDrawDrillCenter = false;
	bool isDrawMeasure = true;
	bool isRedraw = true;
	Rectf viewRect = {-50., -50., 50., 50.};
	Point startDrag = {-1, -1};
	Point shiftDrag = {0, 0};
	
	void drawCoordinates(ImageDraw &draw) {
		Sizef viewSize = viewRect.GetSize();
		double scale = min(iDrawSize.cx / viewSize.cx, iDrawSize.cy / viewSize.cy);
		int shiftX = (int)(-viewRect.left * scale + (iDrawSize.cx - viewSize.cx * scale) / 2.);
		int shiftY = (int)(iDrawSize.cy + viewRect.top * scale - (iDrawSize.cy - viewSize.cy * scale) / 2.);
		draw.DrawLine(shiftX, shiftY, (int)(shiftX + coordSize.cx * Settings::subsampling), shiftY, Settings::measurersLineWidth, Red);
		draw.DrawLine(shiftX, (int)(shiftY - coordSize.cy * Settings::subsampling), shiftX, shiftY, Settings::measurersLineWidth, Green);
	}
	
public:
	bool GetDrawCoordinates() { return isDrawCoordinates; }
	void SetDrawCoordinates(bool isDraw = true) {
		isDrawCoordinates = isDraw;
		Refresh();
	}
	
	bool GetDrawDrillCenter() { return isDrawDrillCenter; }
	void SetDrawDrillCenter(bool isDraw = true) {
		isDrawDrillCenter = isDraw;
		Refresh();
	}
	
	bool GetDrawMeasure() { return isDrawMeasure; }
	void SetDrawMeasure(bool isDraw = true) {
		isDrawMeasure = isDraw;
		Refresh();
	}
	
	void Paint(Draw &w) {
		Size sz = GetSize();
		if (iDraw && ((int)(sz.cx * Settings::subsampling) != iDrawSize.cx || (int)(sz.cy * Settings::subsampling) != iDrawSize.cy)) {
			delete iDraw;
			iDraw = NULL;
		}
		if (iDraw == NULL) {
			iDrawSize = Size((int)(sz.cx * Settings::subsampling), (int)(sz.cy * Settings::subsampling));
			iDraw = new ImageDraw(iDrawSize);
			isRedraw = true;
		}
		if (isRedraw) {
			iDraw->DrawRect(iDrawSize, Settings::viewerBG);
			if (operations != NULL) {
				OperationDrill *od;
				for (Operation* o : *operations) {
					if (o != NULL) {
						od = dynamic_cast<OperationDrill*>(o);
						if (od != NULL) {
							od->Draw(*iDraw, iDrawSize, viewRect, isDrawMeasure, isDrawDrillCenter);
						} else {
							o->Draw(*iDraw, iDrawSize, viewRect, isDrawMeasure);
						}
					}
				}
			}
			if (isDrawCoordinates) drawCoordinates(*iDraw);
			isRedraw = false;
		}
		w.DrawRect(sz, Settings::viewerBG);
		w.DrawImage(shiftDrag.x, shiftDrag.y, sz.cx, sz.cy, *iDraw);
	}
	
	void setOperations(Vector<Operation*>* o) {
		operations = o;
		Refresh();
	}
	
	void MouseWheel(Point p, int zdelta, dword keyflags) {
		double scale = (zdelta < 0) ? 1.2 : 1./1.2;
		viewRect = {
			viewRect.left * scale,
			viewRect.top * scale,
			viewRect.right * scale,
			viewRect.bottom * scale
		};
		Refresh();
	}
	
	virtual void LeftDown(Point p, dword keyflags) {
		startDrag = {p.x, p.y};
		Image m = Ctrl::OverrideCursor(Image::SizeAll());
	}
	
  virtual void MouseMove(Point p, dword keyflags) {
    if (startDrag.x > 0.) {
      shiftDrag.x = p.x - startDrag.x;
      shiftDrag.y = p.y - startDrag.y;
      Ctrl::Refresh();
    }
  }
    
	virtual void LeftUp(Point p, dword keyflags) {
		Sizef viewSize = viewRect.GetSize();
		Size sz = GetSize();
		double scale = min(sz.cy / viewSize.cy, sz.cx / viewSize.cx);
		double shiftX = shiftDrag.x / scale;
		double shiftY = shiftDrag.y / scale;
		viewRect = {
			viewRect.left - shiftX,
			viewRect.top + shiftY,
			viewRect.right - shiftX,
			viewRect.bottom + shiftY
		};
		startDrag = {-1,  -1};
		shiftDrag = {0, 0};
		Refresh();
		Ctrl::OverrideCursor(Image::Arrow());
	}
	
	void showAllView() {
		if (operations != NULL && operations->GetCount() > 0) {
			int cnt = operations->GetCount();
			Operation *o;
			viewRect = {0., 0., 0., 0.};
			for (int i = 0; i < cnt; ++i) {
				o = (*operations)[i];
				if (o != NULL) {
					viewRect.Union(o->getRect());
				}
			}
		} else {
			viewRect = {-100., -100., 100., 100.};
		}
		Sizef viewSize = viewRect.GetSize();
		Size sz = GetSize();
		double scale = min(sz.cy / viewSize.cy, sz.cx / viewSize.cx);
		double cx = ((sz.cx / scale * 1.2) - viewSize.cx) / 2.;
		double cy = ((sz.cy / scale * 1.2) - viewSize.cy) / 2.;
		viewRect = {
			viewRect.left - cx,
			viewRect.top - cy,
			viewRect.right + cx,
			viewRect.bottom + cy
		};
		Refresh();
	}

	void Refresh() {
		isRedraw = true;
		Ctrl::Refresh();
	}
};

#endif