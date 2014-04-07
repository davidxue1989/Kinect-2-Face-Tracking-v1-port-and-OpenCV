//------------------------------------------------------------------------------
// <copyright file="Kinect2Sensor.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "StdAfx.h"
#include "Kinect2Sensor.h"
#include <math.h>


static const float c_JointThickness = 3.0f;
static const float c_TrackedBoneThickness = 6.0f;
static const float c_InferredBoneThickness = 1.0f;
static const float c_HandSize = 30.0f;


Kinect2Sensor::Kinect2Sensor() :
m_pKinect2Sensor(NULL),
m_pColorFrameReader(NULL),
m_pDepthFrameReader(NULL),
m_pCoordinateMapper(NULL),
m_pBodyFrameReader(NULL),
m_pBodyIndexFrameReader(NULL),
m_pColorRGBX(NULL),
m_pDepth(NULL),
m_pBodyIndex(NULL),
m_ppBodies(NULL),
m_VideoBuffer(NULL),
m_DepthBuffer(NULL)


//m_colorBufferSize(0),
//m_colorBufferPitch(0),
//m_depthBufferSize(0),
//m_depthBufferPitch(0)
{
	//m_hNextDepthFrameEvent = NULL;
	//m_hNextVideoFrameEvent = NULL;
	//m_hNextSkeletonEvent = NULL;
	//m_pDepthStreamHandle = NULL;
	//m_pVideoStreamHandle = NULL;
	//m_hThNuiProcess=NULL;
	//m_hEvNuiProcessStop=NULL;
	//m_bNuiInitialized = false;
	//m_FramesTotal = 0;
	//m_SkeletonTotal = 0;
	//m_VideoBuffer = NULL;
	//m_DepthBuffer = NULL;
	//m_ZoomFactor = 1.0f;
	//m_ViewOffset.x = 0;
	//m_ViewOffset.y = 0;
}

Kinect2Sensor::~Kinect2Sensor()
{
	Release();
}

HRESULT Kinect2Sensor::GetVideoConfiguration(FT_CAMERA_CONFIG* videoConfig) {
	//videoConfig->Width = cColorWidth;
	//videoConfig->Height= cColorHeight;
	videoConfig->Width = cColorWidth1;
	videoConfig->Height= cColorHeight1;
	//videoConfig->FocalLength = 531.15f;//dxtodo calculate based on new resolution
	videoConfig->FocalLength = 1064.36f;
	return S_OK;
}
HRESULT Kinect2Sensor::GetDepthConfiguration(FT_CAMERA_CONFIG* depthConfig) {
	//depthConfig->Width = cDepthWidth;
	//depthConfig->Height = cDepthHeight;
	depthConfig->Width = cDepthWidth1;
	depthConfig->Height = cDepthHeight1;
	//depthConfig->FocalLength = 285.63f;//dxtodo calculate based on new resolution
	depthConfig->FocalLength = 364.38f;
	return S_OK;
}
//HRESULT Kinect2Sensor::GetVideoConfiguration(FT_CAMERA_CONFIG* videoConfig)
//{
//    if (!videoConfig)
//    {
//        return E_POINTER;
//    }
//
//    UINT width = m_VideoBuffer ? m_VideoBuffer->GetWidth() : 0;
//    UINT height =  m_VideoBuffer ? m_VideoBuffer->GetHeight() : 0;
//    FLOAT focalLength = 0.f;
//
//    if(width == 640 && height == 480)
//    {
//        focalLength = NUI_CAMERA_COLOR_NOMINAL_FOCAL_LENGTH_IN_PIXELS;
//    }
//    else if(width == 1280 && height == 960)
//    {
//        focalLength = NUI_CAMERA_COLOR_NOMINAL_FOCAL_LENGTH_IN_PIXELS * 2.f;
//    }
//
//    if(focalLength == 0.f)
//    {
//        return E_UNEXPECTED;
//    }
//
//
//    videoConfig->FocalLength = focalLength;
//    videoConfig->Width = width;
//    videoConfig->Height = height;
//    return(S_OK);
//}
//
//HRESULT Kinect2Sensor::GetDepthConfiguration(FT_CAMERA_CONFIG* depthConfig)
//{
//    if (!depthConfig)
//    {
//        return E_POINTER;
//    }
//
//    UINT width = m_DepthBuffer ? m_DepthBuffer->GetWidth() : 0;
//    UINT height =  m_DepthBuffer ? m_DepthBuffer->GetHeight() : 0;
//    FLOAT focalLength = 0.f;
//
//    if(width == 80 && height == 60)
//    {
//        focalLength = NUI_CAMERA_DEPTH_NOMINAL_FOCAL_LENGTH_IN_PIXELS / 4.f;
//    }
//    else if(width == 320 && height == 240)
//    {
//        focalLength = NUI_CAMERA_DEPTH_NOMINAL_FOCAL_LENGTH_IN_PIXELS;
//    }
//    else if(width == 640 && height == 480)
//    {
//        focalLength = NUI_CAMERA_DEPTH_NOMINAL_FOCAL_LENGTH_IN_PIXELS * 2.f;
//    }
//
//    if(focalLength == 0.f)
//    {
//        return E_UNEXPECTED;
//    }
//
//    depthConfig->FocalLength = focalLength;
//    depthConfig->Width = width;
//    depthConfig->Height = height;
//
//    return S_OK;
//}

