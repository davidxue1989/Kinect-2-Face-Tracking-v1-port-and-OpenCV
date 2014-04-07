//------------------------------------------------------------------------------
// <copyright file="FTHelper.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once
#include <FaceTrackLib.h>
#include "Kinect2Sensor.h"
#include "Kinect2SensorWithOpenCV.h"
#include "KinectSensor.h"

typedef void (*FTHelperCallBack)(PVOID lpParam);

class FTHelper
{
public:
    FTHelper();
    ~FTHelper();

    HRESULT Init(HWND hWnd, FTHelperCallBack callBack, PVOID callBackParam);
    HRESULT Stop();
    IFTResult* GetResult()      { return(m_pFTResult);}
    BOOL IsKinectPresent()      { return(m_Kinect2SensorPresent);}
    IFTImage* GetColorImage()   { return(m_colorImage);}
    float GetXCenterFace()      { return(m_XCenterFace);}
    float GetYCenterFace()      { return(m_YCenterFace);}
    void SetDrawMask(BOOL drawMask) { m_DrawMask = drawMask;}
    BOOL GetDrawMask()          { return(m_DrawMask);}
    IFTFaceTracker* GetTracker() { return(m_pFaceTracker);}
    HRESULT GetCameraConfig(FT_CAMERA_CONFIG* cameraConfig);

    
	Kinect2SensorWithOpenCV		m_Kinect2SensorWithOpenCV;
	KinectSensor				m_KinectSensor;


public:
	BOOL                        m_Kinect2SensorPresent;
    IFTFaceTracker*             m_pFaceTracker;
    HWND                        m_hWnd;
    IFTResult*                  m_pFTResult;
    IFTImage*                   m_colorImage;
    IFTImage*                   m_depthImage;
    FT_VECTOR3D                 m_hint3D[2];
    bool                        m_LastTrackSucceeded;
    bool                        m_ApplicationIsRunning;
    FTHelperCallBack            m_CallBack;
    LPVOID                      m_CallBackParam;
    float                       m_XCenterFace;
    float                       m_YCenterFace;
    HANDLE                      m_hFaceTrackingThread;
    BOOL                        m_DrawMask;

    BOOL SubmitFraceTrackingResult(IFTResult* pResult);
    void SetCenterOfImage(IFTResult* pResult);
    void CheckCameraInput();
    DWORD WINAPI FaceTrackingThread();
    static DWORD WINAPI FaceTrackingStaticThread(PVOID lpParam);

	void copyBuffer(IFTImage *src, IFTImage *dst, bool isColorNotDepth);

	//to be supplied to the face tracker for mapping depth to color
	static HRESULT __stdcall DepthToColorMapper(UINT depthFrameWidth, UINT depthFrameHeight, UINT colorFrameWidth, UINT colorFrameHeight,
		FLOAT zoomFactor, POINT viewOffset, LONG depthX, LONG depthY, USHORT depthZ, LONG* pColorX, LONG* pColorY);

};
