// CREATETableDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DBMS.h"
#include "CREATETableDlg.h"
#include "afxdialogex.h"


// CCREATETableDlg �Ի���

IMPLEMENT_DYNAMIC(CCREATETableDlg, CDialogEx)

CCREATETableDlg::CCREATETableDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CREATE_TABLE, pParent)
{

}

CCREATETableDlg::~CCREATETableDlg()
{
}

void CCREATETableDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TABLE_NAME, strTableName);
}

CString CCREATETableDlg::GetTableName()
{
	return strTableName;
}


BEGIN_MESSAGE_MAP(CCREATETableDlg, CDialogEx)
END_MESSAGE_MAP()


// CCREATETableDlg ��Ϣ�������
