// MainDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VysokoRychlostnaKamera.h"
#include "MainDlg.h"
#include "afxdialogex.h"


// MainDlg dialog

IMPLEMENT_DYNAMIC(MainDlg, CDialogEx)

MainDlg::MainDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_MAIN, pParent)
{

}

MainDlg::MainDlg(int cameraIndex, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_MAIN, pParent)
{
	TRACE("Camera selected: %i \n", cameraIndex);
}

MainDlg::~MainDlg()
{
}

void MainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(MainDlg, CDialogEx)
END_MESSAGE_MAP()


// MainDlg message handlers
