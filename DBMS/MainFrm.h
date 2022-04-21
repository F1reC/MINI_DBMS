
// MainFrm.h : CMainFrame ��Ľӿ�
//

#pragma once

class CMainFrame : public CFrameWnd{
protected: // �������л�����
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// ʵ��
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
	
#endif

protected:  // �ؼ���Ƕ���Ա
	CToolBar		m_wndToolBar;
	CStatusBar		m_wndStatusBar;
	CSplitterWnd	m_wndSpliter;
	BOOL			openDatabase;
// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

	BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext * pContext);
public:
	afx_msg void OnDatabaseCreate();
	afx_msg void OnDatabasesDrop();
	afx_msg void OnDatabasesOpen();
	afx_msg void OnTableCreate();
	afx_msg void OnTableRename();
	afx_msg void OnFieldAdd();
	afx_msg void OnRecordInsert();
	void SwitchView(int nViewType);
};


