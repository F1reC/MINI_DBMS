#pragma once


// CCREATETableDlg �Ի���

class CCREATETableDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCREATETableDlg)

public:
	CCREATETableDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCREATETableDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
private:
	CString strTableName;
public:
	CString GetTableName();
};