HRESULT Kinect2Sensor::Init() {

	HRESULT hr;

	//allocate memories for image buffers for face tracking
	m_VideoBuffer = FTCreateImage();
	if (!m_VideoBuffer)
	{
		return E_OUTOFMEMORY;
	}
	hr = m_VideoBuffer->Allocate(cColorWidth, cColorHeight, FTIMAGEFORMAT_UINT8_B8G8R8X8);
	if (FAILED(hr))
	{
		return hr;
	}

	m_DepthBuffer = FTCreateImage();
	if (!m_DepthBuffer)
	{
		return E_OUTOFMEMORY;
	}
	hr = m_DepthBuffer->Allocate(cDepthWidth, cDepthHeight, FTIMAGEFORMAT_UINT16_D13P3);
	if (FAILED(hr))
	{
		return hr;
	}

	//allocate memories for other image buffers
	// create heap storage for color pixel data in RGBX format
	m_pColorRGBX = new RGBQUAD[cColorWidth * cColorHeight];
	// create heap storage for depth pixel data in UINT16 format
	m_pDepth = new UINT16[cDepthWidth * cDepthHeight];
	// create heap storage for body index data in BYTE format
	m_pBodyIndex = new BYTE[cDepthWidth * cDepthHeight];
	// create heap storage for body data in IBody format
	m_ppBodies = new IBody *[BODY_COUNT];
	for (int i = 0; i < BODY_COUNT; i++) {
		m_ppBodies[i] = NULL;
	}

	
	//open default kinect
	hr = GetDefaultKinectSensor(&m_pKinect2Sensor);
	if (FAILED(hr))
	{
		return hr;
	}

	if (m_pKinect2Sensor)
	{
		IColorFrameSource* pColorFrameSource = NULL;
		IDepthFrameSource* pDepthFrameSource = NULL;
		IBodyFrameSource* pBodyFrameSource = NULL;
		IBodyIndexFrameSource* pBodyIndexFrameSource = NULL;

		//get coordinate mapper
		m_pKinect2Sensor->get_CoordinateMapper(&m_coordinateMapper);

		hr = m_pKinect2Sensor->Open();

		//open color stream
		if (SUCCEEDED(hr))
		{
			hr = m_pKinect2Sensor->get_ColorFrameSource(&pColorFrameSource);
		}
		if (SUCCEEDED(hr))
		{
			hr = pColorFrameSource->OpenReader(&m_pColorFrameReader);
		}
		SafeRelease(pColorFrameSource);
		if (FAILED(hr)) {
			//SetStatusMessage(L"Failed open color stream!", 10000, true);//dxtodo
			return E_FAIL;
		}

		//open depth stream
		if (SUCCEEDED(hr))
		{
			hr = m_pKinect2Sensor->get_DepthFrameSource(&pDepthFrameSource);
		}
		if (SUCCEEDED(hr))
		{
			hr = pDepthFrameSource->OpenReader(&m_pDepthFrameReader);
		}
		SafeRelease(pDepthFrameSource);
		if (FAILED(hr)) {
			//SetStatusMessage(L"Failed open depth stream!", 10000, true);//dxtodo
			return E_FAIL;
		}

		//open coordinate mapper and body stream
		if (SUCCEEDED(hr))
		{
			hr = m_pKinect2Sensor->get_CoordinateMapper(&m_pCoordinateMapper);
		}
		if (FAILED(hr)) {
			//SetStatusMessage(L"Failed open coordinate mapper!", 10000, true);//dxtodo
			return E_FAIL;
		}
		if (SUCCEEDED(hr))
		{
			hr = m_pKinect2Sensor->get_BodyFrameSource(&pBodyFrameSource);
		}
		if (SUCCEEDED(hr))
		{
			hr = pBodyFrameSource->OpenReader(&m_pBodyFrameReader);
		}
		SafeRelease(pBodyFrameSource);
		if (FAILED(hr)) {
			//SetStatusMessage(L"Failed open body stream!", 10000, true);//dxtodo
			return E_FAIL;
		}

		//open body index stream
		if (SUCCEEDED(hr))
		{
			hr = m_pKinect2Sensor->get_BodyIndexFrameSource(&pBodyIndexFrameSource);
		}
		if (SUCCEEDED(hr))
		{
			hr = pBodyIndexFrameSource->OpenReader(&m_pBodyIndexFrameReader);
		}
		SafeRelease(pBodyIndexFrameSource);
		if (FAILED(hr)) {
			//SetStatusMessage(L"Failed open body index stream!", 10000, true);//dxtodo
			return E_FAIL;
		}
	}
	else
	{
		//SetStatusMessage(L"No ready Kinect found!", 10000, true);//dxtodo
		return E_FAIL;
	}

	// Start the Nui processing thread
	m_hEvNuiProcessStop = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hThNuiProcess = CreateThread(NULL, 0, ProcessThread, this, 0, NULL);

	return hr;
}
//HRESULT Kinect2Sensor::Init(NUI_IMAGE_TYPE depthType, NUI_IMAGE_RESOLUTION depthRes, BOOL bNearMode, BOOL bFallbackToDefault, NUI_IMAGE_TYPE colorType, NUI_IMAGE_RESOLUTION colorRes, BOOL bSeatedSkeletonMode)
//{
//    HRESULT hr = E_UNEXPECTED;
//
//    Release(); // Deal with double initializations.
//
//    //do not support NUI_IMAGE_TYPE_COLOR_RAW_YUV for now
//    if(colorType != NUI_IMAGE_TYPE_COLOR && colorType != NUI_IMAGE_TYPE_COLOR_YUV
//        || depthType != NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX && depthType != NUI_IMAGE_TYPE_DEPTH)
//    {
//        return E_INVALIDARG;
//    }
//
//    m_VideoBuffer = FTCreateImage();
//    if (!m_VideoBuffer)
//    {
//        return E_OUTOFMEMORY;
//    }
//
//    DWORD width = 0;
//    DWORD height = 0;
//
//    NuiImageResolutionToSize(colorRes, width, height);
//
//    hr = m_VideoBuffer->Allocate(width, height, FTIMAGEFORMAT_UINT8_B8G8R8X8);
//    if (FAILED(hr))
//    {
//        return hr;
//    }
//
//    m_DepthBuffer = FTCreateImage();
//    if (!m_DepthBuffer)
//    {
//        return E_OUTOFMEMORY;
//    }
//
//    NuiImageResolutionToSize(depthRes, width, height);
//
//    hr = m_DepthBuffer->Allocate(width, height, FTIMAGEFORMAT_UINT16_D13P3);
//    if (FAILED(hr))
//    {
//        return hr;
//    }
//
//    m_FramesTotal = 0;
//    m_SkeletonTotal = 0;
//
//    for (int i = 0; i < NUI_SKELETON_COUNT; ++i)
//    {
//        m_HeadPoint[i] = m_NeckPoint[i] = FT_VECTOR3D(0, 0, 0);
//        m_SkeletonTracked[i] = false;
//    }
//
//    m_hNextDepthFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
//    m_hNextVideoFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
//    m_hNextSkeletonEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
//
//    DWORD dwNuiInitDepthFlag = (depthType == NUI_IMAGE_TYPE_DEPTH)? NUI_INITIALIZE_FLAG_USES_DEPTH : NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX;
//
//    hr = NuiInitialize(dwNuiInitDepthFlag | NUI_INITIALIZE_FLAG_USES_SKELETON | NUI_INITIALIZE_FLAG_USES_COLOR);
//    if (FAILED(hr))
//    {
//        return hr;
//    }
//    m_bNuiInitialized = true;
//
//    DWORD dwSkeletonFlags = NUI_SKELETON_TRACKING_FLAG_ENABLE_IN_NEAR_RANGE;
//    if (bSeatedSkeletonMode)
//    {
//        dwSkeletonFlags |= NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT;
//    }
//    hr = NuiSkeletonTrackingEnable( m_hNextSkeletonEvent, dwSkeletonFlags );
//    if (FAILED(hr))
//    {
//        return hr;
//    }
//
//    hr = NuiImageStreamOpen(
//        colorType,
//        colorRes,
//        0,
//        2,
//        m_hNextVideoFrameEvent,
//        &m_pVideoStreamHandle );
//    if (FAILED(hr))
//    {
//        return hr;
//    }
//
//    hr = NuiImageStreamOpen(
//        depthType,
//        depthRes,
//        (bNearMode)? NUI_IMAGE_STREAM_FLAG_ENABLE_NEAR_MODE : 0,
//        2,
//        m_hNextDepthFrameEvent,
//        &m_pDepthStreamHandle );
//    if (FAILED(hr))
//    {
//        if(bNearMode && bFallbackToDefault)
//        {
//            hr = NuiImageStreamOpen(
//                depthType,
//                depthRes,
//                0,
//                2,
//                m_hNextDepthFrameEvent,
//                &m_pDepthStreamHandle );
//        }
//
//        if(FAILED(hr))
//        {
//            return hr;
//        }
//    }
//
//	m_colorResolution = colorRes;
//	m_depthResolution = depthRes;
//
//    // Start the Nui processing thread
//    m_hEvNuiProcessStop=CreateEvent(NULL,TRUE,FALSE,NULL);
//    m_hThNuiProcess=CreateThread(NULL,0,ProcessThread,this,0,NULL);
//
//    return hr;
//}

