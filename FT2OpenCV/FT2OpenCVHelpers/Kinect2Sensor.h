//------------------------------------------------------------------------------
// <copyright file="Kinect2Sensor.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

//#include <NuiApi.h>
// Kinect Header files
#include <Kinect.h>
#include <FaceTrackLib.h>
//#include "FrameRateTracker.h"

class Kinect2Sensor
{
public:
	//kinect 2.0 resolutions:
	static const int        cColorWidth = 1920;
	static const int        cColorHeight = 1080;
	static const int        cDepthWidth = 512;
	static const int        cDepthHeight = 424;
	//kinect 1.0 resolutions:
	static const int cColorWidth1 = 1280;
	static const int cColorHeight1 = 960;
	static const int cDepthWidth1 = 640;
	static const int cDepthHeight1 = 480;
	//offsets to center the cropped image when putting Kinect 2 image into Kinect 1 resolutions
	static const int v1to2offsetColorWidth = (cColorWidth - cColorWidth1) / 2;
	static const int v1to2offsetColorHeight = (cColorHeight - cColorHeight1) / 2;

    Kinect2Sensor();
    ~Kinect2Sensor();
	void Release();
	HRESULT Init();
	void Update();

	//interfaces for OpenCV intergration
	virtual HRESULT GrabColorMat() { return S_OK; };
	virtual HRESULT GrabDepthMat() { return S_OK; };
	virtual HRESULT GrabDepthArgbMat() { return S_OK; };

	HRESULT GrabColorFT();
	HRESULT GrabDepthFT();
	IFTImage*   GetVideoBuffer(){ return(m_VideoBuffer); };
	IFTImage*   GetDepthBuffer(){ return(m_DepthBuffer); };
	float       GetZoomFactor() { return(1.0f); };
	POINT*      GetViewOffSet() { m_ViewOffset.x = 0; m_ViewOffset.y = 0; return (&m_ViewOffset); };
	POINT m_ViewOffset;
	HRESULT     GetVideoConfiguration(FT_CAMERA_CONFIG* videoConfig);
	HRESULT     GetDepthConfiguration(FT_CAMERA_CONFIG* depthConfig);

	RGBQUAD*                m_pColorRGBX;
	UINT16*					m_pDepth;
	BYTE*					m_pBodyIndex;
	IBody**					m_ppBodies;

	IFTImage*   m_VideoBuffer;
	IFTImage*   m_DepthBuffer;


	//helpers:
	void ProcessBody(INT64 nTime, int nBodyCount, IBody** ppBodies);
	/// <summary>
	/// Converts a body point to screen space
	/// </summary>
	/// <param name="bodyPoint">body point to tranform</param>
	/// <param name="width">width (in pixels) of output buffer</param>
	/// <param name="height">height (in pixels) of output buffer</param>
	/// <returns>point in screen-space</returns>
	D2D1_POINT_2F BodyToScreen(const CameraSpacePoint& bodyPoint, int width, int height);
	/// <summary>
	/// Draws a body 
	/// </summary>
	/// <param name="pJoints">joint data</param>
	/// <param name="pJointPoints">joint positions converted to screen space</param>
	void                    DrawBody(const Joint* pJoints, const D2D1_POINT_2F* pJointPoints);
	/// <summary>
	/// Draws a hand symbol if the hand is tracked: red circle = closed, green circle = opened; blue circle = lasso
	/// </summary>
	/// <param name="handState">state of the hand</param>
	/// <param name="handPosition">position of the hand</param>
	void                    DrawHand(HandState handState, const D2D1_POINT_2F& handPosition);
	/// <summary>
	/// Draws one bone of a body (joint to joint)
	/// </summary>
	/// <param name="pJoints">joint data</param>
	/// <param name="pJointPoints">joint positions converted to screen space</param>
	/// <param name="pJointPoints">joint positions converted to screen space</param>
	/// <param name="joint0">one joint of the bone to draw</param>
	/// <param name="joint1">other joint of the bone to draw</param>
	void                    DrawBone(const Joint* pJoints, const D2D1_POINT_2F* pJointPoints, JointType joint0, JointType joint1);

	HRESULT     GetClosestHint(FT_VECTOR3D* pHint3D);


	IKinectSensor*          m_pKinect2Sensor;// Current Kinect
	IColorFrameReader*      m_pColorFrameReader;// Color reader
	IDepthFrameReader*      m_pDepthFrameReader;// Depth reader
	ICoordinateMapper*      m_pCoordinateMapper;// Body reader
	IBodyFrameReader*       m_pBodyFrameReader;// Body reader
	IBodyIndexFrameReader*  m_pBodyIndexFrameReader;// Body index reader
	ICoordinateMapper *m_coordinateMapper;

	static DWORD WINAPI ProcessThread(PVOID pParam);
	HANDLE      m_hThNuiProcess;
	HANDLE      m_hEvNuiProcessStop;
};


// Safe release for interfaces
template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
	if (pInterfaceToRelease != NULL)
	{
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}