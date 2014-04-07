
// FT2OpenCVView.h : interface of the CFT2OpenCVView class
//

#pragma once


class CFT2OpenCVView : public CView
{
protected: // create from serialization only
	CFT2OpenCVView();
	DECLARE_DYNCREATE(CFT2OpenCVView)

// Attributes
public:
	CFT2OpenCVDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CFT2OpenCVView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in FT2OpenCVView.cpp
inline CFT2OpenCVDoc* CFT2OpenCVView::GetDocument() const
   { return reinterpret_cast<CFT2OpenCVDoc*>(m_pDocument); }
#endif

