/*=============================================================================
  Copyright (C) 2012 - 2016 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        ApiController.cpp

  Description: Implementation file for the ApiController helper class that
               demonstrates how to implement an asynchronous, continuous image
               acquisition with VimbaCPP.

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
#include <afxwin.h>
#include <ApiController.h>
#include "Common/StreamSystemInfo.h"
#include "Common/ErrorCodeToMessage.h"
namespace AVT {
namespace VmbAPI {
namespace Examples {

enum { NUM_FRAMES = 3, };

#define AUTO_VALUE_OFF "Off"
#define AUTO_VALUE_ON "Continuous"

ApiController::ApiController()
// Get a reference to the Vimba singleton
    : m_system( VimbaSystem::GetInstance() )
{
}

ApiController::~ApiController()
{
}

void ApiController::SetROI(int x, int y, int w, int h, const std::string &rStrCameraID)
{
	VmbErrorType res = m_system.OpenCameraByID(rStrCameraID.c_str(), VmbAccessModeFull, m_pCamera);
	if (VmbErrorSuccess == res)
	{
		VmbInt64_t width = w;
		//res = SetFeatureIntValue(m_pCamera, "OffsetX", x);
		std::string namew = "Width";
		FeaturePtr      pFeaturew;
		VmbErrorType    resultw;
		resultw = SP_ACCESS(m_pCamera)->GetFeatureByName(namew.c_str(), pFeaturew);
		if (VmbErrorSuccess == resultw)
		{
			resultw = SP_ACCESS(pFeaturew)->SetValue(width);
		}

		VmbInt64_t height = h;
		//res = SetFeatureIntValue(m_pCamera, "OffsetX", x);
		std::string nameh = "Height";
		FeaturePtr      pFeatureh;
		VmbErrorType    resulth;
		resulth = SP_ACCESS(m_pCamera)->GetFeatureByName(nameh.c_str(), pFeatureh);
		if (VmbErrorSuccess == resulth)
		{
			resulth = SP_ACCESS(pFeatureh)->SetValue(height);
		}

		VmbInt64_t offsetx = x;
		//res = SetFeatureIntValue(m_pCamera, "OffsetX", x);
		std::string name = "OffsetX";
		FeaturePtr      pFeature;
		VmbErrorType    result;
		result = SP_ACCESS(m_pCamera)->GetFeatureByName(name.c_str(), pFeature);
		if (VmbErrorSuccess == result)
		{
			result = SP_ACCESS(pFeature)->SetValue(offsetx);
		}

		VmbInt64_t offsety = y;
		//res = SetFeatureIntValue(m_pCamera, "OffsetX", x);
		std::string namey = "OffsetY";
		FeaturePtr      pFeature2;
		VmbErrorType    result2;
		result2 = SP_ACCESS(m_pCamera)->GetFeatureByName(namey.c_str(), pFeature2);
		if (VmbErrorSuccess == result2)
		{
			result2 = SP_ACCESS(pFeature)->SetValue(offsety);
		}
	}

	//close the camera
	SP_ACCESS(m_pCamera)->Close();
}

//
// Translates Vimba error codes to readable error messages
//
// Parameters:
//  [in]    eErr        The error code to be converted to string
//
// Returns:
//  A descriptive string representation of the error code
//
string_type ApiController::ErrorCodeToMessage( VmbErrorType eErr ) const
{
    return AVT::VmbAPI::Examples::ErrorCodeToMessage( eErr );
}

//
// Starts the Vimba API and loads all transport layers
//
// Returns:
//  An API status code
//
VmbErrorType ApiController::StartUp()
{
    VmbErrorType res;

    // Start Vimba
    res = m_system.Startup();
    if( VmbErrorSuccess == res )
    {
        // Register an observer whose callback routine gets triggered whenever a camera is plugged in or out
        res = m_system.RegisterCameraListObserver( ICameraListObserverPtr( new CameraObserver() ) );
    }

    return res;
}

//
// Shuts down the API
//
void ApiController::ShutDown()
{
    // Release Vimba
    m_system.Shutdown();
}

/** read an integer feature from camera.
*/
inline VmbErrorType GetFeatureIntValue( const CameraPtr &camera, const std::string &featureName, VmbInt64_t & value )
{
    if( SP_ISNULL( camera ) )
    {
        return VmbErrorBadParameter;
    }
    FeaturePtr      pFeature;
    VmbErrorType    result;
    result = SP_ACCESS( camera )->GetFeatureByName( featureName.c_str(), pFeature );
    if( VmbErrorSuccess == result )
    {
        result = SP_ACCESS( pFeature )->GetValue( value );
    }
    return result;
}
/** write an integer feature from camera.
*/
inline VmbErrorType SetFeatureIntValue( const CameraPtr &camera, const std::string &featureName, VmbInt64_t value )
{
    if( SP_ISNULL( camera ) )
    {
        return VmbErrorBadParameter;
    }
    FeaturePtr      pFeature;
    VmbErrorType    result;
    result = SP_ACCESS( camera )->GetFeatureByName( featureName.c_str(), pFeature );
    if( VmbErrorSuccess == result )
    {
        result = SP_ACCESS( pFeature )->SetValue( value );
    }
    return result;
}

