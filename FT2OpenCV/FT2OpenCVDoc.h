
// FT2OpenCVDoc.h : interface of the CFT2OpenCVDoc class
//


#pragma once
#include "FT2OpenCVHelpers\FTHelper.h"

class CFT2OpenCVDoc : public CDocument
{
protected: // create from serialization only
	CFT2OpenCVDoc();
	DECLARE_DYNCREATE(CFT2OpenCVDoc)

// Attributes
public:

	HWND m_hWnd;
	FTHelper                    m_FTHelper;
	IFTImage*                   m_pImageBuffer;
	IFTImage*                   m_pVideoBuffer;
	static void FTHelperCallingBack(LPVOID lpParam);
	virtual void OnCloseDocument();


// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CFT2OpenCVDoc();
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
};
