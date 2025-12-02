#pragma once

typedef struct tagVPE_CONTOUR_VALUE {
	int nLeft;
	int nRight;
	int nStart;
	int nEnd;

	int nLeftMaxCV;
	int nRightMaxCV;

	int nLeftAvgCV;
	int nRightAvgCV;

	int nLeftMaxNum;
	int nRightMaxNum;

	int nLeftValidCount;
	int nRightValidCount;

	int nDeltaLeft;
	int nDeltaRight;

	int nLeftSumCV;
	int nRightSumCV;

	float fLeftSumCV;
	float fRightSumCV;
	float fLeftMaxCV;
	float fRightMaxCV;
} VPE_CONTOUR_VALUE;

typedef struct tagVPE_CONTOUR_DATA {
	int pnAvgHistogramTmp[HISTOGRAM_SIZE];
	int pnBaseHistogramTmp[HISTOGRAM_SIZE];
	int pnAvgSumHistogramTmp[HISTOGRAM_SIZE];
	int pnFastHistogramTmp[HISTOGRAM_SIZE];

	int pnBaseHistogram[HISTOGRAM_SIZE];
	int pnFastHistogram[HISTOGRAM_SIZE];
	int pnAvgSumHistogram[HISTOGRAM_SIZE];
	int pnAvgHistogram[HISTOGRAM_SIZE];

	int nLeftRows;
	int nRightRows;

	int nLeftSum;
	int nRightSum;

	int nLeftValidCount;
	int nRightValidCount;

	int nLeftMaxNum;
	int nRightMaxNum;

	int nLeftMaxCV;
	int nRightMaxCV;
	int nLeftAvgCV;
	int nRightAvgCV;

	float fLeftSumCV;
	float fRightSumCV;
	float fLeftMaxCV;
	float fRightMaxCV;

	int nLeftSumCV;
	int nRightSumCV;

	float fSumCV;
	float fAvgCV;
	float fMinCV;
	float fMaxCV;
	float fScaleCV;

	float fMSDCV;
	float fRMSDCV;
	float fScale;
} VPE_CONTOUR_DATA;



class CVulpesEngineContour
{
public:
	CVulpesEngineContour(void* lpdata);
	~CVulpesEngineContour();

public:
	void InitContour(int nWidth, int nHeight);
	void InitContourValue(VPE_CONTOUR_VALUE* pValue);
	void DrawContour(float* pfImage, int nWidth, int nHeight, int nFreq, float** ppfDeltaData, int nFrameCount, int nDeltaDataIndex, int nDeltaDataCnt, int* pnImage);
	void DrawContourCV();
	void CalculateContourSketch();
	void CalculateContourPosition(int* pnPosition);
	void CalculateContourFill();
	void CalculateContourFree();
	void SetContour();
	void ContourSketchFilter();
	void ContourPositionFilter();
	void CalculateContour();
	int CalculateContourAvgAm(int nStartIndex, int nHeightIndex);
	void CalculateContourHistogram(int* pnHistogram, int nCnt, int nVal1, int nVal2);
	int CalculateHistogramAvg(int nCase, int nVal);
	void CalculateContourSumCV(int nStart, int nEnd, float* pfContourSumCV, float* pfContourMaxCV, int* nMaxNum, int* nValCount, int* pnHistogram, float* pfImageCV);
	void Release();
	void Reset();

public:
	void* m_pEngine;
	SIZE m_szContourImage;
	unsigned char* m_pucContourImage;

	float** m_ppfDeltaData;
	float* m_pfImageSrc;

	int* m_pnImageSrc;
	int* m_pnImage;
	int* m_pnImageTmp;
	int* m_pnImageRows;

	int m_nFreq;
	int m_nAccumFrameCnt;
	int m_nDeltaDataIndex;
	int m_nDeltaDataCount;
	int m_nAvg;
	int m_nAvgPosition;

	VPE_CONTOUR_VALUE*	m_pContourFillData;
	VPE_CONTOUR_VALUE*	m_pContourFreeData;
	VPE_CONTOUR_VALUE*	m_pContourData;
	VPE_CONTOUR_VALUE m_sContourFillData[48];

	VPE_CONTOUR_DATA m_sContourCVData;
};