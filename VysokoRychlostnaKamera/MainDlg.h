#pragma once

#include <vector>
#include <afxwin.h>
#include <atlimage.h>

#include <VimbaCPP.h>

//#include <ApiController.h>
// MainDlg dialog

class MainDlg : public CDialogEx
{
	DECLARE_DYNAMIC(MainDlg)

public:
	MainDlg(CWnd* pParent = nullptr);   // standard constructor
	MainDlg(int cameraIndex, CWnd* pParent = nullptr);
	virtual ~MainDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MAIN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
