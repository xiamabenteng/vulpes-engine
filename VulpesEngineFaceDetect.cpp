#include "stdafx.h"
#include "VulpesEngineFaceDetect.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include "opencv2/contrib/contrib.hpp"

using namespace cv;

CString strFileName = L"";
bool    bReady = 0;
bool    bReadyStr = 0;
Mat     matFrame;
Mat     matFrameG;
Mat     matFrameGray;
int     nWidth = 0;
int     nHeight = 0;
int     nBpp = 0;

bool    bCase = false;

int     nQu_l = 3;
float   flScale_l = 2.0f;
int     minSize_l = 64;
int     maxSize_l = 640;
CascadeClassifier faceCase;
vector<Rect> faces;
#define COFF_SCALE 1.0f

void SetFace_File()
{
	if (strFileName.GetLength() < 1)
		return;

	WCHAR buff[1024];
	DWORD dwGot = GetModuleFileName(NULL, buff, 1024);
	CString str = buff;
	int pos = str.ReverseFind('\\');
	str = str.Left(pos + 1);
	str.Format(_T("%s%s"), str, strFileName);
	string file = CW2A(str);
	bCase = faceCase.load(file.c_str());
}


CVulpesEngineFaceDetect::CVulpesEngineFaceDetect()
{
	m_nSize = 0;
}


CVulpesEngineFaceDetect::~CVulpesEngineFaceDetect()
{
}

void CVulpesEngineFaceDetect::SetFile(LPCWSTR str)
{
	strFileName = str;
	bReadyStr = false;
}

void CVulpesEngineFaceDetect::RunFaceDetect(unsigned char* ptr, int nw, int nh, int bpp, int nQu, float flScale, int MinSize, int maxSize)
{
	if (nw != nWidth || nh != nHeight)
	{
		matFrame.release();
		matFrame.create(nh, nw, CV_8UC1);

		nWidth = nw;
		nHeight = nh;
	}

	SetFace_File();

	IplImage *ipl = &matFrame.operator IplImage();
	unsigned char *ptr_d = (unsigned char *)ipl->imageData;

	memcpy(ptr_d, ptr, nw * nh);
	unsigned char* ptr1 = ptr + (nh - 1) * nw;

	for (int i = 0; i < nh; i++)
	{
		memcpy(ptr_d, ptr1, nw);
		ptr_d += nw;
		ptr1 -= nw;
	}

	int nRows = nHeight;
	int nCols = nWidth;

	if (flScale >= 1.0f)
	{
		nRows = (int)((float)nRows / (flScale * COFF_SCALE)); //* 1.3f
		nCols = (int)((float)nCols / (flScale * COFF_SCALE));// * 1.3f
	}

	if (matFrameGray.rows != nRows || matFrameGray.cols != nCols)
	{
		matFrameGray.release();
		matFrameGray.create(nRows, nCols, CV_8UC1);
	}

	Size dsize;

	dsize.height = matFrameGray.size.p[0];
	dsize.width = matFrameGray.size.p[1];
	resize(matFrame, matFrameGray, dsize);
	equalizeHist(matFrameGray, matFrameGray);
	//normalize()
	nQu_l = nQu;
	flScale_l = flScale * COFF_SCALE;// *1.3f;
	minSize_l = int((float)MinSize / flScale_l);// *COFF_SCALE); // 1.3f
	maxSize_l = int((float)maxSize);// / flScale_l);// *COFF_SCALE); // 1.3f / flScale_l

	if (!bCase)
	{
		m_nSize = 0;
		return;
	}
	DWORD64 dwS = GetCurrentMillisecond();
	faceCase.detectMultiScale(matFrameGray, faces, 1.1000000000000089, nQu_l, 0, Size(minSize_l, minSize_l), Size(maxSize_l, maxSize_l));
	int nsize = 0;
	RECT rrt[100];
	for (int i = 0; i < (int)faces.size(); i++) {
		if (nsize >= 100)
			break;

		rrt[nsize].left = faces.at(i).x;
		rrt[nsize].top = faces.at(i).y;
		rrt[nsize].bottom = faces.at(i).y + faces.at(i).height;
		rrt[nsize].right = faces.at(i).x + faces.at(i).width;

		rrt[nsize].left = (int)((float)rrt[nsize].left * flScale_l);
		rrt[nsize].top = (int)((float)rrt[nsize].top * flScale_l);
		rrt[nsize].right = (int)((float)rrt[nsize].right * flScale_l);
		rrt[nsize].bottom = (int)((float)rrt[nsize].bottom * flScale_l);
		nsize++;
	}

	memcpy(rtList, rrt, sizeof(RECT) * nsize);
	m_nSize = nsize;
}


bool CVulpesEngineFaceDetect::StopFaceDetect()
{
	return true;
}


DWORD64 CVulpesEngineFaceDetect::GetCurrentMillisecond() {
	FILETIME ft = { 0 };
	SYSTEMTIME st = { 0 };
	ULARGE_INTEGER ull = { 0 };
	::GetSystemTime(&st);
	::SystemTimeToFileTime(&st, &ft);
	ull.LowPart = ft.dwLowDateTime;
	ull.HighPart = ft.dwHighDateTime;
	return (ull.QuadPart - 116444736000000000ULL) / 10000ULL;
}
