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
	Sizef coordSize = {100., 100.};
	Vector<Operation*>* operations = NULL;
	bool isDrawCoordinates = true;
	bool isDrawDrillCenter = false;
	bool isDrawMeasure = true;
	Rectf viewRect = {-50., -50., 50., 50.};
	Point startDrag = {-1, -1};
	Point shiftDrag = {0, 0};
	
	void drawCoordinates(DrawPainter &p, double scale) {
		p.Move(0., 0.); p.Line(coordSize.cx / scale, 0., true);
		p.Stroke(Settings::measurersLineWidth / scale, Red());
		p.Move(0., 0.); p.Line(0., coordSize.cy / scale, true);
		p.Stroke(Settings::measurersLineWidth / scale, Green());
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
		Sizef viewSize = viewRect.GetSize();
		DrawPainter p(w, sz);
		p.Clear(Settings::viewerBG);
		double scale = min(sz.cx / viewSize.cx, sz.cy / viewSize.cy);
		Xform2D transform = Xform2D::Translation(shiftDrag.x, shiftDrag.y);
		transform = Xform2D::Translation(-viewRect.left, -viewRect.bottom) * Xform2D::Scale(scale, -scale) * transform;
		
		p.Transform(transform);
		if (operations != NULL) {
			OperationDrill *od;
			for (Operation* o : *operations) {
				if (o != NULL) {
					od = dynamic_cast<OperationDrill*>(o);
					if (od != NULL) {
						od->Draw(p, viewRect, scale, isDrawMeasure, isDrawDrillCenter);
					} else {
						o->Draw(p, viewRect, scale, isDrawMeasure);
					}
				}
			}
		}
		if (isDrawCoordinates) drawCoordinates(p, scale);
	}
	
	void setOperations(Vector<Operation*>* o) {
		operations = o;
		Refresh();
	}
	
	void MouseWheel(Point p, int zdelta, dword keyflags) {
		double scale = (zdelta < 0) ? 1.2 : (1. / 1.2);
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
		Ctrl::OverrideCursor(Image::SizeAll());
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
};

#endif