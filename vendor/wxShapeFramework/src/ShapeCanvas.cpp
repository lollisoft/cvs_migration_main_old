/***************************************************************
 * Name:      ShapeCanvas.cpp
 * Purpose:   Implements shape canvas class
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-07-22
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

// TODO: wxSFShapeCanvas: Implement canvas thumbnail (wxSFCanvasThumbnail class)

#include <wx/wfstream.h>
#include <wx/mstream.h>
#include <wx/txtstrm.h>
#include <wx/clipbrd.h>

#include <wx/dataobj.h>
#include "ShapeCanvas.h"
#include "DiagramManager.h"
#include "ScaledPaintDC.h"
#include "ShapeDataObject.h"
#include "EditTextShape.h"
#include "BitmapShape.h"
#include "SFEvents.h"

IMPLEMENT_DYNAMIC_CLASS(wxSFCanvasSettings, xsSerializable);

wxSFCanvasSettings::wxSFCanvasSettings() : xsSerializable()
{
    m_nScale = 1;
    m_nBackgroundColor = sfdvSHAPECANVAS_BACKGROUNDCOLOR;
    m_nCommonHoverColor = sfdvSHAPECANVAS_HOVERCOLOR;
    m_fMultiselection = sfdvSHAPECANVAS_MULTISELECTION;
    m_fMultiSizeChange = sfdvSHAPECANVAS_MULTISIZECHANGE;
    m_fShowGrid = sfdvSHAPECANVAS_SHOWGRID;
    m_fUseGrid = sfdvSHAPECANVAS_USEGRID;
    m_nGridSize = sfdvSHAPECANVAS_GRIDSIZE;
    m_nGridColor = sfdvSHAPECANVAS_GRIDCOLOR;

    XS_SERIALIZE(m_nScale, wxT("scale"));
    XS_SERIALIZE_EX(m_nBackgroundColor, wxT("background_color"), sfdvSHAPECANVAS_BACKGROUNDCOLOR);
    XS_SERIALIZE_EX(m_nCommonHoverColor, wxT("hover_color"), sfdvSHAPECANVAS_HOVERCOLOR);
    XS_SERIALIZE_EX(m_fMultiselection, wxT("multiselection"), sfdvSHAPECANVAS_MULTISELECTION);
    XS_SERIALIZE_EX(m_fMultiSizeChange, wxT("multichange"), sfdvSHAPECANVAS_MULTISIZECHANGE);
    XS_SERIALIZE_EX(m_fShowGrid, wxT("show_grid"), sfdvSHAPECANVAS_SHOWGRID);
    XS_SERIALIZE_EX(m_fUseGrid, wxT("use_grid"), sfdvSHAPECANVAS_USEGRID);
    XS_SERIALIZE_EX(m_nGridSize, wxT("grid_size"), sfdvSHAPECANVAS_GRIDSIZE);
    XS_SERIALIZE_EX(m_nGridColor, wxT("grid_color"), sfdvSHAPECANVAS_GRIDCOLOR);
    XS_SERIALIZE(m_arrAcceptedShapes, wxT("accepted_shapes"));
}

static const wxChar* dataFormatID = wxT("ShapeFrameWorkDataFormat1_0");

BEGIN_EVENT_TABLE(wxSFShapeCanvas, wxScrolledWindow)
	EVT_PAINT(wxSFShapeCanvas::OnPaint)
	EVT_ERASE_BACKGROUND(wxSFShapeCanvas::OnEraseBackground)
	EVT_LEFT_DOWN(wxSFShapeCanvas::_OnLeftDown)
	EVT_LEFT_UP(wxSFShapeCanvas::_OnLeftUp)
	EVT_RIGHT_DOWN(wxSFShapeCanvas::_OnRightDown)
	EVT_RIGHT_UP(wxSFShapeCanvas::_OnRightUp)
	EVT_LEFT_DCLICK(wxSFShapeCanvas::_OnLeftDoubleClick)
	EVT_RIGHT_DCLICK(wxSFShapeCanvas::_OnRightDoubleClick)
	EVT_MOTION(wxSFShapeCanvas::_OnMouseMove)
	EVT_KEY_DOWN(wxSFShapeCanvas::_OnKeyDown)
	EVT_ENTER_WINDOW(wxSFShapeCanvas::OnEnterWindow)
	EVT_LEAVE_WINDOW(wxSFShapeCanvas::OnLeaveWindow)
END_EVENT_TABLE()

wxSFShapeCanvas::wxSFShapeCanvas(wxSFDiagramManager* manager, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
: wxScrolledWindow(parent, id, pos, size, style)
{
    // initialize shape manager
    wxASSERT_MSG( manager, wxT("Shape manager has not been properly set in shape canvas's constructor."));
    if(!manager)return;
    wxASSERT_MSG( manager, wxT("Shape manager must be initialized before a creation of assigned canvas."));
    if(!manager->GetRootItem())return;

    m_pManager = manager;
    m_pManager->SetShapeCanvas(this);

	SetScrollbars(5, 5, 100, 100);
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);

	// initialize data members
	m_fCanSaveStateOnMouseUp = false;

	m_nWorkingMode = modeREADY;
	m_pSelectedHandle = NULL;
	m_pNewLineShape = NULL;

	// initialize multiedit rectangle
	m_shpMultiEdit.SetParentManager(m_pManager);
	m_shpMultiEdit.SetId(0);
	m_shpMultiEdit.CreateHandles();
	m_shpMultiEdit.Select(true);
	m_shpMultiEdit.Show(false);
	m_shpMultiEdit.ShowHandles(true);

	m_formatShapes.SetId(dataFormatID);

	m_CanvasHistory.SetParentCanvas(this);
	if( m_pManager )SaveCanvasState();
}

wxSFShapeCanvas::~wxSFShapeCanvas(void)
{
	//Clear();
}

//----------------------------------------------------------------------------------//
// Painting functions
//----------------------------------------------------------------------------------//

void wxSFShapeCanvas::OnPaint(wxPaintEvent& event)
{
	// use double-buffered painting
	int sx, sy, x, y;

	wxPaintDC paintDC(this);
	GetClientSize(&sx, &sy);

	wxBitmap outbmp(sx, sy);
	wxSFScaledPaintDC dc(outbmp, m_Settings.m_nScale);

#if wxCHECK_VERSION(2, 8, 0)
	if(dc.IsOk())
#else // replacement code for old version
	if(dc.Ok())
#endif
	{
        DrawContent(dc, sfFROM_PAINT);
        dc.GetDeviceOrigin(&x, &y);

        paintDC.Blit(0, 0, sx, sy, &dc, -x, -y);
	}
}

void wxSFShapeCanvas::DrawContent(wxSFScaledPaintDC& dc, bool fromPaint)
{
    wxASSERT( m_pManager );
    if(!m_pManager)return;
    wxASSERT(m_pManager->GetRootItem());
    if(!m_pManager->GetRootItem())return;

    wxSFShapeBase *pShape = NULL, *pParentShape = NULL;

	// prepare window dc
	PrepareDC(dc);

	// erase background
	dc.SetBackground(wxBrush(m_Settings.m_nBackgroundColor));
	dc.Clear();

	// show grid
	if(m_Settings.m_fShowGrid)
	{
		wxRect gridRct(wxPoint(0, 0), GetVirtualSize()+m_Settings.m_nGridSize);
		int maxx = int(gridRct.GetRight()/m_Settings.m_nScale);
		int maxy = int(gridRct.GetBottom()/m_Settings.m_nScale);

		dc.SetPen(wxColor(m_Settings.m_nGridColor));
		for(int x = gridRct.GetLeft(); x <= maxx; x += m_Settings.m_nGridSize.x)
		{
			dc.DrawLine(x, 0, x, maxy);
		}
		for(int y = gridRct.GetTop(); y <= maxy; y += m_Settings.m_nGridSize.y)
		{
			dc.DrawLine(0, y, maxx, y);
		}
	}

	if(fromPaint)
	{
		wxRect updRct;
		wxRect bbRct;
		ShapeList m_lstToDraw;
		// get the update rect list
		wxRegionIterator upd(GetUpdateRegion());

		// get all existing shapes
		m_pManager->GetShapes(CLASSINFO(wxSFShapeBase), m_lstToDraw);

		if(m_nWorkingMode == modeSHAPEMOVE)
		{
			ShapeList m_lstSelected;
			// get selected shapes
			GetSelectedShapes(m_lstSelected);

			while(upd)
			{
				// get damaged region
				updRct = DP2LP(upd.GetRect());

				// draw unselected non line-based shapes first...
				wxShapeListNode *node = m_lstToDraw.GetFirst();
				while(node)
				{
					pShape = (wxSFShapeBase*)node->GetData();
					pParentShape = pShape->GetParentShape();

					if( pShape->Intersects(updRct) )
					{
						if( m_lstSelected.IndexOf(pShape)!=wxNOT_FOUND )
						{
							node = node->GetNext();
							// remove selected shapes from the list
							m_lstToDraw.DeleteObject(pShape);
						}
						else if( !pShape->IsKindOf(CLASSINFO(wxSFLineShape)) )
						{
						    if( !pParentShape || (pParentShape && !pParentShape->IsKindOf(CLASSINFO(wxSFLineShape))) )
						    {
                                pShape->Draw(dc, sfWITHOUTCHILDREN);
						    }

							node = node->GetNext();
							// remove already drawn shape from the list
							m_lstToDraw.DeleteObject(pShape);
						}
						else
							node = node->GetNext();
					}
					else
						node = node->GetNext();
				}

				// draw dragged shapes ...
				node = m_lstSelected.GetFirst();
				while(node)
				{
					pShape = (wxSFShapeBase*)node->GetData();
					pParentShape = pShape->GetParentShape();

					if( pShape->Intersects(updRct) )
					{
                        if( !pParentShape || (pParentShape && !pParentShape->IsKindOf(CLASSINFO(wxSFLineShape))) )
                        {
                            pShape->Draw(dc);
                        }
						node = node->GetNext();
						// remove already drawn shape from the list
						m_lstSelected.DeleteObject(pShape);
					}
					else
						node = node->GetNext();
				}

				// ... and draw connections
				node = m_lstToDraw.GetFirst();
				while(node)
				{
					pShape = node->GetData();

                    if( pShape->IsKindOf(CLASSINFO(wxSFLineShape)) )
                    {
                        pShape->GetCompleteBoundingBox(bbRct, wxSFShapeBase::bbSELF | wxSFShapeBase::bbCHILDREN);
                        if( bbRct.Intersects(updRct) )
                        {
							pShape->Draw(dc);
							node = node->GetNext();
							// remove already drawn shape from the list
							m_lstToDraw.DeleteObject(pShape);
						}
						else
							node = node->GetNext();
					}
					else
						node = node->GetNext();
				}

				upd++;
			}
		}
		else
		{
			while(upd)
			{
				updRct = DP2LP(upd.GetRect());

				// draw parent shapes (children are processed by parent objects)
				wxShapeListNode *node = m_lstToDraw.GetFirst();
				while(node)
				{
					pShape = node->GetData();
					pParentShape = pShape->GetParentShape();

					if( pShape->Intersects(updRct) )
					{
						if( !pShape->IsKindOf(CLASSINFO(wxSFLineShape)) )
						{
						    if( !pParentShape || (pParentShape && !pParentShape->IsKindOf(CLASSINFO(wxSFLineShape))) )
						    {
                                pShape->Draw(dc, sfWITHOUTCHILDREN);
                                node = node->GetNext();
                                // remove already drawn shape from the list
                                m_lstToDraw.DeleteObject(pShape);
						    }
                            else
                                node = node->GetNext();
						}
						else
							node = node->GetNext();
					}
					else
						node = node->GetNext();
				}

				// draw connections
				node = m_lstToDraw.GetFirst();
				while(node)
				{
					pShape = node->GetData();
                    if( pShape->IsKindOf(CLASSINFO(wxSFLineShape)) )
                    {
                        pShape->GetCompleteBoundingBox(bbRct, wxSFShapeBase::bbSELF | wxSFShapeBase::bbCHILDREN);
                        if( bbRct.Intersects(updRct) )
                        {
							pShape->Draw(dc);
							node = node->GetNext();
							// remove already drawn shape from the list
							m_lstToDraw.DeleteObject(pShape);
						}
						else
							node = node->GetNext();
					}
					else
						node = node->GetNext();
				}

				upd++;
			}
		}

		// draw line shape being created
		if(m_pNewLineShape)
		{
			m_pNewLineShape->Draw(dc, sfWITHOUTCHILDREN);
		}

		// draw multiselection if neccessary
		if(m_shpMultiEdit.IsVisible())
		{
			m_shpMultiEdit.Draw(dc);
		}
	}
	else
	{
		// draw parent shapes (children are processed by parent objects)
		wxSerializableListNode *node = m_pManager->GetRootItem()->GetFirstChildNode();
		while(node)
		{
			pShape = (wxSFShapeBase*)node->GetData();

			if(!pShape->IsKindOf(CLASSINFO(wxSFLineShape)))
			{
				pShape->Draw(dc);
			}

			node = node->GetNext();
		}

		// draw connections
		node = m_pManager->GetRootItem()->GetFirstChildNode();
		while(node)
		{
			pShape = (wxSFShapeBase*)node->GetData();
			if(pShape->IsKindOf(CLASSINFO(wxSFLineShape)))
			{
				pShape->Draw(dc);
			}
			node = node->GetNext();
		}
	}
}

void wxSFShapeCanvas::OnEraseBackground(wxEraseEvent &event)
{
	// do nothing to suppress window flickering
}

void wxSFShapeCanvas::RefreshCanvas(bool erase, wxRect rct)
{
    /*wxRect updRct = LP2DP(rct);

    updRct.Inflate(int(10/m_Settings.m_nScale), int(10/m_Settings.m_nScale));

    RefreshRect(updRct, erase);*/

	wxPoint lpos = DP2LP(wxPoint(0, 0));

	rct.Inflate(int(10/m_Settings.m_nScale), int(10/m_Settings.m_nScale));
	rct.Offset(-lpos.x, -lpos.y);

	RefreshRect(wxRect(int(rct.x*m_Settings.m_nScale), int(rct.y*m_Settings.m_nScale), int(rct.width*m_Settings.m_nScale), int(rct.height*m_Settings.m_nScale)), erase);
}

//----------------------------------------------------------------------------------//
// Virtual functions
//----------------------------------------------------------------------------------//

void wxSFShapeCanvas::OnLeftDown(wxMouseEvent& event)
{
	// HINT: override it for custom actions...

	wxASSERT(m_pManager);
	if(!m_pManager)return;

    DeleteAllTextCtrls();
	SetFocus();

	wxPoint lpos = DP2LP(event.GetPosition());

	m_fCanSaveStateOnMouseUp = false;

	switch(m_nWorkingMode)
	{
	case modeREADY:
		{
			m_pSelectedHandle = GetTopmostHandleAtPosition(lpos);

			if(!m_pSelectedHandle)
			{
				wxSFShapeBase* pSelectedShape = GetShapeAtPosition(lpos);

				if(pSelectedShape)
				{
					// perform selection
					ShapeList m_lstSelection;
					GetSelectedShapes(m_lstSelection);

					// cancel previous selections if neccessary...
					if( m_lstSelection.IndexOf(pSelectedShape->GetParentShape()) != wxNOT_FOUND )
					{
						DeselectAll();
					}
					else if(!event.ControlDown())
					{
						if(m_lstSelection.IndexOf(pSelectedShape) == wxNOT_FOUND)
						{
							DeselectAll();
						}
					}

					pSelectedShape->Select(true);
					//pSelectedShape->ShowHandles(true);

					GetSelectedShapes(m_lstSelection);
					// remove child shapes from the selection
					ValidateSelection(m_lstSelection);

					if(m_lstSelection.GetCount()>1)
					{
						HideAllHandles();
					}
					m_shpMultiEdit.Show(false);
					//m_shpMultiEdit.ShowHandles(false);

					// inform selected shapes about begin of dragging...
					wxShapeListNode *node = m_lstSelection.GetFirst();
					while(node)
					{
						node->GetData()->_OnBeginDrag(FitPositionToGrid(lpos));
						node = node->GetNext();
					}

					// call user defined actions
					pSelectedShape->OnLeftClick(FitPositionToGrid(lpos));

					m_nWorkingMode = modeSHAPEMOVE;
				}
				else
				{
					DeselectAll();
					if(m_Settings.m_fMultiselection)
					{
                        m_shpMultiEdit.Show(true);
                        m_shpMultiEdit.ShowHandles(false);
                        m_shpMultiEdit.SetRelativePosition(wxRealPoint(lpos.x, lpos.y));
                        m_shpMultiEdit.SetRectSize(wxRealPoint(0, 0));
                        m_nWorkingMode = modeMULTISELECTION;
					}
					else
					{
					    m_nWorkingMode = modeREADY;
					}
				}

				// update canvas
				Refresh(false);
			}
			else
			{
				if(m_pSelectedHandle->GetParentShape() == &m_shpMultiEdit)
				{
					if(m_Settings.m_fMultiSizeChange)
					{
					    m_nWorkingMode = modeMULTIHANDLEMOVE;
					}
					else
                        m_nWorkingMode = modeREADY;
				}
				else
				{
					m_nWorkingMode = modeHANDLEMOVE;
					switch(m_pSelectedHandle->GetType())
					{
                    case wxSFShapeHandle::hndLINESTART:
                        {
                            wxSFLineShape* pLine = (wxSFLineShape*)m_pSelectedHandle->GetParentShape();
                            pLine->SetLineMode(wxSFLineShape::modeSRCCHANGE);
                            pLine->SetUnfinishedPoint(lpos);
                        }
                        break;

                    case wxSFShapeHandle::hndLINEEND:
                        {
                            wxSFLineShape* pLine = (wxSFLineShape*)m_pSelectedHandle->GetParentShape();
                            pLine->SetLineMode(wxSFLineShape::modeTRGCHANGE);
                            pLine->SetUnfinishedPoint(lpos);
                        }
                        break;

                    default:
                        break;
					}
				}
				m_pSelectedHandle->OnBeginDrag(FitPositionToGrid(lpos));
			}
		}
		break;

	case modeCREATECONNECTION:
		{
		    // update the line shape being created
		    if(m_pNewLineShape)
		    {
		        wxSFShapeBase* pShapeUnder = GetShapeAtPosition(lpos);
                //if(pShapeUnder && !pShapeUnder->IsKindOf(CLASSINFO(wxSFLineShape)))
				if(pShapeUnder)
                {
					if((m_pNewLineShape->GetSrcShapeId() == -1) &&
                        (pShapeUnder->IsConnectionAccepted(m_pNewLineShape->GetClassInfo()->GetClassName())))
                        //(!pShapeUnder->GetAcceptedTrgNeighbours().IsEmpty()))
                    {
                        m_pNewLineShape->SetSrcShapeId(pShapeUnder->GetId());

						// swith on the "under-construcion" mode
						m_pNewLineShape->SetUnfinishedPoint(lpos);
                    }
                    else if((m_pNewLineShape->GetTrgShapeId() == -1) &&
                            (pShapeUnder != m_pNewLineShape) &&
                            (pShapeUnder->IsConnectionAccepted(m_pNewLineShape->GetClassInfo()->GetClassName())))
                    {
                        // find out whether the target shape can be connected to the source shape
                        wxSFShapeBase* pSourceShape = m_pManager->FindShape(m_pNewLineShape->GetSrcShapeId());

                        if( pSourceShape &&
                            pShapeUnder->IsSrcNeighbourAccepted(pSourceShape->GetClassInfo()->GetClassName()) &&
                            pSourceShape->IsTrgNeighbourAccepted(pShapeUnder->GetClassInfo()->GetClassName()) )
                        {
                            m_pNewLineShape->SetTrgShapeId(pShapeUnder->GetId());
                            m_pNewLineShape->CreateHandles();

                            // swith off the "under-construcion" mode
                            m_pNewLineShape->SetLineMode(wxSFLineShape::modeREADY);

                            // inform user that the line is completed
                            OnConnectionFinished(m_pNewLineShape);

                            m_pNewLineShape->Refresh();

                            m_nWorkingMode = modeREADY;
                            m_pNewLineShape = NULL;

                            SaveCanvasState();
                        }
                    }
                }
                else
                {
                    if(m_pNewLineShape->GetSrcShapeId() != -1)
                    {
                        wxPoint flpos = FitPositionToGrid(lpos);
                        m_pNewLineShape->GetControlPoints().Append(new wxRealPoint(flpos.x, flpos.y));
                    }
                }
		    }
		}
		break;

	default:
		m_nWorkingMode = modeREADY;
	}
}

