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

// VtailDoc.cpp : implementation of the CVtailDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Vtail.h"
#endif

#include "VtailDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CVtailDoc

IMPLEMENT_DYNCREATE(CVtailDoc, CDocument)

BEGIN_MESSAGE_MAP(CVtailDoc, CDocument)
END_MESSAGE_MAP()


// CVtailDoc construction/destruction

CVtailDoc::CVtailDoc()	
{
	// TODO: add one-time construction code here

}

CVtailDoc::~CVtailDoc()
{
}

//BOOL CVtailDoc::OnNewDocument()
//{
//	if (!CDocument::OnNewDocument())
//		return FALSE;
//
//	// TODO: add reinitialization code here
//	// (SDI documents will reuse this document)
//
//	return TRUE;
//}




// CVtailDoc serialization

CString CVtailDoc::GetStrFileSize()
{
	CString strSize;
	if (m_ullFileSize <= 1000)
	{
		strSize.Format(_T("%d B"), m_ullFileSize);
	}
	else if (m_ullFileSize <= 1000000)
	{
		strSize.Format(_T("%d KB"), m_ullFileSize/1000);
	}
	else if (m_ullFileSize <= 1000000000)
	{
		strSize.Format(_T("%d MB"), m_ullFileSize / 1000000);
	}
	else if (m_ullFileSize <= 1000000000000)
	{
		strSize.Format(_T("%d GB"), m_ullFileSize / 1000000000);
	}
	else 
	{
		strSize = _T("VERY VERY LARGE FILE!!!");
	}
	return strSize;
}

UINT CVtailDoc::Refresh(LPVOID pParam)
{
	CVtailDoc* pDoc = static_cast<CVtailDoc*>(pParam);
	while (true)
	{
		if (pDoc->m_bIsClosing)
		{
			break;
		}
		pDoc->LoadFileContent(pDoc->GetFilePath());
		Sleep(5000);
	}
	pDoc->m_bThreadIsRunning = FALSE;
	return TRUE;
}

void CVtailDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{		
	}
	else 
	{
		//LoadFileContent(ar.GetFile()->GetFilePath());
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CVtailDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(m_strThumbnailContent, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CVtailDoc::InitializeSearchContent()
{
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// Use the entire text file content as the search content.
	SetSearchContent(m_strSearchContent);
}

void CVtailDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CVtailDoc diagnostics

#ifdef _DEBUG
void CVtailDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CVtailDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// Load file content
void CVtailDoc::LoadFileContent(CString strFilePath)
{

	m_strFilePath = strFilePath;
	FILE *fStream;
	errno_t errCode = _tfopen_s(&fStream, m_strFilePath, _T("r, ccs=UNICODE"));
	if (errCode != 0)
	{
		return;
	}
	CStdioFile file{ fStream };
	m_ullFileSize = file.GetLength();
	CFileStatus fileStatus;
	file.GetStatus(fileStatus);
	m_lastModifiedTime = fileStatus.m_mtime;
	CString strContent;
	CString strLine;
	while (file.ReadString(strLine))
	{
		strContent += strLine + _T("\r\n");
	}
	file.Close();	
	BOOL bResult = ::SetWindowText(reinterpret_cast<CEditView*>(m_viewList.GetHead())->GetSafeHwnd(), strContent);
}


void CVtailDoc::OnCloseDocument()
{
	// TODO: Add your specialized code here and/or call the base class

	CDocument::OnCloseDocument();
}


BOOL CVtailDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	m_strFilePath = lpszPathName;
	if (m_thread == NULL)
	{
		m_thread = AfxBeginThread(Refresh, this);
		m_thread->m_bAutoDelete = TRUE;
	}

	return TRUE;
}
