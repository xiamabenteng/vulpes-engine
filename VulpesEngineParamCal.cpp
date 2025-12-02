#include "stdafx.h"
#include "VulpesEngineCore.h"


CVulpesEngineParamCal::CVulpesEngineParamCal(void* lpData)
{
	m_pEngine = lpData;
	m_ParamDataFastPixCV.Init();
}

CVulpesEngineParamCal::~CVulpesEngineParamCal() {
	m_ParamDataFastPixCV.Init();
}

void CVulpesEngineParamCal::Init() {
	m_ParamDataFastPixCV.Init();
}

double CVulpesEngineParamCal::Calculate(float fVal1, float fVal2, int nParamId)
{
	CVulpesEngineCore* pEngine = (CVulpesEngineCore*)m_pEngine;

	double dValue = 0.0;
	double dVal[3];

	dVal[0] = pEngine->m_dCurrentFrameTime;
	dVal[1] = (double)fVal1;
	dVal[2] = (double)fVal2;

	switch (nParamId)
	{
	case STAT_RESULT_FAST_PIX_CV:
		m_ParamDataFastPixCV.AddValue(dVal);
		m_ParamDataFastPixCV.GetStartIndex();
		dValue = _CalParamFastPixCV();
		break;
	default:
		break;
	}
	return dValue;
}

double CVulpesEngineParamCal::_CalParamFastPixCV() {

	CVulpesEngineCore* pEngine = (CVulpesEngineCore*)m_pEngine;
	double dFastPixCV = 0.0;
	int nCnt = 0;
	if (m_ParamDataFastPixCV.m_nStoreCnt) {
		nCnt = (m_ParamDataFastPixCV.m_nCurrentIndex - m_ParamDataFastPixCV.m_nStartIndex) % m_ParamDataFastPixCV.m_nStoreCnt;
	}
	double dImageData1Sum = 0.0;
	double dImageData2Sum = 0.0;
	int nIndex = 0;
	for (int i = 0; i < nCnt; i++) {
		nIndex = ((m_ParamDataFastPixCV.m_nCurrentIndex - i - 1 + m_ParamDataFastPixCV.m_nStoreCnt) % m_ParamDataFastPixCV.m_nStoreCnt) * 3;
		dImageData1Sum += m_ParamDataFastPixCV.m_pdStoreData[nIndex + 1];
		dImageData2Sum += m_ParamDataFastPixCV.m_pdStoreData[nIndex + 2];
	}

	double dCnt = (double)nCnt;
	double dImageData1Avg = dImageData1Sum / dCnt;
	double dImageData2Avg = dImageData2Sum / dCnt;
	double dImageData1DeltaSum = 0.0;
	double dImageData2DeltaSum = 0.0;
	double dImageData11MultiplySum = 0.0;
	double dImageData12MultiplySum = 0.0;
	double dImageData22MultiplySum = 0.0;
	double dImageData1Delta = 0.0;
	double dImageData2Delta = 0.0;
	for (int i = 0; i < nCnt; i++) {
		nIndex = ((m_ParamDataFastPixCV.m_nCurrentIndex - i - 1 + m_ParamDataFastPixCV.m_nStoreCnt) % m_ParamDataFastPixCV.m_nStoreCnt) * 3;
		dImageData1Delta = m_ParamDataFastPixCV.m_pdStoreData[nIndex + 1] - dImageData1Avg;
		dImageData2Delta = m_ParamDataFastPixCV.m_pdStoreData[nIndex + 2] - dImageData2Avg;

		dImageData1DeltaSum += dImageData1Delta;
		dImageData2DeltaSum += dImageData2Delta;

		dImageData11MultiplySum += pow(dImageData1Delta, 2);
		dImageData12MultiplySum += dImageData1Delta * dImageData2Delta;
		dImageData22MultiplySum += pow(dImageData2Delta, 2);
	}

	if (dImageData11MultiplySum == 0.0 || dImageData12MultiplySum == 0.0 || dImageData22MultiplySum == 0.0 ) {
		return 0.0;
	}
	dFastPixCV = (dImageData12MultiplySum - (dImageData2DeltaSum * dImageData1DeltaSum / dCnt)) / (sqrt(dImageData22MultiplySum - pow(dImageData2DeltaSum, 2) / dCnt) * sqrt(dImageData11MultiplySum - pow(dImageData1DeltaSum, 2) / dCnt));
	return dFastPixCV;
}
