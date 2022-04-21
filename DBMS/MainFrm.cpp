
// MainFrm.cpp : CMainFrame ���ʵ��
//

#include "stdafx.h"
#include "DBMS.h"
#include "DBTreeView.h"
#include "DBMSView.h"
#include "MainFrm.h"
#include "DBEntity.h"
#include "DBMSDoc.h"
#include "CREATEDatabaseDlg.h"
#include "CREATETableDlg.h"
#include "DBLogic.h"
#include "TableLogic.h"
#include "AppException.h"
#include "Global.h"
#include "RENAMETableDlg.h"
#include "ADDFieldDlg.h"
#include "TableView.h"
#include "RecordsView.h"
#include "INSERTRecordDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_DATABASE_CREATE, &CMainFrame::OnDatabaseCreate)
	ON_COMMAND(ID_DATABASES_DROP, &CMainFrame::OnDatabasesDrop)
	ON_COMMAND(ID_DATABASES_OPEN, &CMainFrame::OnDatabasesOpen)
	ON_COMMAND(ID_TABLE_CREATE, &CMainFrame::OnTableCreate)
	ON_COMMAND(ID_TABLE_ALTER, &CMainFrame::OnTableRename)
	ON_COMMAND(ID_TABLE_RENAME, &CMainFrame::OnTableRename)
	ON_COMMAND(ID_FIELD_ADD, &CMainFrame::OnFieldAdd)
	ON_COMMAND(ID_RECORD_INSERT, &CMainFrame::OnRecordInsert)
END_MESSAGE_MAP()

static UINT indicators[] = {
	ID_SEPARATOR,           // ״̬��ָʾ��
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame ����/����

CMainFrame::CMainFrame(){
	openDatabase = FALSE;
}

CMainFrame::~CMainFrame(){}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct){
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME)){
		TRACE0("δ�ܴ���������\n");
		return -1;      // δ�ܴ���
	}

	if (!m_wndStatusBar.Create(this)){
		TRACE0("δ�ܴ���״̬��\n");
		return -1;      // δ�ܴ���
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: �������Ҫ��ͣ������������ɾ��������
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs){
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return TRUE;
}

// CMainFrame ���

#ifdef _DEBUG
void CMainFrame::AssertValid() const{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext){
	if (!m_wndSpliter.CreateStatic(this, 1, 2)){
		AfxMessageBox(_T("Failed to create split window��"));
		return FALSE;
	}

	if (!m_wndSpliter.CreateView(0, 0, RUNTIME_CLASS(CDBTreeView), CSize(200, 0), pContext)){
		return FALSE;
	}

	if (!m_wndSpliter.CreateView(0, 1, RUNTIME_CLASS(CDBMSView), CSize(0, 0), pContext)){
		return FALSE;
	}
	return TRUE;
}
// CMainFrame ��Ϣ�������

void CMainFrame::OnDatabaseCreate(){
	CCREATEDatabaseDlg dlg;
	int res = dlg.DoModal();
	if (res == IDOK) {
		//	Get the object of document
		CDBMSDoc* pDoc = (CDBMSDoc*)this->GetActiveDocument();
		int nCount = pDoc->GetDBNum();
		for (int i = 0; i < nCount; i++){
			CString strName = pDoc->GetDBAt(i)->GetName();
			if (dlg.GetDatabaseName().CollateNoCase(strName) == 0) {
				AfxMessageBox(_T("The Database has been existed��"));
				return;
			}
		}
		CDBEntity *eDB = pDoc->CreateDatabase(dlg.GetDatabaseName());

		pDoc->UpdateAllViews(NULL, UPDATE_CREATE_DATABASE, eDB);
	}
}

void CMainFrame::OnDatabasesDrop(){
	CDBMSDoc* pDoc = (CDBMSDoc*)this->GetActiveDocument();
	int res = MessageBoxA(NULL, "Are you sure to DROP the selected database?", "DROP DATABASE", MB_OKCANCEL);
	if (res == MB_OK) {
		pDoc->UpdateAllViews(NULL, UPDATE_DROP_DATABASE, NULL);
	}
}

void CMainFrame::OnDatabasesOpen(){
	CDBMSDoc* pDoc = (CDBMSDoc*)this->GetActiveDocument();

	CString strError = pDoc->GetError();
	if (strError.GetLength() > 0)
	{
		AfxMessageBox(strError);
		pDoc->SetError(_T(""));
	}

	openDatabase = TRUE;

	pDoc->UpdateAllViews(NULL, UPDATE_OPEN_DATABASE, NULL);
	
}

void CMainFrame::OnTableCreate(){
	if (openDatabase == TRUE) {
		CCREATETableDlg dlg;
		int res = dlg.DoModal();
		if (res == IDOK) {
			CDBMSDoc* pDoc = (CDBMSDoc*)this->GetActiveDocument();

			int nCount = pDoc->GetTableNum();
			for (int i = 0; i < nCount; i++){
				CString strName = pDoc->GetTBAt(i)->GetName();
				if (dlg.GetTableName() == strName){
					AfxMessageBox(_T("The table has been existed��"));
					return;
				}
			}

			CTableEntity* pTable = pDoc->CreateTable(dlg.GetTableName());

			CString strError = pDoc->GetError();

			if (strError.GetLength() != 0){
				AfxMessageBox(strError);
				pDoc->SetError(_T(""));
				return;
			}
			if (pTable != NULL){
				SwitchView(TABLE);
				pDoc->UpdateAllViews(NULL, UPDATE_CREATE_TABLE, pTable);
			}
		}
	} else {
		AfxMessageBox(_T("You should open the databases first!"));
	}
}