int ApiController::GetMaxWidth(const std::string &rStrCameraID)
{
	VmbInt64_t width = -1;
	VmbErrorType res = m_system.OpenCameraByID(rStrCameraID.c_str(), VmbAccessModeFull, m_pCamera);
	if (VmbErrorSuccess == res)
	{
		res = GetFeatureIntValue(m_pCamera, "WidthMax", width);
	}
	SP_ACCESS(m_pCamera)->Close();
	return width;
}

int ApiController::GetMaxHeight(const std::string &rStrCameraID)
{
	VmbInt64_t height = -1;
	VmbErrorType res = m_system.OpenCameraByID(rStrCameraID.c_str(), VmbAccessModeFull, m_pCamera);
	if (VmbErrorSuccess == res)
	{
		res = GetFeatureIntValue(m_pCamera, "HeightMax", height);
	}
	SP_ACCESS(m_pCamera)->Close();
	return height;
}


bool ApiController::SetFullRate(bool enable)
{
	if (m_pCamera == NULL)
		return false;
	
	FeaturePtr pFeature;
	VmbErrorType err = m_pCamera->GetFeatureByName("AcquisitionFrameRateMode", pFeature);
	if (err != VmbErrorSuccess)
		return false;

	// feature value
	CStringA valueStr;
	if (enable)
		valueStr = _T("Off");
	else
		valueStr = _T("Basic");

	// set value
	err = pFeature->SetValue(valueStr);

	return (err == VmbErrorSuccess);
}