void Kinect2Sensor::Release()
{
	// Stop the Nui processing thread
	if (m_hEvNuiProcessStop != NULL)
	{
		// Signal the thread
		SetEvent(m_hEvNuiProcessStop);

		// Wait for thread to stop
		if (m_hThNuiProcess != NULL)
		{
			WaitForSingleObject(m_hThNuiProcess, INFINITE);
			CloseHandle(m_hThNuiProcess);
			m_hThNuiProcess = NULL;
		}
		CloseHandle(m_hEvNuiProcessStop);
		m_hEvNuiProcessStop = NULL;
	}

	//release image buffers for face tracking
	if (m_VideoBuffer)
	{
		m_VideoBuffer->Release();
		m_VideoBuffer = NULL;
	}
	if (m_DepthBuffer)
	{
		m_DepthBuffer->Release();
		m_DepthBuffer = NULL;
	}

	//release other image buffers
	if (m_pColorRGBX)
	{
		delete[] m_pColorRGBX;
		m_pColorRGBX = NULL;
	}
	if (m_pDepth)
	{
		delete [] m_pDepth;
		m_pDepth = NULL;
	}
	if (m_pBodyIndex)
	{
		delete [] m_pBodyIndex;
		m_pBodyIndex = NULL;
	}
	if (m_ppBodies)
	{
		delete [] m_ppBodies;
		m_ppBodies = NULL;
	}
	
	// done with color frame reader
	SafeRelease(m_pColorFrameReader);
	// done with depth frame reader
	SafeRelease(m_pDepthFrameReader);
	// done with coordinate mapper
	SafeRelease(m_pCoordinateMapper);
	// done with body frame reader
	SafeRelease(m_pBodyFrameReader);
	// done with body index frame reader
	SafeRelease(m_pBodyIndexFrameReader);

	// close the Kinect Sensor
	if (m_pKinect2Sensor)
	{
		m_pKinect2Sensor->Close();
	}
	SafeRelease(m_pKinect2Sensor);

	//if (m_bNuiInitialized)
	//{
	//    NuiShutdown();
	//}
	//m_bNuiInitialized = false;

	//if (m_hNextSkeletonEvent && m_hNextSkeletonEvent != INVALID_HANDLE_VALUE)
	//{
	//    CloseHandle(m_hNextSkeletonEvent);
	//    m_hNextSkeletonEvent = NULL;
	//}
	//if (m_hNextDepthFrameEvent && m_hNextDepthFrameEvent != INVALID_HANDLE_VALUE)
	//{
	//    CloseHandle(m_hNextDepthFrameEvent);
	//    m_hNextDepthFrameEvent = NULL;
	//}
	//if (m_hNextVideoFrameEvent && m_hNextVideoFrameEvent != INVALID_HANDLE_VALUE)
	//{
	//    CloseHandle(m_hNextVideoFrameEvent);
	//    m_hNextVideoFrameEvent = NULL;
	//}
	//if (m_VideoBuffer)
	//{
	//    m_VideoBuffer->Release();
	//    m_VideoBuffer = NULL;
	//}
	//if (m_DepthBuffer)
	//{
	//    m_DepthBuffer->Release();
	//    m_DepthBuffer = NULL;
	//}	
}
DWORD WINAPI Kinect2Sensor::ProcessThread(LPVOID pParam) {

	Kinect2Sensor*  pthis = (Kinect2Sensor *) pParam;

	//HANDLE          hEvents[4];//dxtodo

	while (true)
	{
		pthis->Update();
	}
}

