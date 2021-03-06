/***************************************************************
 * Name:      CommonFcn.h
 * Purpose:   Defines set of global (auxiliary) functions
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-07-22
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#pragma once

#include <wx/wxprec.h>
#include <wx/xml/xml.h>

#include "Defs.h"

// data conversion functions

// graphics functions
WXDLLIMPEXP_SF wxColour GetHybridColour(const wxColour& orig, const wxColour& modificator);
WXDLLIMPEXP_SF bool LinesIntersection(const wxRealPoint& from1, const wxRealPoint& to1, const wxRealPoint& from2, const wxRealPoint& to2, wxRealPoint& i);
WXDLLIMPEXP_SF double Distance(const wxRealPoint& pt1, const wxRealPoint& pt2);