CTableEntity* CDBMSDoc::CreateTable(CString strName){
	CTableEntity* pTable = new CTableEntity();
	pTable->SetName(strName);
	CTableLogic tbLogic;
	try{

		if (tbLogic.CreateTable(dbEntity->GetName(), *pTable) == true){

			arrTB.Add(pTable);
		} else {

			delete pTable;
			pTable = NULL;
		}
	}
	catch (CAppException* e){

		if (pTable != NULL){
			delete pTable;
			pTable = NULL;
		}

		strError = e->GetErrorMessage();

		delete e;
	}
	return pTable;
}

void CMainFrame::OnTableRename()
{
	// TODO: �ڴ���������������
	CDBMSDoc* pDoc = (CDBMSDoc*)this->GetActiveDocument();
	if (pDoc->GetSelectedTB() != NULL) {
		CRENAMETableDlg dlg;
		dlg.SetOldTableName(pDoc->GetSelectedTB()->GetName());
		dlg.SetDatabaseName(pDoc->GetDBEntity()->GetName());
		int res = dlg.DoModal();
		if (res == IDOK) {

			int nCount = pDoc->GetTableNum();
			for (int i = 0; i < nCount; i++) {
				CString strName = pDoc->GetTBAt(i)->GetName();
				if (dlg.GetNewTableName() == strName) {
					AfxMessageBox(_T("The table has been existed��"));
					return;
				}
			}
			pDoc->RenameTable(dlg.GetNewTableName());

			pDoc->UpdateAllViews(NULL, UPDATE_RENAME_TABLE, NULL);
		}
	}
	else
		AfxMessageBox(_T("You should choose a Table!"));
}

void CMainFrame::OnFieldAdd()
{
	// TODO: �ڴ���������������
	// Get the object of document
	CDBMSDoc* pDoc = (CDBMSDoc*)this->GetActiveDocument();
	CTableEntity* pTable = pDoc->GetSelectedTB();
	
	if (pTable != NULL) {
		CADDFieldDlg dlg;
		if (pTable->GetFieldNum() == 0)
			dlg.SetPrimaryKey(TRUE);
		else
			dlg.SetPrimaryKey(FALSE);
		int nRes = dlg.DoModal();

		if (nRes == IDOK){

			CFieldEntity field(dlg.GetFieldName(), dlg.GetDatatype(), dlg.GetParam(), 0);
			field.SetDefaultValue(dlg.GetDefaultVal());
			field.SetIsPrimaryKey(dlg.IsPrimaryKey());

			int nCount = pTable->GetFieldNum();
			for (int i = 0; i < nCount; i++)
			{
				if (field.GetName() == pTable->GetFieldAt(i)->GetName())
				{
					AfxMessageBox(_T("The field has been existed��"));
					return;
				}
			}

			pDoc->AddField(field);

			CString strError = pDoc->GetError();
			if (strError.GetLength() != 0)
			{
				AfxMessageBox(strError);
				pDoc->SetError(_T(""));
				return;
			}

			pDoc->UpdateAllViews(NULL, UPDATE_ADD_FIELD, &field);
		}
	}
}

void CMainFrame::SwitchView(int nViewType)
{
	// Get the size of the original window
	CRect rt;
	CView* pOldView = (CView*)m_wndSpliter.GetPane(0, 1);
	pOldView->GetClientRect(&rt);

	// Get the context
	CCreateContext context;
	context.m_pCurrentDoc = pOldView->GetDocument();
	context.m_pCurrentFrame = this;

	// Delete the original view
	m_wndSpliter.DeleteView(0, 1);

	// Depending on the type of view, create a new view
	switch (nViewType)
	{
	case TABLE:	// Table structure view
	{
		// Add a new view
		context.m_pNewViewClass = RUNTIME_CLASS(CTableView);
		m_wndSpliter.CreateView(0, 1, RUNTIME_CLASS(CTableView), rt.Size(), &context);
	}
	break;
	case RECORD:	// Record view
	{
		// Add new view
		context.m_pNewViewClass = RUNTIME_CLASS(CRecordsView);
		m_wndSpliter.CreateView(0, 1, RUNTIME_CLASS(CRecordsView), rt.Size(), &context);
	}
	break;
	case DEFAULT:	// Default view
	{
		// Add new view
		context.m_pNewViewClass = RUNTIME_CLASS(CTableView);
		m_wndSpliter.CreateView(0, 1, RUNTIME_CLASS(CDBMSView), rt.Size(), &context);
	}
	break;
	default:
		break;
	}

	// Update view
	CView* pCurView = (CView*)m_wndSpliter.GetPane(0, 1);
	pCurView->OnInitialUpdate();

	// Update window
	m_wndSpliter.RecalcLayout();
}

void CMainFrame::OnRecordInsert()
{
	// TODO: �ڴ���������������
	if (openDatabase == TRUE)
	{
		CDBMSDoc* pDoc = (CDBMSDoc*)this->GetActiveDocument();

		CTableEntity* pTable = pDoc->GetSelectedTB();
		if (pTable != NULL)
		{
			CINSERTRecordDlg dlg;

			dlg.SetTable(pTable);

			int nRes = dlg.DoModal();

			if (nRes == IDOK)
			{
				CRecordEntity record = dlg.GetRecord();

				CRecordEntity* pRecord = pDoc->InsertRecord(record);

				CString strError = pDoc->GetError();
				if (strError.GetLength() > 0)
				{
					AfxMessageBox(strError);
					pDoc->SetError(_T(""));
				}
				if (pRecord != NULL)
				{
					pDoc->UpdateAllViews(NULL, UPDATE_INSERT_RECORD, pRecord);
				}
			}
		}
	}

}

