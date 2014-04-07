
#include "StdAfx.h"
#include "Kinect2SensorWithOpenCV.h"

#include <opencv2/highgui/highgui.hpp>

void Kinect2SensorWithOpenCV::ConvertColorMatFromFT(IFTImage *src, Mat *dst, bool display) {

	int colorHeight = src->GetHeight();
	int colorWidth = src->GetWidth();

	if (NULL == dst->data)
		dst->create(Size(colorWidth, colorHeight), COLOR_TYPE);

	BYTE* pBuffer = src->GetBuffer();

	// Copy image information into Mat
	for (UINT y = 0; y < colorHeight; ++y)
	{
		// Get row pointer for color Mat
		Vec4b* pColorRow = dst->ptr<Vec4b>(y);

		for (UINT x = 0; x < colorWidth; ++x)
		{
			pColorRow[x] = Vec4b(pBuffer[y * colorWidth * 4 + x * 4 + 0],
				pBuffer[y * colorWidth * 4 + x * 4 + 1],
				pBuffer[y * colorWidth * 4 + x * 4 + 2],
				pBuffer[y * colorWidth * 4 + x * 4 + 3]);
		}
	}

	if (display)
		imshow("dxColor", *dst);
}

void Kinect2SensorWithOpenCV::ConvertDepthMatFromFT(IFTImage *src, Mat *dst, bool display) {

	int depthHeight = src->GetHeight();
	int depthWidth = src->GetWidth();

	if (NULL == dst->data)
		dst->create(Size(depthWidth, depthHeight), DEPTH_TYPE);

	UINT16* pBuffer = reinterpret_cast<UINT16*>(src->GetBuffer());

	// Copy image information into Mat
	for (UINT y = 0; y < depthHeight; ++y)
	{
		// Get row pointer for color Mat
		UINT16* pDepthRow = dst->ptr<UINT16>(y);

		for (UINT x = 0; x < depthWidth; ++x)
		{
			pDepthRow[x] = pBuffer[y * depthWidth + x];
		}
	}

	if (display) {
		Mat img8bit;
		dst->convertTo(img8bit, CV_8U);
		imshow("dxDepth", img8bit);
	}
}
HRESULT Kinect2SensorWithOpenCV::GrabColorMat() {
	WaitForSingleObject(m_hColorMatMutex, INFINITE);

	int colorHeight = Kinect2Sensor::cColorHeight;
	int colorWidth = Kinect2Sensor::cColorWidth;

	if (NULL == m_colorImgMat.data)
		m_colorImgMat.create(Size(colorWidth, colorHeight), COLOR_TYPE);
	
	// Copy image information into Mat
	for (UINT y = 0; y < colorHeight; ++y)
	{
		// Get row pointer for color Mat
		Vec4b* pColorRow = m_colorImgMat.ptr<Vec4b>(y);

		for (UINT x = 0; x < colorWidth; ++x)
		{
			//testing if m_VideoBuffer->GetBuffer() works
			//RGBQUAD *a = reinterpret_cast<RGBQUAD*> (m_VideoBuffer->GetBuffer());
			//pColorRow[x] = Vec4b(a[y * colorWidth + x + 0].rgbBlue,
			//	a[y * colorWidth + x + 0].rgbGreen,
			//	a[y * colorWidth + x + 0].rgbRed,
			//	a[y * colorWidth + x + 0].rgbReserved);

			pColorRow[x] = Vec4b(m_pColorRGBX[y * colorWidth + x].rgbBlue,
				m_pColorRGBX[y * colorWidth + x].rgbGreen,
				m_pColorRGBX[y * colorWidth + x].rgbRed,
				m_pColorRGBX[y * colorWidth + x].rgbReserved);
		}
	}

	//imshow("dx", m_colorImgMat);

	ReleaseMutex(m_hColorMatMutex);
	return S_OK;
}


