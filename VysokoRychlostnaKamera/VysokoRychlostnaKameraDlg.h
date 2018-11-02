
// VysokoRychlostnaKameraDlg.h : header file
//

#pragma once


// CVysokoRychlostnaKameraDlg dialog
class CVysokoRychlostnaKameraDlg : public CDialogEx
{
// Construction
public:
	CVysokoRychlostnaKameraDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VYSOKORYCHLOSTNAKAMERA_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	//afx_msg void OnBnClickedRadio2();
	//afx_msg void OnBnClickedRadio3();
	//afx_msg void OnBnClickedRadio4();
	//afx_msg void OnBnClickedRadioCam4();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
private:
	int my_cameraSelected;
};
