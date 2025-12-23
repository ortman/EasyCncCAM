#ifndef _OPERATION_HPP_
#define _OPERATION_HPP_

#include <Core/Core.h>

#define LAYOUTFILE <EasyCncCAM/OperationTab.lay>
#include <CtrlCore/lay.h>
#include "Tool.hpp"
#include "Settings.hpp"
#include "GCode.h"

using namespace Upp;

class Operation : public Moveable<Operation> {
protected:
	Tool tool;
	double depth = Null;
	double safeToolH = -5.;
	
	void DrawText(DrawPainter &p, double scale, double x, double y, double angle, const String& str, const Font& font, const Color& color) {
		p.Begin();
		p.Translate(x, y);
		p.Scale(1. / scale, -1. / scale);
		p.Rotate(-angle);
		Size sz = GetTextSize(str, font);
		p.Rectangle(0., 0., sz.cx, sz.cy);
		p.Fill(Settings::viewerBG);
		p.Text(0., 0., str, font);
		p.Fill(color);
		p.End();
	}
		
	void DrawMeasureArrow(DrawPainter &p, double scale, double x, double y, double angle) {
		p.Move(x, y);
		double arrowSize = Settings::measurersArrowSize / scale;
		p.Line(x + arrowSize * cos(angle - Settings::measurersArrowAngle),
		       y - arrowSize * sin(angle - Settings::measurersArrowAngle));
		p.Line(x + arrowSize * cos(angle + Settings::measurersArrowAngle),
		       y - arrowSize * sin(angle + Settings::measurersArrowAngle));
		p.Line(x, y);
		p.Fill(Settings::measurersColor);
	}
	
	void DrawMeasureDiameter(DrawPainter &p, double scale, double x, double y, double diameter, const String& text, double angle = 45.) {
		Size textSize = GetTextSize(text, Settings::measurersFont);
		double radius = diameter / 2.;
		double anglePI = angle * M_PI / 180.;
		double xD = (radius + 20. / scale) * cos(anglePI);
		double yD = (radius + 20. / scale) * sin(anglePI);
		int leftAngle = (int)angle % 360;
		bool leftText = leftAngle > 90 && leftAngle < 270;
		DrawText(p, scale,
		        (x + xD) - (leftText ? textSize.cx / scale : 0.),
		        (y + yD) + textSize.cy / scale,
		        0., text, Settings::measurersFont, Settings::measurersColor);
		p.Move(x - xD, y - yD);
		p.Line(x + xD, y + yD);
		p.Line(x + xD + (leftText ? -textSize.cx : textSize.cx) / scale, y + yD);
		p.Stroke(Settings::measurersLineWidth / scale, Settings::measurersColor);
		DrawMeasureArrow(p, scale, (x + radius * cos(anglePI)), (y + radius * sin(anglePI)), -anglePI);
		DrawMeasureArrow(p, scale, (x - radius * cos(anglePI)), (y - radius * sin(anglePI)), -anglePI + M_PI);
	}
	
	void DrawMeasureRadius(DrawPainter &p, double scale, double x, double y, double radius, const String& text, double angle = 45.) {
		Size textSize = GetTextSize(text, Settings::measurersFont);
		double anglePI = angle * M_PI / 180.;
		double xD = (radius + 30. / scale) * cos(anglePI);
		double yD = (radius + 30. / scale) * sin(anglePI);
		int leftAngle = (int)angle % 360;
		bool leftText = leftAngle > 90 && leftAngle < 270;
		DrawText(p, scale,
		         (x + xD) - (leftText ? textSize.cx / scale : 0.),
		         (y + yD) + textSize.cy / scale,
		         0., text, Settings::measurersFont, Settings::measurersColor);
		p.Move(x, y);
		p.Line(x + xD, y + yD);
		p.Line(x + xD + (leftText ? -textSize.cx : textSize.cx) / scale, y + yD);
		p.Stroke(Settings::measurersLineWidth / scale, Settings::measurersColor);
		DrawMeasureArrow(p, scale, x + radius * cos(anglePI), y + radius * sin(anglePI), -anglePI);
	}
	
	void DrawMeasureLine(DrawPainter &p, double scale, double x1, double y1, double x2, double y2, const String& text, double shift = DOUBLE_NULL) {
		double cx = x2 - x1;
		double cy = y2 - y1;
		double angle = atan(cy / cx);
		double len = sqrt(sqr(cx) + sqr(cy));
		double shiftL = (shift == DOUBLE_NULL) ? len * 0.1 : shift;
		double shiftV = shiftL + 5. / scale;
		double sX = x1 + shiftL * cos(angle - M_PI_2);
		double sY = y1 + shiftL * sin(angle - M_PI_2);
		double eX = sX + len * cos(angle);
		double eY = sY + len * sin(angle);
		String str = text.IsEmpty() ? DblStr(round(len)) : text;
		Size txtSz = GetTextSize(str, Settings::measurersFont);
		double txtShiftLen = (len - txtSz.cx / scale) / 2.;
		DrawText(p, scale,
			sX + txtShiftLen * cos(angle),
			sY + txtShiftLen * sin(angle),
			angle,
			str,
			Settings::measurersFont, Settings::measurersColor);
    p.Move(sX, sY); p.Line(eX, eY);
    p.Move(x1, y1); p.Line(x1 + shiftV * cos(angle - M_PI_2), y1 + shiftV * sin(angle - M_PI_2));
    p.Move(x2, y2); p.Line(x2 + shiftV * cos(angle - M_PI_2), y2 + shiftV * sin(angle - M_PI_2));
    p.Stroke(Settings::measurersLineWidth / scale, Settings::measurersColor);
		DrawMeasureArrow(p, scale, sX, sY, -angle);
		DrawMeasureArrow(p, scale, eX, eY, M_PI - angle);
	}
	
public:
	virtual ~Operation() {};
	virtual Rectf getRect() {return {0., 0., 1., 1.};};
	virtual String ToString() {
		return t_("Operation");
	}
	virtual void Draw(DrawPainter &p, Rectf& viewRect, double scale, bool isMeasurers = false) {
	}
	virtual void calculate() {
	}
	Tool& getTool() {
		return tool;
	}
	void setTool(const Tool& tool) {
		this->tool = tool;
		calculate();
	}
	double getDepth() {return depth;}
	void setDepth(double d) {depth = d;}
	virtual const String gcode(GCode& g) {
		return "";
	}
};

#endif