/// <summary>
/// Main processing function
/// </summary>
void Kinect2Sensor::Update()
{
	if (!m_pColorFrameReader || !m_pDepthFrameReader || !m_pBodyFrameReader || !m_pBodyIndexFrameReader)
	{
		return;
	}

	//grab color frame
	IColorFrame* pColorFrame = NULL;
	HRESULT hr = m_pColorFrameReader->AcquireLatestFrame(&pColorFrame);
	if (SUCCEEDED(hr))
	{
		INT64 nTime = 0;
		IFrameDescription* pFrameDescription = NULL;
		int nWidth = 0;
		int nHeight = 0;
		ColorImageFormat imageFormat = ColorImageFormat_None;
		UINT nBufferSize = cColorWidth * cColorHeight * sizeof(RGBQUAD);

		hr = pColorFrame->get_RelativeTime(&nTime);
		if (SUCCEEDED(hr))
		{
			hr = pColorFrame->get_FrameDescription(&pFrameDescription);
		}
		if (SUCCEEDED(hr))
		{
			hr = pFrameDescription->get_Width(&nWidth);
		}
		if (SUCCEEDED(hr))
		{
			hr = pFrameDescription->get_Height(&nHeight);
		}

		ASSERT(nWidth == cColorWidth && nHeight == cColorHeight);

		if (SUCCEEDED(hr))
		{
			hr = pColorFrame->get_RawColorImageFormat(&imageFormat);
		}
		if (SUCCEEDED(hr))
		{
			if (imageFormat == ColorImageFormat_Bgra)
			{
				hr = pColorFrame->CopyRawFrameDataToArray(nBufferSize, reinterpret_cast<BYTE*>(m_pColorRGBX));
			}
			else
			{
				hr = pColorFrame->CopyConvertedFrameDataToArray(nBufferSize, reinterpret_cast<BYTE*>(m_pColorRGBX), ColorImageFormat_Bgra);
			}
		}
		if (SUCCEEDED(hr))
		{
			hr = GrabColorFT();
		}
		if (SUCCEEDED(hr))
		{
			hr = GrabColorMat();
		}
		SafeRelease(pFrameDescription);
	}
	SafeRelease(pColorFrame);


	//grab body index frame (this needs to be before grab depth frame so body index is available to GrabDepthArgbMat()
	IBodyIndexFrame* pBodyIndexFrame = NULL;
	hr = m_pBodyIndexFrameReader->AcquireLatestFrame(&pBodyIndexFrame);
	if (SUCCEEDED(hr))
	{
		INT64 nTime = 0;
		IFrameDescription* pFrameDescription = NULL;
		int nWidth = 0;
		int nHeight = 0;
		UINT nBufferSize = cDepthWidth * cDepthHeight;

		hr = pBodyIndexFrame->get_RelativeTime(&nTime);
		if (SUCCEEDED(hr))
		{
			hr = pBodyIndexFrame->get_FrameDescription(&pFrameDescription);
		}
		if (SUCCEEDED(hr))
		{
			hr = pFrameDescription->get_Width(&nWidth);
		}
		if (SUCCEEDED(hr))
		{
			hr = pFrameDescription->get_Height(&nHeight);
		}

		ASSERT(nWidth == cDepthWidth && nHeight == cDepthHeight);

		if (SUCCEEDED(hr))
		{
			hr = pBodyIndexFrame->CopyFrameDataToArray(nBufferSize, m_pBodyIndex);
		}
		SafeRelease(pFrameDescription);
	}
	SafeRelease(pBodyIndexFrame);

	//grab depth frame
	IDepthFrame* pDepthFrame = NULL;
	hr = m_pDepthFrameReader->AcquireLatestFrame(&pDepthFrame);
	if (SUCCEEDED(hr))
	{
		INT64 nTime = 0;
		IFrameDescription* pFrameDescription = NULL;
		int nWidth = 0;
		int nHeight = 0;
		USHORT nDepthMinReliableDistance = 0;
		USHORT nDepthMaxReliableDistance = 0;
		//UINT nBufferSize = cDepthWidth * cDepthHeight * sizeof(UINT16);//don't do this
		UINT nBufferSize = cDepthWidth * cDepthHeight;

		hr = pDepthFrame->get_RelativeTime(&nTime);
		if (SUCCEEDED(hr))
		{
			hr = pDepthFrame->get_FrameDescription(&pFrameDescription);
		}
		if (SUCCEEDED(hr))
		{
			hr = pFrameDescription->get_Width(&nWidth);
		}
		if (SUCCEEDED(hr))
		{
			hr = pFrameDescription->get_Height(&nHeight);
		}

		ASSERT(nWidth == cDepthWidth && nHeight == cDepthHeight);

		if (SUCCEEDED(hr))
		{
			hr = pDepthFrame->get_DepthMinReliableDistance(&nDepthMinReliableDistance);
		}
		if (SUCCEEDED(hr))
		{
			hr = pDepthFrame->get_DepthMaxReliableDistance(&nDepthMaxReliableDistance);
		}
		if (SUCCEEDED(hr))
		{
			hr = pDepthFrame->CopyFrameDataToArray(nBufferSize, m_pDepth);
		}
		if (SUCCEEDED(hr))
		{
			hr = GrabDepthFT();
		}
		if (SUCCEEDED(hr))
		{
			hr = GrabDepthMat();
		}
		if (SUCCEEDED(hr))
		{
			hr = GrabDepthArgbMat();
		}
		SafeRelease(pFrameDescription);
	}
	SafeRelease(pDepthFrame);

	//grab body frame
	IBodyFrame* pBodyFrame = NULL;
	hr = m_pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);
	if (SUCCEEDED(hr))
	{
		INT64 nTime = 0;
		hr = pBodyFrame->get_RelativeTime(&nTime);
		IBody *ppBodies[BODY_COUNT] = { 0 };
		if (SUCCEEDED(hr))
		{
			hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
		}
		if (SUCCEEDED(hr))
		{
			//copy to local
			for (int i = 0; i < _countof(ppBodies); ++i)
			{
				SafeRelease(m_ppBodies[i]);
				m_ppBodies[i] = ppBodies[i];
			}

			ProcessBody(nTime, BODY_COUNT, ppBodies);
		}
		//for (int i = 0; i < _countof(ppBodies); ++i)
		//{
		//	SafeRelease(ppBodies[i]);
		//}
	}
	SafeRelease(pBodyFrame);
}

