//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : outputtabwindow.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#include "outputtabwindow.h"
#include "wx/ffile.h"
#include "output_pane.h"
#include "wx/sizer.h"
#include "wx/toolbar.h"
#include "wx/xrc/xmlres.h"
#include "macros.h"

#ifndef wxScintillaEventHandler
#define wxScintillaEventHandler(func) \
	(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxScintillaEventFunction, &func)
#endif

OutputTabWindow::OutputTabWindow(wxWindow *parent, wxWindowID id, const wxString &name)
		: wxPanel(parent, id)
		, m_name(name)
        , m_tb(NULL)
		, m_canFocus(true)
        , m_outputScrolls(true)
{
	CreateGUIControl();
}

OutputTabWindow::~OutputTabWindow()
{
}

void OutputTabWindow::CreateGUIControl()
{
	wxBoxSizer *mainSizer = new wxBoxSizer(wxHORIZONTAL);
	SetSizer(mainSizer);
	//Create the toolbar
	m_tb = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_VERTICAL|wxTB_NODIVIDER);

    m_tb->AddTool(XRCID("scroll_on_output"),
                wxT("Scroll on Output"),
                wxXmlResource::Get()->LoadBitmap(wxT("link_editor")),
                wxT("Scroll on Output"),
                wxITEM_CHECK);
    m_tb->ToggleTool(XRCID("scroll_on_output"), m_outputScrolls);
    Connect(XRCID("scroll_on_output"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( OutputTabWindow::OnOutputScrolls ));
    
    m_tb->AddTool(XRCID("clear_all_output"),
                wxT("Clear All"),
                wxXmlResource::Get()->LoadBitmap(wxT("document_delete")),
                wxT("Clear All"));
    Connect( XRCID("clear_all_output"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( OutputTabWindow::OnClearAll ));

	m_tb->AddTool(XRCID("word_wrap_output"),
	            wxT("Word Wrap"),
	            wxXmlResource::Get()->LoadBitmap(wxT("word_wrap")),
	            wxT("Word Wrap"),
	            wxITEM_CHECK);
	Connect( XRCID("word_wrap_output"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( OutputTabWindow::OnWordWrap ));

	m_tb->Realize();
	mainSizer->Add(m_tb, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);

	m_sci = new wxScintilla(this);

	// Hide margins
	m_sci->SetLexer(wxSCI_LEX_CONTAINER);
	m_sci->StyleClearAll();

	// symbol margin
	m_sci->SetMarginType(1, wxSCI_MARGIN_SYMBOL);
	m_sci->SetMarginWidth(2, 0);
	m_sci->SetMarginWidth(1, 0);
	m_sci->SetMarginWidth(0, 0);


	wxFont defFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont font(defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);

	m_sci->StyleSetFont(0, font);
	m_sci->StyleSetBackground(0, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	m_sci->StyleSetBackground(wxSCI_STYLE_DEFAULT, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	m_sci->SetReadOnly(true);

	mainSizer->Add(m_sci, 1, wxEXPAND | wxALL, 1);
	mainSizer->Layout();

	//Connect events
	//------------------------

	Connect(wxEVT_SCI_DOUBLECLICK, wxScintillaEventHandler(OutputTabWindow::OnMouseDClick), NULL, this);
	Connect(wxEVT_SCI_HOTSPOT_CLICK, wxScintillaEventHandler(OutputTabWindow::OnHotspotClicked), NULL, this);
	Connect(wxEVT_SCI_UPDATEUI, wxScintillaEventHandler(OutputTabWindow::OnSciUpdateUI), NULL, this);
}

void OutputTabWindow::OnOutputScrolls(wxCommandEvent &e)
{
    m_outputScrolls = !m_outputScrolls;
}

void OutputTabWindow::OnWordWrap(wxCommandEvent &e)
{
	if (m_sci->GetWrapMode() == wxSCI_WRAP_WORD) {
		m_sci->SetWrapMode(wxSCI_WRAP_NONE);
	} else {
		m_sci->SetWrapMode(wxSCI_WRAP_WORD);
	}
}

void OutputTabWindow::AppendText(const wxString &text)
{
	//----------------------------------------------
	// enable writing
	m_sci->SetReadOnly(false);

    if (m_outputScrolls) {
        // the next 4 lines make sure that the caret is at last line
        // and is visible
        m_sci->SetSelectionEnd(m_sci->GetLength());
        m_sci->SetSelectionStart(m_sci->GetLength());
        m_sci->SetCurrentPos(m_sci->GetLength());
        m_sci->EnsureCaretVisible();
    }
    
	// add the text
	m_sci->InsertText(m_sci->GetLength(), text );

    if (m_outputScrolls) {
        // the next 4 lines make sure that the caret is at last line
        // and is visible
        m_sci->SetSelectionEnd(m_sci->GetLength());
        m_sci->SetSelectionStart(m_sci->GetLength());
        m_sci->SetCurrentPos(m_sci->GetLength());
        m_sci->EnsureCaretVisible();
    }
    
	// enable readonly mode
	m_sci->SetReadOnly(true);
}

void OutputTabWindow::OnClearAll(wxCommandEvent &e)
{
	Clear();
}

void OutputTabWindow::Clear()
{
	m_sci->SetReadOnly(false);
	m_sci->ClearAll();
	m_sci->SetReadOnly(true);
}

void OutputTabWindow::OnCommand(wxCommandEvent &e)
{
	switch (e.GetId()) {
	case wxID_COPY:
		m_sci->Copy();
		break;
	default:
		break;
	}
}

void OutputTabWindow::OnUpdateUI(wxUpdateUIEvent &e)
{
	switch (e.GetId()) {
	case wxID_COPY:
		e.Enable( m_sci->GetSelectionStart() - m_sci->GetSelectionEnd() != 0 );
		break;
	default:
		break;
	}
	e.Skip(false);
}

void OutputTabWindow::OnSciUpdateUI(wxScintillaEvent& event)
{
	RecalcHorizontalScrollbar();
}

void OutputTabWindow::RecalcHorizontalScrollbar()
{
	// recalculate and set the length of horizontal scrollbar
	int maxPixel = 0;
	int startLine = m_sci->GetFirstVisibleLine();
	int endLine =  startLine + m_sci->LinesOnScreen();
	if (endLine >= (m_sci->GetLineCount() - 1))
		endLine--;

	for (int i = startLine; i <= endLine; i++) {
		int visibleLine = (int) m_sci->DocLineFromVisible(i);         //get actual visible line, folding may offset lines
		int endPosition = m_sci->GetLineEndPosition(visibleLine);      //get character position from begin
		int beginPosition = m_sci->PositionFromLine(visibleLine);      //and end of line

		wxPoint beginPos = m_sci->PointFromPosition(beginPosition);
		wxPoint endPos = m_sci->PointFromPosition(endPosition);

		int curLen = endPos.x - beginPos.x;

		if (maxPixel < curLen) //If its the largest line yet
			maxPixel = curLen;
	}

	if (maxPixel == 0)
		maxPixel++;                                 //make sure maxPixel is valid

	int currentLength = m_sci->GetScrollWidth();               //Get current scrollbar size
	if (currentLength != maxPixel) {
		//And if it is not the same, update it
		m_sci->SetScrollWidth(maxPixel);
	}
}
