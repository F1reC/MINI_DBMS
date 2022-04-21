
// DBMSDoc.h : CDBMSDoc ��Ľӿ�
//


#pragma once
#include "DBEntity.h"
#include "TableEntity.h"
#include "RecordEntity.h"

class CDBMSDoc : public CDocument
{
protected: // �������л�����
	CDBMSDoc();
	DECLARE_DYNCREATE(CDBMSDoc)

private:
	CDBEntity *dbEntity;
	CTableEntity *selectedTB;
	CString strError;

	DBARR arrDB;
	TABLEARR arrTB;
	RECORDARR arrRecord;
// ����
public:
	CString GetError();
	void SetError(CString error);
	void SetDBEntity(CDBEntity *e);
	void SetSelectedTB(CTableEntity *e);

	CDBEntity *GetDBEntity();
	CDBEntity * GetDBAt(int index);
	int GetDBNum();
	CTableEntity *GetTBAt(int index);
	int GetTableNum();
	CTableEntity *GetSelectedTB();

	CDBEntity * CreateDatabase(CString databaseName);
	void LoadDatabase();
	void LoadTables();
	void RenameTable(CString newName);
	CFieldEntity * AddField(CFieldEntity & field);
	CRecordEntity * InsertRecord(CRecordEntity & record);
	int GetRecordNum();
	CRecordEntity * GetRecord(int nIndex);
	void LoadRecord(void);
	CTableEntity * CreateTable(CString strName);

// ����
public:

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// ʵ��
public:
	virtual ~CDBMSDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// ����Ϊ����������������������ݵ� Helper ����
	void SetSearchContent(const CString& value);
#endif 
	// SHARED_HANDLERS
};
