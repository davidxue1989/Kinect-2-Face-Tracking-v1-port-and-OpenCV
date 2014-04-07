
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



	//virtual HRESULT GetColorData(NUI_LOCKED_RECT lockedRect);
	//virtual HRESULT GetDepthData(NUI_LOCKED_RECT lockedRect);
	//virtual HRESULT GetDepthDataAsArgb();

	//void drawTrackedPoints(Mat *pImg, IFTResult* pAAMRlt);
	//POINT* m_pFaceModel2DPoint;
	//bool m_faceModelOutDated;

	/// <summary>
	/// Verify image is of the given resolution
	/// </summary>
	/// <param name="pImage">pointer to image to verify</param>
	/// <param name="resolution">resolution of image</param>
	/// <returns>S_OK if image matches given width and height, an error code otherwise</returns>
	//HRESULT VerifySize(const Mat* pImage, NUI_IMAGE_RESOLUTION resolution) const;


	/// <summary>
	/// Gets the color stream resolution
	/// </summary>
	/// <param name="width">pointer to store width in</param>
	/// <param name="height">pointer to store depth in</param>
	/// <returns>S_OK if successful, an error code otherwise</returns>
	//HRESULT GetColorFrameSize(DWORD* width, DWORD* height) const;

	/// <summary>
	/// Gets the depth stream resolution
	/// </summary>
	/// <param name="width">pointer to store width in</param>
	/// <param name="height">pointer to store depth in</param>
	/// <returns>S_OK if successful, an error code otherwise</returns>
	//HRESULT GetDepthFrameSize(DWORD* width, DWORD* height) const;

};