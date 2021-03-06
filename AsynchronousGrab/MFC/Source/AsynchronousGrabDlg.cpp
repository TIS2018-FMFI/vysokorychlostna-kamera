/*=============================================================================
  Copyright (C) 2012 - 2016 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        AsynchronousGrabDlg.cpp

  Description: MFC dialog class for the GUI of the AsynchronousGrab example of
               VimbaCPP.

-------------------------------------------------------------------------------

  THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF TITLE,
  NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR  PURPOSE ARE
  DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=============================================================================*/

#include <stdafx.h>
#include <AsynchronousGrab.h>
#include <AsynchronousGrabDlg.h>
#include "VmbTransform.h"
#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <afxtoolbarimages.h>
#define NUM_COLORS 3
#define BIT_DEPTH 8

using AVT::VmbAPI::FramePtr;
using AVT::VmbAPI::CameraPtrVector;
// Ctor
CAsynchronousGrabDlg::CAsynchronousGrabDlg( CWnd* pParent )
    : CDialog( CAsynchronousGrabDlg::IDD, pParent )
    , m_bIsStreaming( false )
{
    m_hIcon = AfxGetApp()->LoadIcon( IDR_MAINFRAME );
	//current_fps_label = GetDlgItem(IDC_EDIT_CURRENT_FPS);
	//time(&oldTime);
	oldTime = duration_cast<std::chrono::milliseconds>(system_clock::now().time_since_epoch());
}

BEGIN_MESSAGE_MAP( CAsynchronousGrabDlg, CDialog )
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED( IDC_BUTTON_STARTSTOP, &CAsynchronousGrabDlg::OnBnClickedButtonStartstop )

    // Here we add the event handlers for Vimba events
    ON_MESSAGE( WM_FRAME_READY, OnFrameReady )
    ON_MESSAGE( WM_CAMERA_LIST_CHANGED, OnCameraListChanged )
	ON_BN_CLICKED(IDC_BUTTON_SET_ROI, &CAsynchronousGrabDlg::OnBnClickedButtonSetRoi)
	ON_BN_CLICKED(IDC_BUTTON_REPLAY, &CAsynchronousGrabDlg::OnBnClickedButtonReplay)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RECORD_BUTTON, &CAsynchronousGrabDlg::OnBnClickedRecordButton)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_FOLDER, &CAsynchronousGrabDlg::OnBnClickedButtonSelectFolder)
	ON_BN_CLICKED(IDC_BUTTON_SET_EXPOSURE, &CAsynchronousGrabDlg::OnBnClickedButtonSetExposure)
END_MESSAGE_MAP()

BOOL CAsynchronousGrabDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    SetIcon( m_hIcon, TRUE );
    SetIcon( m_hIcon, FALSE );

    // Start Vimba
    VmbErrorType err = m_ApiController.StartUp();
    string_type DialogTitle( _TEXT( "AsynchronousGrab (MFC version) Vimba V" ) );
    SetWindowText( ( DialogTitle+m_ApiController.GetVersion() ).c_str() );
	GetDlgItem(IDC_RECORD_BUTTON)->EnableWindow(FALSE);
	//GetDlgItem(IDC_EDIT_REPLAY_FPS)->SetWindowTextA("2000");

	for (int i = 0; i < 10; i++)
	{
		bufferArr[i] = NULL;
	}

    Log( _TEXT( "Starting Vimba" ), err );
    if( VmbErrorSuccess == err )
    {
        // Initially get all connected cameras
        UpdateCameraListBox();
        string_stream_type strMsg;
        strMsg << "Cameras found..." << m_cameras.size();
        Log( strMsg.str() );
    }

    return TRUE;
}

void CAsynchronousGrabDlg::OnSysCommand( UINT nID, LPARAM lParam )
{
    if( SC_CLOSE == nID )
    {
        // if we are streaming stop streaming
        if( true == m_bIsStreaming )
            OnBnClickedButtonStartstop();

        // Before we close the application we stop Vimba
        m_ApiController.ShutDown();
    }

    CDialog::OnSysCommand( nID, lParam );
}

void CAsynchronousGrabDlg::OnBnClickedButtonStartstop()
{
    VmbErrorType err;
    int nRow = m_ListBoxCameras.GetCurSel();

    if( false == m_bIsStreaming )
    {
        if( -1 < nRow )
        {
            // Start acquisition
            err = m_ApiController.StartContinuousImageAcquisition( m_cameras[nRow] );
            // Set up image for MFC picture box
            if (    VmbErrorSuccess == err
                 && NULL == m_Image )
            {
                m_Image.Create(  m_ApiController.GetWidth(),
                                -m_ApiController.GetHeight(),
                                NUM_COLORS * BIT_DEPTH );
                m_ClearBackground = true;
            }
            Log( _TEXT( "Starting Acquisition" ), err );
            m_bIsStreaming = VmbErrorSuccess == err;
        }
        else
        {
            Log( _TEXT( "Please select a camera." ) );
        }
    }
    else
    {
        m_bIsStreaming = false;
        // Stop acquisition
        err = m_ApiController.StopContinuousImageAcquisition();
        m_ApiController.ClearFrameQueue();
        if( NULL != m_Image )
        {
            m_Image.Destroy();
        }
        Log( _TEXT( "Stopping Acquisition" ), err );
    }

    if( false == m_bIsStreaming )
    {
        m_ButtonStartStop.SetWindowText( _TEXT( "|>" ) );
		GetDlgItem(IDC_BUTTON_SET_ROI)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_REPLAY)->EnableWindow(TRUE);
		GetDlgItem(IDC_RECORD_BUTTON)->EnableWindow(FALSE);
    }
    else
    {
        m_ButtonStartStop.SetWindowText( _TEXT( "||" ) );
		GetDlgItem(IDC_BUTTON_SET_ROI)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_REPLAY)->EnableWindow(FALSE);
		GetDlgItem(IDC_RECORD_BUTTON)->EnableWindow(TRUE);
    }
	isRecording = false;
}

struct ThreadParams
{
	int index;
	CString path;
};

UINT MyThreadProc(LPVOID pParam)
{
	ThreadParams * params = (ThreadParams*)pParam;

	try
	{
		bufferArr[params->index]->Save(params->path);
		//m_Image.Save(params->path);
		bufferArr[params->index]->Destroy();
		delete(bufferArr[params->index]);
		bufferArr[params->index] = NULL;
	}
	catch (const std::exception& e)
	{
		int a = 1;
	}
	
	return 0;
}