void wxSFShapeCanvas::OnLeftUp(wxMouseEvent &event)
{
    // HINT: override it for custom actions...

	wxPoint lpos = DP2LP(event.GetPosition());

	switch(m_nWorkingMode)
	{
	case modeMULTIHANDLEMOVE:
	case modeHANDLEMOVE:
		{
		    m_pSelectedHandle->OnEndDrag(lpos);

			// resize parent shape to fit all its children if neccessary
			if(m_pSelectedHandle->GetParentShape()->GetParentShape())
			{
				//((wxSFShapeBase*)m_pSelectedHandle->GetParentShape()->GetParentShape())->FitToChildren();
				((wxSFShapeBase*)m_pSelectedHandle->GetParentShape()->GetParentShape())->Update();
			}

			// if the handle is line handle then return the line to normal state
			// and re-assign line's source/target shape
			switch(m_pSelectedHandle->GetType())
			{
            case wxSFShapeHandle::hndLINESTART:
            case wxSFShapeHandle::hndLINEEND:
                {
                    wxSFLineShape* pLine = (wxSFLineShape*)m_pSelectedHandle->GetParentShape();
                    pLine->SetLineMode(wxSFLineShape::modeREADY);

                    wxSFShapeBase* pParentShape = GetShapeAtPosition(lpos, 1, searchBOTH);
					//if(pParentShape && !pParentShape->IsKindOf(CLASSINFO(wxSFLineShape))  && (pParentShape->IsConnectionAccepted(pLine->GetClassInfo()->GetClassName())))
                    if(pParentShape && (pParentShape != pLine) && (pParentShape->IsConnectionAccepted(pLine->GetClassInfo()->GetClassName())))
                    {
                        if(m_pSelectedHandle->GetType() == wxSFShapeHandle::hndLINESTART)
                        {
                            wxSFShapeBase* pTrgShape = m_pManager->FindShape( pLine->GetTrgShapeId() );
                            if( pTrgShape && pParentShape->IsTrgNeighbourAccepted(pTrgShape->GetClassInfo()->GetClassName()))
                            {
                                pLine->SetSrcShapeId(pParentShape->GetId());
                            }
                        }
                        else
                        {
                            wxSFShapeBase* pSrcShape = m_pManager->FindShape( pLine->GetSrcShapeId() );
                            if( pSrcShape && pParentShape->IsSrcNeighbourAccepted(pSrcShape->GetClassInfo()->GetClassName()))
                            {
                                pLine->SetTrgShapeId(pParentShape->GetId());
                            }
                        }
                    }
                }
                break;

            default:
                break;
			}

            m_pSelectedHandle = NULL;
			if(m_fCanSaveStateOnMouseUp)SaveCanvasState();
		}
		break;

	case modeSHAPEMOVE:
		{
			// are shapes dropped into accepting shape?
			wxSFShapeBase* pParentShape = GetShapeAtPosition(lpos, 1, searchUNSELECTED);
			if(pParentShape && !pParentShape->AcceptCurrentlyDraggedShapes())pParentShape = NULL;

			ShapeList m_lstSelection;
			GetSelectedShapes(m_lstSelection);

			wxShapeListNode *node = m_lstSelection.GetFirst();
			while(node)
			{
				wxSFShapeBase* pShape = node->GetData();
				pShape->_OnEndDrag(lpos);

				// set new parent
				if((pShape->ContainsStyle(wxSFShapeBase::sfsPARENT_CHANGE)) && !pShape->IsKindOf(CLASSINFO(wxSFLineShape)))
				{
					if(pParentShape)
					{
					    wxRealPoint apos = pShape->GetAbsolutePosition() - pParentShape->GetAbsolutePosition();
                        pShape->SetRelativePosition(apos);
                        pShape->Reparent(pParentShape);

						pShape->DoAlignment();
					}
					else
					{
						if(pShape->GetParentShape())
						{
						    pShape->MoveBy(((wxSFShapeBase*)pShape->GetParentShape())->GetAbsolutePosition());
						}
						pShape->Reparent(m_pManager->GetRootItem());
					}
				}
				node = node->GetNext();
			}

			// resize parent shape to fit all its children
			//if(pParentShape)pParentShape->FitToChildren();
			if(pParentShape)pParentShape->Update();

			if(m_lstSelection.GetCount()>1)
			{
				m_shpMultiEdit.Show(true);
				m_shpMultiEdit.ShowHandles(true);
			}
			else
				m_shpMultiEdit.Show(false);

			MoveShapesFromNegatives();

			if(m_fCanSaveStateOnMouseUp)SaveCanvasState();
		}
		break;

	case modeMULTISELECTION:
		{
			ShapeList m_lstSelection;
            ShapeList shapes;
            m_pManager->GetShapes(CLASSINFO(wxSFShapeBase), shapes);

			wxRect selRect(m_shpMultiEdit.GetBoundingBox().GetLeftTop(), m_shpMultiEdit.GetBoundingBox().GetRightBottom());
			wxShapeListNode *node = shapes.GetFirst();
			while(node)
			{
				wxSFShapeBase* pShape = node->GetData();
#if wxCHECK_VERSION(2, 8, 0)
				if(pShape->IsActive() && selRect.Contains(pShape->GetBoundingBox()))
#else // replacement code for old version
				if(pShape->IsActive() && selRect.Inside(pShape->GetBoundingBox().GetTopLeft()) && selRect.Inside(pShape->GetBoundingBox().GetBottomRight()))
#endif
				{
					pShape->Select(true);
					//pShape->ShowHandles(true);
					m_lstSelection.Append(pShape);
				}
				node = node->GetNext();
			}

			ValidateSelection(m_lstSelection);

			if(m_lstSelection.GetCount() > 0)
			{
				HideAllHandles();
				m_shpMultiEdit.Show(true);
				m_shpMultiEdit.ShowHandles(true);
			}
			else
				m_shpMultiEdit.Show(false);

		}
		break;

    default:
        break;

	}

    if(m_nWorkingMode != modeCREATECONNECTION)
    {
        // update canvas
        m_nWorkingMode = modeREADY;
        UpdateMultieditSize();
        UpdateVirtualSize();
        Refresh();
    }
}

