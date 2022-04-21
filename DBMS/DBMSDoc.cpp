
// DBMSDoc.cpp : CDBMSDoc ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "DBMS.h"
#include "DBLogic.h"
#include "TableLogic.h"
#include "RecordLogic.h"
#include "AppException.h"
#endif

#include "DBMSDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDBMSDoc

IMPLEMENT_DYNCREATE(CDBMSDoc, CDocument)

BEGIN_MESSAGE_MAP(CDBMSDoc, CDocument)
END_MESSAGE_MAP()


// CDBMSDoc ����/����

CDBMSDoc::CDBMSDoc()
{
	// TODO: �ڴ����һ���Թ������
	strError = _T("");
}

CString CDBMSDoc::GetError()
{
	return strError;
}

void CDBMSDoc::SetError(CString error)
{
	this->strError = error;
}

CDBEntity *CDBMSDoc::GetDBEntity()
{
	return dbEntity;
}

void CDBMSDoc::SetDBEntity(CDBEntity *e)
{
	this->dbEntity = e;
}

CDBEntity* CDBMSDoc::GetDBAt(int index)
{
	return arrDB.GetAt(index);
}

int CDBMSDoc::GetDBNum()
{
	return arrDB.GetCount();
}

CTableEntity * CDBMSDoc::GetTBAt(int index)
{
	return arrTB.GetAt(index);
}

int CDBMSDoc::GetTableNum()
{
	return arrTB.GetCount();
}

void CDBMSDoc::SetSelectedTB(CTableEntity * e)
{
	this->selectedTB = e;
}

CTableEntity * CDBMSDoc::GetSelectedTB()
{
	return selectedTB;
}



CDBMSDoc::~CDBMSDoc(){}

BOOL CDBMSDoc::OnNewDocument(){
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)
	dbEntity = new CDBEntity(_T("default"));
	selectedTB = NULL;

	CDBLogic* dbLogic = new CDBLogic();
	try {
		if (!dbLogic->GetDatabase(*dbEntity)){
			if (!dbLogic->CreateDatabase(*dbEntity)){
				throw new CAppException(_T("Failed to create database��"));
			}
		}
		delete dbLogic;
		CString strTitle;
		strTitle.Format(_T("Database(%s)"), dbEntity->GetName());
		this->SetTitle(strTitle);
	}
	catch (CAppException* e){
		strError = e->GetErrorMessage();
		delete e;
	}
	return TRUE;
}




// CDBMSDoc ���л�

void CDBMSDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}

#ifdef SHARED_HANDLERS

// ����ͼ��֧��
void CDBMSDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// �޸Ĵ˴����Ի����ĵ�����
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

// ������������֧��
void CDBMSDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// ���ĵ����������������ݡ�
	// ���ݲ���Ӧ�ɡ�;���ָ�

	// ����:     strSearchContent = _T("point;rectangle;circle;ole object;")��
	SetSearchContent(strSearchContent);
}

void CDBMSDoc::SetSearchContent(const CString& value)
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

// CDBMSDoc ���

#ifdef _DEBUG
void CDBMSDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDBMSDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CDBMSDoc ����
CDBEntity* CDBMSDoc::CreateDatabase(CString databaseName)
{
	CDBEntity* pDB = new CDBEntity(databaseName);
	CDBLogic dbLogic;
	try{
		if (dbLogic.CreateDatabase(*pDB) == true){
			arrDB.Add(pDB);
		}
		else{
			pDB = NULL;
		}
	}
	catch (CAppException* e){
		pDB = NULL;
		strError = e->GetErrorMessage();
		delete e;
	}
	return pDB;
}

void CDBMSDoc::LoadDatabase() {
	CDBLogic dbLogic;
	try{
		int nCount = arrDB.GetCount();
		for (int i = 0; i < nCount; i++){
			CDBEntity* pDB = (CDBEntity*)arrDB.GetAt(i);
			if (pDB != NULL){
				delete pDB;
				pDB = NULL;
			}
		}
		arrDB.RemoveAll();
		dbLogic.GetDatabases(arrDB);
	}
	catch (CAppException* e){
		strError = e->GetErrorMessage();
		delete e;
	}
}