HRESULT Kinect2SensorWithOpenCV::GrabDepthMat() {
	WaitForSingleObject(m_hDepthMatMutex, INFINITE);

	int depthHeight = Kinect2Sensor::cDepthHeight;
	int depthWidth = Kinect2Sensor::cDepthWidth;

	if (NULL == m_depthImgMat.data)
		m_depthImgMat.create(Size(depthWidth, depthHeight), DEPTH_TYPE);
	
	// Copy image information into Mat
	for (UINT y = 0; y < depthHeight; ++y)
	{
		// Get row pointer for depth Mat
		UINT16* pDepthRow = m_depthImgMat.ptr<UINT16>(y);

		for (UINT x = 0; x < depthWidth; ++x)
		{
			////testing if m_DepthBuffer->GetBuffer() works
			//UINT16 *a = reinterpret_cast<UINT16*>(m_DepthBuffer->GetBuffer());
			//pDepthRow[x] = a[y * depthWidth + x];

			pDepthRow[x] = m_pDepth[y * depthWidth + x];
		}
	}

	//Mat img8bit;
	//m_depthImgMat.convertTo(img8bit, CV_8U);
	//imshow("dx", img8bit);

	ReleaseMutex(m_hDepthMatMutex);
	return S_OK;
}

HRESULT Kinect2SensorWithOpenCV::GrabDepthArgbMat() {

	WaitForSingleObject(m_hDepthMatMutex, INFINITE);
	WaitForSingleObject(m_hDepthAsArgbMatMutex, INFINITE);

	if (NULL == m_depthImgMat.data) {
		ReleaseMutex(m_hDepthAsArgbMatMutex);
		ReleaseMutex(m_hDepthMatMutex);
		return E_FAIL;
	}

	int depthHeight = Kinect2Sensor::cDepthHeight;
	int depthWidth = Kinect2Sensor::cDepthWidth;

	if (NULL == m_depthAsArgbMat.data)
		m_depthAsArgbMat.create(Size(depthWidth, depthHeight), DEPTH_RGB_TYPE);

	// Get a copy of the depth image
	Mat depthImage;
	//depthImage.create(depthHeight, depthWidth, DEPTH_TYPE);
	m_depthImgMat.copyTo(depthImage);

	for (UINT y = 0; y < depthHeight; ++y)
	{
		// Get row pointers for Mats
		const USHORT* pDepthRow = depthImage.ptr<USHORT>(y);
		Vec4b* pDepthRgbRow = m_depthAsArgbMat.ptr<Vec4b>(y);

		for (UINT x = 0; x < depthWidth; ++x)
		{
			USHORT raw_depth = pDepthRow[x];

			// If depth value is valid, convert and copy it
			if (raw_depth != 65535)
			{
				//get body index
				BYTE bodyIndx = m_pBodyIndex[y * depthWidth + x];
				int a = m_pBodyIndex[y * depthWidth + x];

				if (a != 255)
					int b = 0;

				UINT8 redPixel, greenPixel, bluePixel;
				DepthShortToRgb(raw_depth, &redPixel, &greenPixel, &bluePixel, bodyIndx);
				pDepthRgbRow[x] = Vec4b(redPixel, greenPixel, bluePixel, 1);
			}
			else
			{
				pDepthRgbRow[x] = 0;
			}
		}
	}

	//imshow("dx", m_depthAsArgbMat);

	ReleaseMutex(m_hDepthAsArgbMatMutex);
	ReleaseMutex(m_hDepthMatMutex);

	return S_OK;
}


/// <summary>
/// Convert a 13-bit depth value into a set of RGB values
/// </summary>
/// <param name="depth">depth value to convert</param>
/// <param name="pRedPixel">value of red pixel</param>
/// <param name="pGreenPixel">value of green pixel</param>
/// <param name="pBluePixel">value of blue pixel</param>
/// <returns>S_OK if successful, an error code otherwise</returns>
HRESULT Kinect2SensorWithOpenCV::DepthShortToRgb(USHORT depth, UINT8* redPixel, UINT8* greenPixel, UINT8* bluePixel, BYTE bodyIndx)
{
	//SHORT realDepth = NuiDepthPixelToDepth(depth);
	SHORT realDepth = depth; //in kinect 2.0, the depth data only contains depth info, no player index, no need to convert
	//USHORT playerIndex = NuiDepthPixelToPlayerIndex(depth);
	USHORT playerIndex = bodyIndx;

	// Convert depth info into an intensity for display
	BYTE b = 255 - static_cast<BYTE>(256 * realDepth / 0x0fff);

	// Color the output based on the player index
	switch (playerIndex)
	{
	case 0:
		*redPixel = b / 2;
		*greenPixel = b / 2;
		*bluePixel = b / 2;
		break;

	case 1:
		*redPixel = b;
		*greenPixel = 0;
		*bluePixel = 0;
		break;

	case 2:
		*redPixel = 0;
		*greenPixel = b;
		*bluePixel = 0;
		break;

	case 3:
		*redPixel = b / 4;
		*greenPixel = b;
		*bluePixel = b;
		break;

	case 4:
		*redPixel = b;
		*greenPixel = b;
		*bluePixel = b / 4;
		break;

	case 5:
		*redPixel = b;
		*greenPixel = b / 4;
		*bluePixel = b;
		break;

	case 6:
		*redPixel = b / 2;
		*greenPixel = b / 2;
		*bluePixel = b;
		break;

	default:
		*redPixel = 255 - (b / 2);
		*greenPixel = 255 - (b / 2);
		*bluePixel = 255 - (b / 2);
		break;

	//default:
	//	*redPixel = 0;
	//	*greenPixel = 0;
	//	*bluePixel = 0;
	//	break;
	}

	return S_OK;
}