void wxSFShapeCanvas::OnLeftDoubleClick(wxMouseEvent& event)
{
	// HINT: override it for custom actions...

    DeleteAllTextCtrls();
	SetFocus();

	wxPoint lpos = DP2LP(event.GetPosition());

	switch(m_nWorkingMode)
	{
    case modeREADY:
        {
            wxSFShapeBase* pShape = GetShapeAtPosition(lpos);
            if(pShape)
            {
                pShape->OnLeftDoubleClick(lpos);
                pShape->Refresh();

                // double click onto a line shape always change its set of
                // control points so the canvas state should be saved now...
                if(pShape->IsKindOf(CLASSINFO(wxSFLineShape)))SaveCanvasState();
            }
        }
        break;

    default:
        break;
	}
}

void wxSFShapeCanvas::OnRightDown(wxMouseEvent& event)
{
	// HINT: override it for custom actions...

    DeleteAllTextCtrls();
	SetFocus();

	wxPoint lpos = DP2LP(event.GetPosition());

	switch(m_nWorkingMode)
	{
    case modeREADY:
        {
            DeselectAll();

            wxSFShapeBase* pShape = GetShapeAtPosition(lpos);
            if(pShape)
            {
                pShape->Select(true);
                pShape->OnRightClick(lpos);
                //pShape->Refresh();
            }
        }
        break;

    default:
        break;
	}

	Refresh();
}

void wxSFShapeCanvas::OnRightUp(wxMouseEvent &event)
{
	// HINT: override it for custom actions...

	wxPoint lpos = DP2LP(event.GetPosition());

	switch(m_nWorkingMode)
	{
	case modeREADY:
		{
		}
		break;

    default:
        break;
	}
}

void wxSFShapeCanvas::OnRightDoubleClick(wxMouseEvent& event)
{
	// HINT: override it for custom actions...

    DeleteAllTextCtrls();
	SetFocus();

	wxPoint lpos = DP2LP(event.GetPosition());

	switch(m_nWorkingMode)
	{
    case modeREADY:
        {
            wxSFShapeBase* pShape = GetShapeAtPosition(lpos);
            if(pShape)
            {
                pShape->OnRightDoubleClick(lpos);
                pShape->Refresh();
            }
        }
        break;

    default:
        break;
	}
}

void wxSFShapeCanvas::OnMouseMove(wxMouseEvent& event)
{
	// HINT: override it for custom actions...

	wxASSERT(m_pManager);
	if(!m_pManager)return;

	wxPoint lpos = DP2LP(event.GetPosition());

	switch(m_nWorkingMode)
	{
	case modeREADY:
	case modeCREATECONNECTION:
		{
			if(!event.Dragging())
			{
				// send event to multiedit shape
				m_shpMultiEdit._OnMouseMove(lpos);

				// send event to all user shapes
                ShapeList shapes;
                m_pManager->GetShapes(CLASSINFO(wxSFShapeBase), shapes);

				wxShapeListNode* node = shapes.GetFirst();
				while(node)
				{
					node->GetData()->_OnMouseMove(lpos);
					node = node->GetNext();
				}

				// update unfinished line if any
				if(m_pNewLineShape)
				{
				    /*wxRect lineRct = m_pNewLineShape->GetBoundingBox();
				    m_pNewLineShape->SetUnfinishedPoint(lpos);
					lineRct.Union(m_pNewLineShape->GetBoundingBox());*/
                    wxRect lineRct, updLineRct;
                    m_pNewLineShape->GetCompleteBoundingBox(lineRct, wxSFShapeBase::bbSELF | wxSFShapeBase::bbCHILDREN);

				    m_pNewLineShape->SetUnfinishedPoint(lpos);
				    m_pNewLineShape->GetCompleteBoundingBox(updLineRct, wxSFShapeBase::bbSELF | wxSFShapeBase::bbCHILDREN);

					lineRct.Union(updLineRct);

					RefreshCanvas(false, lineRct);
				}
			}
		}
		break;

	case modeHANDLEMOVE:
	case modeMULTIHANDLEMOVE:
		{
			if(event.Dragging())
			{
				m_pSelectedHandle->OnDragging(FitPositionToGrid(lpos));
				if(m_nWorkingMode == modeMULTIHANDLEMOVE)UpdateMultieditSize();

				m_fCanSaveStateOnMouseUp = true;
			}
			else
			{
				m_pSelectedHandle = NULL;
				m_nWorkingMode = modeREADY;
			}
		}
		// do not always 'break'; allow shape's highlighting in modeHANDLEMOVE as well
		if(m_nWorkingMode == modeMULTIHANDLEMOVE)break;

	case modeSHAPEMOVE:
		{
			if(event.Dragging())
			{
                ShapeList shapes;
                wxSFShapeBase* pShape;

                m_pManager->GetShapes(CLASSINFO(wxSFShapeBase), shapes);
				wxShapeListNode* node = shapes.GetFirst();
				while(node)
				{
					pShape = node->GetData();

					if(pShape->IsSelected() && (m_nWorkingMode == modeSHAPEMOVE))
					{
						pShape->_OnDragging(FitPositionToGrid(lpos));
					}
					else
						pShape->_OnMouseMove(lpos);

					node = node->GetNext();
				}
				m_fCanSaveStateOnMouseUp = true;
			}
			else
			{
				m_nWorkingMode = modeREADY;
			}
		}
		break;

	case modeMULTISELECTION:
		{
			wxRect shpRct = m_shpMultiEdit.GetBoundingBox();
			m_shpMultiEdit.SetRectSize(wxRealPoint(lpos.x - shpRct.GetLeft(), lpos.y - shpRct.GetTop()));
			Refresh(false);
		}
		break;
	}
}

void wxSFShapeCanvas::OnKeyDown(wxKeyEvent &event)
{
	// HINT: override it for custom actions...

	wxASSERT(m_pManager);
	if(!m_pManager)return;

	ShapeList m_lstSelection;
	GetSelectedShapes(m_lstSelection);

	switch(event.GetKeyCode())
	{
	case WXK_DELETE:
		{
		    // send event to selected shapes
			wxShapeListNode *node = m_lstSelection.GetFirst();
			while(node)
			{
				node->GetData()->_OnKey(event.GetKeyCode());
				node = node->GetNext();
			}

            // delete selected shapes
			m_pManager->RemoveShapes(m_lstSelection);
			m_shpMultiEdit.Show(false);
			SaveCanvasState();
			Refresh(false);
		}
		break;

	case WXK_ESCAPE:
		{
			switch(m_nWorkingMode)
			{
            case modeCREATECONNECTION:
                {
					AbortInteractiveConnection();
                }
                break;

            case modeHANDLEMOVE:
                {
                    if(m_pSelectedHandle && m_pSelectedHandle->GetParentShape()->IsKindOf(CLASSINFO(wxSFLineShape)))
                    {
                        wxSFLineShape* pLine = (wxSFLineShape*)m_pSelectedHandle->GetParentShape();
                        pLine->SetLineMode(wxSFLineShape::modeREADY);
                    }
                }
                break;

            default:
                // send event to selected shapes
                wxShapeListNode *node = m_lstSelection.GetFirst();
                while(node)
                {
                    node->GetData()->_OnKey(event.GetKeyCode());
                    node = node->GetNext();
                }
                break;
			}
			m_nWorkingMode = modeREADY;
			Refresh(false);
		}
		break;

	default:
		{
			wxShapeListNode *node = m_lstSelection.GetFirst();
			while(node)
			{
				node->GetData()->_OnKey(event.GetKeyCode());
				node = node->GetNext();
			}

			//if(m_lstSelection.GetCount() > 1)
			if(m_shpMultiEdit.IsVisible())
			{
				UpdateMultieditSize();
			}
		}
	}
}

