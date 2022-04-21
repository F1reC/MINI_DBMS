// CREATEDatabaseDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DBMS.h"
#include "CREATEDatabaseDlg.h"
#include "afxdialogex.h"


// CCREATEDatabaseDlg �Ի���

IMPLEMENT_DYNAMIC(CCREATEDatabaseDlg, CDialogEx)

CCREATEDatabaseDlg::CCREATEDatabaseDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CREATE_DATABASE, pParent)
	, strDatabaseName(_T(""))
{
}

CCREATEDatabaseDlg::~CCREATEDatabaseDlg()
{
}

void CCREATEDatabaseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_DATABASE_NAME, strDatabaseName);
}

CString CCREATEDatabaseDlg::GetDatabaseName()
{
	return strDatabaseName;
}


BEGIN_MESSAGE_MAP(CCREATEDatabaseDlg, CDialogEx)
END_MESSAGE_MAP()


// CCREATEDatabaseDlg ��Ϣ�������
