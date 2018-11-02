
// VysokoRychlostnaKameraDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VysokoRychlostnaKamera.h"
#include "VysokoRychlostnaKameraDlg.h"
#include "afxdialogex.h"
#include <string>
#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVysokoRychlostnaKameraDlg dialog



CVysokoRychlostnaKameraDlg::CVysokoRychlostnaKameraDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_VYSOKORYCHLOSTNAKAMERA_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	my_cameraSelected = 0;
}

void CVysokoRychlostnaKameraDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_CAM_1, my_cameraSelected);
}

BEGIN_MESSAGE_MAP(CVysokoRychlostnaKameraDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//ON_BN_CLICKED(IDC_RADIO_CAM_1, &CVysokoRychlostnaKameraDlg::OnBnClickedRadio2)
	//ON_BN_CLICKED(IDC_RADIO_CAM_2, &CVysokoRychlostnaKameraDlg::OnBnClickedRadio3)
	//ON_BN_CLICKED(IDC_RADIO_CAM_3, &CVysokoRychlostnaKameraDlg::OnBnClickedRadio4)
	//ON_BN_CLICKED(IDC_RADIO_CAM_4, &CVysokoRychlostnaKameraDlg::OnBnClickedRadioCam4)
	ON_BN_CLICKED(IDCANCEL, &CVysokoRychlostnaKameraDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CVysokoRychlostnaKameraDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CVysokoRychlostnaKameraDlg message handlers

BOOL CVysokoRychlostnaKameraDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVysokoRychlostnaKameraDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVysokoRychlostnaKameraDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



//void CVysokoRychlostnaKameraDlg::OnBnClickedRadio2()
//{
//	// TODO: Add your control notification handler code here
//	//UpdateData(FALSE);
//}
//
//
//void CVysokoRychlostnaKameraDlg::OnBnClickedRadio3()
//{
//	// TODO: Add your control notification handler code here
//	//UpdateData(FALSE);
//}
//
//
//void CVysokoRychlostnaKameraDlg::OnBnClickedRadio4()
//{
//	// TODO: Add your control notification handler code here
//	//UpdateData(FALSE);
//}
//
//
//void CVysokoRychlostnaKameraDlg::OnBnClickedRadioCam4()
//{
//	// TODO: Add your control notification handler code here
//}


void CVysokoRychlostnaKameraDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}

void CVysokoRychlostnaKameraDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	//CDialogEx::OnOK();
	UpdateData(TRUE);

	TRACE("Camera selected: %i \n", my_cameraSelected);
}