bool ApiController::SetExposure(double exposure, bool isAuto)
{
	if (m_pCamera == NULL)
		return false;
	
	FeaturePtr pFeature;
	VmbErrorType err;

	err = m_pCamera->GetFeatureByName("ExposureTime", pFeature);
	if (err == VmbErrorNotFound)
		err = m_pCamera->GetFeatureByName("ExposureTimeAbs", pFeature);
	if (err != VmbErrorSuccess) return false;

	// exposure
	err = pFeature->SetValue(round(exposure));
	if (err != VmbErrorSuccess) return false;

	// auto exposure
	err = m_pCamera->GetFeatureByName("ExposureAuto", pFeature);
	if (err == VmbErrorSuccess)
	{
		err = pFeature->SetValue(isAuto ? AUTO_VALUE_ON : AUTO_VALUE_OFF);
		if (err != VmbErrorSuccess) return false;
	}
	else if (err == VmbErrorNotFound)
	{
		if (isAuto)
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}


bool ApiController::SetFPS(double fps)
{
	if (m_pCamera == NULL)
		return false;
	
	FeaturePtr pFeature;
	VmbErrorType err = m_pCamera->GetFeatureByName("AcquisitionFrameRate", pFeature);
	if (VmbErrorSuccess != err)
	{
		err = m_pCamera->GetFeatureByName("AcquisitionFrameRateAbs", pFeature);
		if (VmbErrorSuccess != err)
			return false;
	}

	err = pFeature->SetValue(fps);
	if (VmbErrorSuccess != err) return false;

	//////////////////////////////////////////////////////////////////////////
	// DO NOT SWITCH CAMERA TRIGGER TO FIXEDRATE
	// REASON: Lebo set FPS ma setnut iba feature fps a nic viac.
	//SetTrigger(ETA_RISING_EDGE, ETS_FIXEDRATE, 0);

	return true;
}

//
// Opens the given camera
// Sets the maximum possible Ethernet packet size
// Adjusts the image format
// Sets up the observer that will be notified on every incoming frame
// Calls the API convenience function to start image acquisition
// Closes the camera in case of failure
//
// Parameters:
//  [in]    rStrCameraID    The ID of the camera to open as reported by Vimba
//
// Returns:
//  An API status code
//
VmbErrorType ApiController::StartContinuousImageAcquisition( const std::string &rStrCameraID )
{
    // Open the desired camera by its ID
    VmbErrorType res = m_system.OpenCameraByID( rStrCameraID.c_str(), VmbAccessModeFull, m_pCamera );
    if( VmbErrorSuccess == res )
    {
		// set roi

		

		//if (VmbErrorSuccess != res)
		//{
		//	printf("rip");
		//}
		//else if (VmbErrorSuccess == res)
		//{
		//	printf("good");
		//}
        // Set the GeV packet size to the highest possible value
        // (In this example we do not test whether this cam actually is a GigE cam)
        FeaturePtr pCommandFeature;
        if( VmbErrorSuccess == m_pCamera->GetFeatureByName( "GVSPAdjustPacketSize", pCommandFeature ) )
        {
            if( VmbErrorSuccess == pCommandFeature->RunCommand() )
            {
                bool bIsCommandDone = false;
                do
                {
                    if( VmbErrorSuccess != pCommandFeature->IsCommandDone( bIsCommandDone ) )
                    {
                        break;
                    }
                } while( false == bIsCommandDone );
            }
        }

        // Save the current width
        res = GetFeatureIntValue( m_pCamera, "Width", m_nWidth );
        if( VmbErrorSuccess == res )
        {
            // Save current height
            res = GetFeatureIntValue( m_pCamera, "Height", m_nHeight );
            if( VmbErrorSuccess == res )
            {
                // Set pixel format. For the sake of simplicity we only support Mono and BGR in this example.
                // Try to set BGR
                res = SetFeatureIntValue( m_pCamera, "PixelFormat", VmbPixelFormatBgr8 );
                if( VmbErrorSuccess != res )
                {
                    // Fall back to Mono
                    res = SetFeatureIntValue( m_pCamera,"PixelFormat", VmbPixelFormatMono8 );
                }
                
				
				

				bool retVal = SetFullRate(false);
				retVal = SetExposure(2500.0, false) && retVal;
				retVal = SetFPS(45) && retVal;
				
				
				// Read back the currently selected pixel format
                res =  GetFeatureIntValue( m_pCamera, "PixelFormat", m_nPixelFormat );

                if( VmbErrorSuccess == res )
                {
                    // Create a frame observer for this camera (This will be wrapped in a shared_ptr so we don't delete it)
                    SP_SET( m_pFrameObserver,new FrameObserver( m_pCamera ) );
                    // Start streaming
                    res = SP_ACCESS( m_pCamera )->StartContinuousImageAcquisition( NUM_FRAMES, m_pFrameObserver );
                }
            }
        }
        if ( VmbErrorSuccess != res )
        {
            // If anything fails after opening the camera we close it
            SP_ACCESS( m_pCamera )->Close();
        }
    }

    return res;
}

//
// Calls the API convenience function to stop image acquisition
// Closes the camera
//
// Returns:
//  An API status code
//
VmbErrorType ApiController::StopContinuousImageAcquisition()
{
    // Stop streaming
    SP_ACCESS( m_pCamera )->StopContinuousImageAcquisition();

    // Close camera
    return  SP_ACCESS( m_pCamera )->Close();
}

//
// Gets all cameras known to Vimba
//
// Returns:
//  A vector of camera shared pointers
//
CameraPtrVector ApiController::GetCameraList()
{
    CameraPtrVector cameras;
    // Get all known cameras
    if( VmbErrorSuccess == m_system.GetCameras( cameras ) )
    {
        // And return them
        return cameras;
    }
    return CameraPtrVector();
}

//
// Gets the width of a frame
//
// Returns:
//  The width as integer
//
int ApiController::GetWidth()
{
    return static_cast<int>( m_nWidth );
}

//
// Gets the height of a frame
//
// Returns:
//  The height as integer
//
int ApiController::GetHeight()
{
    return static_cast<int>( m_nHeight );
}


//
// Gets the pixel format of a frame
//
// Returns:
//  The pixel format as enum
//
VmbPixelFormatType ApiController::GetPixelFormat()
{
    return static_cast<VmbPixelFormatType>( m_nPixelFormat );
}

//
// Gets the oldest frame that has not been picked up yet
//
// Returns:
//  A frame shared pointer
//
FramePtr ApiController::GetFrame()
{
    return SP_DYN_CAST( m_pFrameObserver,FrameObserver )->GetFrame();
}

//
// Clears all remaining frames that have not been picked up
//
void ApiController::ClearFrameQueue()
{
    SP_DYN_CAST( m_pFrameObserver,FrameObserver )->ClearFrameQueue();
}

//
// Queues a given frame to be filled by the API
//
// Parameters:
//  [in]    pFrame          The frame to queue
//
// Returns:
//  An API status code
//
VmbErrorType ApiController::QueueFrame( FramePtr pFrame )
{
    return SP_ACCESS( m_pCamera )->QueueFrame( pFrame );
}

//
// Gets the version of the Vimba API
//
// Returns:
//  The version as string
//
string_type ApiController::GetVersion() const
{
    string_stream_type os;
    os << m_system;
    return os.str();
}
}}} // namespace AVT::VmbAPI::Examples
