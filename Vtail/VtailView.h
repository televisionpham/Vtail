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

// VtailView.h : interface of the CVtailView class
//

#pragma once


class CVtailView : public CEditView
{
protected: // create from serialization only
	CVtailView();
	DECLARE_DYNCREATE(CVtailView)

// Attributes
public:
	CVtailDoc* GetDocument() const;
private:	
	BOOL m_bFollowTail{ FALSE };
	CWinThread* m_thread{ NULL };
	BOOL m_bActivate{ FALSE };

// Operations
public:
	BOOL IsWordWrap() const;
	BOOL SetWordWrap(BOOL bWordWrap);
	BOOL IsFollowTail() const;
private:
	static UINT Refresh(LPVOID pParam);

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CVtailView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	afx_msg void OnOptionsWordwrap();
	afx_msg void OnUpdateOptionsWordwrap(CCmdUI *pCmdUI);
	afx_msg void OnOptionsFollowtail();
	afx_msg void OnUpdateOptionsFollowtail(CCmdUI *pCmdUI);
};

#ifndef _DEBUG  // debug version in VtailView.cpp
inline CVtailDoc* CVtailView::GetDocument() const
   { return reinterpret_cast<CVtailDoc*>(m_pDocument); }
#endif