void wxSFShapeCanvas::OnConnectionFinished(wxSFLineShape* connection)
{
	// HINT: override to perform user-defined actions...

	// ... standard implementation generates the wxEVT_SF_LINE_DONE event.
	long id = -1;
	if( connection ) id = connection->GetId();

    wxSFShapeEvent event( wxEVT_SF_LINE_DONE, id);
    event.SetShape( connection );
    ProcessEvent( event );
}

void wxSFShapeCanvas::OnTextChange(wxSFEditTextShape* text)
{
    // HINT: override it for custom actions...

	// ... standard implementation generates the wxEVT_SF_TEXT_CHANGE event.
	long id = -1;
	if( text ) id = text->GetId();

    wxSFShapeEvent event( wxEVT_SF_TEXT_CHANGE, id);
    event.SetShape( text );
    ProcessEvent( event );
}


//----------------------------------------------------------------------------------//
// Private event handlers functions
//----------------------------------------------------------------------------------//

void wxSFShapeCanvas::_OnLeftDown(wxMouseEvent& event)
{
    this->OnLeftDown(event);

    event.Skip();
}

void wxSFShapeCanvas::_OnLeftDoubleClick(wxMouseEvent& event)
{
    this->OnLeftDoubleClick(event);

    event.Skip();
}

void wxSFShapeCanvas::_OnLeftUp(wxMouseEvent& event)
{
    this->OnLeftUp(event);

    event.Skip();
}

void wxSFShapeCanvas::_OnRightDown(wxMouseEvent& event)
{
    this->OnRightDown(event);

    event.Skip();
}

void wxSFShapeCanvas::_OnRightDoubleClick(wxMouseEvent& event)
{
    this->OnRightDoubleClick(event);

    event.Skip();
}

void wxSFShapeCanvas::_OnRightUp(wxMouseEvent& event)
{
    this->OnRightUp(event);

    event.Skip();
}

void wxSFShapeCanvas::_OnMouseMove(wxMouseEvent& event)
{
    this->OnMouseMove(event);

    event.Skip();
}
void wxSFShapeCanvas::_OnKeyDown(wxKeyEvent& event)
{
    this->OnKeyDown(event);

    event.Skip();
}

void wxSFShapeCanvas::OnEnterWindow(wxMouseEvent& event)
{
	event.Skip();
}

void wxSFShapeCanvas::OnLeaveWindow(wxMouseEvent& event)
{
	wxPoint lpos = DP2LP(event.GetPosition());

	switch(m_nWorkingMode)
	{
	case modeMULTISELECTION:
		UpdateMultieditSize();
		m_shpMultiEdit.Show(false);
        m_nWorkingMode = modeREADY;
        Refresh(false);
		break;

	case modeSHAPEMOVE:
		{
			ShapeList m_lstSelection;
			GetSelectedShapes(m_lstSelection);

			MoveShapesFromNegatives();
			UpdateVirtualSize();

			if(m_lstSelection.GetCount() > 1)
			{
				UpdateMultieditSize();
				m_shpMultiEdit.Show(true);
				m_shpMultiEdit.ShowHandles(true);
			}

            m_nWorkingMode = modeREADY;
            Refresh(false);
		}
		break;

    case modeCREATECONNECTION:
        break;

    case modeHANDLEMOVE:
        {
            if(m_pSelectedHandle)
            {
				if(m_pSelectedHandle->GetParentShape()->IsKindOf(CLASSINFO(wxSFLineShape)))
				{
					wxSFLineShape* pLine = (wxSFLineShape*)m_pSelectedHandle->GetParentShape();
					pLine->SetLineMode(wxSFLineShape::modeREADY);

				}
				else if(m_pSelectedHandle->GetParentShape()->IsKindOf(CLASSINFO(wxSFBitmapShape)))
				{
					wxSFBitmapShape* pBitmap = (wxSFBitmapShape*)m_pSelectedHandle->GetParentShape();
					pBitmap->OnEndHandle(*m_pSelectedHandle);
				}

                SaveCanvasState();
				m_nWorkingMode = modeREADY;
                Refresh(false);
            }
        }
        break;

	case modeMULTIHANDLEMOVE:
		{
			if(m_pSelectedHandle)
			{
				m_pSelectedHandle->OnEndDrag(lpos);

                SaveCanvasState();
				m_nWorkingMode = modeREADY;
                Refresh(false);
			}
		}
		break;

	default:
        m_nWorkingMode = modeREADY;
        Refresh(false);
		break;
	}

	event.Skip();
}

//----------------------------------------------------------------------------------//
// Canvas positions recalculations
//----------------------------------------------------------------------------------//

wxPoint wxSFShapeCanvas::DP2LP(const wxPoint& pos) const
{
	int x,y;
	CalcUnscrolledPosition(pos.x, pos.y, &x, &y);

	return wxPoint(int(x/m_Settings.m_nScale), int(y/m_Settings.m_nScale));
}

wxRect wxSFShapeCanvas::DP2LP(const wxRect& rct) const
{
	int x,y;
	CalcUnscrolledPosition(rct.x, rct.y, &x, &y);

	return wxRect(int(x/m_Settings.m_nScale), int(y/m_Settings.m_nScale), int(rct.width/m_Settings.m_nScale), int(rct.height/m_Settings.m_nScale));
}

wxPoint wxSFShapeCanvas::LP2DP(const wxPoint& pos) const
{
	int x,y;
	CalcScrolledPosition(pos.x, pos.y, &x, &y);

	return wxPoint(int(x*m_Settings.m_nScale), int(y*m_Settings.m_nScale));
}

wxRect wxSFShapeCanvas::LP2DP(const wxRect& rct) const
{
	int x,y;
	CalcScrolledPosition(rct.x, rct.y, &x, &y);

	return wxRect(int(x*m_Settings.m_nScale), int(y*m_Settings.m_nScale), int(rct.width*m_Settings.m_nScale), int(rct.height*m_Settings.m_nScale));
}

void wxSFShapeCanvas::SetScale(double scale)
{
	wxASSERT(m_pManager);
	if(!m_pManager)return;

	if(scale != 0)m_Settings.m_nScale = scale;
	else
		m_Settings.m_nScale = 1;

	// rescale all bitmap shapes if neccessary
	ShapeList lstBitmaps;
	m_pManager->GetShapes(CLASSINFO(wxSFBitmapShape), lstBitmaps);

	wxShapeListNode* node = lstBitmaps.GetFirst();
	while(node)
	{
		node->GetData()->Scale(1, 1);
		node = node->GetNext();
	}

    UpdateVirtualSize();
}

wxPoint wxSFShapeCanvas::FitPositionToGrid(const wxPoint& pos) const
{
	if(m_Settings.m_fUseGrid)
	{
		return wxPoint(pos.x / m_Settings.m_nGridSize.x * m_Settings.m_nGridSize.x,
			pos.y / m_Settings.m_nGridSize.y * m_Settings.m_nGridSize.y);
	}
	else
		return pos;
}

//----------------------------------------------------------------------------------//
// Shapes handling functions
//----------------------------------------------------------------------------------//

