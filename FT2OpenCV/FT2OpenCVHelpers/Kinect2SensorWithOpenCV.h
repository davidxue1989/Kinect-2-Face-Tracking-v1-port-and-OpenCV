
#pragma once
#include "Kinect2Sensor.h"

//#include <FaceTrackLib.h>

#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>

using namespace cv;
class Kinect2SensorWithOpenCV : public Kinect2Sensor {
public:
	// Functions:
	/// <summary>
	/// Constructor
	/// </summary>
	Kinect2SensorWithOpenCV() : Kinect2Sensor() {
		m_hColorMatMutex = CreateMutex(NULL, FALSE, NULL);
		m_hDepthMatMutex = CreateMutex(NULL, FALSE, NULL);
		m_hDepthAsArgbMatMutex = CreateMutex(NULL, FALSE, NULL);
		//m_pFaceModel2DPoint = NULL;
		//m_faceModelOutDated = true;
	}

	/// <summary>
	/// Destructor
	/// </summary>
	~Kinect2SensorWithOpenCV() {
		if (m_hColorMatMutex)
			CloseHandle(m_hColorMatMutex);
		if (m_hDepthMatMutex)
			CloseHandle(m_hDepthMatMutex);
		if (m_hDepthAsArgbMatMutex)
			CloseHandle(m_hDepthAsArgbMatMutex);

		//if (m_pFaceModel2DPoint)
		//	_freea(m_pFaceModel2DPoint);
	}

	// Constants
	// Mat type for each usage
	static const int COLOR_TYPE = CV_8UC4;
	static const int DEPTH_TYPE = CV_16U;
	static const int DEPTH_RGB_TYPE = CV_8UC4;

	Mat m_colorImgMat;
	Mat m_depthImgMat;
	Mat m_depthAsArgbMat;
	// Mutexes that control access to m_colorImgMat and m_depthImgMat
	HANDLE m_hColorMatMutex;
	HANDLE m_hDepthMatMutex;
	HANDLE m_hDepthAsArgbMatMutex;

public:

	virtual HRESULT GrabColorMat();
	virtual HRESULT GrabDepthMat();
	virtual HRESULT GrabDepthArgbMat();
	static void ConvertColorMatFromFT(IFTImage *src, Mat *dst, bool display);
	static void ConvertDepthMatFromFT(IFTImage *src, Mat *dst, bool display);

	 //<summary>
	 //Convert a 13-bit depth value into a set of RGB values
	 //</summary>
	 //<param name="depth">depth value to convert</param>
	 //<param name="pRedPixel">value of red pixel</param>
	 //<param name="pGreenPixel">value of green pixel</param>
	 //<param name="pBluePixel">value of blue pixel</param>
	 //<returns>S_OK if successful, an error code otherwise</returns>
	HRESULT DepthShortToRgb(USHORT depth, UINT8* redPixel, UINT8* greenPixel, UINT8* bluePixel, BYTE bodyIndx);
};