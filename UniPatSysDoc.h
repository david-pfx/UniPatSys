// UniPatSysDoc.h : interface of the CUniPatSysDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNIPATSYSDOC_H__402568CB_E278_11D1_A88F_00000100782D__INCLUDED_)
#define AFX_UNIPATSYSDOC_H__402568CB_E278_11D1_A88F_00000100782D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CCardGame;

class CUniPatSysDoc : public CDocument
{
protected: // create from serialization only
	CUniPatSysDoc();
	DECLARE_DYNCREATE(CUniPatSysDoc)

// Attributes
public:
   CCardGame *m_cardgame;              // pointer to model
   bool m_serialised;                  // true if saved on or loaded from disk

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUniPatSysDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void DeleteContents();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CUniPatSysDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CUniPatSysDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
   virtual BOOL SaveModified();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UNIPATSYSDOC_H__402568CB_E278_11D1_A88F_00000100782D__INCLUDED_)
