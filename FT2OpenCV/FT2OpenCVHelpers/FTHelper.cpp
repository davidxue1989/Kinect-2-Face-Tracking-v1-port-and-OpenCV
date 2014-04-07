//------------------------------------------------------------------------------
// <copyright file="FTHelper.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "StdAfx.h"
#include "FTHelper.h"
#include "Visualize.h"

#include <opencv2/highgui/highgui.hpp>

#ifdef SAMPLE_OPTIONS
#include "Options.h"
#else
PVOID _opt = NULL;
#endif

Kinect2Sensor* g_Kinect2Sensor;

FTHelper::FTHelper()
{
    m_pFaceTracker = NULL;
    m_hWnd = NULL;
    m_pFTResult = NULL;
    m_colorImage = NULL;
    m_depthImage = NULL;
    m_ApplicationIsRunning = false;
    m_LastTrackSucceeded = false;
    m_CallBack = NULL;
    m_XCenterFace = 0;
    m_YCenterFace = 0;
    m_hFaceTrackingThread = NULL;
    m_DrawMask = TRUE;
	g_Kinect2Sensor = &m_Kinect2SensorWithOpenCV;
}

FTHelper::~FTHelper()
{
    Stop();
}

HRESULT FTHelper::Init(HWND hWnd, FTHelperCallBack callBack, PVOID callBackParam)
{
    if (!hWnd || !callBack)
    {
        return E_INVALIDARG;
    }
    m_hWnd = hWnd;
    m_CallBack = callBack;
    m_CallBackParam = callBackParam;
    m_ApplicationIsRunning = true;
	
	m_hFaceTrackingThread = CreateThread(NULL, 0, FaceTrackingStaticThread, (PVOID)this, 0, 0);
    return S_OK;
}

HRESULT FTHelper::Stop()
{
    m_ApplicationIsRunning = false;
    if (m_hFaceTrackingThread)
    {
        WaitForSingleObject(m_hFaceTrackingThread, 1000);
    }
    m_hFaceTrackingThread = 0;
    return S_OK;
}

BOOL FTHelper::SubmitFraceTrackingResult(IFTResult* pResult)
{
    if (pResult != NULL && SUCCEEDED(pResult->GetStatus()))
    {
        if (m_DrawMask)
        {
            FLOAT* pSU = NULL;
            UINT numSU;
            BOOL suConverged;
            m_pFaceTracker->GetShapeUnits(NULL, &pSU, &numSU, &suConverged);
            POINT viewOffset = {0, 0};
            FT_CAMERA_CONFIG cameraConfig;//dxtodo
            if (m_Kinect2SensorPresent)
            {
                g_Kinect2Sensor->GetVideoConfiguration(&cameraConfig);
            }
            else
            {
                cameraConfig.Width = 640;
                cameraConfig.Height = 480;
                cameraConfig.FocalLength = 500.0f;
            }
            IFTModel* ftModel;
            HRESULT hr = m_pFaceTracker->GetFaceModel(&ftModel);
            if (SUCCEEDED(hr))
            {
				hr = VisualizeFaceModel(m_colorImage, ftModel, &cameraConfig, pSU, 1.0, viewOffset, pResult, 0x00FFFF00);
				VisualizeFacetracker(m_colorImage, pResult, 0x2BFF00);
                ftModel->Release();
            }
		}

		if (m_CallBack)
		{
			(*m_CallBack)(m_CallBackParam);
		}
    }
    return TRUE;
}

// We compute here the nominal "center of attention" that is used when zooming the presented image.
void FTHelper::SetCenterOfImage(IFTResult* pResult)
{ //dxnote: this function isn't used
    float centerX = ((float)m_colorImage->GetWidth())/2.0f;
    float centerY = ((float)m_colorImage->GetHeight())/2.0f;
    if (pResult)
    {
        if (SUCCEEDED(pResult->GetStatus()))
        {
            RECT faceRect;
            pResult->GetFaceRect(&faceRect);
            centerX = (faceRect.left+faceRect.right)/2.0f;
            centerY = (faceRect.top+faceRect.bottom)/2.0f;
        }
        m_XCenterFace += 0.02f*(centerX-m_XCenterFace);
        m_YCenterFace += 0.02f*(centerY-m_YCenterFace);
    }
    else
    {
        m_XCenterFace = centerX;
        m_YCenterFace = centerY;
    }
}

// Get a video image and process it.
void FTHelper::CheckCameraInput()
{
    HRESULT hrFT = E_FAIL;

    if (m_Kinect2SensorPresent && g_Kinect2Sensor->GetVideoBuffer())
    {
		RECT roi;
		roi.bottom = m_colorImage->GetHeight();
		roi.left = 0;
		roi.right = m_colorImage->GetWidth();
		roi.top = 0;
        //HRESULT hrCopy = g_Kinect2Sensor->GetVideoBuffer()->CopyTo(m_colorImage, &roi, 0, 0);
		//HRESULT hrCopy = g_Kinect2Sensor->GetVideoBuffer()->CopyTo(m_colorImage, NULL, 0, 0);

		HRESULT hrCopy = S_OK;
		copyBuffer(g_Kinect2Sensor->GetVideoBuffer(), m_colorImage, true);

        if (SUCCEEDED(hrCopy) && g_Kinect2Sensor->GetDepthBuffer())
		{
			//RECT roi1;
			//roi1.bottom = m_depthImage->GetHeight();
			//roi1.left = 0;
			//roi1.right = m_depthImage->GetWidth();
			//roi1.top = 0;
			//hrCopy = g_Kinect2Sensor->GetDepthBuffer()->CopyTo(m_depthImage, &roi1, 0, 0);
			//hrCopy = g_Kinect2Sensor->GetDepthBuffer()->CopyTo(m_depthImage, NULL, 0, 0);
			copyBuffer(g_Kinect2Sensor->GetDepthBuffer(), m_depthImage, false);
        }

        // Do face tracking
        if (SUCCEEDED(hrCopy))
        {
            FT_SENSOR_DATA sensorData(m_colorImage, m_depthImage, g_Kinect2Sensor->GetZoomFactor(), g_Kinect2Sensor->GetViewOffSet());

			UINT w = m_colorImage->GetWidth();
			UINT h = m_colorImage->GetHeight();
			UINT w1 = m_depthImage->GetWidth();
			UINT h1 = m_depthImage->GetHeight();

            FT_VECTOR3D* hint = NULL;
            if (SUCCEEDED(g_Kinect2Sensor->GetClosestHint(m_hint3D)))
            {
                hint = m_hint3D;
            }
            if (m_LastTrackSucceeded)
            {
                hrFT = m_pFaceTracker->ContinueTracking(&sensorData, hint, m_pFTResult);
            }
            else
            {
                hrFT = m_pFaceTracker->StartTracking(&sensorData, NULL, hint, m_pFTResult);
            }
        }
    }

	//dxdebug
	HRESULT a1 = FT_ERROR_UNINITIALIZED, a2 = E_INVALIDARG, a3 = E_POINTER;
	HRESULT b0 = m_pFTResult->GetStatus(), b1 = FT_ERROR_FACE_DETECTOR_FAILED, b2 = FT_ERROR_AAM_FAILED, b3 = FT_ERROR_NN_FAILED, b4 = FT_ERROR_EVAL_FAILED;
	bool a = SUCCEEDED(hrFT);
	bool b = SUCCEEDED(m_pFTResult->GetStatus());

    m_LastTrackSucceeded = SUCCEEDED(hrFT) && SUCCEEDED(m_pFTResult->GetStatus());
    if (m_LastTrackSucceeded)
    {
		SubmitFraceTrackingResult(m_pFTResult);
    }
    else
    {
        m_pFTResult->Reset();
    }
    //SetCenterOfImage(m_pFTResult);
	
	Mat test;
	Kinect2SensorWithOpenCV::ConvertColorMatFromFT(m_colorImage, &test, true);
	Mat test2;
	Kinect2SensorWithOpenCV::ConvertDepthMatFromFT(m_depthImage, &test2, true);
}