void wxSFShapeCanvas::LoadCanvas(const wxString& file)
{
 	wxASSERT(m_pManager);
	if(!m_pManager)return;

    bool fChartLoaded = false;

    wxXmlDocument xmlDoc;
    xmlDoc.Load(file);

    wxXmlNode* root = xmlDoc.GetRoot();
    if(root)
    {
        m_pManager->Clear();
        ClearCanvasHistory();

        if(root->GetName() == wxT("chart"))
        {
            // the file contains only chart without any additional info
            m_pManager->DeserializeObjects(NULL, root);
        }
        else if(root->GetName() == wxT("canvas"))
        {
            // the file contains chart and additional canvas info
            wxXmlNode* child = root->GetChildren();
            while(child)
            {
                if(child->GetName() == wxT("settings"))
                {
                    m_Settings.DeserializeObject(child->GetChildren());

                    m_pManager->GetAcceptedShapes().Clear();
                    WX_APPEND_ARRAY(m_pManager->GetAcceptedShapes(), m_Settings.m_arrAcceptedShapes);
                }
                else if(child->GetName() == wxT("chart"))
                {
                    // only one chart can be loaded at once
                    if(!fChartLoaded)
                    {
                        m_pManager->DeserializeObjects(NULL, child);

                        fChartLoaded = true;
                    }
                }
                child = child->GetNext();
            }
        }
        else
            wxMessageBox(wxT("Unknown file format."), wxT("wxShapeFramework"), wxOK | wxICON_WARNING);

        SetScale(m_Settings.m_nScale);
        SaveCanvasState();
        UpdateVirtualSize();
        Refresh(false);
    }
}

void wxSFShapeCanvas::SaveCanvas(const wxString& file)
{
 	wxASSERT(m_pManager);
	if(!m_pManager)return;

	// create root node and save canvas properties
	wxXmlNode *root = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("canvas"));

    // initialize settings
    m_Settings.m_arrAcceptedShapes.Clear();
    WX_APPEND_ARRAY(m_Settings.m_arrAcceptedShapes, m_pManager->GetAcceptedShapes());

    // serialize settings
    wxXmlNode* settings = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("settings"));
    settings->AddChild(m_Settings.SerializeObject(settings));
    root->AddChild(settings);

    // create chart node
    wxXmlNode* chart = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("chart"));
    m_pManager->SerializeObjects(m_pManager->GetRootItem(), chart, false);
    root->AddChild(chart);

    // create and save XML document
    wxXmlDocument xmlDoc;
    xmlDoc.SetRoot(root);
#if wxCHECK_VERSION(2, 8, 0)
    xmlDoc.Save(file, 2);
#else // replacement code for old version
    xmlDoc.Save(file);
#endif
}

void wxSFShapeCanvas::StartInteractiveConnection(wxClassInfo* shapeInfo, const wxPoint& pos)
{
	wxASSERT(m_pManager);
	if(!m_pManager)return;

    if((m_nWorkingMode == modeREADY) && shapeInfo->IsKindOf(CLASSINFO(wxSFLineShape)))
    {
        m_pNewLineShape = (wxSFLineShape*)m_pManager->AddShape(shapeInfo, sfDONT_SAVE_STATE);
        if(m_pNewLineShape)
        {
            wxPoint lpos = DP2LP(pos);

            m_nWorkingMode = modeCREATECONNECTION;
            m_pNewLineShape->SetLineMode(wxSFLineShape::modeUNDERCONSTRUCTION);

            wxSFShapeBase* pShapeUnder = GetShapeAtPosition(lpos);
            if(pShapeUnder)
            {
                if((m_pNewLineShape->GetSrcShapeId() == -1) && (pShapeUnder->IsConnectionAccepted(m_pNewLineShape->GetClassInfo()->GetClassName())))
                {
                    m_pNewLineShape->SetSrcShapeId(pShapeUnder->GetId());

                    // swith on the "under-construcion" mode
                    m_pNewLineShape->SetUnfinishedPoint(lpos);
                }
            }
        }
    }
}

void wxSFShapeCanvas::AbortInteractiveConnection()
{
	wxASSERT(m_pManager);
	if(!m_pManager)return;

    if(m_pNewLineShape)
    {
        m_pManager->RemoveShape(m_pNewLineShape);
        m_pNewLineShape = NULL;
        OnConnectionFinished(NULL);
    }
	m_nWorkingMode = modeREADY;
	Refresh(false);
}

void wxSFShapeCanvas::SaveCanvasToBMP(const wxString& file)
{
    // create memory DC a draw the canvas content into

    wxRect bmpBB = GetTotalBoundingBox();
#if wxCHECK_VERSION(2, 8, 0)
    bmpBB.Inflate(m_Settings.m_nGridSize);
#else // replacement code for old version
    bmpBB.Inflate(m_Settings.m_nGridSize.x, m_Settings.m_nGridSize.y);
#endif
    wxBitmap outbmp(bmpBB.GetRight(), bmpBB.GetBottom());
    wxSFScaledPaintDC outdc(outbmp, 1);

#if wxCHECK_VERSION(2, 8, 0)
    if(outdc.IsOk())
#else // replacement code for old version
    if(outdc.Ok())
#endif
    {
        DrawContent(outdc, sfFROM_ANYWHERE);
        outbmp.SaveFile(file, wxBITMAP_TYPE_BMP);
        wxMessageBox(wxString::Format(wxT("The chart has been saved to '%s'."), file.GetData()), wxT("ShapeFramework"));
    }
    else
        wxMessageBox(wxT("Could not create output bitmap."), wxT("wxShapeFramework"), wxOK | wxICON_WARNING);
}

int wxSFShapeCanvas::GetSelectedShapes(ShapeList& selection)
{
 	wxASSERT(m_pManager);
	if(!m_pManager)return 0;

	selection.Clear();

    ShapeList shapes;
    m_pManager->GetShapes(CLASSINFO(wxSFShapeBase), shapes);

	wxShapeListNode* node = shapes.GetFirst();
	while(node)
	{
		wxSFShapeBase* pShape = node->GetData();
		if(pShape->IsSelected())selection.Append(pShape);
		node = node->GetNext();
	}

	return (int)selection.GetCount();
}

wxSFShapeBase* wxSFShapeCanvas::GetShapeAtPosition(const wxPoint& pos, int zorder, SEARCHMODE mode)
{
	wxASSERT(m_pManager);
	if(!m_pManager)return NULL;

	int nCounter = 0;
	ShapeList m_lstSortedShapes;
	wxSFShapeBase* pShape;

    // sort shapes list in the way that the line shapes will be at the top of the list
    ShapeList shapes;
    m_pManager->GetShapes(CLASSINFO(wxSFShapeBase), shapes);

	wxShapeListNode *node = shapes.GetFirst();
	while(node)
	{
	    pShape = node->GetData();
	    if(pShape->IsKindOf(CLASSINFO(wxSFLineShape)))
	    {
	        m_lstSortedShapes.Insert(pShape);
	        nCounter++;
	    }
	    else
            m_lstSortedShapes.Insert(nCounter, pShape);

        node = node->GetNext();
	}

    // find the topmost shape according to the given rules
    nCounter = 1;
	node = m_lstSortedShapes.GetFirst();
	while(node)
	{
		pShape = (wxSFShapeBase*)node->GetData();
		if(pShape->IsVisible() && pShape->IsActive() && pShape->IsInside(pos))
		{
			switch(mode)
			{
			case searchSELECTED:
				if(pShape->IsSelected())
				{
					if(nCounter == zorder)return pShape;
					else
						nCounter++;
				}
				break;

			case searchUNSELECTED:
				if(!pShape->IsSelected())
				{
					if(nCounter == zorder)return pShape;
					else
						nCounter++;
				}
				break;

			case searchBOTH:
				if(nCounter == zorder)return pShape;
				else
					nCounter++;
				break;
			}
		}
		node = node->GetNext();
	}

	return NULL;
}

wxSFShapeHandle* wxSFShapeCanvas::GetTopmostHandleAtPosition(const wxPoint& pos)
{
	wxASSERT(m_pManager);
	if(!m_pManager)return NULL;

    wxSFShapeHandle* pHandle;

	// first test multiedit handles...
	wxCHandleListNode* hnode = m_shpMultiEdit.GetHandles().GetFirst();
	while(hnode)
	{
		pHandle = hnode->GetData();
		if(pHandle->IsVisible() && pHandle->IsInside(pos))return pHandle;
		hnode = hnode->GetNext();
	}

	// ... then test normal handles
    ShapeList shapes;
    m_pManager->GetShapes(CLASSINFO(wxSFShapeBase), shapes);

	wxShapeListNode *node = shapes.GetFirst();
	while(node)
	{
		// iterate through all shape's handles
		if(node->GetData()->ContainsStyle(wxSFShapeBase::sfsSIZE_CHANGE))
		{
            wxCHandleListNode* hnode = node->GetData()->GetHandles().GetFirst();
            while(hnode)
            {
                pHandle = hnode->GetData();
                if(pHandle->IsVisible() && pHandle->IsInside(pos))return pHandle;
                hnode = hnode->GetNext();
            }
		}
		node = node->GetNext();
	}

	return NULL;
}

