// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// VtailView.cpp : implementation of the CVtailView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Vtail.h"
#endif

#include "VtailDoc.h"
#include "VtailView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVtailView

IMPLEMENT_DYNCREATE(CVtailView, CEditView)

BEGIN_MESSAGE_MAP(CVtailView, CEditView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CVtailView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_OPTIONS_WORDWRAP, &CVtailView::OnOptionsWordwrap)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_WORDWRAP, &CVtailView::OnUpdateOptionsWordwrap)
	ON_COMMAND(ID_OPTIONS_FOLLOWTAIL, &CVtailView::OnOptionsFollowtail)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_FOLLOWTAIL, &CVtailView::OnUpdateOptionsFollowtail)
END_MESSAGE_MAP()

// CVtailView construction/destruction

CVtailView::CVtailView()
{	
	if (m_thread == NULL)
	{
		m_thread = AfxBeginThread(Refresh, this);
		m_thread->m_bAutoDelete = TRUE;
	}
}

CVtailView::~CVtailView()
{
}

BOOL CVtailView::IsWordWrap() const
{
	return (GetStyle() & ES_AUTOHSCROLL) == 0;
}

BOOL CVtailView::SetWordWrap(BOOL bWordWrap)
{
	bWordWrap = !!bWordWrap;    // make sure ==TRUE || ==FALSE
	if (IsWordWrap() == bWordWrap)
		return FALSE;
	// preserve original control's state.
	CFont* pFont = GetFont();
	int nLen = GetBufferLength();
	TCHAR* pSaveText = new TCHAR[GetBufferLength() + 1];
	GetWindowText(pSaveText, nLen + 1);

	// create new edit control with appropriate style and size.
	DWORD dwStyle = dwStyleDefault & ~(ES_AUTOHSCROLL | WS_HSCROLL | WS_VISIBLE);
	if (!bWordWrap)
		dwStyle |= ES_AUTOHSCROLL | WS_HSCROLL;

	CWnd* pParent = GetParent();
	CRect rect;
	GetWindowRect(rect);
	pParent->ScreenToClient(rect);
	CWnd* pFocus = GetFocus();

	UINT_PTR nID = GetDlgCtrlID();

	HWND hWnd = ::CreateWindowEx(WS_EX_CLIENTEDGE, _T("edit"), NULL, dwStyle,
		rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
		pParent->m_hWnd, (HMENU)nID, AfxGetInstanceHandle(), NULL);

	if (hWnd == NULL)
	{
		delete[] pSaveText;
		return FALSE;
	}

	// set the window text to nothing to make sure following set doesn't fail
	SetWindowText(NULL);

	// restore visual state
	::SetWindowText(hWnd, pSaveText);
	delete[] pSaveText;
	if (pFont != NULL)
	{
		ASSERT(pFont->m_hObject != NULL);
		::SendMessage(hWnd, WM_SETFONT, (WPARAM)pFont->m_hObject, 0);
	}

	// detach old window, attach new
	SetDlgCtrlID((UINT)nID + 1);
	HWND hWndOld = Detach();
	::SetWindowLongPtr(hWndOld, GWLP_WNDPROC, (LONG_PTR)*GetSuperWndProcAddr());
	ASSERT(m_hWnd == NULL);
	SubclassWindow(hWnd);
	ASSERT(m_hWnd == hWnd);
	GetParentFrame()->SendMessage(WM_RECALCPARENT);

	UINT nTabStops = m_nTabStops;
	GetEditCtrl().SetTabStops(nTabStops);

	GetClientRect(&rect);
	SetWindowPos(NULL, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW);
	SetWindowPos(NULL, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER | SWP_DRAWFRAME);
	SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	UpdateWindow();

	// destroy old
	::SetWindowPos(hWndOld, NULL, 0, 0, 0, 0,
		SWP_HIDEWINDOW | SWP_NOREDRAW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
		SWP_NOZORDER);
	::DestroyWindow(hWndOld);

	// restore rest of state...
	GetEditCtrl().LimitText(nMaxSize);
	if (pFocus == this)
		SetFocus();

	// notify container that doc changed
	//GetDocument()->UpdateAllItems(NULL);

	ASSERT_VALID(this);
	return TRUE;
}