HRESULT Kinect2Sensor::GrabColorFT() {
	UINT nBufferSize = cColorWidth * cColorHeight * sizeof(RGBQUAD);
	memcpy(m_VideoBuffer->GetBuffer(), reinterpret_cast<BYTE*>(m_pColorRGBX), min(m_VideoBuffer->GetBufferSize(), nBufferSize));
	return S_OK;
}

HRESULT Kinect2Sensor::GrabDepthFT() {
	UINT nBufferSize = cDepthWidth * cDepthHeight * sizeof(UINT16);
	memcpy(m_DepthBuffer->GetBuffer(), reinterpret_cast<BYTE*>(m_pDepth), min(m_DepthBuffer->GetBufferSize(), nBufferSize));
	return S_OK;
}

void Kinect2Sensor::ProcessBody(INT64 nTime, int nBodyCount, IBody** ppBodies) {
	if (m_pCoordinateMapper)
	{
		//RECT rct;
		//GetClientRect(GetDlgItem(m_hWnd, IDC_VIDEOVIEW), &rct);
		//int width = rct.right;
		//int height = rct.bottom;

		for (int i = 0; i < nBodyCount; ++i)
		{
			IBody* pBody = ppBodies[i];
			if (pBody)
			{
				BOOLEAN bTracked = false;
				HRESULT hr = pBody->get_IsTracked(&bTracked);

				if (SUCCEEDED(hr) && bTracked)
				{
					Joint joints[JointType_Count];
					D2D1_POINT_2F jointPoints[JointType_Count];
					HandState leftHandState = HandState_Unknown;
					HandState rightHandState = HandState_Unknown;

					pBody->get_HandLeftState(&leftHandState);
					pBody->get_HandRightState(&rightHandState);

					hr = pBody->GetJoints(_countof(joints), joints);
					if (SUCCEEDED(hr))
					{
						for (int j = 0; j < _countof(joints); ++j)
						{
							//jointPoints[j] = BodyToScreen(joints[j].Position, width, height);
						}

						//DrawBody(joints, jointPoints);

						//DrawHand(leftHandState, jointPoints[JointType_HandLeft]);
						//DrawHand(rightHandState, jointPoints[JointType_HandRight]);
					}
				}
			}
		}
	}
}