//
//HRESULT Kinect2SensorWithOpenCV::GetColorData(NUI_LOCKED_RECT lockedRect)
//{
//	WaitForSingleObject(m_hColorMatMutex, INFINITE);
//
//	DWORD colorHeight, colorWidth;
//	NuiImageResolutionToSize(m_colorResolution, colorWidth, colorHeight);
//
//	if (NULL == m_colorImgMat.data)
//		m_colorImgMat.create(Size(colorWidth, colorHeight), COLOR_TYPE);
//
//	BYTE* pBuffer = lockedRect.pBits;
//	//INT size = lockedRect.size;
//	INT pitch = lockedRect.Pitch;
//
//	if (pitch == 0)
//		return E_NUI_FRAME_NO_DATA;
//
//	// Copy image information into Mat
//	for (UINT y = 0; y < colorHeight; ++y)
//	{
//		// Get row pointer for color Mat
//		Vec4b* pColorRow = m_colorImgMat.ptr<Vec4b>(y);
//
//		for (UINT x = 0; x < colorWidth; ++x)
//		{
//			pColorRow[x] = Vec4b(pBuffer[y * m_colorBufferPitch + x * 4 + 0],
//				pBuffer[y * m_colorBufferPitch + x * 4 + 1],
//				pBuffer[y * m_colorBufferPitch + x * 4 + 2],
//				pBuffer[y * m_colorBufferPitch + x * 4 + 3]);
//		}
//	}
//	ReleaseMutex(m_hColorMatMutex);
//	return S_OK;
//}
//
//HRESULT Kinect2SensorWithOpenCV::GetDepthData(NUI_LOCKED_RECT lockedRect)
//{
//	WaitForSingleObject(m_hDepthMatMutex, INFINITE);
//
//	DWORD depthHeight, depthWidth;
//	NuiImageResolutionToSize(m_depthResolution, depthWidth, depthHeight);
//
//	if (NULL == m_depthImgMat.data)
//		m_depthImgMat.create(Size(depthWidth, depthHeight), DEPTH_TYPE);
//
//	BYTE* pBuffer = lockedRect.pBits;
//	//INT size = lockedRect.size;
//	INT pitch = lockedRect.Pitch;
//
//	if (pitch == 0)
//		return E_NUI_FRAME_NO_DATA;
//
//	// Copy image information into Mat
//	USHORT* pBufferRun = reinterpret_cast<USHORT*>(pBuffer);
//
//	for (UINT y = 0; y < depthHeight; ++y)
//	{
//		// Get row pointer for depth Mat
//		USHORT* pDepthRow = m_depthImgMat.ptr<USHORT>(y);
//
//		for (UINT x = 0; x < depthWidth; ++x)
//		{
//			pDepthRow[x] = pBufferRun[y * depthWidth + x];
//		}
//	}
//
//	ReleaseMutex(m_hDepthMatMutex);
//
//	return S_OK;
//}
//
//HRESULT Kinect2SensorWithOpenCV::GetDepthDataAsArgb()
//{
//	WaitForSingleObject(m_hDepthMatMutex, INFINITE);
//	WaitForSingleObject(m_hDepthAsArgbMatMutex, INFINITE);
//
//	DWORD depthWidth, depthHeight;
//	//NuiImageResolutionToSize(m_depthResolution, depthWidth, depthHeight);//dxtodo
//
//	if (NULL == m_depthAsArgbMat.data)
//		m_depthAsArgbMat.create(Size(depthWidth, depthHeight), COLOR_TYPE);	
//
//	// Get the depth image
//	Mat depthImage;
//	depthImage.create(depthHeight, depthWidth, DEPTH_TYPE);
//	m_depthImgMat.copyTo(depthImage);
//
//	for (UINT y = 0; y < depthHeight; ++y)
//	{
//		// Get row pointers for Mats
//		const USHORT* pDepthRow = depthImage.ptr<USHORT>(y);
//		Vec4b* pDepthRgbRow = m_depthAsArgbMat.ptr<Vec4b>(y);
//
//		for (UINT x = 0; x < depthWidth; ++x)
//		{
//			USHORT raw_depth = pDepthRow[x];
//
//			// If depth value is valid, convert and copy it
//			if (raw_depth != 65535)
//			{
//				UINT8 redPixel, greenPixel, bluePixel;
//				DepthShortToRgb(raw_depth, &redPixel, &greenPixel, &bluePixel);
//				pDepthRgbRow[x] = Vec4b(redPixel, greenPixel, bluePixel, 1);
//			}
//			else
//			{
//				pDepthRgbRow[x] = 0;
//			}
//		}
//	}
//
//	ReleaseMutex(m_hDepthAsArgbMatMutex);
//	ReleaseMutex(m_hDepthMatMutex);
//
//	return S_OK;
//}



