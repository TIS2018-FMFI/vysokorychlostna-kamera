// MainDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VysokoRychlostnaKamera.h"
#include "MainDlg.h"
#include "afxdialogex.h"

#include <SharedPointerDefines.h>
//#include <AsynchronousGrab.h>
#include "VmbTransform.h"
#include <iostream>
#include <sstream>
#include <string>


// MainDlg dialog
#define NUM_COLORS 3
#define BIT_DEPTH 8

using AVT::VmbAPI::FramePtr;
using AVT::VmbAPI::CameraPtrVector;

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
	//DDX_Control(pDX, IDC_PICTURE_STREAM, m_PictureBoxStream);
}


BEGIN_MESSAGE_MAP(MainDlg, CDialogEx)
END_MESSAGE_MAP()


// MainDlg message handlers