//
// This event handler is triggered through a MFC message posted by the frame observer
//
// Parameters:
//  [in]    status          The frame receive status (complete, incomplete, ...)
//  [in]    lParam          [Unused, demanded by MFC signature]
//
// Returns:
//  Nothing, always returns 0
//
LRESULT CAsynchronousGrabDlg::OnFrameReady( WPARAM status, LPARAM lParam )
{
    if( true == m_bIsStreaming )
    {
        // Pick up frame
        FramePtr pFrame = m_ApiController.GetFrame();
        if( SP_ISNULL( pFrame) )
        {
            Log( _TEXT("frame ptr is NULL, late call") );
            return 0;
        }
        // See if it is not corrupt
        if( VmbFrameStatusComplete == status )
        {
            VmbUchar_t *pBuffer;
            VmbUchar_t *pColorBuffer = NULL;
            VmbErrorType err = pFrame->GetImage( pBuffer );
            if( VmbErrorSuccess == err )
            {
                VmbUint32_t nSize;
                err = pFrame->GetImageSize( nSize );
                if( VmbErrorSuccess == err )
                {
					// Calculate fps
					milliseconds newTime = duration_cast<std::chrono::milliseconds>(system_clock::now().time_since_epoch());
					milliseconds delta = newTime - oldTime;
					long long fps = 1000.0/delta.count();
					oldTime = newTime;

					std::string s = std::to_string(fps);
					CString s2(s.c_str());
					current_fps_label.SetWindowText((LPCTSTR)s2);
					UpdateData(FALSE);

                    VmbPixelFormatType ePixelFormat = m_ApiController.GetPixelFormat();
                    CopyToImage( pBuffer,ePixelFormat, m_Image );
                    

					// Record it
					if (isRecording == true)
					{
						//auto savePath = L"C:\\Users\\Jakub\\Pictures\\test\\saved.png";
						TCHAR szDirectory[MAX_PATH];
						GetCurrentDirectory(sizeof(szDirectory) - 1, szDirectory);

						CString path = szDirectory;
						path.Append(_T("\\"));
						path.Append(currentSavingFolder);
						path.Append(_T("\\"));
						CString str;
						str.Format(_T("%d.png"), savedFrameNumber);
						path.Append(str);

						currentIndex = savedFrameNumber % 10;
						//OutImage.Save(path);
						//CImage temp;

						try
						{
							if (bufferArr[currentIndex] == NULL)
							{
								bufferArr[currentIndex] = new CImage();
								bufferArr[currentIndex]->Create(m_Image.GetWidth(), -m_Image.GetHeight(), NUM_COLORS * BIT_DEPTH);
								//temp.Create(m_Image.GetWidth(), -m_Image.GetHeight(), NUM_COLORS * BIT_DEPTH);
								bool copySuccess = m_Image.BitBlt(bufferArr[currentIndex]->GetDC(), 0, 0, SRCCOPY);
								bufferArr[currentIndex]->ReleaseDC();
								//CopyToImage(pBuffer, ePixelFormat, bufferArr[currentIndex]);

								//int w = m_Image.GetWidth();
								//int h = m_Image.GetHeight();
								//int d = m_Image.GetBPP();
								//BYTE* oldImg;
								////create new image...
								//temp.Create(w, h, d);
								//int s = temp.GetPitch();
								//BYTE* newImg = (BYTE*)temp.GetBits();

								//for (int y = 0; y < h; y++, newImg += s) {
								//	oldImg = m_Image[y];
								//	for (int x = 0; x < w; x++) {
								//		for (int i = 0; i < d; i++)
								//			*newImg = *oldImg, newImg++, oldImg++;
								//	}
								//}

								//bufferArr[currentIndex] = temp;

								ThreadParams * pthreadparams = new ThreadParams();
								pthreadparams->index = currentIndex;
								pthreadparams->path = path;

								AfxBeginThread(MyThreadProc, pthreadparams);
							}
							else
							{
								Log(_TEXT("skipped frame " + savedFrameNumber));
							}
						}
						catch (const std::exception&)
						{
							Log(_TEXT("copy failed"));
						}

						savedFrameNumber++;
					}

					// Display it
					RECT rect;
					m_PictureBoxStream.GetWindowRect(&rect);
					ScreenToClient(&rect);
					InvalidateRect(&rect, false);
                }
            }
        }
        else
        {
            // If we receive an incomplete image we do nothing but logging
            Log( _TEXT( "Failure in receiving image" ), VmbErrorOther );
        }

        // And queue it to continue streaming
        m_ApiController.QueueFrame( pFrame );
    }

    return 0;
}

