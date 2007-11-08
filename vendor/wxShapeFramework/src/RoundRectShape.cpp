/***************************************************************
 * Name:      RoundRectShape.cpp
 * Purpose:   Implements rounded rectangular shape class
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-07-22
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#include <math.h>

#include <wx/dataobj.h>
#include "RoundRectShape.h"
#include "ShapeCanvas.h"
#include "CommonFcn.h"

IMPLEMENT_DYNAMIC_CLASS(wxSFRoundRectShape, wxSFRectShape);

wxSFRoundRectShape::wxSFRoundRectShape(void) : wxSFRectShape()
{
	m_nRadius = sfdvROUNDRECTSHAPE_RADIUS;

	XS_SERIALIZE_EX(m_nRadius, wxT("radius"), sfdvROUNDRECTSHAPE_RADIUS);
}

wxSFRoundRectShape::wxSFRoundRectShape(const wxRealPoint& pos, const wxRealPoint &size, double radius, wxSFDiagramManager* manager)
: wxSFRectShape(pos, size, manager)
{
	m_nRadius = radius;

	XS_SERIALIZE_EX(m_nRadius, wxT("radius"), sfdvROUNDRECTSHAPE_RADIUS);
}

wxSFRoundRectShape::wxSFRoundRectShape(wxSFRoundRectShape& obj) : wxSFRectShape(obj)
{
	m_nRadius = obj.m_nRadius;
}

wxSFRoundRectShape::~wxSFRoundRectShape(void)
{
}

//----------------------------------------------------------------------------------//
// public virtual functions
//----------------------------------------------------------------------------------//

bool wxSFRoundRectShape::IsInside(const wxPoint &pos)
{
	// get original bounding box
	wxRect shpBB = GetBoundingBox();

	// calculate modified boxes
	wxRect hr(shpBB);
	hr.Deflate(0, (int)m_nRadius);
	wxRect vr(shpBB);
	vr.Deflate((int)m_nRadius, 0);

	// test whether given position is inside body rect or rounded corners
#if wxCHECK_VERSION(2, 8, 0)
	if(hr.Contains(pos))return true;
#else // replacement code for old version
	if(hr.Inside(pos))return true;
#endif
#if wxCHECK_VERSION(2, 8, 0)
	else if(vr.Contains(pos))return true;
#else // replacement code for old version
	else if(vr.Inside(pos))return true;
#endif
	else if(IsInCircle(pos, shpBB.GetTopLeft() + wxPoint((int)m_nRadius, (int)m_nRadius)))return true;
#if wxCHECK_VERSION(2, 8, 0)
	else if(IsInCircle(pos, shpBB.GetBottomLeft() + wxPoint((int)m_nRadius, (int)-m_nRadius)))return true;
#else // replacement code for old version
	else if(IsInCircle(pos, wxPoint(shpBB.GetTopLeft().x, shpBB.GetBottomRight().y) + wxPoint((int)m_nRadius, (int)-m_nRadius)))return true;
#endif
#if wxCHECK_VERSION(2, 8, 0)
	else if(IsInCircle(pos, shpBB.GetTopRight() + wxPoint((int)-m_nRadius, (int)m_nRadius)))return true;
#else // replacement code for old version
	else if(IsInCircle(pos, wxPoint(shpBB.GetBottomRight().x, shpBB.GetTopLeft().y) + wxPoint((int)-m_nRadius, (int)m_nRadius)))return true;
#endif
	else if(IsInCircle(pos, shpBB.GetBottomRight() + wxPoint((int)-m_nRadius, (int)-m_nRadius)))return true;

	return false;
}

//----------------------------------------------------------------------------------//
// protected virtual functions
//----------------------------------------------------------------------------------//

void wxSFRoundRectShape::DrawNormal(wxSFScaledPaintDC& dc)
{
	dc.SetPen(m_Border);
	dc.SetBrush(m_Fill);
	dc.DrawRoundedRectangle(GetAbsolutePosition(), GetRectSize(), m_nRadius);
	dc.SetBrush(wxNullBrush);
	dc.SetPen(wxNullPen);
}

void wxSFRoundRectShape::DrawHover(wxSFScaledPaintDC& dc)
{
	dc.SetPen(wxPen(m_nHoverColor, 1));
	dc.SetBrush(m_Fill);
	dc.DrawRoundedRectangle(GetAbsolutePosition(), GetRectSize(), m_nRadius);
	dc.SetBrush(wxNullBrush);
	dc.SetPen(wxNullPen);
}

void wxSFRoundRectShape::DrawHighlighted(wxSFScaledPaintDC& dc)
{
	dc.SetPen(wxPen(m_nHoverColor, 2));
	dc.SetBrush(m_Fill);
	dc.DrawRoundedRectangle(GetAbsolutePosition(), GetRectSize(), m_nRadius);
	dc.SetBrush(wxNullBrush);
	dc.SetPen(wxNullPen);
}

//----------------------------------------------------------------------------------//
// public functions
//----------------------------------------------------------------------------------//

bool wxSFRoundRectShape::IsInCircle(const wxPoint& pos, const wxPoint& center)
{
    return (Distance(wxRealPoint(center.x, center.y), wxRealPoint(pos.x, pos.y)) <= m_nRadius);
}