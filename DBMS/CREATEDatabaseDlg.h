#pragma once


// CCREATEDatabaseDlg �Ի���

class CCREATEDatabaseDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCREATEDatabaseDlg)

public:
	CCREATEDatabaseDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCREATEDatabaseDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CREATE_DATABASE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
private:
	CString strDatabaseName;
public:
	CString GetDatabaseName();
};