//
// This event handler is triggered through a MFC message posted by the camera observer
//
// Parameters:
//  [in]    reason          The reason why the callback of the observer was triggered (plug-in, plug-out, ...)
//  [in]    lParam          [Unused, demanded by MFC signature]
//
// Returns:
//  Nothing, always returns 0
//
LRESULT CAsynchronousGrabDlg::OnCameraListChanged( WPARAM reason, LPARAM lParam )
{
    bool bUpdateList = false;

    // We only react on new cameras being found and known cameras being unplugged
    if( AVT::VmbAPI::UpdateTriggerPluggedIn == reason )
    {
        Log( _TEXT( "Camera list changed. A new camera was discovered by Vimba." ) );
        bUpdateList = true;
    }
    else if( AVT::VmbAPI::UpdateTriggerPluggedOut == reason )
    {
        Log( _TEXT( "Camera list changed. A camera was disconnected from Vimba." ) );
        if( true == m_bIsStreaming )
        {
            OnBnClickedButtonStartstop();
        }
        bUpdateList = true;
    }

    if( true == bUpdateList )
    {
        UpdateCameraListBox();
    }

    m_ButtonStartStop.EnableWindow( 0 < m_cameras.size() || m_bIsStreaming );

    return 0;
}

//
// Copies the content of a byte buffer to a MFC image with respect to the image's alignment
//
// Parameters:
//  [in]    pInbuffer       The byte buffer as received from the cam
//  [in]    ePixelFormat    The pixel format of the frame
//  [out]   OutImage        The filled MFC image
//
void CAsynchronousGrabDlg::CopyToImage( VmbUchar_t *pInBuffer, VmbPixelFormat_t ePixelFormat, CImage &OutImage )
{
    const int               nHeight         = m_ApiController.GetHeight();
    const int               nWidth          = m_ApiController.GetWidth();
    const int               nStride         = OutImage.GetPitch();
    const int               nBitsPerPixel   = OutImage.GetBPP();
    VmbError_t              Result;

    if( ( nWidth*nBitsPerPixel ) /8 != nStride )
    {
        Log( _TEXT( "Vimba only supports stride that is equal to width." ), VmbErrorWrongType );
        return;
    }
    VmbImage                SourceImage,DestinationImage;
    SourceImage.Size        = sizeof( SourceImage );
    DestinationImage.Size   = sizeof( DestinationImage );

    SourceImage.Data        = pInBuffer;
    DestinationImage.Data   = OutImage.GetBits();

    Result = VmbSetImageInfoFromPixelFormat( ePixelFormat, nWidth, nHeight, &SourceImage );
    if( VmbErrorSuccess != Result )
    {
        Log( _TEXT( "Error setting source image info." ), static_cast<VmbErrorType>( Result ) );
        return;
    }
    static const std::string DisplayFormat( "BGR24" );
    Result = VmbSetImageInfoFromString( DisplayFormat.c_str(),DisplayFormat.size(), nWidth,nHeight, &DestinationImage );
    if( VmbErrorSuccess != Result )
    {
        Log( _TEXT( "Error setting destination image info." ),static_cast<VmbErrorType>( Result ) );
        return;
    }
    Result = VmbImageTransform( &SourceImage, &DestinationImage,NULL,0 );
    if( VmbErrorSuccess != Result )
    {
        Log( _TEXT( "Error transforming image." ), static_cast<VmbErrorType>( Result ) );
    }
}



//
// Queries and lists all known camera
//
void CAsynchronousGrabDlg::UpdateCameraListBox()
{
    // Get all cameras currently connected to Vimba
    CameraPtrVector cameras = m_ApiController.GetCameraList();

    // Simply forget about all cameras known so far
    m_ListBoxCameras.ResetContent();
    m_cameras.clear();

    // And query the camera details again
    for(    CameraPtrVector::const_iterator iter = cameras.begin();
            cameras.end() != iter;
            ++iter )
    {
        std::string strCameraName;
        std::string strCameraID;
        if( VmbErrorSuccess != (*iter)->GetName( strCameraName ) )
        {
            strCameraName = "[NoName]";
        }
        // If for any reason we cannot get the ID of a camera we skip it
        if( VmbErrorSuccess == (*iter)->GetID( strCameraID ) )
        {
            
            std::string strInfo = strCameraName + " " + strCameraID;
            m_ListBoxCameras.AddString( CString( strInfo.c_str() ) );
            m_cameras.push_back( strCameraID );
        }
        else
        {
            Log( _TEXT("Could not get camera ID") );
        }
    }

    // Select first cam if none is selected
    if (    -1 == m_ListBoxCameras.GetCurSel()
         && 0 < m_cameras.size() )
    {
        m_ListBoxCameras.SetCurSel( 0 );
    }

    m_ButtonStartStop.EnableWindow( 0 < m_cameras.size() || m_bIsStreaming );
}