/// <summary>
/// Converts a body point to screen space
/// </summary>
/// <param name="bodyPoint">body point to tranform</param>
/// <param name="width">width (in pixels) of output buffer</param>
/// <param name="height">height (in pixels) of output buffer</param>
/// <returns>point in screen-space</returns>
D2D1_POINT_2F Kinect2Sensor::BodyToScreen(const CameraSpacePoint& bodyPoint, int width, int height)
{
	// Calculate the body's position on the screen
	DepthSpacePoint depthPoint = { 0 };
	m_pCoordinateMapper->MapCameraPointToDepthSpace(bodyPoint, &depthPoint);

	float screenPointX = static_cast<float>(depthPoint.X * width) / cDepthWidth;
	float screenPointY = static_cast<float>(depthPoint.Y * height) / cDepthHeight;

	return D2D1::Point2F(screenPointX, screenPointY);
}


/// <summary>
/// Draws a body 
/// </summary>
/// <param name="pJoints">joint data</param>
/// <param name="pJointPoints">joint positions converted to screen space</param>
void Kinect2Sensor::DrawBody(const Joint* pJoints, const D2D1_POINT_2F* pJointPoints)
{
	// Draw the bones

	// Torso
	DrawBone(pJoints, pJointPoints, JointType_Head, JointType_Neck);
	DrawBone(pJoints, pJointPoints, JointType_Neck, JointType_SpineShoulder);
	DrawBone(pJoints, pJointPoints, JointType_SpineShoulder, JointType_SpineMid);
	DrawBone(pJoints, pJointPoints, JointType_SpineMid, JointType_SpineBase);
	DrawBone(pJoints, pJointPoints, JointType_SpineShoulder, JointType_ShoulderRight);
	DrawBone(pJoints, pJointPoints, JointType_SpineShoulder, JointType_ShoulderLeft);
	DrawBone(pJoints, pJointPoints, JointType_SpineBase, JointType_HipRight);
	DrawBone(pJoints, pJointPoints, JointType_SpineBase, JointType_HipLeft);

	// Right Arm    
	DrawBone(pJoints, pJointPoints, JointType_ShoulderRight, JointType_ElbowRight);
	DrawBone(pJoints, pJointPoints, JointType_ElbowRight, JointType_WristRight);
	DrawBone(pJoints, pJointPoints, JointType_WristRight, JointType_HandRight);
	DrawBone(pJoints, pJointPoints, JointType_HandRight, JointType_HandTipRight);
	DrawBone(pJoints, pJointPoints, JointType_WristRight, JointType_ThumbRight);

	// Left Arm
	DrawBone(pJoints, pJointPoints, JointType_ShoulderLeft, JointType_ElbowLeft);
	DrawBone(pJoints, pJointPoints, JointType_ElbowLeft, JointType_WristLeft);
	DrawBone(pJoints, pJointPoints, JointType_WristLeft, JointType_HandLeft);
	DrawBone(pJoints, pJointPoints, JointType_HandLeft, JointType_HandTipLeft);
	DrawBone(pJoints, pJointPoints, JointType_WristLeft, JointType_ThumbLeft);

	// Right Leg
	DrawBone(pJoints, pJointPoints, JointType_HipRight, JointType_KneeRight);
	DrawBone(pJoints, pJointPoints, JointType_KneeRight, JointType_AnkleRight);
	DrawBone(pJoints, pJointPoints, JointType_AnkleRight, JointType_FootRight);

	// Left Leg
	DrawBone(pJoints, pJointPoints, JointType_HipLeft, JointType_KneeLeft);
	DrawBone(pJoints, pJointPoints, JointType_KneeLeft, JointType_AnkleLeft);
	DrawBone(pJoints, pJointPoints, JointType_AnkleLeft, JointType_FootLeft);

	// Draw the joints
	for (int i = 0; i < JointType_Count; ++i)
	{
		//D2D1_ELLIPSE ellipse = D2D1::Ellipse(pJointPoints[i], c_JointThickness, c_JointThickness);

		if (pJoints[i].TrackingState == TrackingState_Inferred)
		{
			//m_pRenderTarget->FillEllipse(ellipse, m_pBrushJointInferred);
		}
		else if (pJoints[i].TrackingState == TrackingState_Tracked)
		{
			//m_pRenderTarget->FillEllipse(ellipse, m_pBrushJointTracked);
		}
	}
}

