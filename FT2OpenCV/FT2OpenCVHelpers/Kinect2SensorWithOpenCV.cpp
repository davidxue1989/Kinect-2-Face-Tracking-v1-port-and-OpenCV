
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