//void Kinect2SensorWithOpenCV::drawTrackedPoints(Mat* pImg, IFTResult* pAAMRlt) {
//	if (!pAAMRlt)
//		return;
//	FT_VECTOR2D* pPts2D;
//	UINT pts2DCount;
//	HRESULT hr = pAAMRlt->Get2DShapePoints(&pPts2D, &pts2DCount);
//	if (FAILED(hr))
//	{
//		return;
//	}
//	if (pts2DCount < 86)
//	{
//		ASSERT(FALSE);
//		return;
//	}
//
//	//check if we have anything good for the tracked points, if so re-allocate space and copy new points, else use the old points
//	bool allBad = true;
//	for (UINT ipt = 0; ipt < pts2DCount; ++ipt)
//		if (pPts2D[ipt].x > 0 || pPts2D[ipt].y > 0)
//			allBad = false;
//	if (!allBad) {
//		if (m_pDepthStreamHandle)
//			_freea(m_pFaceModel2DPoint);
//		m_pFaceModel2DPoint = reinterpret_cast<POINT*>(_malloca(sizeof(POINT) * pts2DCount));
//		if (!m_pFaceModel2DPoint)
//			return;
//		for (UINT ipt = 0; ipt < pts2DCount; ++ipt)
//		{
//			m_pFaceModel2DPoint[ipt].x = LONG(pPts2D[ipt].x + 0.5f);
//			m_pFaceModel2DPoint[ipt].y = LONG(pPts2D[ipt].y + 0.5f);
//		}
//		m_faceModelOutDated = false;
//	}
//	else
//		m_faceModelOutDated = true;
//
//	if (!m_pFaceModel2DPoint)
//		return;
//
//	Scalar color = Scalar(0, 255, 0);
//	if (m_faceModelOutDated)
//		color = Scalar(0, 0, 255);
//
//	vector<Point> leftEyeContour;
//	for (UINT ipt = 0; ipt < 8; ++ipt)
//	{
//		Point p = Point(m_pFaceModel2DPoint[ipt].x, m_pFaceModel2DPoint[ipt].y);
//		leftEyeContour.push_back(p);
//
//		//POINT ptStart = m_pFaceModel2DPoint[ipt];
//		//POINT ptEnd = m_pFaceModel2DPoint[(ipt + 1) % 8];
//		//line(*pImg, Point(ptStart.x, ptStart.y), Point(ptEnd.x, ptEnd.y), color, 1);
//	}
//	//Rect leftEyeBox = boundingRect(leftEyeContour);
//	//rectangle(*pImg, leftEyeBox, color, 1);
//	//todo: use min area bounding box (rotated) instead
//
//	vector<Point> rightEyeContour;
//	for (UINT ipt = 8; ipt < 16; ++ipt)
//	{
//		Point p = Point(m_pFaceModel2DPoint[ipt].x, m_pFaceModel2DPoint[ipt].y);
//		rightEyeContour.push_back(p);
//
//		//POINT ptStart = m_pFaceModel2DPoint[ipt];
//		//POINT ptEnd = m_pFaceModel2DPoint[(ipt - 8 + 1) % 8 + 8];
//		//line(*pImg, Point(ptStart.x, ptStart.y), Point(ptEnd.x, ptEnd.y), color, 1);
//	}
//	//Rect rightEyeBox = boundingRect(rightEyeContour);
//	//rectangle(*pImg, rightEyeBox, color, 1);
//
//
//
///*
//	for (UINT ipt = 16; ipt < 26; ++ipt)
//	{
//		POINT ptStart = m_pFaceModel2DPoint[ipt];
//		POINT ptEnd = m_pFaceModel2DPoint[(ipt - 16 + 1) % 10 + 16];
//		line(*pImg, Point(ptStart.x, ptStart.y), Point(ptEnd.x, ptEnd.y), color, 1);
//	}
//
//	for (UINT ipt = 26; ipt < 36; ++ipt)
//	{
//		POINT ptStart = m_pFaceModel2DPoint[ipt];
//		POINT ptEnd = m_pFaceModel2DPoint[(ipt - 26 + 1) % 10 + 26];
//		line(*pImg, Point(ptStart.x, ptStart.y), Point(ptEnd.x, ptEnd.y), color, 1);
//	}
//
//	for (UINT ipt = 36; ipt < 47; ++ipt)
//	{
//		POINT ptStart = m_pFaceModel2DPoint[ipt];
//		POINT ptEnd = m_pFaceModel2DPoint[ipt + 1];
//		line(*pImg, Point(ptStart.x, ptStart.y), Point(ptEnd.x, ptEnd.y), color, 1);
//	}
//
//	for (UINT ipt = 48; ipt < 60; ++ipt)
//	{
//		POINT ptStart = m_pFaceModel2DPoint[ipt];
//		POINT ptEnd = m_pFaceModel2DPoint[(ipt - 48 + 1) % 12 + 48];
//		line(*pImg, Point(ptStart.x, ptStart.y), Point(ptEnd.x, ptEnd.y), color, 1);
//	}
//
//	for (UINT ipt = 60; ipt < 68; ++ipt)
//	{
//		POINT ptStart = m_pFaceModel2DPoint[ipt];
//		POINT ptEnd = m_pFaceModel2DPoint[(ipt - 60 + 1) % 8 + 60];
//		line(*pImg, Point(ptStart.x, ptStart.y), Point(ptEnd.x, ptEnd.y), color, 1);
//	}
//
//	for (UINT ipt = 68; ipt < 86; ++ipt)
//	{
//		POINT ptStart = m_pFaceModel2DPoint[ipt];
//		POINT ptEnd = m_pFaceModel2DPoint[ipt + 1];
//		line(*pImg, Point(ptStart.x, ptStart.y), Point(ptEnd.x, ptEnd.y), color, 1);
//	}*/
//}