/// <summary>
/// Draws one bone of a body (joint to joint)
/// </summary>
/// <param name="pJoints">joint data</param>
/// <param name="pJointPoints">joint positions converted to screen space</param>
/// <param name="pJointPoints">joint positions converted to screen space</param>
/// <param name="joint0">one joint of the bone to draw</param>
/// <param name="joint1">other joint of the bone to draw</param>
void Kinect2Sensor::DrawBone(const Joint* pJoints, const D2D1_POINT_2F* pJointPoints, JointType joint0, JointType joint1)
{
	TrackingState joint0State = pJoints[joint0].TrackingState;
	TrackingState joint1State = pJoints[joint1].TrackingState;

	// If we can't find either of these joints, exit
	if ((joint0State == TrackingState_NotTracked) || (joint1State == TrackingState_NotTracked))
	{
		return;
	}

	// Don't draw if both points are inferred
	if ((joint0State == TrackingState_Inferred) && (joint1State == TrackingState_Inferred))
	{
		return;
	}

	// We assume all drawn bones are inferred unless BOTH joints are tracked
	if ((joint0State == TrackingState_Tracked) && (joint1State == TrackingState_Tracked))
	{
		//m_pRenderTarget->DrawLine(pJointPoints[joint0], pJointPoints[joint1], m_pBrushBoneTracked, c_TrackedBoneThickness);
	}
	else
	{
		//m_pRenderTarget->DrawLine(pJointPoints[joint0], pJointPoints[joint1], m_pBrushBoneInferred, c_InferredBoneThickness);
	}
}

/// <summary>
/// Draws a hand symbol if the hand is tracked: red circle = closed, green circle = opened; blue circle = lasso
/// </summary>
/// <param name="handState">state of the hand</param>
/// <param name="handPosition">position of the hand</param>
void Kinect2Sensor::DrawHand(HandState handState, const D2D1_POINT_2F& handPosition)
{
	D2D1_ELLIPSE ellipse = D2D1::Ellipse(handPosition, c_HandSize, c_HandSize);

	switch (handState)
	{
	case HandState_Closed:
		//m_pRenderTarget->FillEllipse(ellipse, m_pBrushHandClosed);
		break;

	case HandState_Open:
		//m_pRenderTarget->FillEllipse(ellipse, m_pBrushHandOpen);
		break;

	case HandState_Lasso:
		//m_pRenderTarget->FillEllipse(ellipse, m_pBrushHandLasso);
		break;
	}
}





//DWORD WINAPI Kinect2Sensor::ProcessThread(LPVOID pParam)
//{
//    Kinect2Sensor*  pthis=(Kinect2Sensor *) pParam;
//    HANDLE          hEvents[4];
//
//    // Configure events to be listened on
//    hEvents[0]=pthis->m_hEvNuiProcessStop;
//    hEvents[1]=pthis->m_hNextDepthFrameEvent;
//    hEvents[2]=pthis->m_hNextVideoFrameEvent;
//    hEvents[3]=pthis->m_hNextSkeletonEvent;
//
//    // Main thread loop
//    while (true)
//    {
//        // Wait for an event to be signaled
//        WaitForMultipleObjects(sizeof(hEvents)/sizeof(hEvents[0]),hEvents,FALSE,100);
//
//        // If the stop event is set, stop looping and exit
//        if (WAIT_OBJECT_0 == WaitForSingleObject(pthis->m_hEvNuiProcessStop, 0))
//        {
//            break;
//        }
//
//        // Process signal events
//        if (WAIT_OBJECT_0 == WaitForSingleObject(pthis->m_hNextDepthFrameEvent, 0))
//        {
//            pthis->GotDepthAlert();
//            pthis->m_FramesTotal++;
//			pthis->m_depthFrameRateTracker.Tick();
//        }
//        if (WAIT_OBJECT_0 == WaitForSingleObject(pthis->m_hNextVideoFrameEvent, 0))
//        {
//            pthis->GotVideoAlert();
//			pthis->m_colorFrameRateTracker.Tick();
//        }
//        if (WAIT_OBJECT_0 == WaitForSingleObject(pthis->m_hNextSkeletonEvent, 0))
//        {
//            pthis->GotSkeletonAlert();
//            pthis->m_SkeletonTotal++;
//			pthis->m_skeletonFrameRateTracker.Tick();
//        }
//    }
//
//    return 0;
//}
//
//void Kinect2Sensor::GotVideoAlert( )
//{
//    const NUI_IMAGE_FRAME* pImageFrame = NULL;
//
//    HRESULT hr = NuiImageStreamGetNextFrame(m_pVideoStreamHandle, 0, &pImageFrame);
//    if (FAILED(hr))
//    {
//        return;
//    }
//
//    INuiFrameTexture* pTexture = pImageFrame->pFrameTexture;
//    NUI_LOCKED_RECT LockedRect;
//    pTexture->LockRect(0, &LockedRect, NULL, 0);
//    if (LockedRect.Pitch)
//    {   // Copy video frame to face tracking
//        memcpy(m_VideoBuffer->GetBuffer(), PBYTE(LockedRect.pBits), min(m_VideoBuffer->GetBufferSize(), UINT(pTexture->BufferLen())));
//		m_colorBufferPitch = LockedRect.Pitch;
//		m_colorBufferSize = LockedRect.size;
//
//		GetColorData(LockedRect);
//    }
//    else
//    {
//        OutputDebugString(L"Buffer length of received texture is bogus\r\n");
//    }
//
//    hr = NuiImageStreamReleaseFrame(m_pVideoStreamHandle, pImageFrame);
//
//}
//
//
//void Kinect2Sensor::GotDepthAlert( )
//{
//    const NUI_IMAGE_FRAME* pImageFrame = NULL;
//
//    HRESULT hr = NuiImageStreamGetNextFrame(m_pDepthStreamHandle, 0, &pImageFrame);
//
//    if (FAILED(hr))
//    {
//        return;
//    }
//
//    INuiFrameTexture* pTexture = pImageFrame->pFrameTexture;
//    NUI_LOCKED_RECT LockedRect;
//    pTexture->LockRect(0, &LockedRect, NULL, 0);
//    if (LockedRect.Pitch)
//    {   // Copy depth frame to face tracking
//		memcpy(m_DepthBuffer->GetBuffer(), PBYTE(LockedRect.pBits), min(m_DepthBuffer->GetBufferSize(), UINT(pTexture->BufferLen())));
//		m_depthBufferPitch = LockedRect.Pitch;
//		m_depthBufferSize = LockedRect.size;
//
//		GetDepthData(LockedRect);
//		GetDepthDataAsArgb();
//    }
//    else
//    {
//        OutputDebugString( L"Buffer length of received depth texture is bogus\r\n" );
//    }
//
//    hr = NuiImageStreamReleaseFrame(m_pDepthStreamHandle, pImageFrame);
//}
//
//void Kinect2Sensor::GotSkeletonAlert()
//{
//    NUI_SKELETON_FRAME SkeletonFrame = {0};
//
//    HRESULT hr = NuiSkeletonGetNextFrame(0, &SkeletonFrame);
//    if(FAILED(hr))
//    {
//        return;
//    }
//
//    for( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ )
//    {
//        if( SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED &&
//            NUI_SKELETON_POSITION_TRACKED == SkeletonFrame.SkeletonData[i].eSkeletonPositionTrackingState[NUI_SKELETON_POSITION_HEAD] &&
//            NUI_SKELETON_POSITION_TRACKED == SkeletonFrame.SkeletonData[i].eSkeletonPositionTrackingState[NUI_SKELETON_POSITION_SHOULDER_CENTER])
//        {
//            m_SkeletonTracked[i] = true;
//            m_HeadPoint[i].x = SkeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HEAD].x;
//            m_HeadPoint[i].y = SkeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HEAD].y;
//            m_HeadPoint[i].z = SkeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HEAD].z;
//            m_NeckPoint[i].x = SkeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_CENTER].x;
//            m_NeckPoint[i].y = SkeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_CENTER].y;
//            m_NeckPoint[i].z = SkeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_CENTER].z;
//        }
//        else
//        {
//            m_HeadPoint[i] = m_NeckPoint[i] = FT_VECTOR3D(0, 0, 0);
//            m_SkeletonTracked[i] = false;
//        }
//    }
//}