int wxSFShapeCanvas::GetShapesAtPosition(const wxPoint& pos, ShapeList& shapes)
{
	wxASSERT(m_pManager);
	if(!m_pManager)return 0;

	shapes.Clear();
	wxSFShapeBase *pShape;

    ShapeList lstShapes;
    m_pManager->GetShapes(CLASSINFO(wxSFShapeBase), lstShapes);

	wxShapeListNode *node = lstShapes.GetFirst();
	while(node)
	{
		pShape = node->GetData();
		if(pShape->IsVisible() && pShape->IsActive() && pShape->IsInside(pos))shapes.Append(pShape);
		node = node->GetNext();
	}

	return (int)shapes.GetCount();
}

int wxSFShapeCanvas::GetShapesInside(const wxRect& rct, ShapeList& shapes)
{
	wxASSERT(m_pManager);
	if(!m_pManager)return 0;

	shapes.Clear();
	wxSFShapeBase* pShape;

    ShapeList lstShapes;
    m_pManager->GetShapes(CLASSINFO(wxSFShapeBase), lstShapes);

	wxShapeListNode *node = lstShapes.GetFirst();
	while(node)
	{
		pShape = node->GetData();
		if(pShape->IsVisible() && pShape->IsActive() && pShape->Intersects(rct))shapes.Append(pShape);
		node = node->GetNext();
	}

	return (int)shapes.GetCount();
}

void wxSFShapeCanvas::DeselectAll()
{
	wxASSERT(m_pManager);
	if(!m_pManager)return;

    ShapeList shapes;
    m_pManager->GetShapes(CLASSINFO(wxSFShapeBase), shapes);

	wxShapeListNode *node = shapes.GetFirst();
	while(node)
	{
		node->GetData()->Select(false);
		//node->GetData()->ShowHandles(false);
		node = node->GetNext();
	}

    m_shpMultiEdit.Show(false);
    //m_shpMultiEdit.ShowHandles(false);
}

void wxSFShapeCanvas::SelectAll()
{
	wxASSERT(m_pManager);
	if(!m_pManager)return;

    ShapeList shapes;
    m_pManager->GetShapes(CLASSINFO(wxSFShapeBase), shapes);

	if(shapes.GetCount() > 0)
	{
		wxShapeListNode *node = shapes.GetFirst();
		while(node)
		{
			node->GetData()->Select(true);
			//node->GetData()->ShowHandles(true);
			node = node->GetNext();
		}

		ShapeList lstSelection;
		GetSelectedShapes(lstSelection);
		ValidateSelection(lstSelection);

		HideAllHandles();
		UpdateMultieditSize();
		m_shpMultiEdit.Show(true);
		m_shpMultiEdit.ShowHandles(true);

		Refresh(false);
	}
}

void wxSFShapeCanvas::HideAllHandles()
{
	wxASSERT(m_pManager);
	if(!m_pManager)return;

    ShapeList shapes;
    m_pManager->GetShapes(CLASSINFO(wxSFShapeBase), shapes);

	wxShapeListNode *node = shapes.GetFirst();
	while(node)
	{
		node->GetData()->ShowHandles(false);
		node = node->GetNext();
	}
}

void wxSFShapeCanvas::ValidateSelection(ShapeList& selection)
{
	wxASSERT(m_pManager);
	if(!m_pManager)return;

	ShapeList m_lstShapesToRemove;

	// find child shapes that have parents in the list
	wxShapeListNode *node = selection.GetFirst();
	while(node)
	{
		wxSFShapeBase *pShape = node->GetData();
		if(selection.IndexOf(pShape->GetParentShape()) != wxNOT_FOUND)
		{
			m_lstShapesToRemove.Append(pShape);
		}
		node = node->GetNext();
	}

	// remove child shapes with parents from the list
	node = m_lstShapesToRemove.GetFirst();
	while(node)
	{
		wxSFShapeBase* pShape = node->GetData();

		pShape->Select(false);
		//pShape->ShowHandles(false);
		selection.DeleteObject(pShape);

		node = node->GetNext();
	}

	// move selected shapes to the back of the global list
	node = selection.GetFirst();
	while(node)
	{
		wxSFShapeBase* pShape = node->GetData();

        ((xsSerializable*)pShape->GetParent())->GetChildren().DeleteObject(pShape);
        ((xsSerializable*)pShape->GetParent())->GetChildren().Append(pShape);

		node = node->GetNext();
	}
}

void wxSFShapeCanvas::ValidateSelectionForClipboard(ShapeList& list)
{
    // remove topmost shapes without sfsPARENT_CHANGE style from the selection
    wxSFShapeBase* pShape;
    wxShapeListNode* node = list.GetFirst();
    while(node)
    {
        pShape = node->GetData();
        if(pShape->GetParentShape()
            && !pShape->ContainsStyle(wxSFShapeBase::sfsPARENT_CHANGE)
            && (list.IndexOf(pShape->GetParentShape()) == wxNOT_FOUND))
        {
            list.DeleteObject(pShape);
            node = list.GetFirst();
        }
        else
            node = node->GetNext();
    }
}

void wxSFShapeCanvas::UpdateMultieditSize()
{
	// calculate bounding box
	wxRect unionRct;
	//bool firstRun = true;
	ShapeList m_lstSelection;
	GetSelectedShapes(m_lstSelection);
	wxShapeListNode *node = m_lstSelection.GetFirst();
	while(node)
	{
		if(node == m_lstSelection.GetFirst())
		{
			unionRct = node->GetData()->GetBoundingBox();
			//firstRun = false;
		}
		else
			unionRct.Union(node->GetData()->GetBoundingBox());

        node = node->GetNext();
	}
	unionRct.Inflate(MEOFFSET, MEOFFSET);

	// draw rectangle
	m_shpMultiEdit.SetRelativePosition(wxRealPoint(unionRct.GetPosition().x, unionRct.GetPosition().y));
	m_shpMultiEdit.SetRectSize(wxRealPoint(unionRct.GetSize().x, unionRct.GetSize().y));
}

void wxSFShapeCanvas::SetHoverColour(const wxColour& col)
{
	wxASSERT(m_pManager);
	if(!m_pManager)return;

	m_Settings.m_nCommonHoverColor = col;

	// update Hover color in all existing shapes
    ShapeList shapes;
    m_pManager->GetShapes(CLASSINFO(wxSFShapeBase), shapes);

	wxShapeListNode * node = shapes.GetFirst();
	while(node)
	{
		node->GetData()->SetHoverColour(col);
		node = node->GetNext();
	}
}

wxRect wxSFShapeCanvas::GetTotalBoundingBox() const
{
    wxRect virtRct;

	wxASSERT(m_pManager);

	if(m_pManager)
	{
        // calculate total bounding box (includes all shapes)
        ShapeList shapes;
        m_pManager->GetShapes(CLASSINFO(wxSFShapeBase), shapes);

        wxShapeListNode* node = shapes.GetFirst();
        while(node)
        {
            if(node == shapes.GetFirst())
            {
                virtRct = node->GetData()->GetBoundingBox();
            }
            else
                virtRct.Union(node->GetData()->GetBoundingBox());

            node = node->GetNext();
        }
	}

    return virtRct;
}

wxRect wxSFShapeCanvas::GetSelectionBB()
{
    wxRect bbRct;

    // get selected shapes
    ShapeList lstSelection;
    GetSelectedShapes(lstSelection);

    wxShapeListNode *node = lstSelection.GetFirst();
    while(node)
    {
        node->GetData()->GetCompleteBoundingBox(bbRct, wxSFShapeBase::bbSELF | wxSFShapeBase::bbCHILDREN | wxSFShapeBase::bbCONNECTIONS);
        node = node->GetNext();
    }

    return bbRct;
}

void wxSFShapeCanvas::UpdateVirtualSize()
{
    wxRect virtRct = GetTotalBoundingBox();

    // update virtual area of the scrolled window if neccessary
    if(!virtRct.IsEmpty())
    {
        SetVirtualSize(int(virtRct.GetRight()*m_Settings.m_nScale), int(virtRct.GetBottom()*m_Settings.m_nScale));
    }
    else
        SetVirtualSize(500, 500);
}

void wxSFShapeCanvas::DeleteAllTextCtrls()
{
	wxASSERT(m_pManager);
	if(!m_pManager)return;

	wxSFContentCtrl* pTextCtrl = NULL;
	ShapeList lstShapes;
	m_pManager->GetShapes(CLASSINFO(wxSFEditTextShape), lstShapes);

	wxShapeListNode* node = lstShapes.GetFirst();
	while(node)
	{
		pTextCtrl = ((wxSFEditTextShape*)node->GetData())->GetTextCtrl();
		if(pTextCtrl)pTextCtrl->Quit();
		node = node->GetNext();
	}
}