//
// Prints out a given logging string, error code and the descriptive representation of that error code
//
// Parameters:
//  [in]    strMsg          A given message to be printed out
//  [in]    eErr            The API status code
//
void CAsynchronousGrabDlg::Log( string_type strMsg, VmbErrorType eErr )
{
    strMsg += _TEXT( "..." ) + m_ApiController.ErrorCodeToMessage( eErr );
    m_ListLog.InsertString( 0, strMsg.c_str() );
}

//
// Prints out a given logging string
//
// Parameters:
//  [in]    strMsg          A given message to be printed out
//
void CAsynchronousGrabDlg::Log( string_type strMsg )
{
    m_ListLog.InsertString( 0, strMsg.c_str() );
}

//
// The remaining functions are MFC intrinsic only
//

HCURSOR CAsynchronousGrabDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>( m_hIcon );
}

void CAsynchronousGrabDlg::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CAMERAS, m_ListBoxCameras);
	DDX_Control(pDX, IDC_LIST_LOG, m_ListLog);
	DDX_Control(pDX, IDC_BUTTON_STARTSTOP, m_ButtonStartStop);
	DDX_Control(pDX, IDC_PICTURE_STREAM, m_PictureBoxStream);
	//DDX_Control(pDX, IDC_EDIT_CURRENT_FPS, current_fps_label);
	DDX_Control(pDX, IDC_INPUT_UL_X, UppeLeftX);
	DDX_Control(pDX, IDC_INPUT_UL_Y, UpperLeftY);
	DDX_Control(pDX, IDC_INPUT_LR_X, LowerRightX);
	DDX_Control(pDX, IDC_INPUT_LR_Y, LowerRightY);
	DDX_Control(pDX, IDC_EDIT_EXPOSURE_TIME, ExposureTime); 
	DDX_Control(pDX, IDC_EDIT_REPLAY_FPS, ReplayFPSInput);
}

template <typename T>
CRect fitRect( T w, T h, const CRect &dst)
{
    double sw = static_cast<double>( dst.Width() ) / w;
    double sh = static_cast<double>( dst.Height() ) / h;
    double s = min( sw, sh );
    T new_w = static_cast<T>( w * s );
    T new_h = static_cast<T>( h * s );
    T off_w = (1 + dst.Width() - new_w) /2;
    T off_h = (1 + dst.Height() - new_h) /2;
    return CRect(off_w,off_h, off_w + new_w, off_h + new_h );
}

void CAsynchronousGrabDlg::OnPaint()
{
    if( IsIconic() )
    {
        CPaintDC dc( this );

        SendMessage( WM_ICONERASEBKGND, reinterpret_cast<WPARAM>( dc.GetSafeHdc() ), 0 );

        int cxIcon = GetSystemMetrics( SM_CXICON );
        int cyIcon = GetSystemMetrics( SM_CYICON );
        CRect rect;
        GetClientRect( &rect );
        int x = ( rect.Width() - cxIcon + 1 ) / 2;
        int y = ( rect.Height() - cyIcon + 1 ) / 2;
        dc.DrawIcon( x, y, m_hIcon );
    }
    else
    {
        CDialog::OnPaint();

        if( NULL != m_Image )
        {
            CPaintDC dc( &m_PictureBoxStream );
            CRect rect;
            m_PictureBoxStream.GetClientRect( &rect );
            if( m_ClearBackground)
            {
                m_ClearBackground = false;
                CBrush clearBrush( GetSysColor( COLOR_BTNFACE) );
                dc.FillRect( rect, &clearBrush);
            }
            rect = fitRect( m_Image.GetWidth(), m_Image.GetHeight(), rect );
            // HALFTONE enhances image quality but decreases performance
            dc.SetStretchBltMode( HALFTONE );
            m_Image.StretchBlt( dc.m_hDC, rect );
        }
    }
}


