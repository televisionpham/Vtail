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

// VtailDoc.h : interface of the CVtailDoc class
//


#pragma once
#include "MainFrm.h"

class CVtailDoc : public CDocument
{
protected: // create from serialization only
	CVtailDoc();
	DECLARE_DYNCREATE(CVtailDoc)

// Attributes
public:	

// Operations
public:
	CString GetFilePath() { return m_strFilePath; }
	CString GetStrFileSize();
	CTime GetLastModifiedTime() { return m_lastModifiedTime; }
	static UINT Refresh(LPVOID pParam);
	BOOL m_bIsClosing{ FALSE };
	CWinThread* m_thread{ NULL };
	BOOL m_bThreadIsRunning{ FALSE };

// Overrides
public:
//	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CVtailDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS

#ifdef SHARED_HANDLERS
private:
	CString m_strSearchContent;
	CString m_strThumbnailContent;
#endif // SHARED_HANDLERS
private:
	CString m_strFilePath;
	ULONGLONG m_ullFileSize;
	CTime m_lastModifiedTime;
public:
	// Load file content
	void LoadFileContent(CString strFilePath);
	virtual void OnCloseDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
};