//
///// <summary>
///// Verify image is of the given resolution
///// </summary>
///// <param name="pImage">pointer to image to verify</param>
///// <param name="resolution">resolution of image</param>
///// <returns>S_OK if image matches given width and height, an error code otherwise</returns>
//HRESULT Kinect2SensorWithOpenCV::VerifySize(const Mat* pImage, NUI_IMAGE_RESOLUTION resolution) const
//{
//	DWORD width, height;
//	NuiImageResolutionToSize(resolution, width, height);
//
//	Size size = pImage->size();
//	if (size.height != height || size.width != width)
//	{
//		return E_INVALIDARG;
//	}
//
//	return S_OK;
//}


///// <summary>
///// Gets the color stream resolution
///// </summary>
///// <param name="width">pointer to store width in</param>
///// <param name="height">pointer to store depth in</param>
///// <returns>S_OK if successful, an error code otherwise</returns>
//HRESULT Kinect2SensorWithOpenCV::GetColorFrameSize(DWORD* width, DWORD* height) const
//{
//	// Fail if pointer is invalid
//	if (!width || !height)
//	{
//		return E_POINTER;
//	}
//
//	NuiImageResolutionToSize(m_colorResolution, *width, *height);
//
//	return S_OK;
//}
//
///// <summary>
///// Gets the depth stream resolution
///// </summary>
///// <param name="width">pointer to store width in</param>
///// <param name="height">pointer to store depth in</param>
///// <returns>S_OK if successful, an error code otherwise</returns>
//HRESULT Kinect2SensorWithOpenCV::GetDepthFrameSize(DWORD* width, DWORD* height) const
//{
//	// Fail if pointer is invalid
//	if (!width || !height)
//	{
//		return E_POINTER;
//	}
//
//	NuiImageResolutionToSize(m_depthResolution, *width, *height);
//
//	return S_OK;
//}