void CAsynchronousGrabDlg::loadPng(CString path)
{
	CRect rect;
	m_PictureBoxStream.GetWindowRect(&rect);
	this->ScreenToClient(&rect);

	CBitmap pngBmp;
	CDC bmDC;
	CBitmap *pOldbmp;
	BITMAP  bi;
	UINT xPos = rect.left, yPos = rect.top;

	CClientDC dc(this);

	m_Image.Load(path);

	pngBmp.Attach(m_Image.Detach());

	bmDC.CreateCompatibleDC(&dc);

	pOldbmp = bmDC.SelectObject(&pngBmp);
	pngBmp.GetBitmap(&bi);

	//dc.BitBlt(xPos, yPos, rect.Width(), rect.Height(), &bmDC, 0, 0, SRCCOPY);
	float ratio = ((float)rect.Height() / (float)bi.bmHeight);
	int rectNewWidth = bi.bmWidth * ratio;
	int xOffset = ((rect.Width() - rectNewWidth) / 2);
	dc.SetStretchBltMode(HALFTONE);
	dc.StretchBlt(xPos + xOffset, yPos, rectNewWidth, rect.Height(), &bmDC, 0, 0, bi.bmWidth, bi.bmHeight, SRCCOPY);
	bmDC.SelectObject(pOldbmp);
}

int numDigits(int number)
{
	int digits = 0;
	if (number <= 0) digits = 1;
	while (number) {
		number /= 10;
		digits++;
	}
	return digits;
}

void CAsynchronousGrabDlg::replay()
{
	CFileFind finder;
	
	if (frameCounter > 0)
	{
		int numOfDig = numDigits(frameCounter);
		replayPngPath.Delete(replayPngPath.GetLength() - (5 + lastNumDig), (5 + lastNumDig));
		if (numOfDig > lastNumDig)
		{
			lastNumDig++;
		}
	}

	CString str;
	str.Format(_T("\\%d.png"), frameCounter);
	replayPngPath.Append(str);
	if (finder.FindFile(replayPngPath))
	{
		loadPng(replayPngPath);
		IsReplaying = true;
		frameCounter++;
	}
	else
	{
		GetDlgItem(IDC_BUTTON_STARTSTOP)->EnableWindow(TRUE);
		KillTimer(replayTimer);
		IsReplaying = false;
		Log(_TEXT("Replay finished."));
	}
}

void CAsynchronousGrabDlg::OnBnClickedButtonSetRoi()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CString text;
	int x = 0;
	int y = 0;
	int x2 = 0;
	int y2 = 0;
	int w = 0;
	int h = 0;
	try
	{
		UppeLeftX.GetWindowText(text);
		x = _wtoi(text);
		UpperLeftY.GetWindowText(text);
		y = _wtoi(text);
		LowerRightX.GetWindowText(text);
		x2 = _wtoi(text);
		LowerRightY.GetWindowText(text);
		y2 = _wtoi(text);
	}
	catch (const std::exception&)
	{
		Log(_TEXT("ROI wrong input"));
	}

	w = x2 - x;
	h = y2 - y;
	if (w <= 0 || h <= 0)
	{
		Log(_TEXT("ROI Lower right is wrong"));
		return;
	}

	int nRow = m_ListBoxCameras.GetCurSel();
	if (-1 < nRow)
	{
		if (x + w > m_ApiController.GetMaxWidth(m_cameras[nRow]) || y + h > m_ApiController.GetMaxHeight(m_cameras[nRow]))
		{
			Log(_TEXT("ROI out of camera resolution"));
			return;
		}
		m_ApiController.SetROI(x, y, w, h, m_cameras[nRow]);
		Log(_TEXT("ROI set"));
	}
}

