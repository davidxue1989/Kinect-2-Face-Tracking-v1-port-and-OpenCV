
// FT2OpenCVView.cpp : implementation of the CFT2OpenCVView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "FT2OpenCV.h"
#endif

#include "FT2OpenCVDoc.h"
#include "FT2OpenCVView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFT2OpenCVView

IMPLEMENT_DYNCREATE(CFT2OpenCVView, CView)

BEGIN_MESSAGE_MAP(CFT2OpenCVView, CView)
END_MESSAGE_MAP()

// CFT2OpenCVView construction/destruction

CFT2OpenCVView::CFT2OpenCVView()
{
	// TODO: add construction code here

}

CFT2OpenCVView::~CFT2OpenCVView()
{
}

BOOL CFT2OpenCVView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CFT2OpenCVView drawing

void CFT2OpenCVView::OnDraw(CDC* /*pDC*/)
{
	CFT2OpenCVDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CFT2OpenCVView diagnostics

#ifdef _DEBUG
void CFT2OpenCVView::AssertValid() const
{
	CView::AssertValid();
}

void CFT2OpenCVView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CFT2OpenCVDoc* CFT2OpenCVView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFT2OpenCVDoc)));
	return (CFT2OpenCVDoc*)m_pDocument;
}
#endif //_DEBUG


// CFT2OpenCVView message handlers