void FTHelper::copyBuffer(IFTImage *src, IFTImage *dst, bool isColorNotDepth) {
	BYTE *pSBuff = src->GetBuffer();
	UINT SBuffSz = src->GetBufferSize();
	BYTE *pDBuff = dst->GetBuffer();
	UINT DBuffSz = dst->GetBufferSize();

	UINT Dw = dst->GetWidth();
	UINT Dh = dst->GetHeight();
	UINT Sw = src->GetWidth();
	UINT Sh = src->GetHeight();
	int H = min(Sh, Dh);
	int W = min(Sw, Dw);
	//int H, W, Dw, Dh, Sw, Sh;
	//if (isColorNotDepth) {
	//	W = 1280;
	//	H = 960;
	//	Dw = 1280;
	//	Dh = 960;
	//	Sw = 1920;
	//	Sh = 1080;
	//}
	//else {
	//	W = 512;
	//	H = 424;
	//	Dw = 640;
	//	Dh = 480;
	//	Sw = 512;
	//	Sh = 424;
	//}

	for (int i = 0; i < H; i++)
	{
		if (isColorNotDepth)
			for (int j = 0; j < W * 4; j++)
				pDBuff[i * Dw * 4 + j] = pSBuff[i * Sw * 4 + j];
		else
			for (int j = 0; j < W * 2; j++)
				pDBuff[i * Dw * 2 + j] = pSBuff[i * Sw * 2 + j];
	}
}

DWORD WINAPI FTHelper::FaceTrackingStaticThread(PVOID lpParam)
{
    FTHelper* context = static_cast<FTHelper*>(lpParam);
    if (context)
    {
        return context->FaceTrackingThread();
    }
    return 0;
}

DWORD WINAPI FTHelper::FaceTrackingThread()
{
    FT_CAMERA_CONFIG videoConfig;
    FT_CAMERA_CONFIG depthConfig;
    FT_CAMERA_CONFIG* pDepthConfig = NULL;

	// need to open v1.0 camera for face track library to work (just open it, don't need to do anything else)
	HRESULT hr = m_KinectSensor.Init(NUI_IMAGE_TYPE_DEPTH, NUI_IMAGE_RESOLUTION_320x240, FALSE, FALSE, NUI_IMAGE_TYPE_COLOR, NUI_IMAGE_RESOLUTION_640x480, true);
	if (!SUCCEEDED(hr))
	{
		WCHAR errorText[MAX_PATH];
		ZeroMemory(errorText, sizeof(WCHAR) * MAX_PATH);
		wsprintf(errorText, L"Could not initialize the Kinect sensor. hr=0x%x\n", hr);
		MessageBoxW(m_hWnd, errorText, L"Face Tracker Initialization Error\n", MB_OK);
		return 1;
	}

    // Try to get the Kinect camera to work
    hr = g_Kinect2Sensor->Init();
    if (SUCCEEDED(hr))
    {
        m_Kinect2SensorPresent = TRUE;
        g_Kinect2Sensor->GetVideoConfiguration(&videoConfig);
        g_Kinect2Sensor->GetDepthConfiguration(&depthConfig);
        pDepthConfig = &depthConfig;
        m_hint3D[0] = m_hint3D[1] = FT_VECTOR3D(0, 0, 0);
    }
    else
    {
        m_Kinect2SensorPresent = FALSE;
        WCHAR errorText[MAX_PATH];
        ZeroMemory(errorText, sizeof(WCHAR) * MAX_PATH);
        wsprintf(errorText, L"Could not initialize the Kinect sensor. hr=0x%x\n", hr);
        MessageBoxW(m_hWnd, errorText, L"Face Tracker Initialization Error\n", MB_OK);
        return 1;
    }

    // Try to start the face tracker.
    m_pFaceTracker = FTCreateFaceTracker(_opt);
    if (!m_pFaceTracker)
    {
        MessageBoxW(m_hWnd, L"Could not create the face tracker.\n", L"Face Tracker Initialization Error\n", MB_OK);
        return 2;
    }

	hr = m_pFaceTracker->Initialize(&videoConfig, pDepthConfig, DepthToColorMapper, NULL);
    if (FAILED(hr))
    {
        WCHAR path[512], buffer[1024];
        GetCurrentDirectoryW(ARRAYSIZE(path), path);
        wsprintf(buffer, L"Could not initialize face tracker (%s). hr=0x%x", path, hr);

        MessageBoxW(m_hWnd, /*L"Could not initialize the face tracker.\n"*/ buffer, L"Face Tracker Initialization Error\n", MB_OK);

        return 3;
    }

    hr = m_pFaceTracker->CreateFTResult(&m_pFTResult);
    if (FAILED(hr) || !m_pFTResult)
    {
        MessageBoxW(m_hWnd, L"Could not initialize the face tracker result.\n", L"Face Tracker Initialization Error\n", MB_OK);
        return 4;
    }

    // Initialize the RGB image.
    m_colorImage = FTCreateImage();
    if (!m_colorImage || FAILED(hr = m_colorImage->Allocate(videoConfig.Width, videoConfig.Height, FTIMAGEFORMAT_UINT8_B8G8R8X8)))
	//if (!m_colorImage || FAILED(hr = m_colorImage->Allocate(m_Kinect2Sensor->cColorWidth, m_Kinect2Sensor->cColorHeight, FTIMAGEFORMAT_UINT8_B8G8R8X8)))
    {
        return 5;
    }
	for (int i = 0; i < m_colorImage->GetBufferSize(); i++)
		m_colorImage->GetBuffer()[i] = 0;

    if (pDepthConfig)
    {
        m_depthImage = FTCreateImage();
        if (!m_depthImage || FAILED(hr = m_depthImage->Allocate(depthConfig.Width, depthConfig.Height, FTIMAGEFORMAT_UINT16_D13P3)))
		//if (!m_depthImage || FAILED(hr = m_depthImage->Allocate(m_Kinect2Sensor->cDepthWidth, m_Kinect2Sensor->cDepthHeight, FTIMAGEFORMAT_UINT16_D13P3)))
        {
            return 6;
		}
		for (int i = 0; i < m_depthImage->GetBufferSize(); i++)
			m_depthImage->GetBuffer()[i] = 0;
    }

    SetCenterOfImage(NULL);
    m_LastTrackSucceeded = false;

    while (m_ApplicationIsRunning)
    {
        CheckCameraInput();
        //InvalidateRect(m_hWnd, NULL, FALSE);
        //UpdateWindow(m_hWnd);
        //Sleep(16);
		Sleep(67); //15 fps
    }

    m_pFaceTracker->Release();
    m_pFaceTracker = NULL;

    if(m_colorImage)
    {
        m_colorImage->Release();
        m_colorImage = NULL;
    }

    if(m_depthImage) 
    {
        m_depthImage->Release();
        m_depthImage = NULL;
    }

    if(m_pFTResult)
    {
        m_pFTResult->Release();
        m_pFTResult = NULL;
    }
    m_Kinect2SensorWithOpenCV.Release();
	g_Kinect2Sensor = NULL;
    return 0;
}