void wxSFShapeCanvas::MoveShapesFromNegatives()
{
	wxASSERT(m_pManager);
	if(!m_pManager)return;

	wxSFShapeBase *pShape;
	wxRealPoint shapePos;
	double minx = 0, miny = 0;

	// find the maximal negative position value
    ShapeList shapes;
    m_pManager->GetShapes(CLASSINFO(wxSFShapeBase), shapes);

	wxShapeListNode* node = shapes.GetFirst();
	while(node)
	{
		shapePos = node->GetData()->GetAbsolutePosition();

		if(node == shapes.GetFirst())
		{
			minx = shapePos.x;
			miny = shapePos.y;
		}
		else
		{
            if(shapePos.x < minx)minx = shapePos.x;
            if(shapePos.y < miny)miny = shapePos.y;
		}

		node = node->GetNext();
	}

	// move all parents shape so they (and their children) will be located in the positive values only
	if((minx < 0) || (miny < 0))
	{
		node = shapes.GetFirst();
		while(node)
		{
			pShape = node->GetData();

			if(pShape->GetParentShape() == NULL)
			{
				if(minx < 0)pShape->MoveBy(abs((int)minx), 0);
				if(miny < 0)pShape->MoveBy(0, abs((int)miny));
			}
			node = node->GetNext();
		}
	}
}

void wxSFShapeCanvas::AlignSelected(HALIGN halign, VALIGN valign)
{
    int nCnt = 0;

    wxRealPoint min_pos, max_pos, pos;
    wxRect shapeBB, updRct;
    wxSFShapeBase *pShape, *pParent;

    ShapeList lstSelection;
    GetSelectedShapes(lstSelection);

    updRct = GetSelectionBB();
    updRct.Inflate(MEOFFSET, MEOFFSET);

    // find most distant position
    wxShapeListNode *node = lstSelection.GetFirst();
    while(node)
    {
        pShape = node->GetData();

        if(!pShape->IsKindOf(CLASSINFO(wxSFLineShape)))
        {
            pos = pShape->GetAbsolutePosition();
            shapeBB = pShape->GetBoundingBox();

            if( nCnt == 0 )
            {
                min_pos = pos;
                max_pos.x = pos.x + shapeBB.GetWidth();
                max_pos.y = pos.y + shapeBB.GetHeight();
            }
            else
            {
                if( pos.x < min_pos.x )min_pos.x = pos.x;
                if( pos.y < min_pos.y )min_pos.y = pos.y;
                if( (pos.x + shapeBB.GetWidth()) > max_pos.x )max_pos.x = pos.x + shapeBB.GetWidth();
                if( (pos.y + shapeBB.GetHeight()) > max_pos.y )max_pos.y = pos.y + shapeBB.GetHeight();
            }

            nCnt++;
        }

        node = node->GetNext();
    }

    // if only one non-line shape is in the selection then alignment has no sense so exit...
    if(nCnt < 2) return;

    // set new positions
    node = lstSelection.GetFirst();
    while(node)
    {
        pShape = (wxSFShapeBase*)node->GetData();

        if(!pShape->IsKindOf(CLASSINFO(wxSFLineShape)))
        {
            pos = pShape->GetAbsolutePosition();
            shapeBB = pShape->GetBoundingBox();

            switch(halign)
            {
                case halignLEFT:
                    pShape->MoveTo(min_pos.x, pos.y);
                    break;

                case halignRIGHT:
                    pShape->MoveTo(max_pos.x - shapeBB.GetWidth(), pos.y);
                    break;

                case halignCENTER:
                    pShape->MoveTo((max_pos.x + min_pos.x)/2 - shapeBB.GetWidth()/2, pos.y);
                    break;

                default:
                    break;
            }

            switch(valign)
            {
                case valignTOP:
                    pShape->MoveTo(pos.x, min_pos.y);
                    break;

                case valignBOTTOM:
                    pShape->MoveTo(pos.x, max_pos.y - shapeBB.GetHeight());
                    break;

                case valignMIDDLE:
                    pShape->MoveTo(pos.x, (max_pos.y + min_pos.y)/2 - shapeBB.GetHeight()/2);
                    break;

                default:
                    break;
            }

            // update the shape and its parent
            pShape->Update();
            pParent = (wxSFShapeBase*)pShape->GetParentShape();
            if(pParent)
            {
                pParent->Update();
            }
        }

        node = node->GetNext();
    }

    if(!updRct.IsEmpty())
    {
        UpdateMultieditSize();
        SaveCanvasState();
        RefreshCanvas(false, updRct);
    }
}

//----------------------------------------------------------------------------------//
// Clipboard functions
//----------------------------------------------------------------------------------//

void wxSFShapeCanvas::Copy()
{
	wxASSERT(m_pManager);
	if(!m_pManager)return;

	// copy selected shapes to the clipboard
	if(wxTheClipboard->Open())
	{
		ShapeList lstSelection;
		GetSelectedShapes(lstSelection);

        ValidateSelectionForClipboard(lstSelection);

        if(lstSelection.GetCount() > 0)
        {
            wxSFShapeDataObject* dataObj = new wxSFShapeDataObject(m_formatShapes, lstSelection, m_pManager);
            wxTheClipboard->SetData(dataObj);
        }

		wxTheClipboard->Close();
	}
}

void wxSFShapeCanvas::Cut()
{
	wxASSERT(m_pManager);
	if(!m_pManager)return;

	Copy();

	// remove selected shapes
	ShapeList lstSelection;
	GetSelectedShapes(lstSelection);

    ValidateSelectionForClipboard(lstSelection);

    if(lstSelection.GetCount() > 0)
    {
        m_pManager->RemoveShapes(lstSelection);
        m_shpMultiEdit.Show(false);
        SaveCanvasState();
        Refresh(false);
    }
}

void wxSFShapeCanvas::Paste()
{
	wxASSERT(m_pManager);
	if(!m_pManager)return;

	if(wxTheClipboard->Open())
	{
		// read data object from the clipboars
		wxSFShapeDataObject dataObj(m_formatShapes);
		if(wxTheClipboard->GetData(dataObj))
		{
			// create memory stream with copied data
			wxMemoryBuffer dataBuffer;
			wxMemoryOutputStream outstream;
			wxTextOutputStream tout(outstream);

			tout.WriteString(dataObj.m_Data.GetText());

			wxStreamBuffer* strbuff = outstream.GetOutputStreamBuffer();
			strbuff->ResetBuffer();
			dataBuffer.AppendData(strbuff->GetBufferStart(), strbuff->GetDataLeft());
			dataBuffer.AppendByte(0);

			wxMemoryInputStream instream(dataBuffer.GetData(), dataBuffer.GetDataLen()-1);

			if(instream.IsOk())
			{
				// deserialize XML data
				m_pManager->DeserializeFromXml(instream);

				SaveCanvasState();
				Refresh();
			}
		}
		wxTheClipboard->Close();
	}
}

void wxSFShapeCanvas::Undo()
{
	m_CanvasHistory.RestoreOlderState();
}

void wxSFShapeCanvas::Redo()
{
	m_CanvasHistory.RestoreNewerState();
}

void wxSFShapeCanvas::SaveCanvasState()
{
	m_CanvasHistory.SaveCanvasState();
}

bool wxSFShapeCanvas::CanCopy()
{
	ShapeList lstSelection;
	GetSelectedShapes(lstSelection);
	return !lstSelection.IsEmpty();
}

bool wxSFShapeCanvas::CanCut()
{
	return CanCopy();
}

bool wxSFShapeCanvas::CanPaste()
{
	bool result = false;

	if(wxTheClipboard->Open())
	{
		result = wxTheClipboard->IsSupported(m_formatShapes);
		wxTheClipboard->Close();
	}

	return result;
}

bool wxSFShapeCanvas::CanUndo()
{
	return m_CanvasHistory.CanUndo();
}

bool wxSFShapeCanvas::CanRedo()
{
	return m_CanvasHistory.CanRedo();
}

bool wxSFShapeCanvas::CanAlignSelected()
{
    return ( m_shpMultiEdit.IsVisible() & (m_nWorkingMode == modeREADY) );
}

void wxSFShapeCanvas::ClearCanvasHistory()
{
	m_CanvasHistory.Clear();
}