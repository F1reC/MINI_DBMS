#pragma once


// CREANAMETableDlg �Ի���

class CRENAMETableDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRENAMETableDlg)

public:
	CRENAMETableDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CRENAMETableDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RENAME_TABLE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
private:
	CString newTableName;
	CString oldName;
public:
	CString GetNewTableName();
	void SetDatabaseName(CString name);
	void SetOldTableName(CString oldName);
	CString databaseName;
};