HRESULT FTHelper::GetCameraConfig(FT_CAMERA_CONFIG* cameraConfig)
{
    return m_Kinect2SensorPresent ? g_Kinect2Sensor->GetVideoConfiguration(cameraConfig) : E_FAIL;
}

HRESULT FTHelper::DepthToColorMapper(UINT depthFrameWidth, UINT depthFrameHeight, UINT colorFrameWidth, UINT colorFrameHeight,
	FLOAT zoomFactor, POINT viewOffset, LONG depthX, LONG depthY, USHORT depthZ, LONG* pColorX, LONG* pColorY) {

	if (depthX == 0)
		int a = 0;
	if (depthX == depthFrameWidth)
		int a = 0;

	DepthSpacePoint depthPoint;
	depthPoint.X = depthX;
	depthPoint.Y = depthY;
	ColorSpacePoint colorPoint;

	HRESULT hr = g_Kinect2Sensor->m_coordinateMapper->MapDepthPointToColorSpace(DepthSpacePoint(depthPoint), depthZ, &colorPoint);

	if (SUCCEEDED(hr) && colorPoint.X >= 0 && colorPoint.X <= colorFrameWidth &&colorPoint.Y >= 0 && colorPoint.Y <= colorFrameHeight)
	{
		*pColorX = colorPoint.X;
		*pColorY = colorPoint.Y;
		return S_OK;
	}
	else
	{
		*pColorX = -1;
		*pColorY = -1;
		return E_FAIL;
	}
}