void CAsynchronousGrabDlg::OnBnClickedButtonReplay()
{
	if (IsReplaying)
	{
		GetDlgItem(IDC_BUTTON_STARTSTOP)->EnableWindow(TRUE);
		KillTimer(replayTimer);
		IsReplaying = false;
		Log(_TEXT("Replay finished."));
	}
	else
	{
		GetDlgItem(IDC_BUTTON_STARTSTOP)->EnableWindow(FALSE);
		//replayPngPath = L"D:\\Visual_studio\\VysokoRychlostnaKamera\\AsynchronousGrab\\MFC\\Build\\VS2010\\Tue_Feb__5_20_01_01_2019";
		//if(replayPngPath == NULL)

		frameCounter = 0;
		lastNumDig = 1;
		//replayPngPath.Append(_T("\\0.png"));
		//replay();
		CString text;
		ReplayFPSInput.GetWindowText(text);
		if(_wtoi(text))
			replayFPS = _wtoi(text);
		replayTimer = SetTimer(1, 1000 / replayFPS, NULL); // one event every 1000 ms = 1 s
	}
}


void CAsynchronousGrabDlg::OnTimer(UINT_PTR nIDEvent)
{
	// Timer for replay
	if (nIDEvent == 1)
	{
		replay();
	}
	CDialog::OnTimer(nIDEvent);
}


void CAsynchronousGrabDlg::OnBnClickedRecordButton()
{
	// TODO: Add your control notification handler code here
	if (isRecording == false)
	{
		savedFrameNumber = 0;
		currentSavingFolder = _T("test");

		auto now = std::chrono::system_clock::now();
		std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
		char * time = std::ctime(&nowTime);
		CString timeString = CString(time);
		timeString.Delete(timeString.GetLength()-1,2);
		timeString.Replace(_T(" "),_T("_"));
		timeString.Replace(_T(":"), _T("_"));
		currentSavingFolder = timeString;

		// get surrent dir
		TCHAR szDirectory[MAX_PATH];
		GetCurrentDirectory(sizeof(szDirectory) - 1, szDirectory);

		CString path = szDirectory;
		path.Append(_T("\\"));
		path.Append(currentSavingFolder);

		bool created = CreateDirectory(path, NULL);
		Log(_TEXT("Recording."));
	}
	else {
		Log(_TEXT("Recording finished."));
	}
	isRecording = !isRecording;
}


void CAsynchronousGrabDlg::OnBnClickedButtonSelectFolder()
{
	// TODO: Add your control notification handler code here

	BROWSEINFO   bi;
	ZeroMemory(&bi, sizeof(bi));
	TCHAR   szDisplayName[MAX_PATH];
	szDisplayName[0] = ' ';

	bi.hwndOwner = NULL;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szDisplayName;
	bi.lpszTitle = _T("Please select a folder :");
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	bi.lParam = NULL;
	bi.iImage = 0;

	LPITEMIDLIST   pidl = SHBrowseForFolder(&bi);
	TCHAR   szPathName[MAX_PATH];
	if (NULL != pidl)
	{
		BOOL bRet = SHGetPathFromIDList(pidl, szPathName);
		if (FALSE == bRet)
			return;
		//AfxMessageBox(szPathName);
		replayPngPath = CString(szPathName);
	}
}


void CAsynchronousGrabDlg::OnBnClickedButtonSetExposure()
{
	UpdateData(TRUE);
	CString text;
	ExposureTime.GetWindowText(text);
	int expo = _wtoi(text);
	m_ApiController.SetExposure(double(expo), false);
}