HRESULT Kinect2Sensor::GetClosestHint(FT_VECTOR3D* pHint3D)
{
    int selectedSkeleton = -1;
    float smallestDistance = 0;

    if (!pHint3D)
    {
        return(E_POINTER);
    }
    if (pHint3D[1].x == 0 && pHint3D[1].y == 0 && pHint3D[1].z == 0)
    {
        // Get the skeleton closest to the camera
		for (int i = 0; i < BODY_COUNT; i++)
        {
			IBody* pBody = m_ppBodies[i];
			if (pBody)
			{
				BOOLEAN bTracked = false;
				HRESULT hr = pBody->get_IsTracked(&bTracked);

				CameraSpacePoint headPos;
				if (SUCCEEDED(hr)) {
					if (bTracked)
					{
						Joint joints[JointType_Count];
						hr = pBody->GetJoints(_countof(joints), joints);
						headPos = joints[JointType_Head].Position;
					}
					else
					{
						hr = E_FAIL;
					}
				}

				if (SUCCEEDED(hr) && (smallestDistance == 0 || headPos.Z < smallestDistance))
				{
					smallestDistance = headPos.Z;
					selectedSkeleton = i;
				}
			}
        }
    }
    else
    {   // Get the skeleton closest to the previous position
		for (int i = 0; i < BODY_COUNT; i++)
		{
			IBody* pBody = m_ppBodies[i];
			if (pBody)
			{
				BOOLEAN bTracked = false;
				HRESULT hr = pBody->get_IsTracked(&bTracked);

				CameraSpacePoint headPos;
				if (SUCCEEDED(hr)) {
					Joint joints[JointType_Count];
					hr = pBody->GetJoints(_countof(joints), joints);
					headPos = joints[JointType_Head].Position;
				}

				if (SUCCEEDED(hr) && bTracked)
				{
					float d = abs(headPos.X - pHint3D[1].x) +
						abs(headPos.Y - pHint3D[1].y) +
						abs(headPos.Z - pHint3D[1].z);
					if (smallestDistance == 0 || d < smallestDistance)
					{
						smallestDistance = d;
						selectedSkeleton = i;
					}
				}
			}
        }
    }
    if (selectedSkeleton == -1)
    {
        return E_FAIL;
    }

	IBody* pBody = m_ppBodies[selectedSkeleton];
	if (pBody)
	{
		CameraSpacePoint headPos, neckPos;
		Joint joints[JointType_Count];
		HRESULT hr = pBody->GetJoints(_countof(joints), joints);
		headPos = joints[JointType_Head].Position;
		neckPos = joints[JointType_Neck].Position;

		pHint3D[0].x = neckPos.X;
		pHint3D[0].y = neckPos.Y;
		pHint3D[0].z = neckPos.Z;

		pHint3D[1].x = headPos.X;
		pHint3D[1].y = headPos.Y;
		pHint3D[1].z = headPos.Z;

		return hr;
	}
	else
	{
		return E_FAIL;
	}
}