void CDBMSDoc::LoadTables(){
	CTableLogic tbLogic;

	try{
		int nCount = arrTB.GetCount();
		for (int i = 0; i < nCount; i++){

			CTableEntity* pTable = (CTableEntity*)arrTB.GetAt(i);
			if (pTable != NULL){
				delete pTable;
				pTable = NULL;
			}
		}

		arrTB.RemoveAll();

		tbLogic.GetTables(dbEntity->GetName(), arrTB);
	} catch (CAppException* e){
		strError = e->GetErrorMessage();
		delete e;
	}
}


void CDBMSDoc::RenameTable(CString newName) {
	CTableLogic tbLogic;

	try {
		int nCount = arrTB.GetCount();
		for (int i = 0; i < nCount; i++) {

			CTableEntity* pTable = arrTB.GetAt(i);

			CTableEntity newTable(pTable);
			newTable.SetName(newName);

			if (pTable->GetName().CompareNoCase(selectedTB->GetName()) == 0) {
				if (tbLogic.RenameTable(dbEntity->GetName(), selectedTB->GetName(), &newTable)) {
					pTable->SetName(newName);
					pTable->SetTdPath(newTable.GetTdPath());
					pTable->SetTrdPath(newTable.GetTrdPath());
					pTable->SetLMTime(newTable.GETLMTime());
					SetSelectedTB(pTable);
					break;
				}
			}
		}

	}
	catch (CAppException* e) {
		strError = e->GetErrorMessage();
		delete e;
	}
}



CFieldEntity* CDBMSDoc::AddField(CFieldEntity &field)
{
	CTableLogic tbLogic;
	try
	{
		if (tbLogic.AddField(dbEntity->GetName(), *selectedTB, field) == false)
		{
			return NULL;
		}

		int nCount = selectedTB->GetFieldNum();

		CFieldEntity* pField = selectedTB->GetFieldAt(nCount - 1);
		return pField;
	}
	catch (CAppException* e)
	{
		strError = e->GetErrorMessage();
		delete e;
		return NULL;
	}
}

CRecordEntity* CDBMSDoc::InsertRecord(CRecordEntity &record)
{
	CRecordLogic recordLogic;
	CRecordEntity* pRecord = new CRecordEntity(record);
	try
	{
		// Insert record
		if (recordLogic.Insert(dbEntity->GetName(), *selectedTB, record) == true)
		{
			// Save the record which is added successfully to the record array.
			arrRecord.Add(pRecord);
			return pRecord;
		}
	}
	catch (CAppException* e)
	{
		if (pRecord != NULL)
		{
			delete pRecord;
			pRecord = NULL;
		}
		strError = e->GetErrorMessage();
		delete e;
	}

	return NULL;
}

int CDBMSDoc::GetRecordNum()
{
	return arrRecord.GetCount();
}

CRecordEntity* CDBMSDoc::GetRecord(int nIndex)
{
	return (CRecordEntity*)arrRecord.GetAt(nIndex);
}

void CDBMSDoc::LoadRecord(void)
{
	int nCount = arrRecord.GetCount();

	for (int i = 0; i < nCount; i++)
	{
		CRecordEntity* pRecord = (CRecordEntity*)arrRecord.GetAt(i);

		delete pRecord;
		pRecord = NULL;
	}

	arrRecord.RemoveAll();

	try
	{
		CRecordLogic recordLogic;

		if (recordLogic.SelectAll(*selectedTB, arrRecord) == false)
		{
			nCount = arrRecord.GetCount();

			if (nCount > 0)
			{
				for (int i = 0; i < nCount; i++)
				{
					CRecordEntity* pRecord = (CRecordEntity*)arrRecord.GetAt(i);

					delete pRecord;
					pRecord = NULL;
				}

				arrRecord.RemoveAll();
			}
		}
	}
	catch (CAppException* e)
	{
		strError = e->GetErrorMessage();
		delete e;
	}
}

