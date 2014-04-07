//------------------------------------------------------------------------------
// <copyright file="KinectSensor.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include <FaceTrackLib.h>
#include <NuiApi.h>
//#include "FrameRateTracker.h"

class KinectSensor
{
public:
	static const int        cColorWidth = 1920; //1920x1080 = 16:9;  1440x1080 = 4:3
	static const int        cColorHeight = 1080;
	static const int        cDepthWidth = 512; //512x424 = 64:53;  512x384 = 4:3
	static const int        cDepthHeight = 424;
    KinectSensor();
    ~KinectSensor();

    HRESULT Init(NUI_IMAGE_TYPE depthType, NUI_IMAGE_RESOLUTION depthRes, BOOL bNearMode, BOOL bFallbackToDefault, NUI_IMAGE_TYPE colorType, NUI_IMAGE_RESOLUTION colorRes, BOOL bSeatedSkeletonMode);
    void Release();

    HRESULT     GetVideoConfiguration(FT_CAMERA_CONFIG* videoConfig);
    HRESULT     GetDepthConfiguration(FT_CAMERA_CONFIG* depthConfig);

    IFTImage*   GetVideoBuffer(){ return(m_VideoBuffer); };
    IFTImage*   GetDepthBuffer(){ return(m_DepthBuffer); };
    float       GetZoomFactor() { return(m_ZoomFactor); };
    POINT*      GetViewOffSet() { return(&m_ViewOffset); };
    HRESULT     GetClosestHint(FT_VECTOR3D* pHint3D);

    bool        IsTracked(UINT skeletonId) { return(m_SkeletonTracked[skeletonId]);};
    FT_VECTOR3D NeckPoint(UINT skeletonId) { return(m_NeckPoint[skeletonId]);};
    FT_VECTOR3D HeadPoint(UINT skeletonId) { return(m_HeadPoint[skeletonId]);};

public:
	// Image stream data
	IFTImage*   m_VideoBuffer;
	IFTImage*   m_DepthBuffer;
	INT m_colorBufferSize;
	INT m_colorBufferPitch;
	INT m_depthBufferSize;
	INT m_depthBufferPitch;
	// Image stream resolution information
	NUI_IMAGE_RESOLUTION m_colorResolution;
	NUI_IMAGE_RESOLUTION m_depthResolution;

	//FrameRateTracker m_colorFrameRateTracker;
	//FrameRateTracker m_depthFrameRateTracker;
	//FrameRateTracker m_skeletonFrameRateTracker;

    FT_VECTOR3D m_NeckPoint[NUI_SKELETON_COUNT];
    FT_VECTOR3D m_HeadPoint[NUI_SKELETON_COUNT];
    bool        m_SkeletonTracked[NUI_SKELETON_COUNT];
    FLOAT       m_ZoomFactor;   // video frame zoom factor (it is 1.0f if there is no zoom)
    POINT       m_ViewOffset;   // Offset of the view from the top left corner.

    HANDLE      m_hNextDepthFrameEvent;
    HANDLE      m_hNextVideoFrameEvent;
    HANDLE      m_hNextSkeletonEvent;
    HANDLE      m_pDepthStreamHandle;
    HANDLE      m_pVideoStreamHandle;
    HANDLE      m_hThNuiProcess;
    HANDLE      m_hEvNuiProcessStop;

    bool        m_bNuiInitialized; 
    int         m_FramesTotal;
    int         m_SkeletonTotal;
    
    static DWORD WINAPI ProcessThread(PVOID pParam);
    void GotVideoAlert();
    void GotDepthAlert();
    void GotSkeletonAlert();


	virtual HRESULT GetColorData(NUI_LOCKED_RECT lockedRect) { return S_OK;  };
	virtual HRESULT GetDepthData(NUI_LOCKED_RECT lockedRect) { return S_OK; };
	virtual HRESULT GetDepthDataAsArgb() { return S_OK; };
};
