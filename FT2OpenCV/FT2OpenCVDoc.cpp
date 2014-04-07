
// FT2OpenCVDoc.cpp : implementation of the CFT2OpenCVDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "FT2OpenCV.h"
#endif

#include "FT2OpenCVDoc.h"
#include "FT2OpenCVView.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CFT2OpenCVDoc

IMPLEMENT_DYNCREATE(CFT2OpenCVDoc, CDocument)

BEGIN_MESSAGE_MAP(CFT2OpenCVDoc, CDocument)
END_MESSAGE_MAP()


// CFT2OpenCVDoc construction/destruction

CFT2OpenCVDoc::CFT2OpenCVDoc()
{
	// TODO: add one-time construction code here

}

CFT2OpenCVDoc::~CFT2OpenCVDoc()
{
}

BOOL CFT2OpenCVDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	//get a pointer to the view object
	POSITION posV = GetFirstViewPosition();
	CFT2OpenCVView * view = (CFT2OpenCVView *) GetNextView(posV);
	ASSERT_VALID(view);
	if (!view) {
		AfxMessageBox(_T("Failed to get view!"));
		OnCloseDocument();
	}

	//start the kinect
	m_hWnd = view->GetParentFrame()->m_hWnd;
	m_FTHelper.Init(m_hWnd, FTHelperCallingBack, this);

	namedWindow("dxColor", WINDOW_AUTOSIZE);
	namedWindow("dxDepth", WINDOW_AUTOSIZE);



	return TRUE;
}

void CFT2OpenCVDoc::OnCloseDocument() {

	m_FTHelper.Stop();
}

void CFT2OpenCVDoc::FTHelperCallingBack(PVOID pVoid) {
	CFT2OpenCVDoc* pDoc = reinterpret_cast<CFT2OpenCVDoc*>(pVoid);
	if (pDoc)
	{

	}
}

// CFT2OpenCVDoc serialization

void CFT2OpenCVDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CFT2OpenCVDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CFT2OpenCVDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CFT2OpenCVDoc::SetSearchContent(const CString& value)
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

// CFT2OpenCVDoc diagnostics

#ifdef _DEBUG
void CFT2OpenCVDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CFT2OpenCVDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CFT2OpenCVDoc commands