BOOL CVtailView::IsFollowTail() const
{
	return m_bFollowTail;
}

UINT CVtailView::Refresh(LPVOID pParam)
{	
	CVtailView* pView = static_cast<CVtailView*>(pParam);
	while (true)
	{
		if (pView->m_bActivate)
		{
			CVtailDoc* pDoc = static_cast<CVtailDoc*>(pView->GetDocument());
			pDoc->LoadFileContent(pDoc->GetFilePath());
			if (pView->IsFollowTail())
			{
				pView->GetEditCtrl().SendMessage(EM_SETSEL, 0, -1);
				pView->GetEditCtrl().SendMessage(EM_SETSEL, -1, -1);
				pView->GetEditCtrl().SendMessage(EM_SCROLLCARET, 0, 0);
			}
		}
		Sleep(5000);
	}
	return 0;
}


BOOL CVtailView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	BOOL bPreCreated = CEditView::PreCreateWindow(cs);
	cs.style &= ~(ES_AUTOHSCROLL|WS_HSCROLL);	// Enable word-wrapping

	return bPreCreated;
}


// CVtailView printing


void CVtailView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CVtailView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default CEditView preparation
	return CEditView::OnPreparePrinting(pInfo);
}

void CVtailView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// Default CEditView begin printing
	CEditView::OnBeginPrinting(pDC, pInfo);
}

void CVtailView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// Default CEditView end printing
	CEditView::OnEndPrinting(pDC, pInfo);
}

void CVtailView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CVtailView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CVtailView diagnostics

#ifdef _DEBUG
void CVtailView::AssertValid() const
{
	CEditView::AssertValid();
}

void CVtailView::Dump(CDumpContext& dc) const
{
	CEditView::Dump(dc);
}

CVtailDoc* CVtailView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVtailDoc)));
	return (CVtailDoc*)m_pDocument;
}
#endif //_DEBUG


void CVtailView::OnInitialUpdate()
{
	CEditView::OnInitialUpdate();

	CFont *pFont = new CFont();
	CClientDC dc{ this };
	pFont->CreatePointFont(100, _T("Arial"), &dc);
	SetFont(pFont);
	GetEditCtrl().SetReadOnly(TRUE);		
}

void CVtailView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	CMainFrame* pFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	CMFCRibbonStatusBar* pStatusBar = pFrame->GetStatusBar();
	m_bActivate = bActivate;
	if (bActivate)
	{				
		CMFCRibbonBaseElement* pElement = pStatusBar->GetExElement(0);
		CVtailDoc* pDoc = GetDocument();
		pElement->SetText(pDoc->GetFilePath());		
		pElement = pStatusBar->GetElement(0);				
		pElement->SetText(pDoc->GetStrFileSize());
		pElement = pStatusBar->GetElement(1);
		pElement->SetText(pDoc->GetLastModifiedTime().Format(_T("%d/%m%Y %H:%M:%S")));
		CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
		SetWordWrap(pMainFrame->GetWordWrap());		
	}		
	pStatusBar->RecalcLayout();
	pStatusBar->RedrawWindow();
	CEditView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}


void CVtailView::OnOptionsWordwrap()
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	pMainFrame->SetWordWrap(!pMainFrame->GetWordWrap());	
	SetWordWrap(pMainFrame->GetWordWrap());
}


void CVtailView::OnUpdateOptionsWordwrap(CCmdUI *pCmdUI)
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	pCmdUI->SetCheck(pMainFrame->GetWordWrap());
}


void CVtailView::OnOptionsFollowtail()
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	pMainFrame->SetFollowTail(!pMainFrame->GetFollowTail());
	m_bFollowTail = pMainFrame->GetFollowTail();
}


void CVtailView::OnUpdateOptionsFollowtail(CCmdUI *pCmdUI)
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	pCmdUI->SetCheck(pMainFrame->GetFollowTail());
}
